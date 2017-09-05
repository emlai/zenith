#include "window.h"
#include "texture.h"
#include "geometry.h"
#include "keyboard.h"
#include <SDL.h>
#include <stdexcept>

int Window::windowCount = 0;
bool Window::sdlVideoInitialized = false;
const int Window::fullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;

void Window::initializeSDLVideoSubsystem()
{
    SDL_Init(SDL_INIT_VIDEO);
    sdlVideoInitialized = true;
}

SDL_Window* Window::initWindowHandle(Vector2 size, const char* title, bool fullscreen)
{
    if (!sdlVideoInitialized)
        initializeSDLVideoSubsystem();

    uint32_t windowFlags = SDL_WINDOW_ALLOW_HIGHDPI;

    if (fullscreen)
        windowFlags |= fullscreenFlag;

    SDL_Window* windowHandle = SDL_CreateWindow(title,
                                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                size.x, size.y, windowFlags);

    if (!windowHandle)
        throw std::runtime_error(SDL_GetError());

    ++windowCount;
    return windowHandle;
}

Window::Window(Vector2 size, boost::string_ref title, bool fullscreen)
:   closeRequestReceived(false),
    windowHandle(initWindowHandle(size, title.to_string().c_str(), fullscreen), SDL_DestroyWindow),
    context(*this)
{
    SDL_EventState(SDL_WINDOWEVENT_ENTER, SDL_IGNORE);
    SDL_EventState(SDL_WINDOWEVENT_LEAVE, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEWHEEL, SDL_IGNORE);
    SDL_EventState(SDL_FINGERMOTION, SDL_IGNORE);
    SDL_EventState(SDL_FINGERDOWN, SDL_IGNORE);
    SDL_EventState(SDL_FINGERUP, SDL_IGNORE);
}

Window::~Window()
{
    windowHandle.reset();
    --windowCount;

    if (windowCount == 0)
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        sdlVideoInitialized = false;
    }
}

void Window::toggleFullscreen()
{
    bool isFullscreen = SDL_GetWindowFlags(windowHandle.get()) & fullscreenFlag;
    SDL_SetWindowFullscreen(windowHandle.get(), isFullscreen ? 0 : fullscreenFlag);
    SDL_ShowCursor(isFullscreen);
}

static int filterKeyRepeatEvents(void* userdata, SDL_Event* event)
{
    if (event->type == SDL_KEYDOWN && event->key.repeat
        && event->key.keysym.sym == static_cast<const SDL_Event*>(userdata)->key.keysym.sym)
        return 0;

    return 1;
}

Key Window::waitForInput()
{
    SDL_Event event;
    int msPerAnimationFrame = getAnimationFrameTime();

    while (true)
    {
        auto msUntilNextFrame = msPerAnimationFrame - (SDL_GetTicks() % msPerAnimationFrame);

        if (!SDL_WaitEventTimeout(&event, msUntilNextFrame))
            return NoKey;

        switch (event.type)
        {
            case SDL_KEYDOWN:
                SDL_FilterEvents(filterKeyRepeatEvents, &event);
                return event.key.keysym.sym;
            case SDL_WINDOWEVENT:
                if (handleWindowEvent(event.window.event))
                    return NoKey;
                break;
        }
    }
}

bool Window::handleWindowEvent(int eventType)
{
    switch (eventType)
    {
        case SDL_WINDOWEVENT_CLOSE:
            sendCloseRequest();
            break;
        default:
            return false;
    }

    return true;
}

void Window::sendCloseRequest()
{
    closeRequestReceived = true;
}

bool Window::shouldClose() const
{
    return closeRequestReceived;
}

Vector2 Window::getMousePosition() const
{
    Vector2 position;
    SDL_GetMouseState(&position.x, &position.y);
    return context.mapFromTargetCoordinates(position);
}

void Window::setShowCursor(bool show)
{
    SDL_ShowCursor(show);
}

Vector2 Window::getResolution() const
{
    return getSize() / context.getScale();
}

Vector2 Window::getSize() const
{
    Vector2 size;
    SDL_GetWindowSize(windowHandle.get(), &size.x, &size.y);
    return size;
}

int Window::getWidth() const
{
    int width;
    SDL_GetWindowSize(windowHandle.get(), &width, nullptr);
    return width;
}

int Window::getHeight() const
{
    int height;
    SDL_GetWindowSize(windowHandle.get(), nullptr, &height);
    return height;
}

Vector2 Window::getScreenResolution()
{
    if (!sdlVideoInitialized)
        initializeSDLVideoSubsystem();

    SDL_DisplayMode mode;

    if (SDL_GetDesktopDisplayMode(0, &mode) != 0)
        throw std::runtime_error(SDL_GetError());

    return Vector2(mode.w, mode.h);
}

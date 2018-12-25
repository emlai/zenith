#include "window.h"
#include "geometry.h"
#include "keyboard.h"
#include "state.h"
#include "texture.h"
#include <SDL.h>
#include <cctype>
#include <climits>
#include <stdexcept>

static const auto fullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;

SDL_Window* Window::initWindowHandle(const char* title, bool fullscreen)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_DisplayMode mode;

    if (SDL_GetDesktopDisplayMode(0, &mode) != 0)
        throw std::runtime_error(SDL_GetError());

    uint32_t windowFlags = SDL_WINDOW_ALLOW_HIGHDPI;

    if (fullscreen)
        windowFlags |= fullscreenFlag;

    SDL_Window* windowHandle = SDL_CreateWindow(title,
                                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                mode.w, mode.h, windowFlags);

    if (!windowHandle)
        throw std::runtime_error(SDL_GetError());

    return windowHandle;
}

Window::Window(StateManager* stateManager, std::string_view title, bool fullscreen)
:   stateManager(stateManager),
    closeRequestReceived(false),
    windowHandle(initWindowHandle(std::string(title).c_str(), fullscreen), SDL_DestroyWindow),
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
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Window::setFullscreen(bool enable)
{
    SDL_SetWindowFullscreen(windowHandle.get(), enable ? fullscreenFlag : 0);
}

void Window::toggleFullscreen()
{
    setFullscreen(!isFullscreen());
}

bool Window::isFullscreen() const
{
    return SDL_GetWindowFlags(windowHandle.get()) & fullscreenFlag;
}

Event Window::waitForInput()
{
    SDL_Event event;

    while (true)
    {
        stateManager->render();
        context.updateScreen();

        if (!SDL_PollEvent(&event))
            continue;

        switch (event.type)
        {
            case SDL_KEYDOWN:
            {
                auto key = event.key.keysym.sym;

                // Use shifts only as modifiers.
                if (key == SDLK_LSHIFT || key == SDLK_RSHIFT)
                    continue;

                if ((event.key.keysym.mod & Shift) && key > 0 && key <= UCHAR_MAX && std::isalpha(key))
                    return Event(Key(std::toupper(key)));
                else
                    return Event(Key(key));
            }

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    Vector2 position(event.button.x, event.button.y);
                    return Event(context.mapFromTargetCoordinates(position));
                }
                break;

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

const char* Window::getTitle() const
{
    return SDL_GetWindowTitle(windowHandle.get());
}

#include "window.h"
#include "texture.h"
#include "geometry.h"
#include "keyboard.h"
#include <SDL.h>
#include <stdexcept>

int Window::windowCount = 0;
bool Window::sdlVideoInitialized = false;

SDL_Window* Window::initWindowHandle(Vector2 size, const char* title)
{
    if (!sdlVideoInitialized)
    {
        SDL_Init(SDL_INIT_VIDEO);
        sdlVideoInitialized = true;
    }

    SDL_Window* windowHandle = SDL_CreateWindow(title,
                                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                size.x, size.y, 0);

    if (!windowHandle)
        throw std::runtime_error(SDL_GetError());

    ++windowCount;
    return windowHandle;
}

Window::Window(Vector2 size, const std::string& title)
:   closeRequestReceived(false),
    windowHandle(initWindowHandle(size, title.c_str()), SDL_DestroyWindow),
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
    auto fullscreenFlag = SDL_WINDOW_FULLSCREEN;
    bool isFullscreen = SDL_GetWindowFlags(windowHandle.get()) & fullscreenFlag;
    SDL_SetWindowFullscreen(windowHandle.get(), isFullscreen ? 0 : fullscreenFlag);
    SDL_ShowCursor(isFullscreen);
}

void Window::processInput(KeyDownCallback callback)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
                callback(*this, event.key.keysym.sym, event.key.keysym.mod);
                break;
            case SDL_WINDOWEVENT:
                handleWindowEvent(event.window.event);
                break;
        }
    }
}

Key Window::waitForInput()
{
    SDL_Event event;

    while (SDL_WaitEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
                return event.key.keysym.sym;
            case SDL_WINDOWEVENT:
                if (handleWindowEvent(event.window.event))
                    return NoKey;
                break;
        }
    }

    throw std::runtime_error(SDL_GetError());
}

Key Window::waitForInputWithTimeout(int timeoutMS)
{
    SDL_Event event;

    while (true)
    {
        auto startTime = SDL_GetTicks();

        if (!SDL_WaitEventTimeout(&event, std::max(0, timeoutMS)))
            return NoKey;

        auto endTime = SDL_GetTicks();

        switch (event.type)
        {
            case SDL_KEYDOWN:
                return event.key.keysym.sym;
            case SDL_WINDOWEVENT:
                if (handleWindowEvent(event.window.event))
                    return NoKey;
                break;
        }

        timeoutMS -= endTime - startTime;
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

    position -= context.getViewport().position;

    if (const Rect* view = context.getView())
        position += view->position;

    return position;
}

void Window::setShowCursor(bool show)
{
    SDL_ShowCursor(show);
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

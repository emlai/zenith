#include "engine.h"
#include "keyboard.h"
#include "state.h"
#include "window.h"
#include <functional>
#include <iostream>

Window& Engine::createWindow(Vector2 size, std::string_view title, bool fullscreen)
{
    windows.emplace_back(*this, size, title, fullscreen);
    return windows.back();
}

void Engine::reportErrorToUser(const std::string& text)
{
    if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, std::string(getWindow().getTitle()).c_str(),
                                 text.c_str(), nullptr) != 0)
        std::cerr << text.c_str() << std::endl;
}

void Engine::render(Window& window)
{
    if (getCurrentState().renderPreviousState())
        getPreviousState().render(window);

    getCurrentState().render(window);
}

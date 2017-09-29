#include "engine.h"
#include "keyboard.h"
#include "state.h"
#include "window.h"
#include <functional>
#include <iostream>

Window& Engine::createWindow(Vector2 size, boost::string_ref title, bool fullscreen)
{
    windows.emplace_back(*this, size, title, fullscreen);
    return windows.back();
}

void Engine::reportErrorToUser(boost::string_ref text)
{
    if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, getWindow().getTitle().to_string().c_str(),
                                 text.to_string().c_str(), nullptr) != 0)
        std::cerr << text.to_string().c_str() << std::endl;
}

void Engine::render(Window& window)
{
    if (getCurrentState().renderPreviousState())
        getPreviousState().render(window);

    getCurrentState().render(window);
}

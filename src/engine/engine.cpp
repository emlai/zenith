#include "engine.h"
#include "keyboard.h"
#include "state.h"
#include "window.h"
#include <functional>

Window& Engine::createWindow(Vector2 size, boost::string_ref title, bool fullscreen)
{
    windows.emplace_back(*this, size, title, fullscreen);
    return windows.back();
}

void Engine::mapKey(Key key, const std::function<bool()>& function)
{
    keyboard::mapKey(key, NoMod, function);
}

void Engine::render(Window& window)
{
    if (getCurrentState().renderPreviousState())
        getPreviousState().render(window);

    getCurrentState().render(window);
}

#include "engine.h"
#include "window.h"
#include "keyboard.h"
#include <functional>

void Engine::run()
{
    running = true;

    while (running && !window.shouldClose())
        updateLogic();

    running = false;
}

void Engine::mapKey(Key key, const std::function<bool()>& function)
{
    keyboard::mapKey(key, NoMod, function);
}

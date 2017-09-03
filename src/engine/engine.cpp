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

bool Engine::processInput(Window& window)
{
    if (auto key = window.waitForInputWithTimeout(0))
        if (auto& command = keyboard::getMappedCommand(key, NoMod))
            return command();

    return false;
}

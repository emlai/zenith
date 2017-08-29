#include "engine.h"
#include "window.h"
#include "keyboard.h"
#include <functional>

void Engine::run()
{
    for (running = true; running && !window.shouldClose();)
    {
        while (true)
        {
            render(window);
            window.updateScreen();

            if (processInput(window))
            {
                updateLogic();
                advanceTurn();
            }

            if (!running || window.shouldClose())
                return;
        }
    }
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

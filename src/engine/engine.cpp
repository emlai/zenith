#include "engine.h"
#include "window.h"
#include "keyboard.h"
#include <functional>

void Engine::run(Window& window)
{
    this->window = &window;

    for (running = true; running && !window.shouldClose();)
    {
        auto previousTick = getTick();

        while (getTick() == previousTick)
        {
            render(window);
            window.updateScreen();
            processInput(window);

            if (!running || window.shouldClose())
                return;
        }

        updateLogic();
    }
}

void Engine::mapKey(Key key, std::function<void()> function)
{
    keyboard::mapKey(key, NoMod, function);
}

void Engine::processInput(Window& window)
{
    if (auto key = window.waitForInputWithTimeout(window.getAnimationFrameTime()))
        if (auto& command = keyboard::getMappedCommand(key, NoMod))
            command();
}

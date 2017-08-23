#pragma once

#include "keyboard.h"

class Window;

class Engine
{
public:
    virtual ~Engine() = default;
    void run();

protected:
    Engine(Window& window) : window(window), tick(0), running(false) {}
    void advanceTick() { ++tick; }
    auto getTick() const { return tick; }
    void stop() { running = false; }
    void mapKey(Key, std::function<void()>);
    Window& getWindow() const { return window; }

private:
    virtual void render(Window&) = 0;
    virtual void updateLogic() = 0;
    void processInput(Window&);

    Window& window;
    int tick;
    bool running;
};

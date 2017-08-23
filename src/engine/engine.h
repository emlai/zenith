#pragma once

#include "keyboard.h"

class Window;

class Engine
{
public:
    virtual ~Engine() = default;
    void run();

protected:
    Engine(Window& window) : window(window), turn(0), running(false) {}
    void advanceTurn() { ++turn; }
    auto getTurn() const { return turn; }
    void stop() { running = false; }
    void mapKey(Key, std::function<void()>);
    Window& getWindow() const { return window; }

private:
    virtual void render(Window&) = 0;
    virtual void updateLogic() = 0;
    void processInput(Window&);

    Window& window;
    int turn;
    bool running;
};

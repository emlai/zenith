#pragma once

#include "keyboard.h"

class Window;

class Engine
{
public:
    virtual ~Engine() = default;
    void run();
    void stop() { running = false; }
    void advanceTurn() { ++turn; }
    auto getTurn() const { return turn; }
    Window& getWindow() const { return window; }

protected:
    Engine(Window& window) : window(window), turn(0), running(false) {}
    void setTurn(int turn) { this->turn = turn; }
    void mapKey(Key, const std::function<bool()>&);

private:
    virtual void render(Window&) = 0;
    virtual void updateLogic() = 0;

    Window& window;
    int turn;
    bool running;
};

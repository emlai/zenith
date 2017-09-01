#pragma once

#include "keyboard.h"

class Window;

class Engine
{
public:
    virtual ~Engine() = default;
    void run();
    auto getTurn() const { return turn; }

protected:
    Engine(Window& window) : window(window), turn(0), running(false) {}
    void advanceTurn() { ++turn; }
    void setTurn(int turn) { this->turn = turn; }
    void stop() { running = false; }
    void mapKey(Key, const std::function<bool()>&);
    Window& getWindow() const { return window; }

private:
    virtual void render(Window&) = 0;
    virtual void updateLogic() = 0;
    bool processInput(Window&);

    Window& window;
    int turn;
    bool running;
};

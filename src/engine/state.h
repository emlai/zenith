#pragma once

class Engine;
class Window;

class State
{
public:
    State() = default;
    State(const State&) = delete;
    State(State&&) = default;
    State& operator=(const State&) = delete;
    State& operator=(State&&) = default;
    virtual ~State() = default;
    Engine& getEngine() const { return *engine; }

private:
    friend class Engine;
    virtual void render(Window& window) = 0;
    virtual bool renderPreviousState() const { return false; }

    Engine* engine;
};
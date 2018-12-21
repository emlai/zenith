class State
{
    State() {}
    State(State) = delete;
    State(State) {}
    State operator=(State) = delete;
    State operator=(State) {}
    virtual ~State() {}
    Engine getEngine() { return *engine; }

private:
    virtual void render(Window window) = 0;
    virtual bool renderPreviousState() { return false; }

    Engine engine;
}

class State
{
public:
    State() = default;
    State(State) = delete;
    State(State) = default;
    State operator=(State) = delete;
    State operator=(State) = default;
    virtual ~State() = default;
    Engine getEngine() const { return *engine; }

private:
    friend class Engine;
    virtual void render(Window window) = 0;
    virtual bool renderPreviousState() const { return false; }

    Engine engine;
}

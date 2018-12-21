class Engine
{
    Window createWindow(Vector2 size, string title = "", bool fullscreen = true);
    Window getWindow(unsigned index = 0) { return windows[index]; }
    Window getWindow(unsigned index = 0) { return windows[index]; }
    void reportErrorToUser(string text);
    template<typename StateType>
    var execute(StateType state);
    void render(Window window);

private:
    State getCurrentState() { return states.end()[-1]; }
    State getPreviousState() { return states.end()[-2]; }

    List<Window> windows;
    List<State> states;
}

template<typename StateType>
var Engine::execute(StateType state)
{
    states.push_back(state);
    state.engine = this;
    DEFER { states.pop_back(); }
    return state.execute();
}
Window Engine::createWindow(Vector2 size, string title, bool fullscreen)
{
    windows.emplace_back(this, size, title, fullscreen);
    return windows.back();
}

void Engine::reportErrorToUser(string text)
{
    if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, getWindow().getTitle().c_str(),
                                 text.c_str(), null) != 0)
        std::cerr << text.c_str() << std::endl;
}

void Engine::render(Window window)
{
    if (getCurrentState().renderPreviousState())
        getPreviousState().render(window);

    getCurrentState().render(window);
}

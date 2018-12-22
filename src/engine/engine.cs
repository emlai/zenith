class Engine
{
    List<Window> windows;
    List<State> states;

    Window getWindow(int index = 0) { return windows[index]; }
    State getCurrentState() { return states.end()[-1]; }
    State getPreviousState() { return states.end()[-2]; }

    Window createWindow(Vector2 size, string title, bool fullscreen)
    {
        windows.emplace_back(this, size, title, fullscreen);
        return windows.back();
    }

    var execute<StateType>(StateType state)
    {
        states.Add(state);
        state.engine = this;
        DEFER { states.pop_back(); }
        return state.execute();
    }

    void reportErrorToUser(string text)
    {
        if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, getWindow().getTitle().c_str(),
                                     text.c_str(), null) != 0)
            std::cerr << text.c_str() << std::endl;
    }

    void render(Window window)
    {
        if (getCurrentState().renderPreviousState())
            getPreviousState().render(window);

        getCurrentState().render(window);
    }
}

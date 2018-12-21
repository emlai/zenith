#pragma once

#include "geometry.h"
#include "keyboard.h"
#include "state.h"
#include "utility.h"
#include <boost/utility/string_ref.hpp>
#include <vector>

class Window;

class Engine
{
public:
    Window& createWindow(Vector2 size, boost::string_ref title = "", bool fullscreen = true);
    Window& getWindow(unsigned index = 0) { return windows[index]; }
    const Window& getWindow(unsigned index = 0) const { return windows[index]; }
    void reportErrorToUser(boost::string_ref text);
    template<typename StateType>
    auto execute(StateType& state);
    void render(Window& window);

private:
    friend class Window;
    State& getCurrentState() { return *states.end()[-1]; }
    State& getPreviousState() { return *states.end()[-2]; }

    std::vector<Window> windows;
    std::vector<State*> states;
};

template<typename StateType>
auto Engine::execute(StateType& state)
{
    states.push_back(&state);
    state.engine = this;
    DEFER { states.pop_back(); };
    return state.execute();
}
#include "engine.h"
#include "keyboard.h"
#include "state.h"
#include "window.h"
#include <functional>
#include <iostream>

Window& Engine::createWindow(Vector2 size, boost::string_ref title, bool fullscreen)
{
    windows.emplace_back(*this, size, title, fullscreen);
    return windows.back();
}

void Engine::reportErrorToUser(boost::string_ref text)
{
    if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, getWindow().getTitle().to_string().c_str(),
                                 text.to_string().c_str(), nullptr) != 0)
        std::cerr << text.to_string().c_str() << std::endl;
}

void Engine::render(Window& window)
{
    if (getCurrentState().renderPreviousState())
        getPreviousState().render(window);

    getCurrentState().render(window);
}

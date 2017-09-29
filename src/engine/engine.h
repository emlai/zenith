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

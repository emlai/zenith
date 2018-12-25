#pragma once

#include "utility.h"
#include <vector>

class State;
class Window;

class StateManager
{
public:
    void render() const;
    template<typename T>
    auto execute(T& state, Window* window)
    {
        states.push_back(&state);
        state.stateManager = this;
        state.window = window;
        DEFER { states.pop_back(); };
        return state.execute();
    }

private:
    State* currentState() const { return states[states.size() - 1]; }
    State* previousState() const { return states[states.size() - 2]; }

    std::vector<State*> states;
};

class State
{
public:
    virtual ~State() = default;
    virtual void render() = 0;
    virtual bool renderPreviousState() const { return false; }
    template<typename T>
    auto executeState(T& state) { return stateManager->execute(state, window); }

    StateManager* stateManager = nullptr;
    Window* window = nullptr;
};


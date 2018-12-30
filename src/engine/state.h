#pragma once

#include "assert.h"
#include "keyboard.h"
#include "utility.h"
#include "window.h"
#include <memory>
#include <variant>
#include <vector>

class State;
class StateManager;
class Window;

class StateChange
{
public:
    struct Result
    {
        enum Type { None, Int, Dir, String };

        Result() : type(None) {}
        Result(Result&&);
        ~Result();
        int getInt() const { ASSERT(type == Int); return intValue; }
        Dir8 getDir() const { ASSERT(type == Dir); return dirValue; }
        std::string getString() const { ASSERT(type == String); return std::move(stringValue); }
        explicit operator bool() const { return type != None; }

        Type type;
        union
        {
            int intValue;
            Dir8 dirValue;
            std::string stringValue;
        };
    };

    struct None {};

    struct Push
    {
        Push(std::unique_ptr<State> newState) : newState(std::move(newState)) {}

        std::unique_ptr<State> newState;
    };

    struct Pop
    {
        Pop();
        Pop(int intValue);
        Pop(Dir8 dirValue);
        Pop(std::string stringValue);

        Result result;
    };

    using Variant = std::variant<None, Push, Pop>;

    StateChange(None variant) : variant(std::move(variant)) {}
    StateChange(Push variant) : variant(std::move(variant)) {}
    StateChange(Pop variant) : variant(std::move(variant)) {}

    Variant variant;
};

class State
{
public:
    virtual ~State() = default;
    virtual void render() = 0;
    virtual bool renderPreviousState() const { return false; }
    virtual StateChange update() { return StateChange::None(); }
    virtual StateChange onKeyDown(Key) { return StateChange::None(); }
    virtual StateChange onEvent(Event) { return StateChange::None(); }

    StateManager* stateManager = nullptr;
    Window* window = nullptr;
};

class StateManager
{
public:
    void pushState(std::unique_ptr<State> newState);
    StateChange::Result wait();
    StateChange::Result getResult(StateChange stateChange);
    void render() const;

    void operator()(StateChange::None& none);
    void operator()(StateChange::Push& push);
    void operator()(StateChange::Pop& pop);

    Window* window = nullptr;

private:
    StateChange::Result handleStateChange(StateChange stateChange);
    State* currentState() const;
    State* previousState() const;

    std::vector<std::unique_ptr<State>> states;
};

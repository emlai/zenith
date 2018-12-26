#include "state.h"
#include "window.h"

StateChange::Result::~Result()
{
    switch (type)
    {
        case None:
        case Int:
        case Dir:
            break;

        case String:
            stringValue.~basic_string();
            break;
    }
}

StateChange::Result::Result(Result&& result)
{
    type = result.type;

    switch (type)
    {
        case None:
            break;

        case Int:
            intValue = result.intValue;
            break;

        case Dir:
            dirValue = result.dirValue;
            break;

        case String:
            new (&stringValue) std::string(std::move(result.stringValue));
            break;
    }
}

StateChange::Pop::Pop()
{
    result.type = Result::None;
}

StateChange::Pop::Pop(int intValue)
{
    result.type = Result::Int;
    result.intValue = intValue;
}

StateChange::Pop::Pop(Dir8 dirValue)
{
    result.type = Result::Dir;
    result.dirValue = dirValue;
}

StateChange::Pop::Pop(std::string stringValue)
{
    result.type = Result::String;
    new (&result.stringValue) std::string(std::move(stringValue));
}

void StateManager::pushState(std::unique_ptr<State> newState)
{
    newState->stateManager = this;
    newState->window = window;
    states.push_back(std::move(newState));
}

StateChange::Result StateManager::wait()
{
    auto oldStates = states.size();

    while (states.size() >= oldStates)
    {
        // FIXME: These are duplicated in PlayerController::control().
        render();
        window->context.updateScreen();

        if (auto result = handleStateChange(currentState()->update()))
            return result;

        if (auto event = window->pollEvent())
        {
            if (auto result = handleStateChange(currentState()->onEvent(event)))
                return result;

            switch (event.type)
            {
                case Event::KeyDown:
                    if (auto result = handleStateChange(currentState()->onKeyDown(event.key)))
                        return result;
                    break;

                default:
                    break;
            }
        }
    }

    return StateChange::Result();
}

StateChange::Result StateManager::getResult(StateChange stateChange)
{
    if (auto pop = std::get_if<StateChange::Pop>(&stateChange.variant))
        return std::move(pop->result);

    return StateChange::Result();
}

void StateManager::render() const
{
    if (currentState()->renderPreviousState())
        previousState()->render();

    currentState()->render();
}

void StateManager::operator()(StateChange::None&)
{
}

void StateManager::operator()(StateChange::Push& push)
{
    pushState(std::move(push.newState));
}

void StateManager::operator()(StateChange::Pop&)
{
    states.pop_back();
}

StateChange::Result StateManager::handleStateChange(StateChange stateChange)
{
    std::visit(*this, stateChange.variant);
    return getResult(std::move(stateChange));
}

State* StateManager::currentState() const
{
    assert(states.size() >= 1);
    return &*states[states.size() - 1];
}

State* StateManager::previousState() const
{
    assert(states.size() >= 2);
    return &*states[states.size() - 2];
}

#include "state.h"

void StateManager::render() const
{
    if (currentState()->renderPreviousState())
        previousState()->render();

    currentState()->render();
}

#include "action.h"
#include "creature.h"

double getAPCost(Action action, const Creature& actor)
{
    assert(action != NoAction);

    if (action == Move && actor.isRunning())
        return 0.5;

    return 1;
}

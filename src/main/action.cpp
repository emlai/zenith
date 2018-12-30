#include "action.h"
#include "creature.h"
#include "engine/assert.h"

static bool isMovementAction(Action action)
{
    return action == Move || action == GoUpOrDown;
}

const char* toString(Action action)
{
    switch (action)
    {
        case Wait: return "Wait";
        case Move: return "Move";
        case GoUpOrDown: return "GoUpOrDown";
        case Attack: return "Attack";
        case PickUpItems: return "PickUp";
        case DropItem: return "Drop";
        case UseItem: return "Use";
        case EatItem: return "Eat";
        case Close: return "Close";
        case EnterLookMode: return "LookMode";
        case OpenInventory: return "Inventory";
        case ShowEquipmentMenu: return "EquipmentMenu";
        case ToggleRunning: return "ToggleRunning";
        case NoAction:
        case LastAction:
            break;
    }

    ASSERT(false);
}

double getAPCost(Action action, const Creature& actor)
{
    ASSERT(action != NoAction);

    if (isMovementAction(action) && actor.isRunning())
        return 0.5;

    return 1;
}

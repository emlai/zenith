#include "action.h"
#include "creature.h"
#include "engine/assert.h"

const Action movementActions[8] = { MoveUp, MoveUpLeft, MoveLeft, MoveDownLeft, MoveDown, MoveDownRight, MoveRight, MoveUpRight };

static bool isMovementAction(Action action)
{
    switch (action)
    {
        case MoveUp:
        case MoveUpLeft:
        case MoveLeft:
        case MoveDownLeft:
        case MoveDown:
        case MoveDownRight:
        case MoveRight:
        case MoveUpRight:
        case GoUpOrDown:
            return true;

        default:
            return false;
    }
}

Action getMovementAction(Dir8 direction)
{
    switch (direction)
    {
        case North: return MoveUp;
        case NorthWest: return MoveUpLeft;
        case West: return MoveLeft;
        case SouthWest: return MoveDownLeft;
        case South: return MoveDown;
        case SouthEast: return MoveDownRight;
        case East: return MoveRight;
        case NorthEast: return MoveUpRight;
        default: ASSERT(false);
    }
}

Dir8 getMovementDirection(Action action)
{
    switch (action)
    {
        case MoveUp: return North;
        case MoveUpLeft: return NorthWest;
        case MoveLeft: return West;
        case MoveDownLeft: return SouthWest;
        case MoveDown: return South;
        case MoveDownRight: return SouthEast;
        case MoveRight: return East;
        case MoveUpRight: return NorthEast;
        default: ASSERT(false);
    }
}

const char* toString(Action action)
{
    switch (action)
    {
        case Wait: return "Wait";
        case MoveUp: return "Up";
        case MoveUpLeft: return "UpLeft";
        case MoveLeft: return "Left";
        case MoveDownLeft: return "DownLeft";
        case MoveDown: return "Down";
        case MoveDownRight: return "DownRight";
        case MoveRight: return "Right";
        case MoveUpRight: return "UpRight";
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

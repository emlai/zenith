enum Action : int
{
    NoAction,
    Wait,
    Move,
    GoUpOrDown,
    Attack,
    PickUpItems,
    DropItem,
    UseItem,
    EatItem,
    Close,
    EnterLookMode,
    OpenInventory,
    ShowEquipmentMenu,
    ToggleRunning,
    LastAction
}

const char* toString(Action action);
double getAPCost(Action action, const Creature& actor);
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
    }

    assert(false);
}

double getAPCost(Action action, const Creature& actor)
{
    assert(action != NoAction);

    if (isMovementAction(action) && actor.isRunning())
        return 0.5;

    return 1;
}

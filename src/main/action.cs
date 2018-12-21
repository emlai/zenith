enum Action
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

static class ActionExtensions
{
    static bool isMovementAction(this Action action)
    {
        return action == Move || action == GoUpOrDown;
    }

    static string toString(this Action action)
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

    static double getAPCost(this Action action, Creature actor)
    {
        assert(action != NoAction);

        if (isMovementAction(action) && actor.isRunning())
            return 0.5;

        return 1;
    }
}

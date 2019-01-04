#pragma once

class Creature;
enum Dir8 : int;

enum Action : int
{
    NoAction,
    Wait,
    MoveUp,
    MoveUpLeft,
    MoveLeft,
    MoveDownLeft,
    MoveDown,
    MoveDownRight,
    MoveRight,
    MoveUpRight,
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
};

extern const Action movementActions[8];
Action getMovementAction(Dir8 direction);
Dir8 getMovementDirection(Action action);
const char* toString(Action action);
double getAPCost(Action action, const Creature& actor);

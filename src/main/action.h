#pragma once

class Creature;

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
};

const char* toString(Action action);
double getAPCost(Action action, const Creature& actor);

#pragma once

class Creature;

enum Action : int
{
    NoAction,
    Wait,
    Move,
    Attack,
    PickUpItems,
    DropItem,
    UseItem,
    EatItem,
    Close
};

double getAPCost(Action action, const Creature& actor);

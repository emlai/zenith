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
    Close
};

double getAPCost(Action action, const Creature& actor);

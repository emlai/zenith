#pragma once

#include "../component.h"

class Dig : public Component
{
    bool isUsable() const override { return true; }
    bool use(Creature& digger, Item& digItem, Game& game) override;
};

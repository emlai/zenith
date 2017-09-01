#pragma once

#include "../component.h"

class Dig : public Component
{
    bool isUsable() const override { return true; }
    bool use(Creature& digger, Item& digItem, Game& game) override;
    void save(SaveFile&) const override {}
    void load(const SaveFile&) override {}
};

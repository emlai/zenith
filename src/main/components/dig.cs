#include "../component.h"

class Dig : public Component
{
    bool isUsable() const override { return true; }
    bool use(Creature& digger, Item& digItem, Game& game) override;
    void save(SaveFile&) const override {}
    void load(const SaveFile&) override {}
};
#include "dig.h"
#include "../game.h"
#include "../tile.h"

bool Dig::use(Creature& digger, Item& digItem, Game& game)
{
    if (auto direction = game.askForDirection("Where do you want to dig?"))
    {
        auto* tileToDig = digger.getTileUnder(0).getAdjacentTile(*direction);

        if (tileToDig->hasObject())
        {
            digger.addMessage("You dig the ", tileToDig->getObject()->getName(), ".");
            tileToDig->setObject(nullptr);
        }
        else if (tileToDig->hasCreature())
        {
            digger.addMessage("You can't dig the ", tileToDig->getCreature(0).getName(), ".");
            return false;
        }
        else
            digger.addMessage("You swing the ", digItem.getName(), " at the air.");

        return true;
    }

    return false;
}
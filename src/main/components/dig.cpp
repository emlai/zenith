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
            digger.addMessage("You can't dig the ", tileToDig->getCreature()->getName(), ".");
            return false;
        }
        else
            digger.addMessage("You swing the ", digItem.getName(), " at the air.");

        return true;
    }

    return false;
}

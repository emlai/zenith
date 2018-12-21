class Dig : public Component
{
    bool isUsable() override { return true; }
    bool use(Creature digger, Item digItem, Game game) override;
    void save(SaveFile) override {}
    void load(SaveFile) override {}
}
bool Dig::use(Creature digger, Item digItem, Game game)
{
    if (var direction = game.askForDirection("Where do you want to dig?"))
    {
        var tileToDig = digger.getTileUnder(0).getAdjacentTile(*direction);

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

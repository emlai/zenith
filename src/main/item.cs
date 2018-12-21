enum EquipmentSlot : int;

class Item : Entity
{
    string materialId;
    Sprite sprite;

    virtual void exist() {}
    Sprite getSprite() { return sprite; }

    Item(string id, string materialId)
    :   Item(id, materialId,
             ::getSprite(Game::itemSpriteSheet, Game::itemConfig, id, 0, getMaterialColor(materialId)))
    {
    }

    Item(string id, string materialId, Sprite sprite)
    :   Entity(id, Game::itemConfig),
        materialId(materialId),
        sprite(sprite)
    {
    }

    Item load(SaveFile file)
    {
        var itemId = file.readString();
        Item item;

        if (boost::algorithm::ends_with(itemId, "Corpse"))
        {
            if (file.readBool())
                item = new Corpse(new Creature(file, null));
            else
                item = new Corpse(boost::algorithm::erase_last_copy(itemId, "Corpse"));
        }
        else
        {
            var materialId = file.readString();
            item = new Item(itemId, materialId);
            item.sprite.setMaterialColor(Color32(file.readUint32()));
        }

        for (var component : item.getComponents())
            component.load(file);

        return item;
    }

    void save(SaveFile file)
    {
        file.write(getId());
        file.write(materialId);
        file.writeInt32(sprite.getMaterialColor().value);
        for (var component : getComponents())
            component.save(file);
    }

    bool isUsable()
    {
        for (var component : getComponents())
            if (component.isUsable())
                return true;

        return false;
    }

    bool use(Creature user, Game game)
    {
        assert(isUsable());
        bool returnValue = false;

        for (var component : getComponents())
            if (component.use(user, this, game))
                returnValue = true;

        return returnValue;
    }

    bool isEdible()
    {
        return Game::itemConfig.getOptional<bool>(getId(), "isEdible").get_value_or(false);
    }

    EquipmentSlot getEquipmentSlot()
    {
        var slotString = getConfig().getOptional<string>(getId(), "EquipmentSlot").get_value_or("Hand");

        if (slotString == "Head") return Head;
        if (slotString == "Torso") return Torso;
        if (slotString == "Hand") return Hand;
        if (slotString == "Legs") return Legs;

        std::cerr << "'" << getId() << "' has unknown EquipmentSlot '" << slotString << "'\n";
        assert(false);
        return Hand;
    }

    string getNameAdjective()
    {
        return pascalCaseToSentenceCase(materialId);
    }

    void render(Window window, Vector2 position)
    {
        sprite.render(window, position);
    }

    void renderEquipped(Window window, Vector2 position)
    {
        Vector2 equippedSourceOffset(0, Tile::getSize().y);
        sprite.render(window, position, equippedSourceOffset);
    }

    static Color16 getMaterialColor(string materialId)
    {
        if (!materialId.empty())
        {
            try
            {
                return Color16(static_cast<ushort>(Game::materialConfig.get<int>(materialId, "Color")));
            }
            catch (std::runtime_error)
            {
                if (Game::materialConfig.get<string>(materialId, "Color") == "Random")
                    return Color16(randInt(Color16::max / 2), randInt(Color16::max / 2), randInt(Color16::max / 2));
                else
                    throw;
            }
        }
        else
            return Color16::none;
    }

    string getRandomMaterialId(string itemId)
    {
        var materials = Game::itemConfig.get<List<string>>(itemId, "PossibleMaterials");
        return materials.empty() ? "" : randomElement(materials);
    }
}

class Corpse : Item
{
    Creature creature;
    const int corpseFrame = 2;
    const char corpseGlyph = ',';

    Corpse(Creature creature)
    :   Item(creature.getId() + "Corpse", "", ::getSprite(Game::creatureSpriteSheet, Game::creatureConfig,
        creature.getId(), corpseFrame, Color32::none)),
    creature(creature)
    {
        sprite.setAsciiGlyph(corpseGlyph);
    }

    Corpse(string creatureId)
    :   Item(creatureId + "Corpse", "", ::getSprite(Game::creatureSpriteSheet, Game::creatureConfig,
        creatureId, corpseFrame, Color32::none))
    {
        sprite.setAsciiGlyph(corpseGlyph);
    }

    void exist()
    {
        if (creature)
            creature.exist();
    }

    void renderEquipped(Window window, Vector2 position)
    {
        render(window, position);
    }

    void save(SaveFile file)
    {
        file.write(getId());
        file.write(creature != null);
        if (creature)
            creature.save(file);
    }
}

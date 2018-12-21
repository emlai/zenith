enum EquipmentSlot : int;

class Item : public Entity
{
public:
    Item(string id, string materialId);
    static std::unique_ptr<Item> load(SaveFile file);
    virtual void save(SaveFile file) const;
    virtual void exist() {}
    bool isUsable() const;
    bool use(Creature user, Game game);
    bool isEdible() const;
    EquipmentSlot getEquipmentSlot() const;
    virtual string getNameAdjective() const override;
    void render(Window window, Vector2 position) const;
    virtual void renderEquipped(Window window, Vector2 position) const;
    Sprite getSprite() const { return sprite; }

protected:
    Item(string id, string materialId, Sprite sprite);

    string materialId;
    Sprite sprite;
}

string getRandomMaterialId(string itemId);

class Corpse final : public Item
{
public:
    Corpse(std::unique_ptr<Creature> creature);
    Corpse(string creatureId);
    void exist() override;
    void renderEquipped(Window window, Vector2 position) const override;
    void save(SaveFile file) const override;

private:
    static const int corpseFrame = 2;
    static const char corpseGlyph = ',';

    std::unique_ptr<Creature> creature;
}
static Color16 getMaterialColor(string materialId)
{
    if (!materialId.empty())
    {
        try
        {
            return Color16(static_cast<uint16_t>(Game::materialConfig->get<int>(materialId, "Color")));
        }
        catch (std::runtime_error)
        {
            if (Game::materialConfig->get<string>(materialId, "Color") == "Random")
                return Color16(randInt(Color16::max / 2), randInt(Color16::max / 2), randInt(Color16::max / 2));
            else
                throw;
        }
    }
    else
        return Color16::none;
}

Item::Item(string id, string materialId)
:   Item(id, materialId,
         ::getSprite(*Game::itemSpriteSheet, *Game::itemConfig, id, 0, getMaterialColor(materialId)))
{
}

Item::Item(string id, string materialId, Sprite sprite)
:   Entity(id, *Game::itemConfig),
    materialId(materialId.to_string()),
    sprite(std::move(sprite))
{
}

std::unique_ptr<Item> Item::load(SaveFile file)
{
    var itemId = file.readString();
    std::unique_ptr<Item> item;

    if (boost::algorithm::ends_with(itemId, "Corpse"))
    {
        if (file.readBool())
            item = std::make_unique<Corpse>(std::make_unique<Creature>(file, nullptr));
        else
            item = std::make_unique<Corpse>(boost::algorithm::erase_last_copy(itemId, "Corpse"));
    }
    else
    {
        var materialId = file.readString();
        item = std::make_unique<Item>(itemId, materialId);
        item->sprite.setMaterialColor(Color32(file.readUint32()));
    }

    for (var component : item->getComponents())
        component->load(file);

    return item;
}

void Item::save(SaveFile file) const
{
    file.write(getId());
    file.write(materialId);
    file.writeInt32(sprite.getMaterialColor().value);
    for (var component : getComponents())
        component->save(file);
}

bool Item::isUsable() const
{
    for (var component : getComponents())
        if (component->isUsable())
            return true;

    return false;
}

bool Item::use(Creature user, Game game)
{
    assert(isUsable());
    bool returnValue = false;

    for (var component : getComponents())
        if (component->use(user, *this, game))
            returnValue = true;

    return returnValue;
}

bool Item::isEdible() const
{
    return Game::itemConfig->getOptional<bool>(getId(), "isEdible").get_value_or(false);
}

EquipmentSlot Item::getEquipmentSlot() const
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

string Item::getNameAdjective() const
{
    return pascalCaseToSentenceCase(materialId);
}

void Item::render(Window window, Vector2 position) const
{
    sprite.render(window, position);
}

void Item::renderEquipped(Window window, Vector2 position) const
{
    Vector2 equippedSourceOffset(0, Tile::getSize().y);
    sprite.render(window, position, equippedSourceOffset);
}

string getRandomMaterialId(string itemId)
{
    var materials = Game::itemConfig->get<List<string>>(itemId, "PossibleMaterials");
    return materials.empty() ? "" : randomElement(materials);
}

Corpse::Corpse(std::unique_ptr<Creature> creature)
:   Item(creature->getId() + "Corpse", "", ::getSprite(*Game::creatureSpriteSheet, *Game::creatureConfig,
                                                       creature->getId(), corpseFrame, Color32::none)),
    creature(std::move(creature))
{
    sprite.setAsciiGlyph(corpseGlyph);
}

Corpse::Corpse(string creatureId)
:   Item(creatureId + "Corpse", "", ::getSprite(*Game::creatureSpriteSheet, *Game::creatureConfig,
                                                creatureId, corpseFrame, Color32::none))
{
    sprite.setAsciiGlyph(corpseGlyph);
}

void Corpse::exist()
{
    if (creature)
        creature->exist();
}

void Corpse::renderEquipped(Window window, Vector2 position) const
{
    render(window, position);
}

void Corpse::save(SaveFile file) const
{
    file.write(getId());
    file.write(creature != nullptr);
    if (creature)
        creature->save(file);
}

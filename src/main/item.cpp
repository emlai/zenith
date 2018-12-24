#include "item.h"
#include "creature.h"
#include "game.h"
#include "gui.h"
#include "tile.h"
#include "engine/savefile.h"
#include <iostream>

static Color getMaterialColor(std::string_view materialId)
{
    if (!materialId.empty())
    {
        try
        {
            return Color(Game::materialConfig->get<int>(materialId, "Color"));
        }
        catch (const std::runtime_error&)
        {
            if (Game::materialConfig->get<std::string>(materialId, "Color") == "Random")
                return Color(randInt(0xff / 2), randInt(0xff / 2), randInt(0xff / 2));
            else
                throw;
        }
    }
    else
        return Color::none;
}

Item::Item(std::string_view id, std::string_view materialId)
:   Item(id, materialId,
         ::getSprite(*Game::itemSpriteSheet, *Game::itemConfig, id, 0, getMaterialColor(materialId)))
{
}

Item::Item(std::string_view id, std::string_view materialId, Sprite sprite)
:   Entity(id, *Game::itemConfig),
    materialId(materialId),
    sprite(std::move(sprite))
{
}

std::unique_ptr<Item> Item::load(const SaveFile& file)
{
    auto itemId = file.readString();
    std::unique_ptr<Item> item;
    std::string_view corpseSuffix = "Corpse";

    if (endsWith(itemId, corpseSuffix))
    {
        if (file.readBool())
            item = std::make_unique<Corpse>(std::make_unique<Creature>(nullptr, file));
        else
            item = std::make_unique<Corpse>(removeSuffix(itemId, corpseSuffix));
    }
    else
    {
        auto materialId = file.readString();
        item = std::make_unique<Item>(itemId, materialId);
        item->sprite.setMaterialColor(Color(file.readUint32()));
    }

    for (auto& component : item->getComponents())
        component->load(file);

    return item;
}

void Item::save(SaveFile& file) const
{
    file.write(getId());
    file.write(materialId);
    file.writeInt32(sprite.getMaterialColor().intValue());
    for (auto& component : getComponents())
        component->save(file);
}

bool Item::isUsable() const
{
    for (auto& component : getComponents())
        if (component->isUsable())
            return true;

    return false;
}

bool Item::use(Creature& user, Game& game)
{
    assert(isUsable());
    bool returnValue = false;

    for (auto& component : getComponents())
        if (component->use(user, *this, game))
            returnValue = true;

    return returnValue;
}

bool Item::isEdible() const
{
    return Game::itemConfig->getOptional<bool>(getId(), "isEdible").value_or(false);
}

EquipmentSlot Item::getEquipmentSlot() const
{
    auto slotString = getConfig().getOptional<std::string>(getId(), "EquipmentSlot").value_or("Hand");

    if (slotString == "Head") return Head;
    if (slotString == "Torso") return Torso;
    if (slotString == "Hand") return Hand;
    if (slotString == "Legs") return Legs;

    std::cerr << "'" << getId() << "' has unknown EquipmentSlot '" << slotString << "'\n";
    assert(false);
    return Hand;
}

std::string Item::getNameAdjective() const
{
    return pascalCaseToSentenceCase(materialId);
}

void Item::render(Window& window, Vector2 position) const
{
    sprite.render(window, position);
}

void Item::renderEquipped(Window& window, Vector2 position) const
{
    Vector2 equippedSourceOffset(0, Tile::getSize().y);
    sprite.render(window, position, equippedSourceOffset);
}

std::string getRandomMaterialId(std::string_view itemId)
{
    auto materials = Game::itemConfig->get<std::vector<std::string>>(itemId, "PossibleMaterials");
    return materials.empty() ? "" : randomElement(materials);
}

Corpse::Corpse(std::unique_ptr<Creature> creature)
:   Item(creature->getId() + "Corpse", "", ::getSprite(*Game::creatureSpriteSheet, *Game::creatureConfig,
                                                       creature->getId(), corpseFrame, Color::none)),
    creature(std::move(creature))
{
}

Corpse::Corpse(std::string_view creatureId)
:   Item(creatureId + "Corpse", "", ::getSprite(*Game::creatureSpriteSheet, *Game::creatureConfig,
                                                creatureId, corpseFrame, Color::none))
{
}

void Corpse::exist()
{
    if (creature)
        creature->exist();
}

void Corpse::renderEquipped(Window& window, Vector2 position) const
{
    render(window, position);
}

void Corpse::save(SaveFile& file) const
{
    file.write(getId());
    file.write(creature != nullptr);
    if (creature)
        creature->save(file);
}

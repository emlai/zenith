#include "creature.h"
#include "action.h"
#include "controller.h"
#include "game.h"
#include "msgsystem.h"
#include "tile.h"
#include "engine/savefile.h"
#include <cassert>
#include <cctype>
#include <iomanip>
#include <sstream>

boost::string_ref toString(EquipmentSlot slot)
{
    switch (slot)
    {
        case Head: return "head";
        case Torso: return "torso";
        case Hand: return "hand";
        case Legs: return "legs";
    }

    assert(false);
}

std::vector<Attribute> Creature::initDisplayedAttributes(boost::string_ref id)
{
    std::vector<Attribute> displayedAttributes;

    for (const auto& attribute : Game::creatureConfig->get<std::vector<std::string>>(id, "DisplayedAttributes"))
        displayedAttributes.push_back(stringToAttribute(attribute));

    return displayedAttributes;
}

std::vector<std::vector<int>> Creature::initAttributeIndices(boost::string_ref id)
{
    return Game::creatureConfig->get<std::vector<std::vector<int>>>(id, "AttributeIndices");
}

Creature::Creature(Tile* tile, boost::string_ref id)
:   Creature(tile, id, AIController::get(id, *this))
{
}

Creature::Creature(Tile* tile, boost::string_ref id, std::unique_ptr<Controller> controller)
:   Entity(id, *Game::creatureConfig),
    currentHP(0),
    maxHP(0),
    currentAP(0),
    currentMP(0),
    maxMP(0),
    running(false),
    displayedAttributes(initDisplayedAttributes(id)),
    attributeIndices(initAttributeIndices(id)),
    sprite(getSprite(*Game::creatureSpriteSheet, *Game::creatureConfig, id)),
    controller(std::move(controller))
{
    equipment[Head] = nullptr;
    equipment[Torso] = nullptr;
    equipment[Hand] = nullptr;
    equipment[Legs] = nullptr;

    if (tile)
        tilesUnder.push_back(tile);

    generateAttributes(id);

    if (auto initialEquipment = getConfig().getOptional<std::vector<std::string>>(getId(), "Equipment"))
    {
        for (auto& itemId : *initialEquipment)
        {
            inventory.push_back(make_unique<Item>(itemId, getRandomMaterialId(itemId)));
            equip(inventory.back()->getEquipmentSlot(), &*inventory.back());
        }
    }
}

Creature::Creature(const SaveFile& file, Tile* tile)
:   Entity(file.readString(), *Game::creatureConfig),
    displayedAttributes(initDisplayedAttributes(getId())),
    attributeIndices(initAttributeIndices(getId())),
    sprite(getSprite(*Game::creatureSpriteSheet, *Game::creatureConfig, getId())),
    controller(AIController::get(getId(), *this))
{
    equipment[Head] = nullptr;
    equipment[Torso] = nullptr;
    equipment[Hand] = nullptr;
    equipment[Legs] = nullptr;

    if (tile)
        tilesUnder.push_back(tile);

    for (auto& component : getComponents())
        component->load(file);

    auto seenTilePositionsCount = file.readInt32();
    seenTilePositions.reserve(size_t(seenTilePositionsCount));
    for (int i = 0; i < seenTilePositionsCount; ++i)
        seenTilePositions.insert(file.readVector3());

    auto inventorySize = file.readInt32();
    inventory.reserve(size_t(inventorySize));
    for (int i = 0; i < inventorySize; ++i)
        inventory.push_back(Item::load(file));

    for (auto& slotAndItem : equipment)
    {
        auto itemIndex = file.readInt16();

        if (itemIndex != -1)
            slotAndItem.second = &*inventory[size_t(itemIndex)];
    }

    file.read(attributeValues);
    calculateDerivedStats();
    currentHP = file.readDouble();
    currentAP = file.readDouble();
    currentMP = file.readDouble();
    running = file.readBool();
    file.read(messages);
}

void Creature::save(SaveFile& file) const
{
    file.write(getId());
    for (auto& component : getComponents())
        component->save(file);

    file.writeInt32(uint32_t(seenTilePositions.size()));
    for (auto tilePosition : seenTilePositions)
        file.write(tilePosition);

    file.write(inventory);

    for (auto slotAndItem : equipment)
        file.writeInt16(int16_t(slotAndItem.second ? getInventoryIndex(*slotAndItem.second) : -1));

    file.write(attributeValues);
    file.write(currentHP);
    file.write(currentAP);
    file.write(currentMP);
    file.write(running);
    file.write(messages);
}

void Creature::exist()
{
    if (!isDead())
    {
        bleed();
        regenerate();
    }

    while (currentAP >= fullAP || isDead())
    {
        Action action = controller->control(*this);

        if (!action || isDead())
            break;

        currentAP -= getAPCost(action, *this);
    }
}

void Creature::regenerate()
{
    editHP(0.1);
    editAP(1);
    editMP(0.1);
}

void Creature::render(Window& window, Vector2 position) const
{
    sprite.render(window, position);

    if (!Sprite::useAsciiGraphics())
    {
        for (int slot = equipmentSlots - 1; slot >= 0; --slot)
        {
            if (auto* equipment = getEquipment(static_cast<EquipmentSlot>(slot)))
                equipment->renderEquipped(window, position);
        }
    }
}

void Creature::generateAttributes(boost::string_ref id)
{
    attributeValues.resize(Game::creatureConfig->get<int>(id, "Attributes"));

    auto attributeStrings = Game::creatureConfig->get<std::vector<std::string>>(id, "ConfigAttributes");
    auto configAttributes = stringsToAttributes(attributeStrings);

    for (auto attribute : configAttributes)
    {
        boost::string_ref attributeName = attributeAbbreviations[attribute];
        int baseAttributeValue = Game::creatureConfig->get<int>(id, attributeName);
        setAttribute(attribute, baseAttributeValue + randNormal(2));
    }

    calculateDerivedStats();

    currentHP = maxHP;
    currentMP = maxMP;
}

void Creature::calculateDerivedStats()
{
    double hpRatio = currentHP / maxHP;
    double mpRatio = currentMP / maxMP;

    maxHP = 2 * getAttribute(Endurance) + getAttribute(Strength) / 2;
    maxMP = 2 * getAttribute(Psyche) + getAttribute(Intelligence) / 2;

    currentHP = hpRatio * maxHP;
    currentMP = mpRatio * maxMP;
}

double Creature::getAttribute(Attribute attribute) const
{
    double sum = 0;

    for (auto index : getAttributeIndices(attribute))
        sum += attributeValues[index];

    return sum / getAttributeIndices(attribute).size();
}

void Creature::setAttribute(Attribute attribute, double amount)
{
    for (auto index : getAttributeIndices(attribute))
        attributeValues[index] = amount;
}

void Creature::editAttribute(Attribute attribute, double amount)
{
    for (auto index : getAttributeIndices(attribute))
        attributeValues[index] += amount;
}

int Creature::getFieldOfVisionRadius() const
{
    return int(getAttribute(Perception) * 2);
}

bool Creature::sees(const Tile& tile) const
{
    assert(tile.getLevel() == getLevel());

    if (getDistance(getPosition(), tile.getPosition()) > getFieldOfVisionRadius())
        return false;

    return raycastIntegerBresenham(getPosition(), tile.getPosition(), [&](Vector2 vector)
    {
        auto* currentTile = getWorld().getTile(vector, getLevel());

        if (!currentTile)
            return false;

        if (currentTile != &tile && currentTile->blocksSight())
            return false;

        if (currentTile->getLight().getLuminance() < 0.3)
            return false;

        seenTilePositions.emplace(currentTile->getPosition3D());
        return true;
    });
}

bool Creature::remembers(const Tile& tile) const
{
    return seenTilePositions.find(tile.getPosition3D()) != seenTilePositions.end();
}

std::vector<Creature*> Creature::getCreaturesCurrentlySeenBy(int maxFieldOfVisionRadius) const
{
    std::vector<Creature*> creatures;

    for (int x = -maxFieldOfVisionRadius; x <= maxFieldOfVisionRadius; ++x)
    {
        for (int y = -maxFieldOfVisionRadius; y <= maxFieldOfVisionRadius; ++y)
        {
            auto* tile = getWorld().getTile(getPosition() + Vector2(x, y), getLevel());

            if (!tile)
                continue;

            for (auto& creature : tile->getCreatures())
            {
                if (creature.get() != this && creature->sees(getTileUnder(0)))
                    creatures.push_back(creature.get());
            }
        }
    }

    return creatures;
}

std::vector<Creature*> Creature::getCurrentlySeenCreatures() const
{
    std::vector<Creature*> currentlySeenCreatures;
    auto fieldOfVisionRadius = getFieldOfVisionRadius();

    for (int x = -fieldOfVisionRadius; x <= fieldOfVisionRadius; ++x)
    {
        for (int y = -fieldOfVisionRadius; y <= fieldOfVisionRadius; ++y)
        {
            auto* tile = getWorld().getTile(getPosition() + Vector2(x, y), getLevel());

            if (!tile || !sees(*tile))
                continue;

            for (auto& creature : tile->getCreatures())
            {
                if (creature.get() != this)
                    currentlySeenCreatures.push_back(creature.get());
            }
        }
    }

    return currentlySeenCreatures;
}

Creature* Creature::getNearestEnemy() const
{
    // TODO: Optimize by iterating in a spiral starting from this creature's position.

    Creature* nearestEnemy = nullptr;
    int nearestEnemyDistance = INT_MAX;

    for (auto* other : getCurrentlySeenCreatures())
    {
        if (other->getId() == getId())
            continue;

        int enemyDistance = getDistanceSquared(getPosition(), other->getPosition());

        if (enemyDistance < nearestEnemyDistance)
        {
            nearestEnemy = other;
            nearestEnemyDistance = enemyDistance;
        }
    }

    return nearestEnemy;
}

Action Creature::tryToMoveOrAttack(Dir8 direction)
{
    Tile* destination = getTileUnder(0).getAdjacentTile(direction);

    if (!destination)
        return NoAction;

    if (!destination->getCreatures().empty())
    {
        attack(destination->getCreature(0));
        return Attack;
    }

    if (destination->hasObject())
    {
        bool preventsMovement = destination->getObject()->preventsMovement();
        bool didReactToMovementAttempt = destination->getObject()->reactToMovementAttempt();

        if (preventsMovement)
            return didReactToMovementAttempt ? Wait : NoAction;
    }

    moveTo(*destination);
    return Move;
}

Action Creature::tryToMoveTowardsOrAttack(Creature& target)
{
    auto directionVector = target.getPosition() - getPosition();
    return tryToMoveOrAttack(directionVector.getDir8());
}

void Creature::moveTo(Tile& destination)
{
    getTileUnder(0).transferCreature(*this, destination);
    tilesUnder.clear();
    tilesUnder.push_back(&destination);

    Item* itemOnTile = nullptr;

    for (auto* tile : getTilesUnder())
    {
        if (tile->hasItems())
        {
            if (itemOnTile || tile->getItems().size() > 1)
            {
                addMessage("Many items are lying here.");
                itemOnTile = nullptr;
                break;
            }

            itemOnTile = tile->getItems()[0].get();
        }
    }

    if (itemOnTile)
        addMessage(itemOnTile->getNameIndefinite(), " is lying here.");
}

bool Creature::enter()
{
    for (Tile* tile : getTilesUnder())
    {
        if (!tile->hasObject())
            continue;

        if (tile->getObject()->getId() == "StairsDown")
        {
            moveTo(*tile->getTileBelow());
            return true;
        }

        if (tile->getObject()->getId() == "StairsUp")
        {
            moveTo(*tile->getTileAbove());
            return true;
        }
    }

    return false;
}

void Creature::attack(Creature& target)
{
    double damage = std::max(0.0, getAttribute(ArmStrength) / 2 + randNormal());

    std::ostringstream attackerMessage, targetMessage;

    attackerMessage << "You hit the " << target.getName();
    targetMessage << "The " << getName() << " hits you";

    if (auto* weapon = getEquipment(Hand))
    {
        attackerMessage << " with the " << weapon->getName();
        targetMessage << " with the " << weapon->getName();
    }

    attackerMessage << ".";
    targetMessage << ".";

#ifdef DEBUG
    bool showDamageNumbers = false;
    if (showDamageNumbers)
    {
        attackerMessage << std::fixed << std::setprecision(1) << " (" << damage << ")";
        targetMessage << std::fixed << std::setprecision(1) << " (" << damage << ")";
    }
#endif

    addMessage(attackerMessage.str());
    target.addMessage(targetMessage.str());

    target.takeDamage(damage);
}

void Creature::takeDamage(double amount)
{
    assert(!isDead());

    if (amount > 0)
    {
        currentHP -= amount;

        if (isDead())
            onDeath();
    }
}

void Creature::onDeath()
{
    addMessage("You die.");

    for (auto* observer : getCreaturesCurrentlySeenBy(20))
        observer->addMessage("The ", getName(), " dies.");

    if (getTilesUnder().size() == 1)
    {
        std::unique_ptr<Creature> self = getTileUnder(0).removeSingleTileCreature(*this);
        getTileUnder(0).addItem(make_unique<Corpse>(std::move(self)));
    }
    else
    {
        // TODO: Implement multi-tile creature corpses.
        for (auto* tile : getTilesUnder())
            tile->removeCreature(*this);
    }
}

void Creature::bleed()
{
    auto chance = 1.0 - std::max(0.0, currentHP / maxHP);

    while (true)
    {
        chance -= randFloat();

        if (chance <= 0)
            break;

        for (auto* tile : getTilesUnder())
            tile->addLiquid("Blood");
    }
}

bool Creature::pickUpItem()
{
    for (auto* tile : tilesUnder)
    {
        if (tile->hasItems())
        {
            inventory.push_back(tile->removeTopmostItem());
            return true;
        }
    }

    return false;
}

void Creature::equip(EquipmentSlot slot, Item* itemToEquip)
{
    equipment.at(slot) = itemToEquip;
}

bool Creature::use(Item& itemToUse, Game& game)
{
    assert(itemToUse.isUsable());
    return itemToUse.use(*this, game);
}

void Creature::drop(Item& itemToDrop)
{
    EquipmentSlot equipmentSlot = itemToDrop.getEquipmentSlot();

    if (getEquipment(equipmentSlot) == &itemToDrop)
        equip(equipmentSlot, nullptr);

    getTileUnder(0).addItem(removeItem(itemToDrop));
}

bool Creature::eat(Item& itemToEat)
{
    assert(itemToEat.isEdible());

    if (auto leftoverItemId = Game::itemConfig->getOptional<std::string>(itemToEat.getId(), "leftoverItem"))
        getTileUnder(0).addItem(make_unique<Item>(*leftoverItemId, ""));

    addMessage("You eat the ", itemToEat.getName(), ".");
    removeItem(itemToEat);
    return true;
}

std::unique_ptr<Item> Creature::removeItem(Item& itemToRemove)
{
    auto index = getInventoryIndex(itemToRemove);
    auto removedItem = std::move(inventory[index]);
    inventory.erase(inventory.begin() + index);
    return removedItem;
}

int Creature::getInventoryIndex(const Item& item) const
{
    for (int i = 0; i < int(inventory.size()); ++i)
        if (&*inventory[i] == &item)
            return i;

    assert(false);
}

bool Creature::close(Dir8 direction)
{
    Tile* destination = getTileUnder(0).getAdjacentTile(direction);
    return destination && destination->hasObject() && destination->getObject()->close();
}

Vector2 Creature::getPosition() const
{
    return getTileUnder(0).getPosition();
}

int Creature::getLevel() const
{
    return getTileUnder(0).getLevel();
}

World& Creature::getWorld() const
{
    return getTileUnder(0).getWorld();
}

int Creature::getTurn() const
{
    return getWorld().getTurn();
}

void Creature::setController(std::unique_ptr<Controller> controller)
{
    this->controller = std::move(controller);
}

Attribute stringToAttribute(boost::string_ref string)
{
    auto it = std::find(std::begin(attributeAbbreviations), std::end(attributeAbbreviations), string);

    if (it != std::end(attributeAbbreviations))
        return static_cast<Attribute>(it - std::begin(attributeAbbreviations));

    throw std::invalid_argument("string didn't match any attribute.");
}

std::vector<Attribute> stringsToAttributes(const std::vector<std::string>& strings)
{
    std::vector<Attribute> attributes;

    for (const auto& string : strings)
        attributes.push_back(stringToAttribute(string));

    return attributes;
}

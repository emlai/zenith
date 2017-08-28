#include "creature.h"
#include "game.h"
#include "msgsystem.h"
#include "tile.h"

std::vector<Attribute> Creature::initDisplayedAttributes(boost::string_ref id)
{
    std::vector<Attribute> displayedAttributes;

    for (const auto& attribute : Game::creatureConfig.get<std::vector<std::string>>(id, "DisplayedAttributes"))
        displayedAttributes.push_back(stringToAttribute(attribute));

    return displayedAttributes;
}

std::vector<std::vector<int>> Creature::initAttributeIndices(boost::string_ref id)
{
    return Game::creatureConfig.get<std::vector<std::vector<int>>>(id, "AttributeIndices");
}

Creature::Creature(Tile& tile, boost::string_ref id, std::unique_ptr<CreatureController> controller)
:   Entity(id, Game::creatureConfig),
    tilesUnder({&tile}),
    wieldedItem(nullptr),
    currentHP(0),
    maxHP(0),
    currentAP(0),
    maxAP(0),
    currentMP(0),
    maxMP(0),
    displayedAttributes(initDisplayedAttributes(id)),
    attributeIndices(initAttributeIndices(id)),
    sprite(*Game::creatureSpriteSheet, getSpriteTextureRegion(Game::creatureConfig, id)),
    controller(std::move(controller))
{
    generateAttributes(id);
}

void Creature::exist()
{
    regenerate();
    controller->control(*this);
}

void Creature::regenerate()
{
    editHP(1);
    editAP(1);
    editMP(1);
}

void Creature::render(Window& window) const
{
    sprite.render(window, getPosition() * Tile::size);

    if (wieldedItem)
        wieldedItem->renderWielded(window, getPosition() * Tile::size);
}

void Creature::generateAttributes(boost::string_ref id)
{
    attributes.resize(Game::creatureConfig.get<int>(id, "Attributes"));

    auto attributeStrings = Game::creatureConfig.get<std::vector<std::string>>(id, "ConfigAttributes");
    auto configAttributes = stringsToAttributes(attributeStrings);

    for (auto attribute : configAttributes)
    {
        boost::string_ref attributeName = attributeAbbreviations[attribute];
        int baseAttributeValue = Game::creatureConfig.get<int>(id, attributeName);
        setAttribute(attribute, static_cast<int>(baseAttributeValue + randNormal(2)));
    }

    calculateDerivedStats();

    currentHP = maxHP;
    currentAP = maxAP;
    currentMP = maxMP;
}

void Creature::calculateDerivedStats()
{
    auto hpRatio = double(currentHP) / double(maxHP);
    auto apRatio = double(currentAP) / double(maxAP);
    auto mpRatio = double(currentMP) / double(maxMP);

    maxHP = 2 * getAttribute(Endurance) + getAttribute(Strength) / 2;
    maxAP = 2 * getAttribute(Agility) + getAttribute(Dexterity) / 2;
    maxMP = 2 * getAttribute(Psyche) + getAttribute(Intelligence) / 2;

    currentHP = int(hpRatio * maxHP);
    currentAP = int(apRatio * maxAP);
    currentMP = int(mpRatio * maxMP);
}

int Creature::getAttribute(Attribute attribute) const
{
    int sum = 0;

    for (auto index : getAttributeIndices(attribute))
        sum += attributes[index];

    return sum / getAttributeIndices(attribute).size();
}

void Creature::setAttribute(Attribute attribute, int amount)
{
    for (auto index : getAttributeIndices(attribute))
        attributes[index] = amount;
}

void Creature::editAttribute(Attribute attribute, int amount)
{
    for (auto index : getAttributeIndices(attribute))
        attributes[index] += amount;
}

void Creature::addMessage(std::string&& message)
{
    messages.push_back(std::move(message));
}

bool Creature::sees(const Tile& tile) const
{
    assert(tile.getLevel() == getLevel());

    return raycastIntegerBresenham(getPosition(), tile.getPosition(), [&](Vector2 vector)
    {
        auto* currentTile = getWorld().getTile(vector, getLevel());

        if (!currentTile)
            return false;

        if (currentTile != &tile && currentTile->blocksSight())
            return false;

        seenTiles.insert(currentTile);
        return true;
    });
}

bool Creature::remembers(const Tile& tile) const
{
    return seenTiles.find(&tile) != seenTiles.end();
}

std::vector<Creature*> Creature::getCurrentlySeenCreatures(int fieldOfVisionRadius) const
{
    std::vector<Creature*> currentlySeenCreatures;

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

    int fieldOfVisionRadius = 10;
    Creature* nearestEnemy = nullptr;
    int nearestEnemyDistance = INT_MAX;

    for (auto* other : getCurrentlySeenCreatures(fieldOfVisionRadius))
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

bool Creature::tryToMoveOrAttack(Dir8 direction)
{
    Tile* destination = getTileUnder(0).getAdjacentTile(direction);

    if (!destination)
        return false;

    if (!destination->getCreatures().empty())
    {
        attack(destination->getCreature(0));
        return true;
    }

    if (destination->hasObject())
    {
        bool preventsMovement = destination->getObject()->preventsMovement();
        bool didReactToMovementAttempt = destination->getObject()->reactToMovementAttempt();

        if (preventsMovement)
            return didReactToMovementAttempt;
    }

    moveTo(*destination);
    return true;
}

bool Creature::tryToMoveTowardsOrAttack(Creature& target)
{
    auto directionVector = target.getPosition() - getPosition();
    return tryToMoveOrAttack(directionVector.getDir8());
}

void Creature::moveTo(Tile& destination)
{
    getTileUnder(0).transferCreature(*this, destination);
    tilesUnder.clear();
    tilesUnder.push_back(&destination);
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
    addMessage("You hit the " + target.getName() + ".");
    target.addMessage("The " + getName() + " hits you.");

    target.takeDamage(5);
}

void Creature::takeDamage(int amount)
{
    currentHP -= amount;
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

void Creature::wield(Item* itemToWield)
{
    wieldedItem = itemToWield;
}

void Creature::drop(Item& itemToDrop)
{
    for (auto it = inventory.begin(), end = inventory.end(); it != end; ++it)
    {
        if (it->get() == &itemToDrop)
        {
            if (wieldedItem == &itemToDrop)
                wield(nullptr);

            getTileUnder(0).addItem(std::move(*it));
            inventory.erase(it);
            return;
        }
    }

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

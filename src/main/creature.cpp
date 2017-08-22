#include "creature.h"
#include "msgsystem.h"
#include "tile.h"

std::vector<Attribute> Creature::initDisplayedAttributes(const Config& config, const std::string& id)
{
    std::vector<Attribute> displayedAttributes;

    for (const auto& attribute : config.get<std::vector<std::string>>(id, "DisplayedAttributes"))
        displayedAttributes.push_back(stringToAttribute(attribute));

    return displayedAttributes;
}

std::vector<std::vector<int>> Creature::initAttributeIndices(const Config& config, const std::string& id)
{
    return config.get<std::vector<std::vector<int>>>(id, "AttributeIndices");
}

Creature::Creature(Tile& tile, const std::string& id, const Config& config)
:   tilesUnder({&tile}),
    currentHP(0),
    maxHP(0),
    currentAP(0),
    maxAP(0),
    currentMP(0),
    maxMP(0),
    displayedAttributes(initDisplayedAttributes(config, id)),
    attributeIndices(initAttributeIndices(config, id))
{
    generateAttributes(config, id);
}

void Creature::exist()
{
    regenerate();
}

void Creature::regenerate()
{
    editHP(1);
    editAP(1);
    editMP(1);
}

void Creature::generateAttributes(const Config& config, const std::string& id)
{
    attributes.resize(config.get<int>(id, "Attributes"));

    const auto& attributeStrings = config.get<std::vector<std::string>>(id, "ConfigAttributes");
    auto configAttributes = stringsToAttributes(attributeStrings);

    for (auto attribute : configAttributes)
    {
        const std::string& attributeName = attributeAbbreviations[attribute];
        setAttribute(attribute, config.get<int>(id, attributeName) + randNormal(2));
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

void Creature::tryToMove(Dir8 direction)
{
    Tile* destination = getTileUnder(0).getAdjacentTile(direction);

    if (destination && destination->getCreatures().empty())
        moveTo(*destination);
}

void Creature::moveTo(Tile& destination)
{
    getTileUnder(0).transferCreature(*this, destination);
    tilesUnder.clear();
    tilesUnder.push_back(&destination);
}

Vector2 Creature::getPosition() const
{
    return getTileUnder(0).getPosition();
}

Attribute stringToAttribute(const std::string& string)
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

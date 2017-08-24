#pragma once

#include "creaturecontroller.h"
#include "entity.h"
#include "engine/geometry.h"
#include "engine/sprite.h"
#include "engine/utility.h"
#include <memory>
#include <vector>
#include <string>

class Item;
class Tile;
class Window;

enum Attribute
{
    Strength,
    ArmStrength,
    LegStrength,
    RightArmStrength,
    LeftArmStrength,
    RightLegStrength,
    LeftLegStrength,
    Dexterity,
    RightArmDexterity,
    LeftArmDexterity,
    Agility,
    RightLegAgility,
    LeftLegAgility,
    Endurance,
    Perception,
    Sight,
    Hearing,
    Smell,
    Intelligence,
    Psyche,
    Charisma
};

const int attributes = 21;

const std::string attributeNames[] =
{
    "strength", "arm strength", "leg strength", "", "", "", "", "dexterity", "", "", "agility",
    "", "", "endurance", "perception", "", "", "", "intelligence", "psyche", "charisma"
};

const std::string attributeAbbreviations[] =
{
    "Str", "AStr", "LStr", "", "", "", "", "Dex", "", "", "Agi",
    "", "", "End", "Per", "", "", "", "Int", "Psy", "Cha"
};

const std::string statNames[] = { "HP", "AP", "MP" };

class Creature final : public Entity
{
public:
    Creature(Tile&, const std::string& id, std::unique_ptr<CreatureController> controller);
    void exist();
    void render(Window&) const;

    /// Returns true if the game should advance to the next turn.
    bool tryToMoveOrAttack(Dir8);
    void takeDamage(int amount);
    bool pickUpItem();

    const auto& getTilesUnder() const { return tilesUnder; }
    Tile& getTileUnder(int index) const { return *tilesUnder[index]; }
    Vector2 getPosition() const;
    bool isDead() const { return currentHP <= 0; }
    int getHP() const { return currentHP; }
    int getAP() const { return currentAP; }
    int getMP() const { return currentMP; }
    int getMaxHP() const { return maxHP; }
    int getMaxAP() const { return maxAP; }
    int getMaxMP() const { return maxMP; }
    int getAttribute(Attribute) const;
    const auto& getDisplayedAttributes() const { return displayedAttributes; }
    void addMessage(std::string&& message);
    const std::vector<std::string>& getMessages() const { return messages; }

private:
    void moveTo(Tile&);
    void attack(Creature&);
    void setAttribute(Attribute, int amount);
    void editAttribute(Attribute, int amount);
    void generateAttributes(const std::string&);
    void calculateDerivedStats();
    void editHP(int amount) { currentHP = std::min(currentHP + amount, maxHP); }
    void editAP(int amount) { currentAP = std::min(currentAP + amount, maxAP); }
    void editMP(int amount) { currentMP = std::min(currentMP + amount, maxMP); }
    void regenerate();
    static std::vector<Attribute> initDisplayedAttributes(const std::string&);
    static std::vector<std::vector<int>> initAttributeIndices(const std::string&);
    const auto& getAttributeIndices(int attribute) const { return attributeIndices[attribute]; }

    std::vector<Tile*> tilesUnder;
    std::vector<std::unique_ptr<Item>> inventory;
    int currentHP, maxHP, currentAP, maxAP, currentMP, maxMP;
    std::vector<int> attributes;
    std::vector<Attribute> displayedAttributes;
    std::vector<std::vector<int>> attributeIndices;
    Sprite sprite;
    std::unique_ptr<CreatureController> controller;
    std::vector<std::string> messages;
    static const int configAttributes[8];
};

Attribute stringToAttribute(const std::string&);
std::vector<Attribute> stringsToAttributes(const std::vector<std::string>&);

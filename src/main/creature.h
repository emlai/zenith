#pragma once

#include "creaturecontroller.h"
#include "entity.h"
#include "engine/geometry.h"
#include "engine/sprite.h"
#include "engine/utility.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_set>

class Item;
class Tile;
class Window;
class World;

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
    Creature(Tile&, boost::string_ref id, std::unique_ptr<CreatureController> controller);
    void exist();
    void render() const;

    /// Returns true if the game should advance to the next turn.
    bool tryToMoveOrAttack(Dir8);
    bool tryToMoveTowardsOrAttack(Creature& target);
    bool enter();
    void takeDamage(int amount);
    bool pickUpItem();
    void wield(Item*);
    bool use(Item&, Game& game);
    void drop(Item&);
    bool close(Dir8);

    const auto& getTilesUnder() const { return tilesUnder; }
    Tile& getTileUnder(int index) const { return *tilesUnder[index]; }
    Vector2 getPosition() const;
    int getLevel() const;
    const auto& getInventory() const { return inventory; }
    bool hasWieldedItem() const { return wieldedItem != nullptr; }
    Item* getWieldedItem() const { return wieldedItem; }
    bool isDead() const { return currentHP <= 0; }
    double getHP() const { return currentHP; }
    double getAP() const { return currentAP; }
    double getMP() const { return currentMP; }
    double getMaxHP() const { return maxHP; }
    double getMaxAP() const { return maxAP; }
    double getMaxMP() const { return maxMP; }
    double getAttribute(Attribute) const;
    const auto& getDisplayedAttributes() const { return displayedAttributes; }
    void addMessage(std::string&& message);
    const std::vector<std::string>& getMessages() const { return messages; }
    bool sees(const Tile& tile) const;
    bool remembers(const Tile& tile) const;
    std::vector<Creature*> getCurrentlySeenCreatures(int fieldOfVisionRadius) const;
    Creature* getNearestEnemy() const;

private:
    World& getWorld() const;
    void moveTo(Tile&);
    void attack(Creature&);
    void setAttribute(Attribute, double amount);
    void editAttribute(Attribute, double amount);
    void generateAttributes(boost::string_ref);
    void calculateDerivedStats();
    void editHP(double amount) { currentHP = std::min(currentHP + amount, maxHP); }
    void editAP(double amount) { currentAP = std::min(currentAP + amount, maxAP); }
    void editMP(double amount) { currentMP = std::min(currentMP + amount, maxMP); }
    void regenerate();
    static std::vector<Attribute> initDisplayedAttributes(boost::string_ref);
    static std::vector<std::vector<int>> initAttributeIndices(boost::string_ref);
    const auto& getAttributeIndices(int attribute) const { return attributeIndices[attribute]; }

    std::vector<Tile*> tilesUnder;
    mutable std::unordered_set<const Tile*> seenTiles;
    std::vector<std::unique_ptr<Item>> inventory;
    Item* wieldedItem;
    double currentHP, maxHP, currentAP, maxAP, currentMP, maxMP;
    std::vector<double> attributeValues;
    std::vector<Attribute> displayedAttributes;
    std::vector<std::vector<int>> attributeIndices;
    Sprite sprite;
    std::unique_ptr<CreatureController> controller;
    std::vector<std::string> messages;
    static const int configAttributes[8];
};

Attribute stringToAttribute(boost::string_ref);
std::vector<Attribute> stringsToAttributes(const std::vector<std::string>&);

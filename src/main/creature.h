#pragma once

#include "creaturecontroller.h"
#include "entity.h"
#include "msgsystem.h"
#include "engine/geometry.h"
#include "engine/sprite.h"
#include "engine/utility.h"
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Item;
class Message;
class SaveFile;
class Tile;
class Window;
class World;
enum Action : int;

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

enum EquipmentSlot : int
{
    Hand,
    Head,
    Torso,
    Legs
};

const int equipmentSlots = 4;

boost::string_ref toString(EquipmentSlot slot);

class Creature final : public Entity
{
public:
    Creature(Tile*, boost::string_ref id, std::unique_ptr<CreatureController> controller);
    Creature(const SaveFile& file, Tile* tile);
    void save(SaveFile& file) const;
    void exist();
    void render(Window& window) const;

    Action tryToMoveOrAttack(Dir8);
    Action tryToMoveTowardsOrAttack(Creature& target);
    bool enter();
    void takeDamage(double amount);
    void bleed();
    bool pickUpItem();
    void equip(EquipmentSlot slot, Item* itemToEquip);
    bool use(Item&, Game& game);
    void drop(Item&);
    bool close(Dir8);

    const auto& getTilesUnder() const { return tilesUnder; }
    Tile& getTileUnder(int index) const { return *tilesUnder[index]; }
    Vector2 getPosition() const;
    int getLevel() const;
    const auto& getInventory() const { return inventory; }
    const auto& getEquipment() const { return equipment; }
    Item* getEquipment(EquipmentSlot slot) const { return equipment.at(slot); }
    int getInventoryIndex(const Item& item) const;
    bool isRunning() const { return running; }
    void setRunning(bool running) { this->running = running; }
    bool isDead() const { return currentHP <= 0; }
    double getHP() const { return currentHP; }
    double getAP() const { return currentAP; }
    double getMP() const { return currentMP; }
    double getMaxHP() const { return maxHP; }
    double getMaxMP() const { return maxMP; }
    double getAttribute(Attribute) const;
    const auto& getDisplayedAttributes() const { return displayedAttributes; }
    int getFieldOfVisionRadius() const;
    template<typename... Args>
    void addMessage(Args&&...);
    const std::vector<Message>& getMessages() const { return messages; }
    bool sees(const Tile& tile) const;
    bool remembers(const Tile& tile) const;
    std::vector<Creature*> getCreaturesCurrentlySeenBy(int maxFieldOfVisionRadius) const;
    std::vector<Creature*> getCurrentlySeenCreatures() const;
    Creature* getNearestEnemy() const;
    void setController(std::unique_ptr<CreatureController> controller);

private:
    World& getWorld() const;
    int getTurn() const;
    void moveTo(Tile&);
    void attack(Creature&);
    void setAttribute(Attribute, double amount);
    void editAttribute(Attribute, double amount);
    void generateAttributes(boost::string_ref);
    void calculateDerivedStats();
    void editHP(double amount) { currentHP = std::min(currentHP + amount, maxHP); }
    void editAP(double amount) { currentAP += amount; }
    void editMP(double amount) { currentMP = std::min(currentMP + amount, maxMP); }
    void regenerate();
    void onDeath();
    static std::vector<Attribute> initDisplayedAttributes(boost::string_ref);
    static std::vector<std::vector<int>> initAttributeIndices(boost::string_ref);
    const auto& getAttributeIndices(int attribute) const { return attributeIndices[attribute]; }

    std::vector<Tile*> tilesUnder;
    mutable std::unordered_set<Vector3> seenTilePositions;
    std::vector<std::unique_ptr<Item>> inventory;
    std::unordered_map<EquipmentSlot, Item*> equipment;
    double currentHP, maxHP, currentAP, currentMP, maxMP;
    bool running;
    std::vector<double> attributeValues;
    std::vector<Attribute> displayedAttributes;
    std::vector<std::vector<int>> attributeIndices;
    Sprite sprite;
    std::unique_ptr<CreatureController> controller;
    std::vector<Message> messages;

    static constexpr double fullAP = 1.0;
    static const int configAttributes[8];
};

template<typename... Args>
void Creature::addMessage(Args&&... messageParts)
{
    std::stringstream stream;
    auto expansion = { (stream << messageParts, 0)... };
    (void) expansion;
    std::string message = stream.str();
    message[0] = char(std::toupper(message[0]));

    if (!messages.empty() && messages.back().getText() == message)
        messages.back().increaseCount(getTurn());
    else
        messages.emplace_back(std::move(message), getTurn());
}

Attribute stringToAttribute(boost::string_ref);
std::vector<Attribute> stringsToAttributes(const std::vector<std::string>&);

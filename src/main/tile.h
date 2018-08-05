#pragma once

#include "entity.h"
#include "creature.h"
#include "item.h"
#include "liquid.h"
#include "object.h"
#include "engine/color.h"
#include "engine/geometry.h"
#include "engine/sprite.h"
#include <boost/utility/string_ref.hpp>
#include <memory>
#include <vector>

class Area;
class LightSource;
class Object;
class SaveFile;
class Window;
class World;

class Tile
{
public:
    Tile(World& world, Vector2 position, int level, boost::string_ref groundId);
    Tile(const SaveFile& file, World& world, Vector2 position, int level);
    void save(SaveFile& file) const;
    void exist();
    void render(Window& window, int zIndex, bool fogOfWar, bool renderLight) const;
    template<typename... Args>
    Creature* spawnCreature(Args&&...);
    bool hasCreature() const { return !creatures.empty(); }
    const std::vector<std::unique_ptr<Creature>>& getCreatures() const { return creatures; }
    Creature& getCreature(int index) const { return *creatures[index]; }
    void transferCreature(Creature&, Tile&);
    std::unique_ptr<Creature> removeSingleTileCreature(Creature&);
    void removeCreature(Creature&);
    bool hasItems() const { return !items.empty(); }
    const std::vector<std::unique_ptr<Item>>& getItems() const { return items; }
    std::unique_ptr<Item> removeTopmostItem();
    void addItem(std::unique_ptr<Item> item);
    void addLiquid(boost::string_ref materialId);
    bool hasObject() const { return bool(object); }
    Object* getObject() { return object.get(); }
    const Object* getObject() const { return object.get(); }
    void setObject(std::unique_ptr<Object>);
    boost::string_ref getGroundId() const { return groundId; }
    void setGround(boost::string_ref groundId);
    std::vector<Entity*> getEntities() const;
    std::vector<LightSource*> getLightSources() const;
    Color32 getLight() const { return light; }
    void emitLight();
    void addLight(Color32 light) { this->light.lighten(light); }
    void resetLight();
    bool blocksSight() const;
    Tile* getAdjacentTile(Dir8) const;
    Tile* getPreExistingAdjacentTile(Dir8) const;
    Tile* getTileBelow() const;
    Tile* getTileAbove() const;
    World& getWorld() const { return world; }
    Vector2 getPosition() const { return position; }
    Vector3 getPosition3D() const { return Vector3(position) + Vector3(0, 0, level); }
    int getLevel() const { return level; }
    Vector2 getCenterPosition() const { return position * getSize() + getSize() / 2; }
    static Vector2 getSize();
    static Vector2 getMaxSize();
    static const Vector2 spriteSize;

private:
    std::string getTooltip() const;
    void addCreature(std::unique_ptr<Creature> creature) { creatures.push_back(std::move(creature)); }

    std::vector<std::unique_ptr<Creature>> creatures;
    std::vector<std::unique_ptr<Item>> items;
    std::vector<Liquid> liquids;
    std::unique_ptr<Object> object;
    World& world;
    Vector2 position;
    int level;
    std::string groundId;
    Sprite groundSprite;
    Color32 light;
};

template<typename... Args>
Creature* Tile::spawnCreature(Args&&... creatureArgs)
{
    addCreature(make_unique<Creature>(this, std::forward<Args>(creatureArgs)...));
    return creatures.back().get();
}

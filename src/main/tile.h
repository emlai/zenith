#pragma once

#include "entity.h"
#include "creature.h"
#include "item.h"
#include "liquid.h"
#include "object.h"
#include "engine/color.h"
#include "engine/geometry.h"
#include "engine/sprite.h"
#include <string_view>
#include <memory>
#include <vector>

class Area;
class LightSource;
class Object;
class SaveFile;
class Window;
class World;

enum class RenderLayer
{
    Bottom,
    Top
};

constexpr RenderLayer renderLayers[] = { RenderLayer::Bottom, RenderLayer::Top };

class Tile
{
public:
    Tile(World& world, Vector2 position, int level, std::string_view groundId);
    Tile(const SaveFile& file, World& world, Vector2 position, int level);
    void save(SaveFile& file) const;
    void exist();
    void render(Window& window, RenderLayer layer, bool fogOfWar, bool renderLight) const;
    Creature* spawnCreature(std::string_view id, std::unique_ptr<Controller> controller = nullptr);
    Creature* spawnCreature(const SaveFile& file);
    bool hasCreature() const { return creature != nullptr; }
    Creature* getCreature() const { return creature; }
    void setCreature(Creature* creature) { this->creature = creature; }
    void removeCreature();
    bool hasItems() const { return !items.empty(); }
    const std::vector<std::unique_ptr<Item>>& getItems() const { return items; }
    std::unique_ptr<Item> removeTopmostItem();
    void addItem(std::unique_ptr<Item> item);
    void addLiquid(std::string_view materialId);
    bool hasObject() const { return object != nullptr; }
    Object* getObject() { return object.get(); }
    const Object* getObject() const { return object.get(); }
    void setObject(std::unique_ptr<Object>);
    std::string_view getGroundId() const { return groundId; }
    void setGround(std::string_view groundId);
    std::vector<Entity*> getEntities() const;
    std::vector<LightSource*> getLightSources() const;
    Color getLight() const { return light; }
    void emitLight();
    void addLight(Color light) { this->light.lighten(light); }
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
    static const Vector2 spriteSize;

private:
    std::string getTooltip() const;

    Creature* creature = nullptr;
    std::vector<std::unique_ptr<Item>> items;
    std::vector<Liquid> liquids;
    std::unique_ptr<Object> object;
    World& world;
    Vector2 position;
    int level;
    std::string groundId;
    Sprite groundSprite;
    Color light;
};

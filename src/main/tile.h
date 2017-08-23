#pragma once

#include "entity.h"
#include "creature.h"
#include "engine/config.h"
#include "engine/geometry.h"
#include "engine/sprite.h"
#include <boost/utility/string_ref.hpp>
#include <vector>

class Area;
class Window;
class World;

class Tile : public Entity
{
public:
    Tile(World& world, Vector2 position, boost::string_ref groundId, const Config& groundConfig,
         const Texture& groundSpriteSheet);
    void exist() override;
    void render(Window& window, int zIndex) const;
    template<typename... Args>
    Creature* spawnCreature(Args&&...);
    const auto& getCreatures() const { return creatures; }
    Creature& getCreature(int index) const { return *creatures[index]; }
    void transferCreature(Creature&, Tile&);
    Tile* getAdjacentTile(Dir8) const;
    World& getWorld() const { return world; }
    Vector2 getPosition() const { return position; }
    static const int size = 20;
    static const Vector2 sizeVector;

private:
    void addCreature(std::unique_ptr<Creature> creature) { creatures.push_back(std::move(creature)); }

    std::vector<std::unique_ptr<Creature>> creatures;
    World& world;
    Vector2 position;
    Sprite groundSprite;
};

template<typename... Args>
Creature* Tile::spawnCreature(Args&&... creatureArgs)
{
    addCreature(std::make_unique<Creature>(*this, std::forward<Args>(creatureArgs)...));
    return creatures.back().get();
}

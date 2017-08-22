#pragma once

#include "entity.h"
#include "creature.h"
#include "engine/config.h"
#include "engine/geometry.h"
#include <vector>

class Area;

class Tile : public Entity
{
public:
    Tile(Area&, Vector2 position);
    void exist() override;
    template<typename... Args>
    Creature* spawnCreature(Args&&...);
    const auto& getCreatures() const { return creatures; }
    Creature& getCreature(int index) const { return *creatures[index]; }
    void transferCreature(Creature&, Tile&);
    Tile* getAdjacentTile(Dir8) const;
    Vector2 getPosition() const { return position; }
    static const int size = 24;
    static const Vector2 sizeVector;

private:
    void addCreature(std::unique_ptr<Creature> creature) { creatures.push_back(std::move(creature)); }

    std::vector<std::unique_ptr<Creature>> creatures;
    Area& location;
    Vector2 position;
};

template<typename... Args>
Creature* Tile::spawnCreature(Args&&... creatureArgs)
{
    addCreature(std::make_unique<Creature>(*this, std::forward<Args>(creatureArgs)...));
    return creatures.back().get();
}

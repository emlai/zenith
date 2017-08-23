#pragma once

#include "tile.h"
#include <vector>

class Window;
class World;

class Area : public Entity
{
public:
    Area(World& world, Vector2 position, const Config& groundConfig, const Texture& groundSpriteSheet);
    void exist() override;
    Tile& getTileAt(Vector2 position);
    const Tile& getTileAt(Vector2 position) const;
    World& getWorld() const { return world; }
    Vector2 getPosition() const { return position; }
    static const int size = 64;
    static const Vector2 sizeVector;

private:
    std::vector<Tile> tiles;
    World& world;
    Vector2 position;
};

Tile& getRandomTile(Area& area);
const Tile& getRandomTile(const Area& area);

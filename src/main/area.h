#pragma once

#include "tile.h"
#include <vector>

class Window;
class World;

class Area : public Entity
{
public:
    Area(World& world, Vector2 position, const Config& groundConfig, const Texture& groundSpriteSheet);

private:
    friend class World;

    void exist() override;
    Tile& getRandomTile();
    const Tile& getRandomTile() const;
    World& getWorld() const { return world; }
    Vector2 getPosition() const { return position; }
    static const int size = 64;
    static const Vector2 sizeVector;

    Tile& getTileAt(Vector2 position);
    const Tile& getTileAt(Vector2 position) const;

    std::vector<Tile> tiles;
    World& world;
    Vector2 position;
};

#pragma once

#include "tile.h"
#include "engine/geometry.h"
#include <vector>

class SaveFile;
class Tile;
class Window;
class World;

class Area
{
public:
    Area(World& world, Vector2 position, int level);
    Area(const SaveFile& file, World& world, Vector2 position, int level);
    void save(SaveFile& file) const;
    Tile& getTileAt(Vector2 position);

    static const int size = 64;
    static const Vector2 sizeVector;

    std::vector<Tile> tiles;
    World& world;
    Vector2 position;
};

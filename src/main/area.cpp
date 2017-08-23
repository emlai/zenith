#include "area.h"
#include "tile.h"

const Vector2 Area::sizeVector = Vector2(Area::size, Area::size);

Area::Area(World& world, Vector2 position)
:   world(world), position(position)
{
    tiles.reserve(size * size);

    for (Vector2 pos(0, 0); pos.y < size; ++pos.y)
        for (pos.x = 0; pos.x < size; ++pos.x)
            tiles.emplace_back(world, position * sizeVector + pos, "Grass");
}

void Area::exist()
{
    for (auto& tile : tiles)
        tile.exist();
}

Tile& Area::getTileAt(Vector2 position)
{
    assert(position.x >= 0 && position.x < size);
    assert(position.y >= 0 && position.y < size);
    return tiles[position.x + size * position.y];
}

const Tile& Area::getTileAt(Vector2 position) const
{
    assert(position.x >= 0 && position.x < size);
    assert(position.y >= 0 && position.y < size);
    return tiles[position.x + size * position.y];
}

const Tile& Area::getRandomTile() const
{
    return getTileAt(makeRandomVector(sizeVector));
}

Tile& Area::getRandomTile()
{
    return getTileAt(makeRandomVector(sizeVector));
}

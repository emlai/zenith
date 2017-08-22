#include "area.h"
#include "tile.h"

const Vector2 Area::sizeVector = Vector2(Area::size, Area::size);

Area::Area()
{
    tiles.reserve(size * size);

    for (Vector2 pos(0, 0); pos.y < size; ++pos.y)
        for (pos.x = 0; pos.x < size; ++pos.x)
            tiles.emplace_back(*this, pos);
}

void Area::exist()
{
    for (auto& tile : tiles)
        tile.exist();
}

const Tile& getRandomTile(const Area& area)
{
    return area.getTileAt(makeRandomVector(area.sizeVector));
}

Tile& getRandomTile(Area& area)
{
    return area.getTileAt(makeRandomVector(area.sizeVector));
}

#include "area.h"
#include "tile.h"
#include "engine/savefile.h"

const Vector2 Area::sizeVector = Vector2(Area::size, Area::size);

Area::Area(World& world, Vector2 position, int level)
:   world(world), position(position)
{
    tiles.reserve(size * size);
    boost::string_ref groundId = level < 0 ? "DirtFloor" : "Grass";

    for (Vector2 pos(0, 0); pos.y < size; ++pos.y)
    {
        for (pos.x = 0; pos.x < size; ++pos.x)
        {
            tiles.emplace_back(world, position * sizeVector + pos, level, groundId);

            if (level < 0)
                tiles.back().setObject(std::make_unique<Object>("Ground"));
        }
    }
}

Area::Area(const SaveFile& file, World& world, Vector2 position, int level)
:   world(world), position(position)
{
    tiles.reserve(size * size);

    for (Vector2 pos(0, 0); pos.y < size; ++pos.y)
    {
        for (pos.x = 0; pos.x < size; ++pos.x)
            tiles.emplace_back(file, world, position * sizeVector + pos, level);
    }
}

void Area::save(SaveFile& file) const
{
    for (auto& tile : tiles)
        tile.save(file);
}

Tile& Area::getTileAt(Vector2 position)
{
    assert(position.x >= 0 && position.x < size);
    assert(position.y >= 0 && position.y < size);
    return tiles[position.x + size * position.y];
}

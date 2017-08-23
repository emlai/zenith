#include "world.h"

World::World(const Config& groundConfig, const Texture& groundSpriteSheet)
:   groundConfig(groundConfig), groundSpriteSheet(groundSpriteSheet)
{
}

void World::exist()
{
    areas.at({0, 0})->exist();
}

void World::render(Window& window, Rect region) const
{
    forEachTile(region, [&](const Tile& tile) { tile.render(window, 0); });
    forEachTile(region, [&](const Tile& tile) { tile.render(window, 2); });
}

Area* World::getOrCreateArea(Vector2 position)
{
    if (auto* area = getArea(position))
        return area;

    auto area = std::make_unique<Area>(*this, position, groundConfig, groundSpriteSheet);
    return areas.emplace(position, std::move(area)).first->second.get();
}

Area* World::getArea(Vector2 position) const
{
    auto it = areas.find(position);
    if (it != areas.end())
        return it->second.get();
    return nullptr;
}

/// Performs integer division, rounding towards negative infinity.
static int divideRoundingDown(int dividend, int divisor)
{
    const int quotient = dividend / divisor;

    if ((dividend % divisor != 0) && ((dividend < 0) != (divisor < 0)))
        return quotient - 1;
    else
        return quotient;
}

Vector2 World::globalPositionToAreaPosition(Vector2 position)
{
    return Vector2(divideRoundingDown(position.x, Area::size),
                   divideRoundingDown(position.y, Area::size));
}

Vector2 World::globalPositionToTilePosition(Vector2 position)
{
    Vector2 tilePosition = position % Area::size;
    if (tilePosition.x < 0) tilePosition.x += Area::size;
    if (tilePosition.y < 0) tilePosition.y += Area::size;
    return tilePosition;
}

Tile* World::getOrCreateTile(Vector2 position)
{
    if (auto* area = getOrCreateArea(globalPositionToAreaPosition(position)))
        return &area->getTileAt(globalPositionToTilePosition(position));

    return nullptr;
}

Tile* World::getTile(Vector2 position) const
{
    if (auto* area = getArea(globalPositionToAreaPosition(position)))
        return &area->getTileAt(globalPositionToTilePosition(position));

    return nullptr;
}

void World::forEachTile(Rect region, const std::function<void(const Tile&)>& function) const
{
    for (int x = region.getLeft(); x < region.getRight(); ++x)
        for (int y = region.getTop(); y < region.getBottom(); ++y)
            if (const auto* tile = getTile(Vector2(x, y)))
                function(*tile);
}

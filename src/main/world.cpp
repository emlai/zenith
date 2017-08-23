#include "world.h"
#include "worldgen.h"

World::World()
{
}

void World::exist(Rect region)
{
    // Collect the creatures into a vector to avoid updating the same creature more than once.
    std::vector<Creature*> creaturesToUpdate;

    forEachTile(region, [&](Tile& tile)
    {
        for (auto& creature : tile.getCreatures())
            creaturesToUpdate.push_back(creature.get());
    });

    creaturesToUpdate.erase(std::unique(creaturesToUpdate.begin(), creaturesToUpdate.end()),
                            creaturesToUpdate.end());

    for (auto* creature : creaturesToUpdate)
        if (!creature->isDead())
            creature->exist();

    forEachTile(region, [&](Tile& tile) { tile.exist(); });

    for (auto* creature : creaturesToUpdate)
        if (creature->isDead())
            for (auto* tile : creature->getTilesUnder())
                tile->removeCreature(*creature);
}

void World::render(Window& window, Rect region)
{
    for (int zIndex = 0; zIndex < 4; ++zIndex)
        forEachTile(region, [&](const Tile& tile) { tile.render(window, zIndex); });
}

Area* World::getOrCreateArea(Vector2 position)
{
    if (auto* area = getArea(position))
        return area;

    auto& area = *areas.emplace(position, std::make_unique<Area>(*this, position)).first->second;
    WorldGenerator generator(*this);
    generator.generateRegion(Rect(position * Area::sizeVector, Area::sizeVector));
    return &area;
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

void World::forEachTile(Rect region, const std::function<void(Tile&)>& function)
{
    for (int x = region.getLeft(); x <= region.getRight(); ++x)
        for (int y = region.getTop(); y <= region.getBottom(); ++y)
            if (auto* tile = getOrCreateTile(Vector2(x, y)))
                function(*tile);
}

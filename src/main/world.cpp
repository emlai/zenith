#include "world.h"
#include "game.h"
#include "tile.h"
#include "worldgen.h"
#include "components/lightsource.h"

int World::getTurn() const
{
    return game.getTurn();
}

void World::exist(Rect region, int level)
{
    // Collect the creatures into a vector to avoid updating the same creature more than once.
    std::vector<Creature*> creaturesToUpdate;

    forEachTile(region, level, [&](Tile& tile)
    {
        for (auto& creature : tile.getCreatures())
            creaturesToUpdate.push_back(creature.get());
    });

    creaturesToUpdate.erase(std::unique(creaturesToUpdate.begin(), creaturesToUpdate.end()),
                            creaturesToUpdate.end());

    for (auto* creature : creaturesToUpdate)
        if (!creature->isDead())
            creature->exist();

    for (auto* creature : creaturesToUpdate)
        if (creature->isDead())
        {
            if (creature->getTilesUnder().size() == 1)
            {
                auto dead = creature->getTileUnder(0).removeSingleTileCreature(*creature);
                creature->getTileUnder(0).addItem(std::make_unique<Corpse>(std::move(dead)));
            }
            else
            {
                // TODO: Implement multi-tile creature corpses.
                for (auto* tile : creature->getTilesUnder())
                    tile->removeCreature(*creature);
            }
        }
}

void World::render(Window& window, Rect region, int level, const Creature& player)
{
    auto lightRegion = region.inset(Vector2(-LightSource::maxRadius, -LightSource::maxRadius));
    forEachTile(lightRegion, level, [&](Tile& tile) { tile.resetLight(); });
    forEachTile(lightRegion, level, [&](Tile& tile) { tile.emitLight(); });

    std::vector<std::pair<const Tile*, bool>> tilesToRender;
    tilesToRender.reserve(region.getArea());

    forEachTile(region, level, [&](const Tile& tile)
    {
        if (player.sees(tile))
            tilesToRender.emplace_back(&tile, false);
        else if (player.remembers(tile))
            tilesToRender.emplace_back(&tile, true);
    });

    for (int zIndex = 0; zIndex < 7; ++zIndex)
        for (auto tileAndFogOfWar : tilesToRender)
            tileAndFogOfWar.first->render(window, zIndex, tileAndFogOfWar.second);
}

Area* World::getOrCreateArea(Vector2 position, int level)
{
    if (auto* area = getArea(position, level))
        return area;

    auto& area = *areas.emplace(std::make_pair(position, level),
                                std::make_unique<Area>(*this, position, level)).first->second;
    WorldGenerator generator(*this);
    generator.generateRegion(Rect(position * Area::sizeVector, Area::sizeVector), level);
    return &area;
}

Area* World::getArea(Vector2 position, int level) const
{
    auto it = areas.find({position, level});
    if (it != areas.end())
        return it->second.get();
    return nullptr;
}

Vector2 World::globalPositionToAreaPosition(Vector2 position)
{
    return position.divideRoundingDown(Area::size);
}

Vector2 World::globalPositionToTilePosition(Vector2 position)
{
    Vector2 tilePosition = position % Area::size;
    if (tilePosition.x < 0) tilePosition.x += Area::size;
    if (tilePosition.y < 0) tilePosition.y += Area::size;
    return tilePosition;
}

Tile* World::getOrCreateTile(Vector2 position, int level)
{
    if (auto* area = getOrCreateArea(globalPositionToAreaPosition(position), level))
        return &area->getTileAt(globalPositionToTilePosition(position));

    return nullptr;
}

Tile* World::getTile(Vector2 position, int level) const
{
    if (auto* area = getArea(globalPositionToAreaPosition(position), level))
        return &area->getTileAt(globalPositionToTilePosition(position));

    return nullptr;
}

void World::forEachTile(Rect region, int level, const std::function<void(Tile&)>& function)
{
    for (int x = region.getLeft(); x <= region.getRight(); ++x)
        for (int y = region.getTop(); y <= region.getBottom(); ++y)
            if (auto* tile = getOrCreateTile(Vector2(x, y), level))
                function(*tile);
}

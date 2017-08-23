#include "worldgen.h"
#include "game.h"
#include "object.h"
#include "world.h"
#include "engine/geometry.h"
#include "engine/math.h"

WorldGenerator::WorldGenerator(World& world)
:   world(world)
{
}

void WorldGenerator::generateRegion(Rect region)
{
    generateBuildings(region);
    generateItems(region);
    generateCreatures(region);
}

void WorldGenerator::generateBuildings(Rect region)
{
    auto density = 0.75;

    while (randFloat() < density)
    {
        auto minSize = 4;
        auto maxSize = 10;
        auto size = makeRandomVector(minSize, maxSize);
        auto topLeftPosition = region.position + makeRandomVector(region.size - size);
        generateBuilding(Rect(topLeftPosition, size));
    }
}

void WorldGenerator::generateBuilding(Rect region)
{
    generateRoom(region);
}

void WorldGenerator::generateRoom(Rect region)
{
    auto wallId = "BrickWall";
    auto floorId = "WoodenFloor";
    auto doorId = "Door";

    std::vector<Tile*> nonCornerWalls;
    const unsigned nonCornerWallCount = region.getPerimeter() - 8;
    nonCornerWalls.reserve(nonCornerWallCount);

    auto isCorner = [&](Vector2 position)
    {
        return (position.x == region.getLeft() || position.x == region.getRight())
            && (position.y == region.getTop() || position.y == region.getBottom());
    };

    auto generateWall = [&](Vector2 position)
    {
        if (auto* tile = world.getOrCreateTile(position))
        {
            tile->setObject(std::make_unique<Object>(wallId));

            if (!isCorner(position))
                nonCornerWalls.push_back(tile);
        }
    };

    for (int x = region.getLeft(); x <= region.getRight(); ++x)
    {
        generateWall(Vector2(x, region.getTop()));
        generateWall(Vector2(x, region.getBottom()));
    }

    for (int y = region.getTop() + 1; y < region.getBottom(); ++y)
    {
        generateWall(Vector2(region.getLeft(), y));
        generateWall(Vector2(region.getRight(), y));
    }

    assert(nonCornerWalls.size() == nonCornerWallCount);
    randomElement(nonCornerWalls)->setObject(std::make_unique<Object>(doorId));

    world.forEachTile(region, [&](Tile& tile) { tile.setGround(floorId); });
}

void WorldGenerator::generateItems(Rect region)
{
    auto density = 0.75;

    while (randFloat() < density)
    {
        std::string itemId = randomElement(Game::itemConfig.getToplevelKeys());
        world.getTile(makeRandomVectorInside(region))->addItem(std::make_unique<Item>(itemId));
    }
}

void WorldGenerator::generateCreatures(Rect region)
{
    auto density = 0.75;

    while (randFloat() < density)
        world.getTile(makeRandomVectorInside(region))->spawnCreature("Bat", std::make_unique<AIController>());
}

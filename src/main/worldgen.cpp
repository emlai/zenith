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

void WorldGenerator::generateRegion(Rect region, int level)
{
    generateBuildings(region, level);
    generateItems(region, level);
    generateCreatures(region, level);
}

void WorldGenerator::generateBuildings(Rect region, int level)
{
    auto minSize = 4;
    auto maxSize = 10;

    if (world.getTile(region.position, level + 1) != nullptr)
    {
        world.forEachTile(region, level + 1, [&](Tile& tile)
        {
            if (tile.hasObject() && tile.getObject()->getId() == "StairsDown")
            {
                // TODO: Make this building exactly the same size as the one above it, so that the
                // generation of the building always succeeds, so that we don't have to remove any
                // StairsDown from the above building, which is nasty.

                auto size = makeRandomVector(minSize, maxSize);
                // Makes sure the StairsUp are inside the building.
                auto topLeftPosition = tile.getPosition() - Vector2(1, 1) - makeRandomVector(size - Vector2(3, 3));
                bool didGenerateBuilding = generateBuilding(Rect(topLeftPosition, size), level);

                if (didGenerateBuilding)
                    tile.getTileBelow()->setObject(std::make_unique<Object>("StairsUp"));
                else
                    tile.setObject(nullptr);
            }
        });
    }

    auto density = 0.75;

    while (randFloat() < density)
    {
        auto size = makeRandomVector(minSize, maxSize);
        auto topLeftPosition = region.position + makeRandomVector(region.size - size);
        generateBuilding(Rect(topLeftPosition, size), level);
    }
}

bool WorldGenerator::generateBuilding(Rect region, int level)
{
    return generateRoom(region, level);
}

bool WorldGenerator::generateRoom(Rect region, int level)
{
    bool canGenerateHere = true;

    world.forEachTile(region, level, [&](const Tile& tile)
    {
        if (tile.getGroundId() == "WoodenFloor" && !tile.hasObject())
            canGenerateHere = false;
    });

    if (!canGenerateHere)
        return false;

    auto wallId = "BrickWall";
    auto floorId = "WoodenFloor";
    auto doorId = "Door";

    world.forEachTile(region, level, [&](Tile& tile)
    {
        tile.setGround(floorId);
        tile.setObject(nullptr);
    });

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
        if (auto* tile = world.getOrCreateTile(position, level))
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

    auto stairsProbability = 0.75;

    if (randFloat() < stairsProbability)
    {
        Rect roomInnerRegion = region.inset(Vector2(1, 1));
        Tile* stairsTile = world.getTile(makeRandomVectorInside(roomInnerRegion), level);
        stairsTile->setObject(std::make_unique<Object>("StairsDown"));
    }

    return true;
}

void WorldGenerator::generateItems(Rect region, int level)
{
    auto density = 0.75;

    while (randFloat() < density)
    {
        std::string itemId = randomElement(Game::itemConfig.getToplevelKeys());
        Tile* tile = world.getTile(makeRandomVectorInside(region), level);
        tile->addItem(std::make_unique<Item>(std::move(itemId)));
    }
}

void WorldGenerator::generateCreatures(Rect region, int level)
{
    auto density = 0.75;

    while (randFloat() < density)
    {
        Tile* tile = world.getTile(makeRandomVectorInside(region), level);
        tile->spawnCreature("Bat", std::make_unique<AIController>());
    }
}

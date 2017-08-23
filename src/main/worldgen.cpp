#include "worldgen.h"
#include "object.h"
#include "world.h"
#include "engine/geometry.h"
#include "engine/math.h"

WorldGenerator::WorldGenerator(World& world,
                               const Config& objectConfig, const Texture& objectSpriteSheet)
:   world(world), objectConfig(objectConfig), objectSpriteSheet(objectSpriteSheet)
{
}

void WorldGenerator::generateRegion(Rect region)
{
    generateBuildings(region);
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

    auto generateWall = [&](Vector2 position)
    {
        if (auto* tile = world.getOrCreateTile(position))
            tile->setObject(Object(wallId, objectConfig, objectSpriteSheet));
    };

    for (int x = region.getLeft(); x <= region.getRight(); ++x)
        generateWall(Vector2(x, region.getTop()));

    for (int x = region.getLeft(); x <= region.getRight(); ++x)
        generateWall(Vector2(x, region.getBottom()));

    for (int y = region.getTop() + 1; y < region.getBottom(); ++y)
        generateWall(Vector2(region.getLeft(), y));

    for (int y = region.getTop() + 1; y < region.getBottom(); ++y)
        generateWall(Vector2(region.getRight(), y));
}

#include "worldgen.h"
#include "game.h"
#include "object.h"
#include "world.h"
#include "engine/geometry.h"
#include "engine/math.h"
#include <unordered_set>

Building::Building(std::vector<Room>&& rooms)
:   rooms(std::move(rooms))
{
    assert(this->rooms.size() >= 1);
}

std::vector<Tile*> Building::getDoorTiles() const
{
    std::vector<Tile*> doorTiles;

    for (auto& room : rooms)
        for (auto* doorTile : room.getDoorTiles())
            doorTiles.push_back(doorTile);

    return doorTiles;
}

WorldGenerator::WorldGenerator(World& world)
:   world(world)
{
}

void WorldGenerator::generateRegion(Rect region, int level)
{
    auto buildings = generateBuildings(region, level);

    if (level < 0)
        generatePaths(region, level, buildings);

    generateItems(region, level);
    generateCreatures(region, level);
}

std::vector<Building> WorldGenerator::generateBuildings(Rect region, int level)
{
    auto minSize = 4;
    auto maxSize = 10;
    unsigned buildingsToGenerate = randInt(1, 10);

    std::vector<Building> buildings;

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
                auto building = generateBuilding(Rect(topLeftPosition, size), level);

                if (building)
                {
                    tile.getTileBelow()->setObject(std::make_unique<Object>("StairsUp"));
                    buildings.push_back(std::move(*building));
                }
                else
                    tile.setObject(nullptr);
            }
        });
    }

    while (buildings.size() < buildingsToGenerate)
    {
        auto size = makeRandomVector(minSize, maxSize);
        auto topLeftPosition = region.position + makeRandomVector(region.size - size);

        if (auto building = generateBuilding(Rect(topLeftPosition, size), level))
            buildings.push_back(std::move(*building));
    }

    if (!buildings.empty())
    {
        auto& randomRoom = randomElement(randomElement(buildings).getRooms());
        Tile* stairsTile = world.getTile(makeRandomVectorInside(randomRoom.getInnerRegion()), level);
        stairsTile->setObject(std::make_unique<Object>("StairsDown"));
    }

    return buildings;
}

boost::optional<Building> WorldGenerator::generateBuilding(Rect region, int level)
{
    if (auto room = generateRoom(region, level))
    {
        std::vector<Room> rooms;
        rooms.push_back(std::move(*room));
        return Building(std::move(rooms));
    }
    else
        return boost::none;
}

boost::optional<Room> WorldGenerator::generateRoom(Rect region, int level)
{
    bool canGenerateHere = true;

    world.forEachTile(region, level, [&](const Tile& tile)
    {
        if (tile.getGroundId() == "WoodenFloor" && !tile.hasObject())
            canGenerateHere = false;
    });

    if (!canGenerateHere)
        return boost::none;

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
    auto* doorTile = randomElement(nonCornerWalls);
    doorTile->setObject(std::make_unique<Object>(doorId));

    return Room(region, { doorTile });
}

Tile* WorldGenerator::findPathStart(Tile& tile) const
{
    for (auto direction : { North, East, South, West })
    {
        auto* adjacentTile = tile.getAdjacentTile(direction);

        if (adjacentTile && adjacentTile->getGroundId() != "WoodenFloor")
            return adjacentTile;
    }

    return nullptr;
}

static int heuristicCostEstimate(const Tile& a, const Tile& b)
{
    Vector2 distance = abs(a.getPosition() - b.getPosition());
    return distance.x * distance.y;
}

static std::vector<Tile*> reconstructPath(const std::unordered_map<Tile*, Tile*>& cameFrom, Tile* current)
{
    std::vector<Tile*> totalPath = { current };

    while (cameFrom.find(current) != cameFrom.end())
    {
        current = cameFrom.at(current);
        totalPath.push_back(current);
    }

    return totalPath;
}

std::vector<Tile*> WorldGenerator::findPathAStar(Tile& source, Tile& target,
                                                 const std::function<bool(Tile&)>& isAllowed) const
{
    std::unordered_set<Tile*> closedSet;
    std::unordered_set<Tile*> openSet = { &source };
    std::unordered_map<Tile*, Tile*> sources;
    std::unordered_map<Tile*, int> costs = {{ &source, 0 }};
    std::unordered_map<Tile*, int> estimatedCosts = {{ &source, heuristicCostEstimate(source, target) }};

    auto comparator = [&](Tile* a, Tile* b) { return estimatedCosts.at(a) < estimatedCosts.at(b); };

    while (!openSet.empty())
    {
        auto iteratorToCurrent = std::min_element(openSet.begin(), openSet.end(), comparator);
        auto current = *iteratorToCurrent;

        if (current == &target)
            return reconstructPath(sources, current);

        openSet.erase(iteratorToCurrent);
        closedSet.insert(current);

        for (auto direction : { North, East, South, West })
        {
            Tile* neighbor = current->getPreExistingAdjacentTile(direction);

            if (!neighbor || !isAllowed(*neighbor))
                continue;

            if (closedSet.find(neighbor) != closedSet.end())
                continue;

            auto it = costs.find(current);
            auto tentativeCost = (it == costs.end() ? INT_MAX - 1 : it->second) + 1;

            if (openSet.find(neighbor) == openSet.end())
                openSet.insert(neighbor);
            else
            {
                auto it = costs.find(neighbor);
                auto neighborCost = it == costs.end() ? INT_MAX - 1 : it->second;

                if (tentativeCost >= neighborCost)
                    continue;
            }

            sources.emplace(neighbor, current);
            costs.emplace(neighbor, tentativeCost);

            auto neighborIt = costs.find(neighbor);
            auto neighborCost = neighborIt == costs.end() ? INT_MAX - 1 : neighborIt->second;
            estimatedCosts.emplace(neighbor, neighborCost + heuristicCostEstimate(*neighbor, target));
        }
    }

    return {};
}

void WorldGenerator::generatePaths(Rect region, int level, const std::vector<Building>& buildings)
{
    if (buildings.empty())
        return;

    auto& buildingA = randomElement(buildings);

    for (auto& buildingB : buildings)
    {
        if (&buildingA == &buildingB)
            continue;

        for (auto* doorA : buildingA.getDoorTiles())
        {
            auto* pathStart = findPathStart(*doorA);

            if (!pathStart)
                continue;

            for (auto* doorB : buildingB.getDoorTiles())
            {
                auto* pathEnd = findPathStart(*doorB);

                if (!pathEnd)
                    continue;

                auto path = findPathAStar(*pathStart, *pathEnd, [](Tile& tile)
                {
                    return !tile.hasObject() || tile.getObject()->getId() == "Door"
                        || tile.getObject()->getId().starts_with("Stairs");
                });

                if (!path.empty())
                    continue;

                path = findPathAStar(*pathStart, *pathEnd, [](Tile& tile)
                {
                    return !tile.hasObject() || tile.getObject()->getId() != "BrickWall";
                });

                for (auto* pathTile : path)
                    pathTile->setObject(nullptr);
            }
        }
    }
}

void WorldGenerator::generateItems(Rect region, int level)
{
    auto density = 0.75;

    while (randFloat() < density)
    {
        std::string itemId = randomElement(Game::itemConfig.getToplevelKeys());
        auto materials = Game::itemConfig.get<std::vector<std::string>>(itemId, "PossibleMaterials");
        std::string materialId = materials.empty() ? "" : randomElement(materials);
        Tile* tile = world.getTile(makeRandomVectorInside(region), level);
        tile->addItem(std::make_unique<Item>(std::move(itemId), std::move(materialId)));
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

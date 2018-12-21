class Room
{
    Room(Rect region, List<Tile>&& doorTiles)
    :   region(region), doorTiles(std::move(doorTiles)) {}
    Rect getRegion() { return region; }
    Rect getInnerRegion() { return region.inset(Vector2(1, 1)); }
    const List<Tile> getDoorTiles() { return doorTiles; }

private:
    Rect region;
    List<Tile> doorTiles;
}

class Building
{
    Building(List<Room>&& rooms);
    const List<Room>& getRooms() { return rooms; }
    List<Tile> getDoorTiles();

private:
    List<Room> rooms;
    List<Tile> doorTiles;
}

class WorldGenerator
{
    WorldGenerator(World world);
    void generateRegion(Rect region, int level);

private:
    List<Building> generateBuildings(Rect region, int level);
    Building? generateBuilding(Rect region, int level);
    Room? generateRoom(Rect region, int level);
    Tile findPathStart(Tile tile);
    List<Tile> findPathAStar(Tile source, Tile target,
                                     const std::function<bool(Tile)>& isAllowed);
    void generatePaths(const List<Building>& buildings);
    void generateItems(Rect region, int level);
    void generateCreatures(Rect region, int level);

    World world;
}
Building::Building(List<Room>&& rooms)
:   rooms(std::move(rooms))
{
    assert(this.rooms.size() >= 1);
}

List<Tile> Building::getDoorTiles()
{
    List<Tile> doorTiles;

    for (var room : rooms)
        for (var doorTile : room.getDoorTiles())
            doorTiles.push_back(doorTile);

    return doorTiles;
}

WorldGenerator::WorldGenerator(World world)
:   world(world)
{
}

void WorldGenerator::generateRegion(Rect region, int level)
{
    var buildings = generateBuildings(region, level);

    if (level < 0)
        generatePaths(buildings);

    generateItems(region, level);
    generateCreatures(region, level);
}

List<Building> WorldGenerator::generateBuildings(Rect region, int level)
{
    var minSize = 4;
    var maxSize = 10;
    unsigned buildingsToGenerate = randInt(1, 10);

    List<Building> buildings;

    if (world.getTile(region.position, level + 1) != nullptr)
    {
        world.forEachTile(region, level + 1, [&](Tile tile)
        {
            if (tile.hasObject() && tile.getObject().getId() == "StairsDown")
            {
                // TODO: Make this building exactly the same size as the one above it, so that the
                // generation of the building always succeeds, so that we don't have to remove any
                // StairsDown from the above building, which is nasty.

                var size = makeRandomVector(minSize, maxSize);
                // Makes sure the StairsUp are inside the building.
                var topLeftPosition = tile.getPosition() - Vector2(1, 1) - makeRandomVector(size - Vector2(3, 3));
                var building = generateBuilding(Rect(topLeftPosition, size), level);

                if (building)
                {
                    tile.getTileBelow().setObject(std::make_unique<Object>("StairsUp"));
                    buildings.push_back(std::move(*building));
                }
                else
                    tile.setObject(nullptr);
            }
        });
    }

    while (buildings.size() < buildingsToGenerate)
    {
        var size = makeRandomVector(minSize, maxSize);
        var topLeftPosition = region.position + makeRandomVector(region.size - size);

        if (var building = generateBuilding(Rect(topLeftPosition, size), level))
            buildings.push_back(std::move(*building));
    }

    if (!buildings.empty())
    {
        var randomRoom = randomElement(randomElement(buildings).getRooms());
        Tile stairsTile = world.getTile(makeRandomVectorInside(randomRoom.getInnerRegion()), level);
        stairsTile.setObject(std::make_unique<Object>("StairsDown"));
    }

    return buildings;
}

Building? WorldGenerator::generateBuilding(Rect region, int level)
{
    if (var room = generateRoom(region, level))
    {
        List<Room> rooms;
        rooms.push_back(std::move(*room));
        return Building(std::move(rooms));
    }
    else
        return boost::none;
}

Room? WorldGenerator::generateRoom(Rect region, int level)
{
    bool canGenerateHere = true;

    world.forEachTile(region, level, [&](Tile tile)
    {
        if (tile.getGroundId() == "WoodenFloor" && !tile.hasObject())
            canGenerateHere = false;
    });

    if (!canGenerateHere)
        return boost::none;

    var wallId = "BrickWall";
    var floorId = "WoodenFloor";
    var doorId = "Door";

    world.forEachTile(region, level, [&](Tile tile)
    {
        tile.setGround(floorId);
        tile.setObject(nullptr);
    });

    List<Tile> nonCornerWalls;
    const unsigned nonCornerWallCount = region.getPerimeter() - 8;
    nonCornerWalls.reserve(nonCornerWallCount);

    var isCorner = [&](Vector2 position)
    {
        return (position.x == region.getLeft() || position.x == region.getRight())
            && (position.y == region.getTop() || position.y == region.getBottom());
    }

    var generateWall = [&](Vector2 position)
    {
        if (var tile = world.getOrCreateTile(position, level))
        {
            tile.setObject(std::make_unique<Object>(wallId));

            if (!isCorner(position))
                nonCornerWalls.push_back(tile);
        }
    }

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
    var doorTile = randomElement(nonCornerWalls);
    doorTile.setObject(std::make_unique<Object>(doorId));

    return Room(region, { doorTile });
}

Tile WorldGenerator::findPathStart(Tile tile)
{
    for (var direction : { North, East, South, West })
    {
        var adjacentTile = tile.getAdjacentTile(direction);

        if (adjacentTile && adjacentTile.getGroundId() != "WoodenFloor")
            return adjacentTile;
    }

    return nullptr;
}

static int heuristicCostEstimate(Tile a, Tile b)
{
    Vector2 distance = abs(a.getPosition() - b.getPosition());
    return distance.x * distance.y;
}

static List<Tile> reconstructPath(const Dictionary<Tile, Tile>& cameFrom, Tile current)
{
    List<Tile> totalPath = { current }

    while (cameFrom.find(current) != cameFrom.end())
    {
        current = cameFrom.at(current);
        totalPath.push_back(current);
    }

    return totalPath;
}

List<Tile> WorldGenerator::findPathAStar(Tile source, Tile target,
                                                 const std::function<bool(Tile)>& isAllowed)
{
    boost::unordered_set<Tile> closedSet;
    boost::unordered_set<Tile> openSet;
    openSet.emplace(source);
    Dictionary<Tile, Tile> sources;
    Dictionary<Tile, int> costs;
    costs.emplace(source, 0);
    Dictionary<Tile, int> estimatedCosts;
    estimatedCosts.emplace(source, heuristicCostEstimate(source, target));

    var comparator = [&](Tile a, Tile b) { return estimatedCosts.at(a) < estimatedCosts.at(b); }

    while (!openSet.empty())
    {
        var iteratorToCurrent = std::min_element(openSet.begin(), openSet.end(), comparator);
        var current = *iteratorToCurrent;

        if (current == target)
            return reconstructPath(sources, current);

        openSet.erase(iteratorToCurrent);
        closedSet.insert(current);

        for (var direction : { North, East, South, West })
        {
            Tile neighbor = current.getPreExistingAdjacentTile(direction);

            if (!neighbor || !isAllowed(*neighbor))
                continue;

            if (closedSet.find(neighbor) != closedSet.end())
                continue;

            var it = costs.find(current);
            var tentativeCost = (it == costs.end() ? INT_MAX - 1 : it.second) + 1;

            if (openSet.find(neighbor) == openSet.end())
                openSet.insert(neighbor);
            else
            {
                var it = costs.find(neighbor);
                var neighborCost = it == costs.end() ? INT_MAX - 1 : it.second;

                if (tentativeCost >= neighborCost)
                    continue;
            }

            sources.emplace(neighbor, current);
            costs.emplace(neighbor, tentativeCost);

            var neighborIt = costs.find(neighbor);
            var neighborCost = neighborIt == costs.end() ? INT_MAX - 1 : neighborIt.second;
            estimatedCosts.emplace(neighbor, neighborCost + heuristicCostEstimate(*neighbor, target));
        }
    }

    return {}
}

void WorldGenerator::generatePaths(const List<Building>& buildings)
{
    if (buildings.empty())
        return;

    var buildingA = randomElement(buildings);

    for (var buildingB : buildings)
    {
        if (buildingA == buildingB)
            continue;

        for (var doorA : buildingA.getDoorTiles())
        {
            var pathStart = findPathStart(*doorA);

            if (!pathStart)
                continue;

            for (var doorB : buildingB.getDoorTiles())
            {
                var pathEnd = findPathStart(*doorB);

                if (!pathEnd)
                    continue;

                var path = findPathAStar(*pathStart, *pathEnd, [](Tile tile)
                {
                    return !tile.hasObject() || tile.getObject().getId() == "Door"
                        || tile.getObject().getId().starts_with("Stairs");
                });

                if (!path.empty())
                    continue;

                path = findPathAStar(*pathStart, *pathEnd, [](Tile tile)
                {
                    return !tile.hasObject() || tile.getObject().getId() != "BrickWall";
                });

                for (var pathTile : path)
                    pathTile.setObject(nullptr);
            }
        }
    }
}

void WorldGenerator::generateItems(Rect region, int level)
{
    var density = 0.75;

    while (randFloat() < density)
    {
        string itemId = randomElement(Game::itemConfig.getToplevelKeys());
        std::unique_ptr<Item> item;

        if (itemId == "Corpse")
        {
            string creatureId = randomElement(Game::creatureConfig.getToplevelKeys());
            item = std::make_unique<Corpse>(std::move(creatureId));
        }
        else
            item = std::make_unique<Item>(std::move(itemId), getRandomMaterialId(itemId));

        Tile tile = nullptr;

        while (!tile || tile.hasObject())
            tile = world.getTile(makeRandomVectorInside(region), level);

        tile.addItem(std::move(item));
    }
}

void WorldGenerator::generateCreatures(Rect region, int level)
{
    var density = 0.75;

    while (randFloat() < density)
    {
        Tile tile = nullptr;

        while (!tile || tile.hasObject())
            tile = world.getTile(makeRandomVectorInside(region), level);

        tile.spawnCreature("Bat");
    }
}

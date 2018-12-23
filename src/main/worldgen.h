#pragma once

#include "engine/geometry.h"
#include <optional>
#include <vector>

class Tile;
class World;
struct Rect;

class Room
{
public:
    Room(Rect region, std::vector<Tile*>&& doorTiles)
    :   region(region), doorTiles(std::move(doorTiles)) {}
    Rect getInnerRegion() const { return region.inset(Vector2(1, 1)); }

    Rect region;
    std::vector<Tile*> doorTiles;
};

class Building
{
public:
    Building(std::vector<Room>&& rooms);
    std::vector<Tile*> getDoorTiles() const;

    std::vector<Room> rooms;
};

class WorldGenerator
{
public:
    WorldGenerator(World& world);
    void generateRegion(Rect region, int level);
    std::vector<Building> generateBuildings(Rect region, int level);
    std::optional<Building> generateBuilding(Rect region, int level);
    std::optional<Room> generateRoom(Rect region, int level);
    Tile* findPathStart(Tile& tile) const;
    std::vector<Tile*> findPathAStar(Tile& source, Tile& target, const std::function<bool(Tile&)>& isAllowed) const;
    void generatePaths(const std::vector<Building>& buildings);
    void generateItems(Rect region, int level);
    void generateCreatures(Rect region, int level);

    World& world;
};

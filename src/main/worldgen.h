#pragma once

#include "engine/geometry.h"
#include <boost/optional.hpp>
#include <vector>

class Tile;
class World;
struct Rect;

class Room
{
public:
    Room(Rect region, std::vector<Tile*>&& doorTiles)
    :   region(region), doorTiles(std::move(doorTiles)) {}
    Rect getRegion() const { return region; }
    Rect getInnerRegion() const { return region.inset(Vector2(1, 1)); }
    const std::vector<Tile*> getDoorTiles() const { return doorTiles; }

private:
    Rect region;
    std::vector<Tile*> doorTiles;
};

class Building
{
public:
    Building(std::vector<Room>&& rooms);
    const std::vector<Room>& getRooms() const { return rooms; }
    std::vector<Tile*> getDoorTiles() const;

private:
    std::vector<Room> rooms;
    std::vector<Tile*> doorTiles;
};

class WorldGenerator
{
public:
    WorldGenerator(World& world);
    void generateRegion(Rect region, int level);

private:
    std::vector<Building> generateBuildings(Rect region, int level);
    boost::optional<Building> generateBuilding(Rect region, int level);
    boost::optional<Room> generateRoom(Rect region, int level);
    Tile* findPathStart(Tile& tile) const;
    std::vector<Tile*> findPathAStar(Tile& source, Tile& target,
                                     const std::function<bool(Tile&)>& isAllowed) const;
    void generatePaths(const std::vector<Building>& buildings);
    void generateItems(Rect region, int level);
    void generateCreatures(Rect region, int level);

    World& world;
};

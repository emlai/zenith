#pragma once

class World;
struct Rect;

class WorldGenerator
{
public:
    WorldGenerator(World& world);
    void generateRegion(Rect region, int level);

private:
    void generateBuildings(Rect region, int level);
    bool generateBuilding(Rect region, int level);
    bool generateRoom(Rect region, int level);
    void generateItems(Rect region, int level);
    void generateCreatures(Rect region, int level);

    World& world;
};

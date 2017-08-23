#pragma once

class World;
struct Rect;

class WorldGenerator
{
public:
    WorldGenerator(World& world);
    void generateRegion(Rect region);

private:
    void generateBuildings(Rect region);
    void generateBuilding(Rect region);
    void generateRoom(Rect region);

    World& world;
};

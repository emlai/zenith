#pragma once

class Config;
class Texture;
class World;
struct Rect;

class WorldGenerator
{
public:
    WorldGenerator(World& world,
                   const Config& objectConfig, const Texture& objectSpriteSheet,
                   const Config& groundConfig, const Texture& groundSpriteSheet);
    void generateRegion(Rect region);

private:
    void generateBuildings(Rect region);
    void generateBuilding(Rect region);
    void generateRoom(Rect region);

    World& world;
    const Config& objectConfig;
    const Texture& objectSpriteSheet;
    const Config& groundConfig;
    const Texture& groundSpriteSheet;
};

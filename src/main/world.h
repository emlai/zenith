#pragma once

#include "area.h"
#include "engine/color.h"
#include "engine/geometry.h"
#include <functional>
#include <memory>
#include <unordered_map>

class Creature;
class Game;
class Tile;

namespace std
{
    template<>
    struct hash<std::pair<Vector2, int>>
    {
        size_t operator()(std::pair<Vector2, int> vectorAndLevel) const
        {
            return (vectorAndLevel.first.x * 73856093)
                ^ (vectorAndLevel.first.y * 19349663)
                ^ (vectorAndLevel.second * 83492791);
        }
    };
}

class World
{
public:
    World(const Game& game) : game(game) {}
    int getTurn() const;
    void exist(Rect region, int level);
    void render(Window&, Rect region, int level, const Creature& player);
    Tile* getOrCreateTile(Vector2 position, int level);
    Tile* getTile(Vector2 position, int level) const;
    void forEachTile(Rect region, int level, const std::function<void(Tile&)>&);
    Color32 getSunlight() const { return sunlight; }

private:
    Area* getOrCreateArea(Vector2 position, int level);
    Area* getArea(Vector2 position, int level) const;
    static Vector2 globalPositionToAreaPosition(Vector2 position);
    static Vector2 globalPositionToTilePosition(Vector2 position);

    const Game& game;
    std::unordered_map<std::pair<Vector2, int>, std::unique_ptr<Area>> areas;
    Color32 sunlight = 0x888888FF;
};

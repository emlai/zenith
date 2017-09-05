#pragma once

#include "area.h"
#include "engine/color.h"
#include "engine/geometry.h"
#include <fstream>
#include <functional>
#include <memory>
#include <unordered_map>

class Creature;
class Game;
class SaveFile;
class Tile;

class World
{
public:
    World(const Game& game) : game(game), sunlight(0x888888FF) {}
    void load(const SaveFile& file);
    void save(SaveFile& file) const;
    int getTurn() const;
    void exist(Rect region, int level);
    void render(Window&, Rect region, int level, const Creature& player);
    Tile* getOrCreateTile(Vector2 position, int level);
    Tile* getTile(Vector2 position, int level);
    void forEachTile(Rect region, int level, const std::function<void(Tile&)>&);
    Color32 getSunlight() const { return sunlight; }

private:
    Area* getOrCreateArea(Vector3 position);
    Area* getArea(Vector3 position);
    static Vector3 globalPositionToAreaPosition(Vector2 position, int level);
    static Vector2 globalPositionToTilePosition(Vector2 position);

    const Game& game;
    std::unordered_map<Vector3, Area> areas;
    Color32 sunlight;
};

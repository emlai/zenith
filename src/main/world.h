#pragma once

#include "area.h"
#include "engine/color.h"
#include "engine/geometry.h"
#include <unordered_map>
#include <fstream>
#include <functional>
#include <memory>

class Creature;
class Game;
class SaveFile;
class Tile;

class World
{
public:
    World() = default;
    World(const World&) = delete;
    World(World&&) = default;
    World& operator=(const World&) = delete;
    World& operator=(World&&) = default;
    void load(SaveFile& file);
    void save(SaveFile& file) const;
    int getTurn() const;
    void exist(Rect region, int level);
    void render(Window&, Rect region, int level, const Creature& player);
    Tile* getOrCreateTile(Vector2 position, int level);
    Tile* getTile(Vector2 position, int level);
    std::vector<Tile*> getTiles(Rect region, int level);
    Creature* addCreature(std::unique_ptr<Creature> creature);
    std::unique_ptr<Creature> removeCreature(Creature* creature);
    Color getSunlight() const { return sunlight; }

    const Game* game = nullptr;

private:
    Area* getOrCreateArea(Vector3 position);
    Area* getArea(Vector3 position);
    static Vector3 globalPositionToAreaPosition(Vector2 position, int level);
    static Vector2 globalPositionToTilePosition(Vector2 position);

    std::unordered_map<Vector3, Area> areas;
    std::unordered_map<Vector3, int64_t> savedAreaOffsets;
    std::vector<std::unique_ptr<Creature>> creatures;
    std::unique_ptr<SaveFile> saveFile;
    Color sunlight = Color(0x888888FF);
};

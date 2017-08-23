#pragma once

#include "area.h"
#include "entity.h"
#include "engine/geometry.h"
#include <functional>
#include <memory>
#include <unordered_map>

class World : public Entity
{
public:
    World();
    void exist(Rect region);
    void render(Window&, Rect region);
    Tile* getOrCreateTile(Vector2 position);
    Tile* getTile(Vector2 position) const;
    void forEachTile(Rect region, const std::function<void(Tile&)>&);

private:
    Area* getOrCreateArea(Vector2 position);
    Area* getArea(Vector2 position) const;
    static Vector2 globalPositionToAreaPosition(Vector2 position);
    static Vector2 globalPositionToTilePosition(Vector2 position);

    std::unordered_map<Vector2, std::unique_ptr<Area>> areas;
};

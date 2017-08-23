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
    World(const Config& groundConfig, const Texture& groundSpriteSheet);
    void exist() override;
    void render(Window&, Rect region) const;
    Tile* getOrCreateTile(Vector2 position);
    Tile* getTile(Vector2 position) const;

private:
    Area* getOrCreateArea(Vector2 position);
    Area* getArea(Vector2 position) const;
    void forEachTile(Rect region, const std::function<void(const Tile&)>&) const;
    static Vector2 globalPositionToAreaPosition(Vector2 position);
    static Vector2 globalPositionToTilePosition(Vector2 position);

    std::unordered_map<Vector2, std::unique_ptr<Area>> areas;
    const Config& groundConfig;
    const Texture& groundSpriteSheet;
};

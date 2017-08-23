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
    Area* getOrCreateArea(Vector2 position);
    Area* getArea(Vector2 position) const;
    Tile* getOrCreateTile(Vector2 position);
    Tile* getTile(Vector2 position) const;

private:
    void forEachTile(Rect region, const std::function<void(const Tile&)>&) const;

    std::unordered_map<Vector2, std::unique_ptr<Area>> areas;
    const Config& groundConfig;
    const Texture& groundSpriteSheet;
};

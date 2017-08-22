#pragma once

#include "area.h"
#include "entity.h"
#include "engine/geometry.h"
#include <memory>
#include <unordered_map>

class World : public Entity
{
public:
    World(const Config& groundConfig, const Texture& groundSpriteSheet);
    void exist() override;
    void render(Window&) const;
    Area* getArea(Vector2 position);

private:
    std::unordered_map<Vector2, std::unique_ptr<Area>> areas;
    const Config& groundConfig;
    const Texture& groundSpriteSheet;
};

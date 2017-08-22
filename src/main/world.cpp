#include "world.h"

World::World(const Config& groundConfig, const Texture& groundSpriteSheet)
:   groundConfig(groundConfig), groundSpriteSheet(groundSpriteSheet)
{
}

void World::exist()
{
    areas.at({0, 0})->exist();
}

void World::render(Window& window) const
{
    areas.at({0, 0})->render(window, 0);
    areas.at({0, 0})->render(window, 2);
}

Area* World::getArea(Vector2 position)
{
    auto it = areas.find(position);
    if (it == areas.end())
        it = areas.emplace(position, std::make_unique<Area>(groundConfig, groundSpriteSheet)).first;
    return it->second.get();
}

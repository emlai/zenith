#include "tile.h"
#include "area.h"
#include "engine/texture.h"
#include <cassert>

const Vector2 Tile::sizeVector = Vector2(Tile::size, Tile::size);

static Rect getGroundTextureRegion(const Config& groundConfig, boost::string_ref groundId)
{
    auto components = groundConfig.get<std::vector<int>>(groundId.to_string(), "spritePosition");
    auto offsetX = randInt(groundConfig.get<int>(groundId.to_string(), "spriteMultiplicity") - 1);
    return Rect(components.at(0) + offsetX * Tile::size, components.at(1), Tile::size, Tile::size);
}

Tile::Tile(Area& location, Vector2 position, boost::string_ref groundId, const Config& groundConfig,
           const Texture& groundSpriteSheet)
:   location(location),
    position(position),
    groundSprite(groundSpriteSheet, getGroundTextureRegion(groundConfig, groundId))
{
}

void Tile::exist()
{
    for (auto it = creatures.begin(); it != creatures.end();)
    {
        if (!(*it)->isDead())
            (*it++)->exist();
        else
            it = creatures.erase(it);
    }
}

void Tile::render(Window& window, int zIndex) const
{
    switch (zIndex)
    {
        case 0: groundSprite.render(window, position * sizeVector); break;
        case 1: /* TODO: render items etc. */ break;
        case 2: /* TODO: render creatures etc. */ break;
        default: assert(false);
    }
}

void Tile::transferCreature(Creature& creature, Tile& destination)
{
    for (auto it = creatures.begin(); it != creatures.end(); ++it)
    {
        if (it->get() == &creature)
        {
            destination.addCreature(std::move(*it));
            creatures.erase(it);
            return;
        }
    }

    assert(false);
}

Tile* Tile::getAdjacentTile(Dir8 direction) const
{
    auto adjacentPosition = position + direction;

    if (!adjacentPosition.isWithin(location.sizeVector))
        return nullptr;

    return &location.getTileAt(adjacentPosition);
}

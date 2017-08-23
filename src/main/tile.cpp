#include "tile.h"
#include "area.h"
#include "gui.h"
#include "world.h"
#include "engine/texture.h"
#include <cassert>

const Vector2 Tile::sizeVector = Vector2(Tile::size, Tile::size);

Tile::Tile(World& world, Vector2 position, boost::string_ref groundId, const Config& groundConfig,
           const Texture& groundSpriteSheet)
:   world(world),
    position(position),
    groundSprite(groundSpriteSheet, getSpriteTextureRegion(groundConfig, groundId))
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
        case 0:
            groundSprite.render(window, position * sizeVector);
            break;
        case 1:
            if (object)
                object->render(window, position * sizeVector);
            break;
        case 2:
            for (const auto& creature : creatures)
                creature->render(window);
            break;
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

void Tile::setObject(boost::optional<Object> newObject)
{
    object = std::move(newObject);
}

Tile* Tile::getAdjacentTile(Dir8 direction) const
{
    return getWorld().getOrCreateTile(getPosition() + direction);
}

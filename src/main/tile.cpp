#include "tile.h"
#include "area.h"
#include <cassert>

const Vector2 Tile::sizeVector = Vector2(Tile::size, Tile::size);

Tile::Tile(Area& location, Vector2 position)
:   location(location), position(position)
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

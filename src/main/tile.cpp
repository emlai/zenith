#include "tile.h"
#include "area.h"
#include "game.h"
#include "gui.h"
#include "world.h"
#include "engine/texture.h"
#include <cassert>

const Vector2 Tile::sizeVector = Vector2(Tile::size, Tile::size);

Tile::Tile(World& world, Vector2 position, boost::string_ref groundId)
:   world(world),
    position(position),
    groundSprite(*Game::groundSpriteSheet, getSpriteTextureRegion(Game::groundConfig, groundId))
{
}

void Tile::exist()
{
}

void Tile::render(Window& window, int zIndex) const
{
    switch (zIndex)
    {
        case 0:
            groundSprite.render(window, position * sizeVector);
            break;
        case 1:
            for (const auto& item : items)
                item->render(window, position * sizeVector);
            break;
        case 2:
            if (object)
                object->render(window, position * sizeVector);
            break;
        case 3:
            for (const auto& creature : creatures)
                creature->render(window);
            break;
        case 4:
        {
#ifdef TOOLTIP
            Rect tileRect(position * sizeVector, sizeVector);

            if (window.getMousePosition().isWithin(tileRect))
            {
                Game::cursorTexture->setColor(GUIColor::White);
                Game::cursorTexture->render(nullptr, &tileRect);

                window.getFont().setArea(tileRect.offset(Vector2(size * 1.2, 0)));
                window.getFont().print(getTooltip());
            }
#endif
            break;
        }
        default:
            assert(false);
    }

}

std::string Tile::getTooltip() const
{
    std::string tooltip;

    for (auto& creature : creatures)
    {
        tooltip += creature->getName();
        tooltip += '\n';
    }

    for (auto& item : items)
    {
        tooltip += item->getName();
        tooltip += '\n';
    }

    if (object)
    {
        tooltip += object->getName();
        tooltip += '\n';
    }

    return tooltip;
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

void Tile::removeCreature(Creature& creature)
{
    auto newEnd = std::remove_if(creatures.begin(), creatures.end(),
                                 [&](auto& ptr) { return ptr.get() == &creature; });
    creatures.erase(newEnd, creatures.end());
}

std::unique_ptr<Item> Tile::removeTopmostItem()
{
    auto item = std::move(items.back());
    items.pop_back();
    return item;
}

void Tile::addItem(std::unique_ptr<Item> item)
{
    items.push_back(std::move(item));
}

void Tile::setObject(std::unique_ptr<Object> newObject)
{
    object = std::move(newObject);
}

void Tile::setGround(boost::string_ref groundId)
{
    groundSprite = Sprite(*Game::groundSpriteSheet, getSpriteTextureRegion(Game::groundConfig, groundId));
}

Tile* Tile::getAdjacentTile(Dir8 direction) const
{
    return getWorld().getOrCreateTile(getPosition() + direction);
}

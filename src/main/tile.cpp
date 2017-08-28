#include "tile.h"
#include "area.h"
#include "game.h"
#include "gui.h"
#include "world.h"
#include "components/lightsource.h"
#include "engine/texture.h"
#include <cassert>

const int Tile::size = 20;
const Vector2 Tile::sizeVector = Vector2(Tile::size, Tile::size);

Tile::Tile(World& world, Vector2 position, int level, boost::string_ref groundId)
:   world(world),
    position(position),
    level(level),
    groundSprite(*Game::groundSpriteSheet, getSpriteTextureRegion(Game::groundConfig, groundId)),
    groundId(groundId),
    light(Color32::black)
{
}

void Tile::render(Window& window, int zIndex, bool fogOfWar) const
{
    switch (zIndex)
    {
        case 0:
            groundSprite.render(position * sizeVector);
            break;
        case 1:
            for (const auto& item : items)
                item->render(position * sizeVector);
            break;
        case 2:
            if (object)
                object->render(position * sizeVector);
            break;
        case 3:
            if (fogOfWar)
                break;

            for (const auto& creature : creatures)
                creature->render();
            break;
        case 4:
            if (fogOfWar)
                break;

            window.getGraphicsContext().renderRectangle(Rect(position * sizeVector, sizeVector),
                                                        light, BlendMode::LinearLight);
            break;
        case 5:
            if (fogOfWar)
                Game::fogOfWarTexture->render(position * sizeVector);
            break;
        case 6:
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

std::unique_ptr<Creature> Tile::removeSingleTileCreature(Creature& creature)
{
    assert(creature.getTilesUnder().size() == 1);

    for (auto it = creatures.begin(); it != creatures.end(); ++it)
    {
        if (it->get() == &creature)
        {
            auto removed = std::move(*it);
            creatures.erase(it);
            return removed;
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
    this->groundId = groundId.to_string();
    groundSprite = Sprite(*Game::groundSpriteSheet, getSpriteTextureRegion(Game::groundConfig, groundId));
}

std::vector<Entity*> Tile::getEntities() const
{
    std::vector<Entity*> entities;

    for (auto& creature : creatures)
    {
        entities.push_back(creature.get());

        if (creature->hasWieldedItem())
            entities.push_back(creature->getWieldedItem());
    }

    for (auto& item : items)
        entities.push_back(item.get());

    if (object)
        entities.push_back(object.get());

    return entities;
}

std::vector<LightSource*> Tile::getLightSources() const
{
    std::vector<LightSource*> lightSources;

    for (auto* entity : getEntities())
        for (auto* lightSource : entity->getComponentsOfType<LightSource>())
            lightSources.push_back(lightSource);

    return lightSources;
}

void Tile::emitLight()
{
    for (auto* lightSource : getLightSources())
        lightSource->emitLight(world, getCenterPosition(), level);
}

void Tile::resetLight()
{
    if (getLevel() >= 0)
        light = world.getSunlight();
    else
        light = Color32::black;
}

bool Tile::blocksSight() const
{
    return hasObject() && getObject()->blocksSight();
}

Tile* Tile::getAdjacentTile(Dir8 direction) const
{
    return getWorld().getOrCreateTile(getPosition() + direction, level);
}

Tile* Tile::getPreExistingAdjacentTile(Dir8 direction) const
{
    return getWorld().getTile(getPosition() + direction, level);
}

Tile* Tile::getTileBelow() const
{
    return getWorld().getOrCreateTile(getPosition(), level - 1);
}

Tile* Tile::getTileAbove() const
{
    return getWorld().getOrCreateTile(getPosition(), level + 1);
}

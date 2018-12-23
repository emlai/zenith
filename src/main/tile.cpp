#include "tile.h"
#include "area.h"
#include "game.h"
#include "gui.h"
#include "world.h"
#include "components/lightsource.h"
#include "engine/savefile.h"
#include "engine/texture.h"
#include <cassert>
#include <cmath>

const Vector2 Tile::spriteSize(20, 20);

Tile::Tile(World& world, Vector2 position, int level, std::string_view groundId)
:   world(world),
    position(position),
    level(level),
    groundId(groundId),
    groundSprite(getSprite(*Game::groundSpriteSheet, *Game::groundConfig, groundId)),
    light(Color::black)
{
}

Tile::Tile(const SaveFile& file, World& world, Vector2 position, int level)
:   world(world),
    position(position),
    level(level),
    groundId(file.readString()),
    groundSprite(getSprite(*Game::groundSpriteSheet, *Game::groundConfig, groundId)),
    light(Color::black)
{
    auto creatureCount = file.readInt32();
    creatures.reserve(size_t(creatureCount));
    for (int i = 0; i < creatureCount; ++i)
        creatures.push_back(std::make_unique<Creature>(file, this));

    auto itemCount = file.readInt32();
    items.reserve(size_t(itemCount));
    for (int i = 0; i < itemCount; ++i)
        items.push_back(Item::load(file));

    auto liquidCount = file.readInt32();
    liquids.reserve(size_t(liquidCount));
    for (int i = 0; i < liquidCount; ++i)
        liquids.push_back(Liquid(file));

    if (file.readBool())
        object = std::make_unique<Object>(file);
}

void Tile::save(SaveFile& file) const
{
    file.write(groundId);
    file.write(creatures);
    file.write(items);
    file.write(liquids);
    file.write(object != nullptr);
    if (object)
        object->save(file);
}

void Tile::exist()
{
    for (auto it = liquids.begin(); it != liquids.end();)
    {
        if (it->exists())
        {
            it->exist();
            ++it;
        }
        else
            it = liquids.erase(it);
    }

    for (auto& item : items)
        item->exist();
}

void Tile::render(Window& window, int zIndex, bool fogOfWar, bool renderLight) const
{
    Vector2 renderPosition = position * getSize();

    switch (zIndex)
    {
        case 0:
            groundSprite.render(window, renderPosition);

            for (auto& liquid : liquids)
                liquid.render(window, renderPosition);
            break;
        case 1:
            for (const auto& item : items)
                item->render(window, renderPosition);
            break;
        case 2:
            if (object)
                object->render(window, renderPosition);
            break;
        case 3:
            if (fogOfWar)
                break;

            for (const auto& creature : creatures)
                creature->render(window, renderPosition);
            break;
        case 4:
            if (fogOfWar || !renderLight)
                break;

            window.getGraphicsContext().renderFilledRectangle(Rect(renderPosition, getSize()),
                                                              light, BlendMode::LinearLight);
            break;
        case 5:
            if (fogOfWar)
                Game::fogOfWarTexture->render(window, renderPosition, getSize());
            break;
        case 6:
        {
            bool showTooltip = true;
            if (showTooltip)
            {
                Rect tileRect(renderPosition, getSize());

                if (window.getMousePosition().isWithin(tileRect))
                {
                    Game::cursorPosition = getPosition();
                    double cursorBreathRateMS = 150.0;
                    double sine = std::sin(SDL_GetTicks() / cursorBreathRateMS);
                    double minAlpha = 0.0;
                    double maxAlpha = 0.5;
                    double currentAlpha = minAlpha + (sine + 1) / 2 * (maxAlpha - minAlpha);
                    auto cursorColor = Color(0xFF, 0xFF, 0xFF, currentAlpha * 0xFF);
                    Game::cursorTexture->setColor(cursorColor);
                    Game::cursorTexture->render(window, tileRect);

                    auto tooltip = getTooltip();
                    if (!tooltip.empty())
                    {
                        int lineHeight = 2;
                        Rect lineArea(tileRect.position.x + getSize().x,
                                      tileRect.position.y + getSize().y / 2 - lineHeight / 2,
                                      getSize().x / 2, lineHeight);
                        window.getGraphicsContext().renderFilledRectangle(lineArea, GUIColor::Black);

                        Vector2 inset = Vector2(window.getFont().getColumnWidth(),
                                                window.getFont().getRowHeight() / 2);
                        Rect tooltipArea(Vector2(lineArea.getRight(), tileRect.getTop()),
                                         window.getFont().getTextSize(tooltip) + inset * 2);
                        window.getGraphicsContext().renderFilledRectangle(tooltipArea.inset(Vector2(0, 1)), GUIColor::Black);
                        window.getGraphicsContext().renderFilledRectangle(tooltipArea.inset(Vector2(1, 0)), GUIColor::Black);
                        window.getFont().setArea(tooltipArea.offset(inset));
                        window.getFont().print(window, tooltip, TextColor::White, GUIColor::Black, true, PreserveLines);
                    }
                }
            }
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

    for (auto& item : reverse(items))
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

void Tile::addLiquid(std::string_view materialId)
{
    liquids.push_back(Liquid(materialId));
}

void Tile::setObject(std::unique_ptr<Object> newObject)
{
    object = std::move(newObject);
}

void Tile::setGround(std::string_view groundId)
{
    this->groundId = groundId;
    groundSprite = getSprite(*Game::groundSpriteSheet, *Game::groundConfig, groundId);
}

std::vector<Entity*> Tile::getEntities() const
{
    std::vector<Entity*> entities;

    for (auto& creature : creatures)
    {
        entities.push_back(creature.get());

        for (auto slotAndItem : creature->getEquipment())
            if (slotAndItem.second)
                entities.push_back(slotAndItem.second);
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
        light = Color::black;
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

Vector2 Tile::getSize()
{
    return spriteSize;
}

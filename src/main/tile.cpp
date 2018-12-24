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
    if (file.readBool())
        spawnCreature(file);

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
    file.write(creature != nullptr);
    if (creature)
        creature->save(file);
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

void Tile::render(Window& window, RenderLayer layer, bool fogOfWar, bool renderLight) const
{
    Vector2 renderPosition = position * getSize();

    switch (layer)
    {
        case RenderLayer::Bottom:
            groundSprite.render(window, renderPosition);

            for (auto& liquid : liquids)
                liquid.render(window, renderPosition);

            for (const auto& item : items)
                item->render(window, renderPosition);

            if (object)
                object->render(window, renderPosition);

            if (fogOfWar)
            {
                Game::fogOfWarTexture->render(window, renderPosition, getSize());
            }
            else
            {
                if (creature)
                    creature->render(window, renderPosition);

                if (renderLight)
                    window.context.renderFilledRectangle(Rect(renderPosition, getSize()), light, BlendMode::LinearLight);
            }
            break;

        case RenderLayer::Top:
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
                        window.context.renderFilledRectangle(lineArea, GUIColor::Black);

                        Vector2 inset = Vector2(window.context.font->getColumnWidth(),
                                                window.context.font->getRowHeight() / 2);
                        Rect tooltipArea(Vector2(lineArea.getRight(), tileRect.getTop()),
                                         window.context.font->getTextSize(tooltip) + inset * 2);
                        window.context.renderFilledRectangle(tooltipArea.inset(Vector2(0, 1)), GUIColor::Black);
                        window.context.renderFilledRectangle(tooltipArea.inset(Vector2(1, 0)), GUIColor::Black);
                        window.context.font->setArea(tooltipArea.offset(inset));
                        window.context.font->print(window, tooltip, TextColor::White, GUIColor::Black, true, PreserveLines);
                    }
                }
            }
            break;
        }
    }
}

std::string Tile::getTooltip() const
{
    std::string tooltip;

    if (creature)
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

Creature* Tile::spawnCreature(std::string_view id, std::unique_ptr<Controller> controller)
{
    auto creature = world.addCreature(std::make_unique<Creature>(this, id, std::move(controller)));
    setCreature(creature);
    return creature;
}

Creature* Tile::spawnCreature(const SaveFile& file)
{
    auto creature = world.addCreature(std::make_unique<Creature>(this, file));
    setCreature(creature);
    return creature;
}

void Tile::removeCreature()
{
    creature = nullptr;
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

    if (creature)
    {
        entities.push_back(creature);

        for (auto item : creature->getEquipment())
            if (item)
                entities.push_back(item);
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

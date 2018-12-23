#include "gui.h"
#include "tile.h"
#include "engine/config.h"
#include <cctype>
#include <stdexcept>

static Rect getSpriteTextureRegion(const Config& config, std::string_view id)
{
    auto components = config.get<std::vector<int>>(id, "spritePosition");
    auto offsetX = randInt(config.get<int>(id, "spriteMultiplicity") - 1);
    return Rect(Vector2(components.at(0) + offsetX, components.at(1)) * Tile::spriteSize, Tile::spriteSize);
}

Sprite getSprite(const Texture& spriteSheet, const Config& config, std::string_view id,
                 int frame, Color32 materialColor)
{
    int animationFrames = frame == 0 ? config.get<int>(id, "animationFrames") : 1;
    Sprite sprite(spriteSheet, getSpriteTextureRegion(config, id), materialColor, animationFrames);
    sprite.setFrame(frame);
    return sprite;
}

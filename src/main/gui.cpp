#include "gui.h"
#include "tile.h"
#include "engine/config.h"
#include <cctype>
#include <stdexcept>

const Color White(0xFFEEEEFF);
const Color Gray(0x777788FF);
const Color DarkGray(0x222233FF);
const Color Black(0x000000FF);
const Color Red(0xFF1144FF);
const Color Green(0x22CC22FF);
const Color Blue(0x6666FFFF);
const Color Yellow(0xFFDD11FF);
const Color Brown(0xEE7711FF);

static Rect getSpriteTextureRegion(const Config& config, std::string_view id)
{
    auto components = config.get<std::vector<int>>(id, "spritePosition");
    auto offsetX = randInt(config.get<int>(id, "spriteMultiplicity") - 1);
    return Rect(Vector2(components.at(0) + offsetX, components.at(1)) * Tile::spriteSize, Tile::spriteSize);
}

Sprite getSprite(const Texture& spriteSheet, const Config& config, std::string_view id, int frame, Color materialColor)
{
    int animationFrames = frame == 0 ? config.get<int>(id, "animationFrames") : 1;
    Sprite sprite(spriteSheet, getSpriteTextureRegion(config, id), materialColor, animationFrames);
    sprite.setFrame(frame);
    return sprite;
}

#include "gui.h"
#include "tile.h"
#include "engine/config.h"
#include <cctype>
#include <stdexcept>

Rect getSpriteTextureRegion(const Config& config, boost::string_ref id)
{
    auto components = config.get<std::vector<int>>(id, "spritePosition");
    auto offsetX = randInt(config.get<int>(id, "spriteMultiplicity") - 1);
    return Rect(Vector2(components.at(0) + offsetX, components.at(1)) * Tile::spriteSize, Tile::spriteSize);
}

Sprite getSprite(const Texture& spriteSheet, const Config& config, boost::string_ref id,
                 int frame, Color32 materialColor)
{
    auto asciiSprite = config.getOptional<std::string>(id, "asciiSprite");

    if (!asciiSprite || asciiSprite->size() != 1 || !std::isprint((*asciiSprite)[0]))
        throw std::runtime_error("invalid asciiSprite on '" + id + "', should be 1 printable ASCII character");

    Color32 asciiColor(config.getOptional<uint32_t>(id, "asciiColor").get_value_or(materialColor.value));

    int animationFrames = frame == 0 ? config.get<int>(id, "animationFrames") : 1;
    Sprite sprite(spriteSheet, getSpriteTextureRegion(config, id), (*asciiSprite)[0], asciiColor,
                  materialColor, animationFrames);
    sprite.setFrame(frame);
    return sprite;
}

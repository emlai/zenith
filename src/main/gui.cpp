#include "gui.h"
#include "tile.h"
#include "engine/config.h"

Sprite getSprite(const Texture& spriteSheet, const Config& config, boost::string_ref id)
{
    int animationFrames = config.get<int>(id, "animationFrames");
    return Sprite(spriteSheet, getSpriteTextureRegion(config, id), Color32::none, animationFrames);
}

Rect getSpriteTextureRegion(const Config& config, boost::string_ref id)
{
    auto components = config.get<std::vector<int>>(id, "spritePosition");
    auto offsetX = randInt(config.get<int>(id, "spriteMultiplicity") - 1);
    return Rect(Vector2(components.at(0) + offsetX, components.at(1)) * Tile::size, Tile::sizeVector);
}

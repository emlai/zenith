#include "sprite.h"
#include "geometry.h"
#include "texture.h"

Sprite::Sprite(const Texture& texture, Rect textureRegion)
:   texture(texture), textureRegion(textureRegion)
{
}

void Sprite::render(Window& window, Vector2 position) const
{
    Rect target(position, textureRegion.size);
    texture.render(&textureRegion, &target);
}

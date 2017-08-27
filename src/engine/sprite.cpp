#include "sprite.h"
#include "geometry.h"
#include "texture.h"

Sprite::Sprite(const Texture& texture, Rect textureRegion, Color32 materialColor)
:   texture(&texture), textureRegion(textureRegion), materialColor(materialColor), frame(0)
{
}

void Sprite::render(Window& window, Vector2 position) const
{
    Rect source = textureRegion.offset(Vector2(frame * textureRegion.getWidth(), 0));
    Rect target(position, textureRegion.size);

    if (materialColor)
        texture->render(source, target, materialColor);
    else
        texture->render(source, target);
}

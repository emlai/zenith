#include "sprite.h"
#include "font.h"
#include "geometry.h"
#include "texture.h"
#include "window.h"
#include <SDL.h>

Sprite::Sprite(const Texture& texture, Rect textureRegion, Color materialColor, int animationFrames)
:   texture(&texture), textureRegion(textureRegion), materialColor(materialColor),
    animationFrames(animationFrames), frame(0)
{
}

Vector2 Sprite::getSize() const
{
    return textureRegion.size;
}

void Sprite::render(Window& window, Vector2 position, Vector2 sourceOffset) const
{
    int msPerAnimationFrame = window.getGraphicsContext().getAnimationFrameTime();
    int animationFrame = SDL_GetTicks() / msPerAnimationFrame % animationFrames;
    sourceOffset.x += (animationFrame + frame) * textureRegion.getWidth();
    Rect source = textureRegion.offset(sourceOffset);
    Rect target(position, textureRegion.size);

    if (materialColor)
        texture->render(window, source, target, materialColor);
    else
        texture->render(window, source, target);
}

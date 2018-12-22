#include "sprite.h"
#include "font.h"
#include "geometry.h"
#include "texture.h"
#include "window.h"
#include <SDL.h>

bool Sprite::asciiGraphics;
BitmapFont* Sprite::asciiGraphicsFont;

Sprite::Sprite(const Texture& texture, Rect textureRegion, char asciiGlyph, Color32 asciiColor,
               Color32 materialColor, int animationFrames)
:   texture(&texture), textureRegion(textureRegion), materialColor(materialColor),
    animationFrames(animationFrames), frame(0), asciiGlyph(asciiGlyph), asciiColor(asciiColor)
{
}

Vector2 Sprite::getSize() const
{
    if (useAsciiGraphics())
        return getAsciiGraphicsFont()->getCharSize();
    else
        return textureRegion.size;
}

void Sprite::render(Window& window, Vector2 position, Vector2 sourceOffset) const
{
    if (useAsciiGraphics())
    {
        bool blend = false;
        auto* font = getAsciiGraphicsFont();
        font->setArea(Rect(position, getSize()));
        font->print(window, std::string_view(&asciiGlyph, 1), asciiColor, Color32::none, blend);
    }
    else
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
}

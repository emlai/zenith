#include "color.h"
#include "geometry.h"

class BitmapFont;
class Texture;
class Window;

class Sprite
{
public:
    Sprite(const Texture& texture, Rect textureRegion, char asciiGlyph, Color32 asciiColor,
           Color32 materialColor = Color32::none, int animationFrames = 1);
    Vector2 getSize() const;
    int getWidth() const { return getSize().x; }
    int getHeight() const { return getSize().y; }
    void render(Window& window, Vector2 position, Vector2 sourceOffset = Vector2(0, 0)) const;
    Color32 getMaterialColor() const { return materialColor; }
    void setMaterialColor(Color32 color) { materialColor = color; }
    void setFrame(int newFrame) { frame = newFrame; }
    void setAsciiGlyph(char glyph) { asciiGlyph = glyph; }
    static bool useAsciiGraphics() { return asciiGraphics; }
    static void useAsciiGraphics(bool asciiGraphics) { Sprite::asciiGraphics = asciiGraphics; }
    static BitmapFont* getAsciiGraphicsFont() { return asciiGraphicsFont; }
    static void setAsciiGraphicsFont(BitmapFont* font) { asciiGraphicsFont = font; }

private:
    const Texture* texture;
    Rect textureRegion;
    Color32 materialColor;
    int animationFrames;
    int frame;
    char asciiGlyph;
    Color32 asciiColor;
    static bool asciiGraphics;
    static BitmapFont* asciiGraphicsFont;
};
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
        font->print(window, boost::string_ref(&asciiGlyph, 1), asciiColor, Color32::none, blend);
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

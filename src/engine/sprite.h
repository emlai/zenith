#pragma once

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

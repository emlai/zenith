class Sprite
{
    Texture texture;
    Rect textureRegion;
    Color32 materialColor;
    int animationFrames;
    int frame;
    char asciiGlyph;
    Color32 asciiColor;
    static bool asciiGraphics;
    static BitmapFont asciiGraphicsFont;

    int getWidth() { return getSize().x; }
    int getHeight() { return getSize().y; }
    Color32 getMaterialColor() { return materialColor; }
    void setMaterialColor(Color32 color) { materialColor = color; }
    void setFrame(int newFrame) { frame = newFrame; }
    void setAsciiGlyph(char glyph) { asciiGlyph = glyph; }
    static bool useAsciiGraphics() { return asciiGraphics; }
    static void useAsciiGraphics(bool asciiGraphics) { asciiGraphics = asciiGraphics; }
    static BitmapFont getAsciiGraphicsFont() { return asciiGraphicsFont; }
    static void setAsciiGraphicsFont(BitmapFont font) { asciiGraphicsFont = font; }

    Sprite(Texture texture, Rect textureRegion, char asciiGlyph, Color32 asciiColor,
        Color32 materialColor, int animationFrames)
    :   texture(texture), textureRegion(textureRegion), materialColor(materialColor),
    animationFrames(animationFrames), frame(0), asciiGlyph(asciiGlyph), asciiColor(asciiColor)
    {
    }

    Vector2 getSize()
    {
        if (useAsciiGraphics())
            return getAsciiGraphicsFont()->getCharSize();
        else
            return textureRegion.size;
    }

    void render(Window window, Vector2 position, Vector2 sourceOffset)
    {
        if (useAsciiGraphics())
        {
            bool blend = false;
            var font = getAsciiGraphicsFont();
            font->setArea(Rect(position, getSize()));
            font->print(window, string(asciiGlyph, 1), asciiColor, Color32::none, blend);
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
}

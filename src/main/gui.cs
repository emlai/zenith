static class GUIColor
{
    public static readonly Color32 White = Color32.fromColor16(0xFEEF);
    public static readonly Color32 Gray = Color32.fromColor16(0x778F);
    public static readonly Color32 Black = Color32.fromColor16(0x000F);
}

static class TextColor
{
    static readonly Color32 White = GUIColor.White;
    static readonly Color32 Gray = GUIColor.Gray;
    static readonly Color32 Red = Color32.fromColor16(0xF14F);
    static readonly Color32 Green = Color32.fromColor16(0x2C2F);
    static readonly Color32 Blue = Color32.fromColor16(0x66FF);
    static readonly Color32 Yellow = Color32.fromColor16(0xFD1F);
    static readonly Color32 Brown = Color32.fromColor16(0xE71F);
}

static class GUI
{
    const Vector2 spacing(12, 12);
    const int fontHeight = 12;
    const int questionAreaHeight = spacing.y + fontHeight + spacing.y;

    static Rect getSidebarArea(Window window)
    {
        var height = (window.getResolution().y - questionAreaHeight) / 2 - spacing.y / 2;
        var width = height * 3 / 4;
        return Rect(window.getResolution().x - width - spacing.x, questionAreaHeight, width, height);
    }

    static Rect getQuestionArea(Window window)
    {
        return Rect(spacing.x, spacing.y, window.getResolution().x - spacing.x * 2, fontHeight);
    }

    static Rect getWorldViewport(Window window)
    {
        var top = questionAreaHeight;
        return Rect(0, top, window.getResolution().x - getSidebarArea(window).getWidth() - spacing.x * 2,
                    window.getResolution().y - top);
    }

    static Rect getMessageArea(Window window)
    {
        var sidebarArea = getSidebarArea(window);
        return Rect(sidebarArea.getLeft(), sidebarArea.getBottom() + spacing.y,
                    sidebarArea.getWidth(), sidebarArea.getHeight());
    }

    static Rect getInventoryArea(Window window)
    {
        return getWorldViewport(window).inset(spacing);
    }

#if DEBUG
    static Rect getCommandLineArea(Window window)
    {
        var worldViewport = getWorldViewport(window);
        var questionArea = getQuestionArea(window);
        return Rect(worldViewport.getLeft() + spacing.x, worldViewport.getTop() + spacing.y,
                    worldViewport.getWidth() - spacing.x * 2, questionArea.getHeight());
    }

    static Rect getDebugMessageArea(Window window)
    {
        var commandLineArea = getCommandLineArea(window);
        return Rect(commandLineArea.getLeft(), commandLineArea.getBottom() + spacing.y,
                    commandLineArea.getWidth(), 60);
    }
#endif
}

static class SpriteTextures
{
    static Rect getSpriteTextureRegion(Config config, string id)
    {
        var components = config.get<List<int>>(id, "spritePosition");
        var offsetX = randInt(config.get<int>(id, "spriteMultiplicity") - 1);
        return Rect(Vector2(components.at(0) + offsetX, components.at(1)) * Tile::spriteSize, Tile::spriteSize);
    }

    static Sprite getSprite(Texture spriteSheet, Config config, string id, int frame, Color32 materialColor)
    {
        var asciiSprite = config.getOptional<string>(id, "asciiSprite");

        if (!asciiSprite || asciiSprite.size() != 1 || !std::isprint((asciiSprite)[0]))
            throw std::runtime_error("invalid asciiSprite on '" + id + "', should be 1 printable ASCII character");

        Color32 asciiColor(config.getOptional<uint>
        (id, "asciiColor").get_value_or(materialColor.value));

        int animationFrames = frame == 0 ? config.get<int>(id, "animationFrames") : 1;
        Sprite sprite(spriteSheet, getSpriteTextureRegion
        (config, id), (asciiSprite)[0], asciiColor,
        materialColor, animationFrames);
        sprite.setFrame(frame);
        return sprite;
    }
}

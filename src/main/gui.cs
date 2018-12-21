namespace GUIColor
{
    const Color16 White(0xFEEF);
    const Color16 Gray(0x778F);
    const Color16 Black(0x000F);
}

namespace TextColor
{
    const Color16 White(GUIColor::White);
    const Color16 Gray(GUIColor::Gray);
    const Color16 Red(0xF14F);
    const Color16 Green(0x2C2F);
    const Color16 Blue(0x66FF);
    const Color16 Yellow(0xFD1F);
    const Color16 Brown(0xE71F);
}

namespace GUI
{
    const Vector2 spacing(12, 12);
    const int fontHeight = 12;
    const int questionAreaHeight = spacing.y + fontHeight + spacing.y;

    Rect getSidebarArea(Window window)
    {
        var height = (window.getResolution().y - questionAreaHeight) / 2 - spacing.y / 2;
        var width = height * 3 / 4;
        return Rect(window.getResolution().x - width - spacing.x, questionAreaHeight, width, height);
    }

    Rect getQuestionArea(Window window)
    {
        return Rect(spacing.x, spacing.y, window.getResolution().x - spacing.x * 2, fontHeight);
    }

    Rect getWorldViewport(Window window)
    {
        var top = questionAreaHeight;
        return Rect(0, top, window.getResolution().x - getSidebarArea(window).getWidth() - spacing.x * 2,
                    window.getResolution().y - top);
    }

    Rect getMessageArea(Window window)
    {
        var sidebarArea = getSidebarArea(window);
        return Rect(sidebarArea.getLeft(), sidebarArea.getBottom() + spacing.y,
                    sidebarArea.getWidth(), sidebarArea.getHeight());
    }

    Rect getInventoryArea(Window window)
    {
        return getWorldViewport(window).inset(spacing);
    }

#ifdef DEBUG
    Rect getCommandLineArea(Window window)
    {
        var worldViewport = getWorldViewport(window);
        var questionArea = getQuestionArea(window);
        return Rect(worldViewport.getLeft() + spacing.x, worldViewport.getTop() + spacing.y,
                    worldViewport.getWidth() - spacing.x * 2, questionArea.getHeight());
    }

    Rect getDebugMessageArea(Window window)
    {
        var commandLineArea = getCommandLineArea(window);
        return Rect(commandLineArea.getLeft(), commandLineArea.getBottom() + spacing.y,
                    commandLineArea.getWidth(), 60);
    }
#endif
}

Sprite getSprite(Texture spriteSheet, Config config, string id,
                 int frame = 0, Color32 materialColor = Color32::none);
Rect getSpriteTextureRegion(Config config, string id)
{
    var components = config.get<List<int>>(id, "spritePosition");
    var offsetX = randInt(config.get<int>(id, "spriteMultiplicity") - 1);
    return Rect(Vector2(components.at(0) + offsetX, components.at(1)) * Tile::spriteSize, Tile::spriteSize);
}

Sprite getSprite(Texture spriteSheet, Config config, string id,
                 int frame, Color32 materialColor)
{
    var asciiSprite = config.getOptional<string>(id, "asciiSprite");

    if (!asciiSprite || asciiSprite.size() != 1 || !std::isprint((*asciiSprite)[0]))
        throw std::runtime_error("invalid asciiSprite on '" + id + "', should be 1 printable ASCII character");

    Color32 asciiColor(config.getOptional<uint>(id, "asciiColor").get_value_or(materialColor.value));

    int animationFrames = frame == 0 ? config.get<int>(id, "animationFrames") : 1;
    Sprite sprite(spriteSheet, getSpriteTextureRegion(config, id), (*asciiSprite)[0], asciiColor,
                  materialColor, animationFrames);
    sprite.setFrame(frame);
    return sprite;
}

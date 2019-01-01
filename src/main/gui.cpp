#include "gui.h"
#include "tile.h"
#include "engine/config.h"
#include "engine/math.h"
#include "engine/window.h"

const Color White(0xFFEEEEFF);
const Color Gray(0x777788FF);
const Color DarkGray(0x222233FF);
const Color Black(0x000000FF);
const Color Red(0xFF1144FF);
const Color Green(0x22CC22FF);
const Color Blue(0x6666FFFF);
const Color Yellow(0xFFDD11FF);
const Color Brown(0xEE7711FF);

static Rect getSpriteTextureRegion(const Config& config, std::string_view id)
{
    auto components = config.get<std::vector<int>>(id, "spritePosition");
    auto offsetX = randInt(config.get<int>(id, "spriteMultiplicity") - 1);
    return Rect(Vector2(components.at(0) + offsetX, components.at(1)) * Tile::spriteSize, Tile::spriteSize);
}

Sprite getSprite(const Texture& spriteSheet, const Config& config, std::string_view id, int frame, Color materialColor)
{
    int animationFrames = frame == 0 ? config.get<int>(id, "animationFrames") : 1;
    Sprite sprite(spriteSheet, getSpriteTextureRegion(config, id), materialColor, animationFrames);
    sprite.setFrame(frame);
    return sprite;
}

Rect GUI::getSidebarArea(const Window& window)
{
    auto height = (window.getResolution().y - questionAreaHeight) / 2 - spacing.y / 2;
    auto width = height * 3 / 4;
    return Rect(window.getResolution().x - width - spacing.x, questionAreaHeight, width, height);
}

Rect GUI::getQuestionArea(const Window& window)
{
    return Rect(spacing.x, spacing.y, window.getResolution().x - spacing.x * 2, fontHeight);
}

Rect GUI::getWorldViewport(const Window& window)
{
    auto top = questionAreaHeight;
    auto resolution = window.getResolution();
    return Rect(0, top, resolution.x - getSidebarArea(window).getWidth() - spacing.x * 2, resolution.y - top);
}

Rect GUI::getMessageArea(const Window& window)
{
    auto sidebarArea = getSidebarArea(window);
    return Rect(sidebarArea.getLeft(), sidebarArea.getBottom() + spacing.y,
                sidebarArea.getWidth(), sidebarArea.getHeight());
}

Rect GUI::getInventoryArea(const Window& window)
{
    return getWorldViewport(window).inset(spacing);
}

#ifdef DEBUG

Rect GUI::getCommandLineArea(const Window& window)
{
    auto worldViewport = getWorldViewport(window);
    auto questionArea = getQuestionArea(window);
    return Rect(worldViewport.getLeft() + spacing.x, worldViewport.getTop() + spacing.y,
                worldViewport.getWidth() - spacing.x * 2, questionArea.getHeight());
}

Rect GUI::getDebugMessageArea(const Window& window)
{
    auto commandLineArea = getCommandLineArea(window);
    return Rect(commandLineArea.getLeft(), commandLineArea.getBottom() + spacing.y,
                commandLineArea.getWidth(), 60);
}

#endif

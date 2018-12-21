#include "engine/color.h"
#include "engine/geometry.h"
#include "engine/window.h"
#include <boost/utility/string_ref.hpp>
#include <memory>
#include <vector>

class Config;
class Sprite;

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

    inline Rect getSidebarArea(const Window& window)
    {
        auto height = (window.getResolution().y - questionAreaHeight) / 2 - spacing.y / 2;
        auto width = height * 3 / 4;
        return Rect(window.getResolution().x - width - spacing.x, questionAreaHeight, width, height);
    }

    inline Rect getQuestionArea(const Window& window)
    {
        return Rect(spacing.x, spacing.y, window.getResolution().x - spacing.x * 2, fontHeight);
    }

    inline Rect getWorldViewport(const Window& window)
    {
        auto top = questionAreaHeight;
        return Rect(0, top, window.getResolution().x - getSidebarArea(window).getWidth() - spacing.x * 2,
                    window.getResolution().y - top);
    }

    inline Rect getMessageArea(const Window& window)
    {
        auto sidebarArea = getSidebarArea(window);
        return Rect(sidebarArea.getLeft(), sidebarArea.getBottom() + spacing.y,
                    sidebarArea.getWidth(), sidebarArea.getHeight());
    }

    inline Rect getInventoryArea(const Window& window)
    {
        return getWorldViewport(window).inset(spacing);
    }

#ifdef DEBUG
    inline Rect getCommandLineArea(const Window& window)
    {
        auto worldViewport = getWorldViewport(window);
        auto questionArea = getQuestionArea(window);
        return Rect(worldViewport.getLeft() + spacing.x, worldViewport.getTop() + spacing.y,
                    worldViewport.getWidth() - spacing.x * 2, questionArea.getHeight());
    }

    inline Rect getDebugMessageArea(const Window& window)
    {
        auto commandLineArea = getCommandLineArea(window);
        return Rect(commandLineArea.getLeft(), commandLineArea.getBottom() + spacing.y,
                    commandLineArea.getWidth(), 60);
    }
#endif
}

Sprite getSprite(const Texture& spriteSheet, const Config& config, boost::string_ref id,
                 int frame = 0, Color32 materialColor = Color32::none);
#include "gui.h"
#include "tile.h"
#include "engine/config.h"
#include <cctype>
#include <stdexcept>

Rect getSpriteTextureRegion(const Config& config, boost::string_ref id)
{
    auto components = config.get<std::vector<int>>(id, "spritePosition");
    auto offsetX = randInt(config.get<int>(id, "spriteMultiplicity") - 1);
    return Rect(Vector2(components.at(0) + offsetX, components.at(1)) * Tile::spriteSize, Tile::spriteSize);
}

Sprite getSprite(const Texture& spriteSheet, const Config& config, boost::string_ref id,
                 int frame, Color32 materialColor)
{
    auto asciiSprite = config.getOptional<std::string>(id, "asciiSprite");

    if (!asciiSprite || asciiSprite->size() != 1 || !std::isprint((*asciiSprite)[0]))
        throw std::runtime_error("invalid asciiSprite on '" + id + "', should be 1 printable ASCII character");

    Color32 asciiColor(config.getOptional<uint32_t>(id, "asciiColor").get_value_or(materialColor.value));

    int animationFrames = frame == 0 ? config.get<int>(id, "animationFrames") : 1;
    Sprite sprite(spriteSheet, getSpriteTextureRegion(config, id), (*asciiSprite)[0], asciiColor,
                  materialColor, animationFrames);
    sprite.setFrame(frame);
    return sprite;
}
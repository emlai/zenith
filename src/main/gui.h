#pragma once

#include "engine/color.h"
#include "engine/geometry.h"
#include "engine/window.h"
#include <string_view>
#include <memory>
#include <vector>

class Config;
class Sprite;

extern const Color White;
extern const Color Gray;
extern const Color DarkGray;
extern const Color Black;
extern const Color Red;
extern const Color Green;
extern const Color Blue;
extern const Color Yellow;
extern const Color Brown;

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

Sprite getSprite(const Texture& spriteSheet, const Config& config, std::string_view id,
                 int frame = 0, Color materialColor = Color::none);

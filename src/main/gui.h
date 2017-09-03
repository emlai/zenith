#pragma once

#include "engine/color.h"
#include "engine/geometry.h"
#include "engine/window.h"
#include <boost/utility/string_ref.hpp>
#include <vector>

class Config;

namespace GUIColor
{
    const Color16 White = 0xFEEF;
    const Color16 Gray = 0x778F;
    const Color16 Black = 0x001F;
}

namespace TextColor
{
    const Color16 White = GUIColor::White;
    const Color16 Gray = GUIColor::Gray;
    const Color16 Red = 0xF14F;
    const Color16 Green = 0x2C2F;
    const Color16 Blue = 0x66FF;
    const Color16 Yellow = 0xFD1F;
    const Color16 Brown = 0xE71F;
}

namespace GUI
{
    const Vector2 windowSize = Vector2(640, 480);
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

Rect getSpriteTextureRegion(const Config& config, boost::string_ref id);

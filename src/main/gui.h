#pragma once

#include "tile.h"
#include "engine/color.h"
#include "engine/config.h"
#include "engine/geometry.h"
#include "engine/window.h"
#include <boost/utility/string_ref.hpp>
#include <vector>

namespace GUIColor
{
    const Color16 White = 0xFEEF;
    const Color16 Gray = 0xBBBF;
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

    inline Rect getWorldViewport(const Window& window)
    {
        return Rect(24, 36, window.getResolution().x - 160, window.getResolution().y - 132);
    }

    inline Rect getSidebarArea(const Window& window)
    {
        return Rect(window.getResolution().x - 84, 48, 84, window.getResolution().y - 24);
    }

    inline Rect getMessageArea(const Window& window)
    {
        return Rect(36, window.getResolution().y - 85, window.getResolution().x - 136, 60);
    }

#ifdef DEBUG
    const Vector2 commandLinePosition = Vector2(36, 36);

    inline Rect getDebugMessageArea(const Window& window)
    {
        return Rect(36, 48, window.getResolution().x - 124, 60);
    }
#endif
}

inline Rect getSpriteTextureRegion(const Config& config, boost::string_ref id)
{
    auto components = config.get<std::vector<int>>(id, "spritePosition");
    auto offsetX = randInt(config.get<int>(id, "spriteMultiplicity") - 1);
    return Rect(Vector2(components.at(0) + offsetX, components.at(1)) * Tile::size, Tile::sizeVector);
}

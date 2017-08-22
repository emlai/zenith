#pragma once

#include "tile.h"
#include "engine/color.h"
#include "engine/config.h"
#include "engine/geometry.h"
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
    const Rect viewport = Rect(24, 36, windowSize.x - 160, windowSize.y - 132);
    const Rect sidebar = Rect(int(windowSize.x - 84), 48, 84, windowSize.y - 24);
    const Rect messageArea = Rect(36, int(windowSize.y - 108), windowSize.x - 136, 60);
#ifdef DEBUG
    const Vector2 commandLinePosition = Vector2(36, 36);
    const Rect debugMessageArea = Rect(36, 48, windowSize.x - 124, 60);
#endif
}

inline Rect getSpriteTextureRegion(const Config& config, boost::string_ref id)
{
    auto components = config.get<std::vector<int>>(id.to_string(), "spritePosition");
    auto offsetX = randInt(config.get<int>(id.to_string(), "spriteMultiplicity") - 1);
    return Rect(components.at(0) + offsetX * Tile::size, components.at(1), Tile::size, Tile::size);
}

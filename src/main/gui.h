#pragma once

#include "engine/color.h"
#include "engine/geometry.h"
#include <string_view>

struct Color;
class Config;
class Sprite;
class Texture;
class Window;

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

    Rect getSidebarArea(const Window& window);
    Rect getQuestionArea(const Window& window);
    Rect getWorldViewport(const Window& window);
    Rect getMessageArea(const Window& window);
    Rect getInventoryArea(const Window& window);

#ifdef DEBUG
    Rect getCommandLineArea(const Window& window);
    Rect getDebugMessageArea(const Window& window);
#endif
}

Sprite getSprite(const Texture& spriteSheet, const Config& config, std::string_view id,
                 int frame = 0, Color materialColor = Color::none);

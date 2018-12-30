#pragma once

#include "color.h"
#include "geometry.h"
#include <SDL.h>
#include <memory>
#include <vector>

struct SDL_Renderer;
struct SDL_Surface;

struct Rect;
class Window;

class Texture
{
public:
    Texture(std::string_view fileName, Color transparentColor = Color::none);
    Texture(uint32_t pixelFormat, Vector2 size);
    void setBlendMode(bool);
    void setColor(Color) const;
    void render(Window& window, Vector2 position, Vector2 size = Vector2::zero) const;
    void render(Window& window, Rect target) const;
    void render(Window& window, Rect source, Rect target) const;
    void render(Window& window, Rect source, Rect target, Color materialColor) const;
    Vector2 getSize() const;
    int getWidth() const;
    int getHeight() const;

    std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)> surface;
};

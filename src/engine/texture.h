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
    Texture(boost::string_ref fileName, Color32 transparentColor = Color32::none);
    Texture(uint32_t pixelFormat, Vector2 size);
    void setBlendMode(bool);
    void setColor(Color32) const;
    void render(Window& window, Vector2 position, Vector2 size = Vector2::zeroVector) const;
    void render(Window& window, Rect target) const;
    void render(Window& window, Rect source, Rect target) const;
    void render(Window& window, Rect source, Rect target, Color32 materialColor) const;
    Vector2 getSize() const;
    int getWidth() const;
    int getHeight() const;
    SDL_Surface* getSurface() const { return surface.get(); }

private:
    std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)> surface;
    std::vector<Color32> pixelData;
};

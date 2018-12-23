#pragma once

#include "color.h"
#include "geometry.h"
#include "texture.h"
#include <SDL.h>
#include <optional>
#include <memory>

class BitmapFont;
class Window;

enum class BlendMode { Normal, LinearLight };

class GraphicsContext
{
public:
    GraphicsContext(const Window&);
    void setViewport(const Rect* viewport);
    Rect getViewport() const;
    void setView(const Rect* view);
    const Rect* getView() const { return &*view; }
    void setScale(double scale);
    double getScale() const;
    void setAnimationFrameRate(int framesPerSecond);
    void updateScreen();
    void renderRectangle(Rect rectangle, Color color);
    void renderFilledRectangle(Rect rectangle, Color color, BlendMode blendMode = BlendMode::Normal);
    void clearScreen();
    Vector2 mapFromTargetCoordinates(Vector2) const;
    Rect mapToTargetCoordinates(Rect) const;

    const Window& window;
    std::unique_ptr<SDL_Renderer, void (&)(SDL_Renderer*)> renderer;
    std::unique_ptr<SDL_Texture, void (&)(SDL_Texture*)> framebuffer;
    Texture targetTexture;
    std::optional<Rect> viewport;
    std::optional<Rect> view;
    BitmapFont* font;
    int animationFrameTime;
};

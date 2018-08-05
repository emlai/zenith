#pragma once

#include "color.h"
#include "geometry.h"
#include "texture.h"
#include <SDL.h>
#include <boost/optional.hpp>
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
    const Rect* getView() const { return view.get_ptr(); }
    void setFont(BitmapFont&);
    BitmapFont& getFont();
    void setScale(double scale);
    double getScale() const;
    void setAnimationFrameRate(int framesPerSecond);
    int getAnimationFrameTime() const { return animationFrameTime; }
    void updateScreen();
    SDL_Renderer* getRenderer() const { return renderer.get(); }
    void renderRectangle(Rect rectangle, Color32 color);
    void renderFilledRectangle(Rect rectangle, Color32 color, BlendMode blendMode = BlendMode::Normal);

private:
    friend class Texture;
    friend class Window;

    void clearScreen();
    Vector2 mapFromTargetCoordinates(Vector2) const;
    Rect mapToTargetCoordinates(Rect) const;

    const Window& window;
    std::unique_ptr<SDL_Renderer, void (&)(SDL_Renderer*)> renderer;
    std::unique_ptr<SDL_Texture, void (&)(SDL_Texture*)> framebuffer;
    Texture targetTexture;
    boost::optional<Rect> viewport;
    boost::optional<Rect> view;
    BitmapFont* font;
    int animationFrameTime;
};

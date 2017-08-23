#pragma once

#include "color.h"
#include "geometry.h"
#include <SDL.h>
#include <boost/optional.hpp>
#include <memory>

class TargetTexture;
class BitmapFont;
class Window;

class GraphicsContext
{
public:
    GraphicsContext(const Window&);
    void setDrawColor(Color32);
    void setClearColor(Color32);
    void setViewport(const Rect* viewport);
    Rect getViewport() const;
    void setView(const Rect* view);
    const Rect* getView() const { return view.get_ptr(); }
    void setFont(BitmapFont&);
    BitmapFont& getFont();
    void setRenderTarget(TargetTexture*);
    void setAnimationFrameRate(int framesPerSecond);
    auto getAnimationFrameTime() const { return animationFrameTime; }
    void updateScreen();
    SDL_Renderer* getRenderer() const { return renderer.get(); }

private:
    friend class Texture;

    void applyFrameLimit();
    void clearScreen();
    void setRenderColor(Color32);

    std::unique_ptr<SDL_Renderer, void (&)(SDL_Renderer*)> renderer;
    Color32 drawColor;
    Color32 clearColor;
    boost::optional<Rect> view;
    BitmapFont* font;
    int animationFrameTime;
};

#pragma once

#include "color.h"
#include <SDL.h>
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
    void setFont(BitmapFont&);
    BitmapFont& getFont();
    void setRenderTarget(TargetTexture*);
    void setAnimationFrameRate(int framesPerSecond);
    auto getAnimationFrameTime() const { return animationFrameTime; }
    void updateScreen();
    SDL_Renderer* getRenderer() const { return renderer.get(); }

private:
    void applyFrameLimit();
    void clearScreen();
    void setRenderColor(Color32);

    std::unique_ptr<SDL_Renderer, void (&)(SDL_Renderer*)> renderer;
    Color32 drawColor;
    Color32 clearColor;
    BitmapFont* font;
    int animationFrameTime;
};

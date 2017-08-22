#include "graphics.h"
#include "texture.h"
#include "color.h"
#include "font.h"
#include "geometry.h"
#include "window.h"

GraphicsContext::GraphicsContext(const Window& window)
:   renderer(SDL_CreateRenderer(window.windowHandle.get(), -1,
                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE),
             SDL_DestroyRenderer),
    animationFrameTime(10)
{
    if (!renderer)
        throw std::runtime_error(SDL_GetError());

    SDL_SetRenderDrawColor(renderer.get(), 0x0, 0x0, 0x0, 0xFF);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer.get(), window.getWidth(), window.getHeight());
    SDL_RenderClear(renderer.get());
}

void GraphicsContext::setAnimationFrameRate(int framesPerSecond)
{
    animationFrameTime = 1000 / framesPerSecond;
}

void GraphicsContext::setDrawColor(Color32 color)
{
    drawColor = color;
    setRenderColor(color);
}

void GraphicsContext::setClearColor(Color32 color)
{
    clearColor = color;
    clearScreen();
}

void GraphicsContext::setViewport(const Rect* viewport)
{
    SDL_RenderSetViewport(renderer.get(), reinterpret_cast<const SDL_Rect*>(viewport));
}

void GraphicsContext::setView(const Rect* view)
{
    if (view)
        this->view = *view;
    else
        this->view = boost::none;
}

void GraphicsContext::setFont(BitmapFont& font)
{
    this->font = &font;
}

BitmapFont& GraphicsContext::getFont()
{
    return *font;
}

void GraphicsContext::setRenderTarget(TargetTexture* target)
{
    SDL_SetRenderTarget(renderer.get(), target ? target->getSDLTexture() : nullptr);
}

void GraphicsContext::updateScreen()
{
    SDL_RenderPresent(renderer.get());
    clearScreen();
}

void GraphicsContext::clearScreen()
{
    setRenderColor(clearColor);
    SDL_RenderClear(renderer.get());
    setRenderColor(drawColor);
}

void GraphicsContext::setRenderColor(Color32 color)
{
    SDL_SetRenderDrawColor(renderer.get(),
                           static_cast<uint8_t>(color.getRed()),
                           static_cast<uint8_t>(color.getGreen()),
                           static_cast<uint8_t>(color.getBlue()),
                           static_cast<uint8_t>(color.getAlpha()));
}

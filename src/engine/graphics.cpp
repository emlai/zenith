#include "graphics.h"
#include "texture.h"
#include "color.h"
#include "font.h"
#include "geometry.h"
#include "window.h"

GraphicsContext::GraphicsContext(const Window& window)
:   renderer(SDL_CreateRenderer(window.windowHandle.get(), -1, 0), SDL_DestroyRenderer),
    framebuffer(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                  window.getWidth(), window.getHeight()), SDL_DestroyTexture),
    targetTexture(window, SDL_PIXELFORMAT_RGBA8888, window.getSize()),
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
    if (viewport)
        this->viewport = *viewport;
    else
        this->viewport = boost::none;
}

Rect GraphicsContext::getViewport() const
{
    if (viewport)
        return *viewport;

    return Rect(Vector2(0, 0), targetTexture.getSize());
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

void GraphicsContext::updateScreen()
{
    SDL_Surface* surface = targetTexture.getSurface();
    SDL_UpdateTexture(framebuffer.get(), nullptr, surface->pixels, surface->pitch);
    SDL_RenderCopy(renderer.get(), framebuffer.get(), nullptr, nullptr);
    SDL_RenderPresent(renderer.get());
    clearScreen();
}

void GraphicsContext::clearScreen()
{
    SDL_FillRect(targetTexture.getSurface(), nullptr, clearColor);
}

void GraphicsContext::setRenderColor(Color32 color)
{
    SDL_SetRenderDrawColor(renderer.get(),
                           static_cast<uint8_t>(color.getRed()),
                           static_cast<uint8_t>(color.getGreen()),
                           static_cast<uint8_t>(color.getBlue()),
                           static_cast<uint8_t>(color.getAlpha()));
}

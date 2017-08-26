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

void GraphicsContext::renderRectangle(Rect rectangle, Color32 color, BlendMode blendMode)
{
    if (view)
        rectangle.position -= view->position;

    rectangle.position += getViewport().position;

    switch (blendMode)
    {
        case BlendMode::Normal:
            uint8_t rgba[4];
            SDL_GetRenderDrawColor(renderer.get(), &rgba[0], &rgba[1], &rgba[2], &rgba[3]);
            setRenderColor(color);
            SDL_RenderFillRect(renderer.get(), reinterpret_cast<const SDL_Rect*>(&rectangle));
            SDL_SetRenderDrawColor(renderer.get(), rgba[0], rgba[1], rgba[2], rgba[3]);
            break;

        case BlendMode::LinearLight:
            SDL_Surface* targetSurface = targetTexture.getSurface();

            if (rectangle.getLeft() < 0 || rectangle.getTop() < 0
                || rectangle.getRight() >= targetSurface->w || rectangle.getBottom() >= targetSurface->h)
                return;

            double dstR = color.getRed() / 255.0;
            double dstG = color.getGreen() / 255.0;
            double dstB = color.getBlue() / 255.0;
            uint32_t* pixels = static_cast<uint32_t*>(targetSurface->pixels);
            auto targetWidth = targetSurface->w;

            for (auto y = rectangle.getTop(); y <= rectangle.getBottom(); ++y)
            {
                for (auto x = rectangle.getLeft(); x <= rectangle.getRight(); ++x)
                {
                    uint32_t* pixel = pixels + (y * targetWidth + x);
                    double srcR = ((*pixel & 0xFF000000) >> 24) / 255.0;
                    double srcG = ((*pixel & 0x00FF0000) >> 16) / 255.0;
                    double srcB = ((*pixel & 0x0000FF00) >> 8) / 255.0;

                    auto blendLinearLight = [](auto& src, auto dst)
                    {
                        if (dst > 0.5)
                            src += 2.0 * dst - 1.0;
                        else
                            src = src + 2.0 * dst - 1.0;
                    };

                    blendLinearLight(srcR, dstR);
                    blendLinearLight(srcG, dstG);
                    blendLinearLight(srcB, dstB);

                    auto wrap = [](auto& src)
                    {
                        if (src > 1.0)
                            src = 1.0;
                        else if (src < 0.0)
                            src = 0.0;
                    };

                    wrap(srcR);
                    wrap(srcG);
                    wrap(srcB);

                    *pixel = int(255 * srcR) << 24 | int(255 * srcG) << 16 | int(255 * srcB) << 8 | 255;
                }
            }
            break;
    }
}

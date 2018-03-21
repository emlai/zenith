#include "graphics.h"
#include "texture.h"
#include "color.h"
#include "font.h"
#include "geometry.h"
#include "window.h"

GraphicsContext::GraphicsContext(const Window& window)
:   window(window),
    renderer(SDL_CreateRenderer(window.windowHandle.get(), -1, 0), SDL_DestroyRenderer),
    framebuffer(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                  window.getResolution().x, window.getResolution().y), SDL_DestroyTexture),
    targetTexture(SDL_PIXELFORMAT_RGBA8888, window.getResolution()),
    animationFrameTime(10)
{
    if (!renderer)
        throw std::runtime_error(SDL_GetError());

    SDL_SetRenderDrawColor(renderer.get(), 0x0, 0x0, 0x0, 0xFF);
    SDL_RenderClear(renderer.get());
}

void GraphicsContext::setScale(double scale)
{
    SDL_RenderSetScale(renderer.get(), float(scale), float(scale));
    framebuffer.reset(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                        window.getResolution().x, window.getResolution().y));
    targetTexture = Texture(SDL_PIXELFORMAT_RGBA8888, window.getResolution());
}

double GraphicsContext::getScale() const
{
    float scale;
    SDL_RenderGetScale(renderer.get(), &scale, nullptr);
    return double(scale);
}

void GraphicsContext::setAnimationFrameRate(int framesPerSecond)
{
    animationFrameTime = 1000 / framesPerSecond;
}

void GraphicsContext::setViewport(const Rect* viewport)
{
    if (viewport)
        this->viewport = *viewport;
    else
        this->viewport = std::nullopt;
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
        this->view = std::nullopt;
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
    SDL_FillRect(targetTexture.getSurface(), nullptr, 0);
}

Vector2 GraphicsContext::mapFromTargetCoordinates(Vector2 position) const
{
    position /= getScale();
    position -= getViewport().position;

    if (view)
        position += view->position;

    return position;
}

Rect GraphicsContext::mapToTargetCoordinates(Rect rectangle) const
{
    if (view)
        rectangle.position -= view->position;

    rectangle.position += getViewport().position;
    // TODO: Clip based on viewport size.

    return rectangle;
}

void GraphicsContext::renderRectangle(Rect rectangle, Color32 color)
{
    rectangle = mapToTargetCoordinates(rectangle);
    SDL_Rect topLine = { rectangle.getLeft(), rectangle.getTop(), rectangle.getWidth(), 1 };
    SDL_Rect bottomLine = { rectangle.getLeft(), rectangle.getBottom(), rectangle.getWidth(), 1 };
    SDL_Rect leftLine = { rectangle.getLeft(), rectangle.getTop(), 1, rectangle.getHeight() };
    SDL_Rect rightLine = { rectangle.getRight(), rectangle.getTop(), 1, rectangle.getHeight() };
    SDL_FillRect(targetTexture.getSurface(), &topLine, color.value);
    SDL_FillRect(targetTexture.getSurface(), &bottomLine, color.value);
    SDL_FillRect(targetTexture.getSurface(), &leftLine, color.value);
    SDL_FillRect(targetTexture.getSurface(), &rightLine, color.value);
}

void GraphicsContext::renderFilledRectangle(Rect rectangle, Color32 color, BlendMode blendMode)
{
    rectangle = mapToTargetCoordinates(rectangle);

    switch (blendMode)
    {
        case BlendMode::Normal:
            SDL_FillRect(targetTexture.getSurface(), reinterpret_cast<const SDL_Rect*>(&rectangle), color.value);
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

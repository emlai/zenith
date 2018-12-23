#include "graphics.h"
#include "texture.h"
#include "color.h"
#include "font.h"
#include "geometry.h"
#include "window.h"
#include <cstring>

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

void GraphicsContext::updateScreen()
{
    void* pixels;
    int pitch;
    int result = SDL_LockTexture(framebuffer.get(), nullptr, &pixels, &pitch);
    assert(result == 0);

    SDL_Surface* surface = targetTexture.surface.get();
    std::memcpy(pixels, surface->pixels, surface->h * pitch);

    SDL_UnlockTexture(framebuffer.get());

    SDL_RenderCopy(renderer.get(), framebuffer.get(), nullptr, nullptr);
    SDL_RenderPresent(renderer.get());
    clearScreen();
}

void GraphicsContext::clearScreen()
{
    SDL_FillRect(targetTexture.surface.get(), nullptr, 0);
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

void GraphicsContext::renderRectangle(Rect rectangle, Color color)
{
    rectangle = mapToTargetCoordinates(rectangle);
    SDL_Rect topLine = { rectangle.getLeft(), rectangle.getTop(), rectangle.getWidth(), 1 };
    SDL_Rect bottomLine = { rectangle.getLeft(), rectangle.getBottom(), rectangle.getWidth(), 1 };
    SDL_Rect leftLine = { rectangle.getLeft(), rectangle.getTop(), 1, rectangle.getHeight() };
    SDL_Rect rightLine = { rectangle.getRight(), rectangle.getTop(), 1, rectangle.getHeight() };
    SDL_FillRect(targetTexture.surface.get(), &topLine, color.intValue());
    SDL_FillRect(targetTexture.surface.get(), &bottomLine, color.intValue());
    SDL_FillRect(targetTexture.surface.get(), &leftLine, color.intValue());
    SDL_FillRect(targetTexture.surface.get(), &rightLine, color.intValue());
}

void GraphicsContext::renderFilledRectangle(Rect rectangle, Color color, BlendMode blendMode)
{
    rectangle = mapToTargetCoordinates(rectangle);

    switch (blendMode)
    {
        case BlendMode::Normal:
            SDL_FillRect(targetTexture.surface.get(), reinterpret_cast<const SDL_Rect*>(&rectangle), color.intValue());
            break;

        case BlendMode::LinearLight:
            SDL_Surface* targetSurface = targetTexture.surface.get();
            auto left = rectangle.getLeft();
            auto top = rectangle.getTop();
            auto right = rectangle.getRight();
            auto bottom = rectangle.getBottom();

            if (left < 0 || top < 0 || right >= targetSurface->w || bottom >= targetSurface->h)
                return;

            float dstR = color.r / 255.0f;
            float dstG = color.g / 255.0f;
            float dstB = color.b / 255.0f;
            uint32_t* pixels = static_cast<uint32_t*>(targetSurface->pixels);
            auto targetWidth = targetSurface->w;

            for (auto y = top; y <= bottom; ++y)
            {
                for (auto x = left; x <= right; ++x)
                {
                    uint32_t* pixel = pixels + (y * targetWidth + x);

                    float srcR = ((*pixel & 0xFF000000) >> 24) / 255.0f;
                    float srcG = ((*pixel & 0x00FF0000) >> 16) / 255.0f;
                    float srcB = ((*pixel & 0x0000FF00) >> 8) / 255.0f;

                    srcR = (dstR > 0.5f) * (srcR + 2.0f * (dstR - 0.5f)) + (dstR <= 0.5f) * (srcR + 2.0f * dstR - 1.0f);
                    srcG = (dstG > 0.5f) * (srcG + 2.0f * (dstG - 0.5f)) + (dstG <= 0.5f) * (srcG + 2.0f * dstG - 1.0f);
                    srcB = (dstB > 0.5f) * (srcB + 2.0f * (dstB - 0.5f)) + (dstB <= 0.5f) * (srcB + 2.0f * dstB - 1.0f);

                    srcR = srcR < 0.0f ? 0.0f : srcR > 1.0f ? 1.0f : srcR;
                    srcG = srcG < 0.0f ? 0.0f : srcG > 1.0f ? 1.0f : srcG;
                    srcB = srcB < 0.0f ? 0.0f : srcB > 1.0f ? 1.0f : srcB;

                    *pixel = uint32_t(255 * srcR) << 24 | uint32_t(255 * srcG) << 16 | uint32_t(255 * srcB) << 8 | 255;
                }
            }
            break;
    }
}

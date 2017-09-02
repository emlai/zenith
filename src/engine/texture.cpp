#include "texture.h"
#include "geometry.h"
#include "window.h"
#include <SDL.h>
#include <stdexcept>

Texture::Texture(std::vector<Color32>&& pixelData, uint32_t pixelFormat,
                 Vector2 size)
:   surface(SDL_CreateRGBSurfaceWithFormatFrom(pixelData.data(), size.x, size.y,
                                               SDL_BITSPERPIXEL(pixelFormat),
                                               size.x * SDL_BYTESPERPIXEL(pixelFormat), pixelFormat),
            SDL_FreeSurface),
    pixelData(std::move(pixelData))
{
    setBlendMode(true);
}

Texture::Texture(boost::string_ref fileName, Color32 transparentColor)
:   surface(SDL_LoadBMP(fileName.to_string().c_str()), SDL_FreeSurface)
{
    if (!surface)
        throw std::runtime_error("Unable to load " + fileName + ": " + SDL_GetError());

    if (auto* converted = SDL_ConvertSurfaceFormat(surface.get(), SDL_PIXELFORMAT_RGBA8888, 0))
        surface.reset(converted);

    if (transparentColor)
    {
        auto colorKey = SDL_MapRGB(surface->format,
                                   static_cast<uint8_t>(transparentColor.getRed()),
                                   static_cast<uint8_t>(transparentColor.getGreen()),
                                   static_cast<uint8_t>(transparentColor.getBlue()));
        SDL_SetColorKey(surface.get(), 1, colorKey);
    }
}

Texture::Texture(uint32_t pixelFormat, Vector2 size)
:   surface(SDL_CreateRGBSurfaceWithFormat(0, size.x, size.y, 32, pixelFormat), SDL_FreeSurface)
{
}

void Texture::setBlendMode(bool state)
{
    SDL_SetSurfaceBlendMode(surface.get(), state ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
}

void Texture::render(Window& window, Vector2 position) const
{
    render(window, Rect(Vector2::zeroVector, getSize()), Rect(position, getSize()));
}

void Texture::render(Window& window, Rect target) const
{
    render(window, Rect(Vector2::zeroVector, getSize()), target);
}

void Texture::render(Window& window, Rect source, Rect target) const
{
    target = window.context.mapToTargetCoordinates(target);

    SDL_BlitSurface(surface.get(),
                    reinterpret_cast<SDL_Rect*>(&source),
                    window.context.targetTexture.getSurface(),
                    reinterpret_cast<SDL_Rect*>(&target));
}

// TODO: Move this functionality out of the engine to the game.
void Texture::render(Window& window, Rect source, Rect target, Color32 materialColor) const
{
    target = window.context.mapToTargetCoordinates(target);

    SDL_Surface* targetSurface = window.context.targetTexture.getSurface();
    const uint32_t* sourcePixels = static_cast<const uint32_t*>(surface->pixels);
    uint32_t* targetPixels = static_cast<uint32_t*>(targetSurface->pixels);
    auto sourceWidth = surface->w;
    auto targetWidth = targetSurface->w;

    uint32_t transparentColor;
    if (SDL_GetColorKey(surface.get(), &transparentColor) != 0)
        transparentColor = 0;

    for (auto y = source.getTop(); y <= source.getBottom(); ++y)
    {
        for (auto x = source.getLeft(); x <= source.getRight(); ++x)
        {
            uint32_t pixel = sourcePixels[y * sourceWidth + x];

            if (pixel == transparentColor)
                continue;

            const uint8_t* abgr = reinterpret_cast<const uint8_t*>(&sourcePixels[y * sourceWidth + x]);
            bool isMagenta = abgr[3] > 0 && abgr[3] == abgr[1] && abgr[2] == 0;

            if (isMagenta)
            {
                auto brightness = abgr[3] / 128.0;
                pixel = (materialColor * brightness).value;
            }

            auto targetY = y - source.getTop() + target.getTop();
            auto targetX = x - source.getLeft() + target.getLeft();
            targetPixels[targetY * targetWidth + targetX] = pixel;
        }
    }
}

Vector2 Texture::getSize() const
{
    return Vector2(surface->w, surface->h);
}

int Texture::getWidth() const
{
    return surface->w;
}

int Texture::getHeight() const
{
    return surface->h;
}

void Texture::setColor(Color32 color) const
{
    SDL_SetSurfaceColorMod(surface.get(),
                           uint8_t(color.getRed()),
                           uint8_t(color.getGreen()),
                           uint8_t(color.getBlue()));
}

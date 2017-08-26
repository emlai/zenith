#include "texture.h"
#include "geometry.h"
#include "window.h"
#include <SDL.h>
#include <stdexcept>

Texture::Texture(const Window& window, std::vector<Color32>&& pixelData, uint32_t pixelFormat,
                 Vector2 size)
:   window(window),
    surface(SDL_CreateRGBSurfaceWithFormatFrom(pixelData.data(), size.x, size.y,
                                               SDL_BITSPERPIXEL(pixelFormat),
                                               size.x * SDL_BYTESPERPIXEL(pixelFormat), pixelFormat),
            SDL_FreeSurface),
    pixelData(std::move(pixelData))
{
    setBlendMode(true);
}

Texture::Texture(const Window& window, boost::string_ref fileName, Color32 transparentColor)
:   window(window), surface(SDL_LoadBMP(fileName.to_string().c_str()), SDL_FreeSurface)
{
    if (!surface)
        throw std::runtime_error("Unable to load " + fileName + ": " + SDL_GetError());

    if (transparentColor)
    {
        auto colorKey = SDL_MapRGB(surface->format, transparentColor.getRed(),
                                   transparentColor.getGreen(), transparentColor.getBlue());
        SDL_SetColorKey(surface.get(), 1, colorKey);
    }
}

Texture::Texture(const Window& window, uint32_t pixelFormat, Vector2 size)
:   window(window),
    surface(SDL_CreateRGBSurfaceWithFormat(0, size.x, size.y, 32, pixelFormat), SDL_FreeSurface)
{
}

void Texture::setBlendMode(bool state)
{
    SDL_SetSurfaceBlendMode(surface.get(), state ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
}

void Texture::render(Rect source, Rect target) const
{
    if (window.context.view)
        target.position -= window.context.view->position;

    target.position += window.context.getViewport().position;
    // TODO: Clip based on viewport size.

    SDL_BlitSurface(surface.get(),
                    reinterpret_cast<SDL_Rect*>(&source),
                    window.context.targetTexture.getSurface(),
                    reinterpret_cast<SDL_Rect*>(&target));
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

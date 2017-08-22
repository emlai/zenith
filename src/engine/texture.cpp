#include "texture.h"
#include "geometry.h"
#include "window.h"
#include <SDL.h>
#include <stdexcept>

Texture::Texture(const Window& window, const void* pixelData, uint32_t pixelFormat, Vector2 size)
:   Texture(window, SDL_TEXTUREACCESS_STATIC, pixelFormat, size)
{
    SDL_UpdateTexture(sdlTexture.get(), nullptr, pixelData, size.x * SDL_BYTESPERPIXEL(pixelFormat));
    setBlendMode(true);
}

Texture::Texture(const Window& window, const std::string& fileName, Color32 transparentColor)
:   window(window), sdlTexture(nullptr, SDL_DestroyTexture)
{
    std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)&> surface(SDL_LoadBMP(fileName.c_str()),
                                                                     SDL_FreeSurface);
    if (!surface)
        throw std::runtime_error("Unable to load " + fileName + ": " + SDL_GetError());

    if (transparentColor)
    {
        auto colorKey = SDL_MapRGB(surface->format, transparentColor.getRed(),
                                   transparentColor.getGreen(), transparentColor.getBlue());
        SDL_SetColorKey(surface.get(), 1, colorKey);
    }

    sdlTexture.reset(SDL_CreateTextureFromSurface(getRenderer(), surface.get()));
}

Texture::Texture(const Window& window, SDL_TextureAccess textureAccess, uint32_t pixelFormat, Vector2 size)
:   window(window),
    sdlTexture(SDL_CreateTexture(getRenderer(), pixelFormat, textureAccess, size.x, size.y), SDL_DestroyTexture)
{
}

void Texture::setBlendMode(bool state)
{
    SDL_SetTextureBlendMode(sdlTexture.get(), state ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
}

SDL_Renderer* Texture::getRenderer() const
{
    return window.context.getRenderer();
}

void Texture::render(const Rect* source, const Rect* target) const
{
    if (target && window.context.view)
    {
        Rect offsetTarget = target->offset(-window.context.view->position);
        SDL_RenderCopy(getRenderer(), sdlTexture.get(),
                       reinterpret_cast<const SDL_Rect*>(source),
                       reinterpret_cast<const SDL_Rect*>(&offsetTarget));
    }
    else
    {
        SDL_RenderCopy(getRenderer(), sdlTexture.get(),
                       reinterpret_cast<const SDL_Rect*>(source),
                       reinterpret_cast<const SDL_Rect*>(target));
    }
}

Vector2 Texture::getSize() const
{
    Vector2 size;
    SDL_QueryTexture(sdlTexture.get(), nullptr, nullptr, &size.x, &size.y);
    return size;
}

int Texture::getWidth() const
{
    int width;
    SDL_QueryTexture(sdlTexture.get(), nullptr, nullptr, &width, nullptr);
    return width;
}

int Texture::getHeight() const
{
    int height;
    SDL_QueryTexture(sdlTexture.get(), nullptr, nullptr, nullptr, &height);
    return height;
}

void Texture::setColor(Color32 color) const
{
    SDL_SetTextureColorMod(getSDLTexture(),
                           uint8_t(color.getRed()),
                           uint8_t(color.getGreen()),
                           uint8_t(color.getBlue()));
}

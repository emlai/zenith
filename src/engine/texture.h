#pragma once

#include "color.h"
#include "geometry.h"
#include "graphics.h"
#include <SDL.h>
#include <memory>

struct SDL_Renderer;
struct SDL_Texture;

struct Rect;
class Window;

class Texture
{
public:
    Texture(const Window&, const void* pixelData, uint32_t pixelFormat, Vector2 size);
    Texture(const Window&, const std::string& fileName, Color32 transparentColor = Color32::none);
    void setBlendMode(bool);
    void setColor(Color32) const;
    void render(const Rect* source, const Rect* target) const;
    Vector2 getSize() const;
    int getWidth() const;
    int getHeight() const;

protected:
    Texture(const Window&, SDL_TextureAccess, uint32_t pixelFormat, Vector2 size);
    SDL_Texture* getSDLTexture() const { return sdlTexture.get(); }

private:
    SDL_Renderer* renderer;
    std::unique_ptr<SDL_Texture, void (&)(SDL_Texture*)> sdlTexture;
};

class TargetTexture : public Texture
{
public:
    TargetTexture(const Window& window, uint32_t pixelFormat, Vector2 size)
    :   Texture(window, SDL_TEXTUREACCESS_TARGET, pixelFormat, size)
    {
    }
    friend void GraphicsContext::setRenderTarget(TargetTexture*);
};

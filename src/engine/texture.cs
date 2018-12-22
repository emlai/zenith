class Texture
{
    SDL_Surface surface;
    List<Color32> pixelData;

    struct PixelFormatMasks
    {
        uint red, green, blue, alpha;
    }

    static PixelFormatMasks pixelFormatEnumToMasks(uint pixelFormat)
    {
        int bpp;
        PixelFormatMasks masks;

        if (!SDL_PixelFormatEnumToMasks(pixelFormat, bpp, masks.red, masks.green, masks.blue, masks.alpha))
            throw new Exception("SDL_PixelFormatEnumToMasks: invalid pixel format");

        return masks;
    }

    static SDL_Surface createSurfaceWithFormat(uint pixelFormat, Vector2 size)
    {
        PixelFormatMasks masks = pixelFormatEnumToMasks(pixelFormat);
        return SDL_CreateRGBSurface(0, size.x, size.y, SDL_BITSPERPIXEL(pixelFormat),
                                    masks.red, masks.green, masks.blue, masks.alpha);
    }

    Texture(string fileName, Color32 transparentColor)
    :   surface(SDL_LoadBMP(fileName.c_str()), SDL_FreeSurface)
    {
        if (!surface)
            throw new Exception("Unable to load " + fileName + ": " + SDL_GetError());

        if (var converted = SDL_ConvertSurfaceFormat(surface.get(), SDL_PIXELFORMAT_RGBA8888, 0))
            surface.reset(converted);

        if (transparentColor)
        {
            var colorKey = SDL_MapRGB(surface.format,
                                       static_cast<byte>(transparentColor.getRed()),
                                       static_cast<byte>(transparentColor.getGreen()),
                                       static_cast<byte>(transparentColor.getBlue()));
            SDL_SetColorKey(surface.get(), 1, colorKey);
        }

        setBlendMode(true);
    }

    Texture(uint pixelFormat, Vector2 size)
    :   surface(createSurfaceWithFormat(pixelFormat, size), SDL_FreeSurface)
    {
        setBlendMode(true);
    }

    void setBlendMode(bool state)
    {
        SDL_SetSurfaceBlendMode(surface.get(), state ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
    }

    void render(Window window, Vector2 position, Vector2 size)
    {
        if (size.isZero())
            size = getSize();

        render(window, Rect(Vector2::zeroVector, size), Rect(position, size));
    }

    void render(Window window, Rect target)
    {
        render(window, Rect(Vector2::zeroVector, getSize()), target);
    }

    void render(Window window, Rect source, Rect target)
    {
        target = window.context.mapToTargetCoordinates(target);

        SDL_BlitSurface(surface.get(),
                        reinterpret_cast<SDL_Rect>(source),
                        window.context.targetTexture.getSurface(),
                        reinterpret_cast<SDL_Rect>(target));
    }

    // TODO: Move this functionality out of the engine to the game.
    void render(Window window, Rect source, Rect target, Color32 materialColor)
    {
        target = window.context.mapToTargetCoordinates(target);

        SDL_Surface targetSurface = window.context.targetTexture.getSurface();
        uint sourcePixels = (uint) surface.pixels;
        uint targetPixels = (uint) targetSurface.pixels;
        var sourceWidth = surface.w;
        var targetWidth = targetSurface.w;

        uint transparentColor;
        if (SDL_GetColorKey(surface.get(), transparentColor) != 0)
            transparentColor = 0;

        for (var y = source.getTop(); y <= source.getBottom(); ++y)
        {
            for (var x = source.getLeft(); x <= source.getRight(); ++x)
            {
                uint pixel = sourcePixels[y * sourceWidth + x];

                if (pixel == transparentColor)
                    continue;

                byte abgr = reinterpret_cast<byte>(sourcePixels[y * sourceWidth + x]);
                bool isMagenta = abgr[3] > 0 && abgr[3] == abgr[1] && abgr[2] == 0;

                if (isMagenta)
                {
                    var brightness = abgr[3] / 128.0;
                    pixel = (materialColor * brightness).value;
                }

                var targetY = y - source.getTop() + target.getTop();
                var targetX = x - source.getLeft() + target.getLeft();
                targetPixels[targetY * targetWidth + targetX] = pixel;
            }
        }
    }

    Vector2 getSize()
    {
        return new Vector2(surface.w, surface.h);
    }

    int getWidth()
    {
        return surface.w;
    }

    int getHeight()
    {
        return surface.h;
    }

    void setColor(Color32 color)
    {
        SDL_SetSurfaceColorMod(surface.get(),
                               byte(color.getRed()),
                               byte(color.getGreen()),
                               byte(color.getBlue()));
        SDL_SetSurfaceAlphaMod(surface.get(),
                               byte(color.getAlpha()));
    }
}

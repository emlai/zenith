enum BlendMode { Normal, LinearLight }

class GraphicsContext
{
    Window window;
    SDL_Renderer renderer;
    SDL_Texture framebuffer;
    Texture targetTexture;
    Rect? viewport;
    Rect? view;
    BitmapFont font;
    int animationFrameTime;

    Rect getView() { return view.get_ptr(); }
    void setAnimationFrameRate(int framesPerSecond);
    var getAnimationFrameTime() { return animationFrameTime; }
    SDL_Renderer getRenderer() { return renderer.get(); }

    GraphicsContext(Window window)
    :   window(window),
        renderer(SDL_CreateRenderer(window.windowHandle.get(), -1, 0), SDL_DestroyRenderer),
        framebuffer(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                      window.getResolution().x, window.getResolution().y), SDL_DestroyTexture),
        targetTexture(SDL_PIXELFORMAT_RGBA8888, window.getResolution()),
        animationFrameTime(10)
    {
        if (!renderer)
            throw new Exception(SDL_GetError());

        SDL_SetRenderDrawColor(renderer.get(), 0x0, 0x0, 0x0, 0xFF);
        SDL_RenderClear(renderer.get());
    }

    void setScale(double scale)
    {
        SDL_RenderSetScale(renderer.get(), float(scale), float(scale));
        framebuffer.reset(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                            window.getResolution().x, window.getResolution().y));
        targetTexture = Texture(SDL_PIXELFORMAT_RGBA8888, window.getResolution());
    }

    double getScale()
    {
        float scale;
        SDL_RenderGetScale(renderer.get(), scale, null);
        return double(scale);
    }

    void setAnimationFrameRate(int framesPerSecond)
    {
        animationFrameTime = 1000 / framesPerSecond;
    }

    void setViewport(Rect viewport)
    {
        if (viewport)
            this.viewport = viewport;
        else
            this.viewport = null;
    }

    Rect getViewport()
    {
        if (viewport)
            return viewport;

        return Rect(Vector2(0, 0), targetTexture.getSize());
    }

    void setView(Rect view)
    {
        if (view)
            this.view = view;
        else
            this.view = null;
    }

    void setFont(BitmapFont font)
    {
        this.font = font;
    }

    BitmapFont getFont()
    {
        return font;
    }

    void updateScreen()
    {
        SDL_Surface surface = targetTexture.getSurface();
        SDL_UpdateTexture(framebuffer.get(), null, surface.pixels, surface.pitch);
        SDL_RenderCopy(renderer.get(), framebuffer.get(), null, null);
        SDL_RenderPresent(renderer.get());
        clearScreen();
    }

    void clearScreen()
    {
        SDL_FillRect(targetTexture.getSurface(), null, 0);
    }

    Vector2 mapFromTargetCoordinates(Vector2 position)
    {
        position /= getScale();
        position -= getViewport().position;

        if (view)
            position += view.position;

        return position;
    }

    Rect mapToTargetCoordinates(Rect rectangle)
    {
        if (view)
            rectangle.position -= view.position;

        rectangle.position += getViewport().position;
        // TODO: Clip based on viewport size.

        return rectangle;
    }

    void renderRectangle(Rect rectangle, Color32 color)
    {
        rectangle = mapToTargetCoordinates(rectangle);
        SDL_Rect topLine = { rectangle.getLeft(), rectangle.getTop(), rectangle.getWidth(), 1 }
        SDL_Rect bottomLine = { rectangle.getLeft(), rectangle.getBottom(), rectangle.getWidth(), 1 }
        SDL_Rect leftLine = { rectangle.getLeft(), rectangle.getTop(), 1, rectangle.getHeight() }
        SDL_Rect rightLine = { rectangle.getRight(), rectangle.getTop(), 1, rectangle.getHeight() }
        SDL_FillRect(targetTexture.getSurface(), topLine, color.value);
        SDL_FillRect(targetTexture.getSurface(), bottomLine, color.value);
        SDL_FillRect(targetTexture.getSurface(), leftLine, color.value);
        SDL_FillRect(targetTexture.getSurface(), rightLine, color.value);
    }

    void renderFilledRectangle(Rect rectangle, Color32 color, BlendMode blendMode)
    {
        rectangle = mapToTargetCoordinates(rectangle);

        switch (blendMode)
        {
            case BlendMode::Normal:
                SDL_FillRect(targetTexture.getSurface(), reinterpret_cast<SDL_Rect>(rectangle), color.value);
                break;

            case BlendMode::LinearLight:
                SDL_Surface targetSurface = targetTexture.getSurface();

                if (rectangle.getLeft() < 0 || rectangle.getTop() < 0
                    || rectangle.getRight() >= targetSurface.w || rectangle.getBottom() >= targetSurface.h)
                    return;

                double dstR = color.getRed() / 255.0;
                double dstG = color.getGreen() / 255.0;
                double dstB = color.getBlue() / 255.0;
                uint pixels = (uint) targetSurface.pixels;
                var targetWidth = targetSurface.w;

                for (var y = rectangle.getTop(); y <= rectangle.getBottom(); ++y)
                {
                    for (var x = rectangle.getLeft(); x <= rectangle.getRight(); ++x)
                    {
                        uint pixel = pixels + (y * targetWidth + x);
                        double srcR = ((pixel & 0xFF000000) >> 24) / 255.0;
                        double srcG = ((pixel & 0x00FF0000) >> 16) / 255.0;
                        double srcB = ((pixel & 0x0000FF00) >> 8) / 255.0;

                        var blendLinearLight = [](var src, var dst)
                        {
                            if (dst > 0.5)
                                src += 2.0 * dst - 1.0;
                            else
                                src = src + 2.0 * dst - 1.0;
                        }

                        blendLinearLight(srcR, dstR);
                        blendLinearLight(srcG, dstG);
                        blendLinearLight(srcB, dstB);

                        var wrap = [](var src)
                        {
                            if (src > 1.0)
                                src = 1.0;
                            else if (src < 0.0)
                                src = 0.0;
                        }

                        wrap(srcR);
                        wrap(srcG);
                        wrap(srcB);

                        pixel = (int) (255 * srcR) << 24 | (int) (255 * srcG) << 16 | (int) (255 * srcB) << 8 | 255;
                    }
                }
                break;
        }
    }
}


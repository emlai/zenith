#pragma once

#include "texture.h"
#include "graphics.h"
#include "keyboard.h"
#include <memory>

class Engine;
struct SDL_Window;

class Event
{
public:
    enum Type { KeyDown, MouseButtonDown };

    Event(Key key) : type(KeyDown), key(key) {}
    Event(Vector2 mousePosition) : type(MouseButtonDown), mousePosition(mousePosition) {}

    Type type;
    union
    {
        Key key;
        Vector2 mousePosition;
    };
};

class Window
{
public:
    Window(Engine& engine, std::string_view title = "", bool fullscreen = true);
    Window(Window&& window) = default;
    ~Window();
    Event waitForInput();
    Vector2 getMousePosition() const;
    void setShowCursor(bool show);
    void setFullscreen(bool enable);
    void toggleFullscreen();
    bool isFullscreen() const;
    void sendCloseRequest();
    void setViewport(const Rect* viewport) { context.setViewport(viewport); }
    void setView(const Rect* view) { context.setView(view); }
    void setFont(BitmapFont& font) { context.setFont(font); }
    BitmapFont& getFont() { return context.getFont(); }
    void setAnimationFrameRate(int fps) { context.setAnimationFrameRate(fps); }
    auto getAnimationFrameTime() const { return context.getAnimationFrameTime(); }
    void updateScreen() { context.updateScreen(); }
    bool shouldClose() const;
    Vector2 getResolution() const;
    Vector2 getSize() const;
    int getWidth() const;
    int getHeight() const;
    std::string_view getTitle() const;
    GraphicsContext& getGraphicsContext() { return context; }

    enum { CloseRequest = -2 };

private:
    friend class GraphicsContext;
    friend class Texture;

    static SDL_Window* initWindowHandle(const char* title, bool fullscreen);
    bool handleWindowEvent(int eventType);

    Engine* engine;
    bool closeRequestReceived;
    std::unique_ptr<SDL_Window, void (&)(SDL_Window*)> windowHandle;
    GraphicsContext context;
    static const int fullscreenFlag;
};

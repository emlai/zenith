#pragma once

#include "texture.h"
#include "graphics.h"
#include "keyboard.h"
#include <memory>

struct SDL_Window;

class Window
{
public:
    Window(Vector2 size, const std::string& title = "");
    ~Window();
    void processInput(KeyDownCallback);
    Key waitForInput();
    Key waitForInputWithTimeout(int timeoutMS);
    Vector2 getMousePosition() const;
    void setShowCursor(bool show);
    void toggleFullscreen();
    void sendCloseRequest();
    void setClearColor(Color32 color) { context.setClearColor(color); }
    void setViewport(const Rect* viewport) { context.setViewport(viewport); }
    void setView(const Rect* view) { context.setView(view); }
    void setFont(BitmapFont& font) { context.setFont(font); }
    BitmapFont& getFont() { return context.getFont(); }
    void setAnimationFrameRate(int fps) { context.setAnimationFrameRate(fps); }
    auto getAnimationFrameTime() const { return context.getAnimationFrameTime(); }
    void updateScreen() { context.updateScreen(); }
    bool shouldClose() const;
    Vector2 getSize() const;
    int getWidth() const;
    int getHeight() const;

    enum { CloseRequest = -2 };

private:
    friend class GraphicsContext;
    friend class Texture;

    static SDL_Window* initWindowHandle(Vector2 size, const char* title);
    bool handleWindowEvent(int eventType);

    bool closeRequestReceived;
    std::unique_ptr<SDL_Window, void (&)(SDL_Window*)> windowHandle;
    GraphicsContext context;
    static int windowCount;
    static bool sdlVideoInitialized;
};

#pragma once

#include "texture.h"
#include "graphics.h"
#include "keyboard.h"
#include <memory>

class StateManager;
struct SDL_Window;

class Event
{
public:
    enum Type { None, KeyDown, MouseButtonDown };

    Event() : type(None) {}
    explicit Event(Key key) : type(KeyDown), key(key) {}
    explicit Event(Vector2 mousePosition) : type(MouseButtonDown), mousePosition(mousePosition) {}
    explicit operator bool() const { return type != None; }

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
    Window(StateManager* stateManager, std::string_view title = "", bool fullscreen = true);
    ~Window();
    Event convertEvent(const SDL_Event& event);
    Event pollEvent();
    Event waitForInput();
    Vector2 getMousePosition() const;
    void setFullscreen(bool enable);
    void toggleFullscreen();
    bool isFullscreen() const;
    void sendCloseRequest();
    bool shouldClose() const;
    Vector2 getResolution() const;
    Vector2 getSize() const;
    static SDL_Window* initWindowHandle(const char* title, bool fullscreen);
    bool handleWindowEvent(int eventType);

    enum { CloseRequest = -2 };

    StateManager* stateManager; // FIXME: Window doesn't have to know about StateManager, only how to render.
    bool closeRequestReceived;
    std::unique_ptr<SDL_Window, void (&)(SDL_Window*)> windowHandle;
    GraphicsContext context;
};

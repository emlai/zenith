class Event
{
    enum Type { KeyDown, MouseButtonDown }

    Event(Key key) : type(KeyDown), key(key) {}
    Event(Vector2 mousePosition) : type(MouseButtonDown), mousePosition(mousePosition) {}

    Type type;
    union
    {
        Key key;
        Vector2 mousePosition;
    }
}

class Window
{
    enum { CloseRequest = -2 }

    Engine engine;
    bool closeRequestReceived;
    SDL_Window windowHandle;
    GraphicsContext context;
    static int windowCount;
    static bool sdlVideoInitialized;
    const int fullscreenFlag;
    int windowCount = 0;
    bool sdlVideoInitialized = false;
    const int fullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;

    void initializeSDLVideoSubsystem()
    {
        SDL_Init(SDL_INIT_VIDEO);
        sdlVideoInitialized = true;
    }

    SDL_Window initWindowHandle(Vector2 size, string title, bool fullscreen)
    {
        if (!sdlVideoInitialized)
            initializeSDLVideoSubsystem();

        uint windowFlags = SDL_WINDOW_ALLOW_HIGHDPI;

        if (fullscreen)
            windowFlags |= fullscreenFlag;

        SDL_Window windowHandle = SDL_CreateWindow(title,
                                                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                    size.x, size.y, windowFlags);

        if (!windowHandle)
            throw std::runtime_error(SDL_GetError());

        ++windowCount;
        return windowHandle;
    }

    Window(Engine engine, Vector2 size, string title, bool fullscreen)
    :   engine(engine),
        closeRequestReceived(false),
        windowHandle(initWindowHandle(size, title.c_str(), fullscreen), SDL_DestroyWindow),
        context(*this)
    {
        SDL_EventState(SDL_WINDOWEVENT_ENTER, SDL_IGNORE);
        SDL_EventState(SDL_WINDOWEVENT_LEAVE, SDL_IGNORE);
        SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
        SDL_EventState(SDL_MOUSEWHEEL, SDL_IGNORE);
        SDL_EventState(SDL_FINGERMOTION, SDL_IGNORE);
        SDL_EventState(SDL_FINGERDOWN, SDL_IGNORE);
        SDL_EventState(SDL_FINGERUP, SDL_IGNORE);
    }

    ~Window()
    {
        windowHandle.reset();
        --windowCount;

        if (windowCount == 0)
        {
            SDL_QuitSubSystem(SDL_INIT_VIDEO);
            sdlVideoInitialized = false;
        }
    }

    void setFullscreen(bool enable)
    {
        SDL_SetWindowFullscreen(windowHandle.get(), enable ? fullscreenFlag : 0);
    }

    void toggleFullscreen()
    {
        setFullscreen(!isFullscreen());
    }

    bool isFullscreen()
    {
        return SDL_GetWindowFlags(windowHandle.get()) & fullscreenFlag;
    }

    static int filterKeyRepeatEvents(void userdata, SDL_Event event)
    {
        if (event.type == SDL_KEYDOWN && event.key.repeat
            && event.key.keysym.sym == static_cast<SDL_Event>(userdata).key.keysym.sym)
            return 0;

        return 1;
    }

    Event waitForInput()
    {
        SDL_Event event;

        while (true)
        {
            engine.render(*this);
            updateScreen();

            if (!SDL_PollEvent(event))
                continue;

            switch (event.type)
            {
                case SDL_KEYDOWN:
                {
                    SDL_FilterEvents(filterKeyRepeatEvents, event);

                    var key = event.key.keysym.sym;

                    // Use shifts only as modifiers.
                    if (key == SDLK_LSHIFT || key == SDLK_RSHIFT)
                        continue;

                    if ((event.key.keysym.mod & Shift) && key > 0 && key <= UCHAR_MAX && std::isalpha(key))
                        return Event(Key(std::toupper(key)));
                    else
                        return Event(Key(key));
                }

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        Vector2 position(event.button.x, event.button.y);
                        return Event(context.mapFromTargetCoordinates(position));
                    }
                    break;

                case SDL_WINDOWEVENT:
                    if (handleWindowEvent(event.window.event))
                        return NoKey;
                    break;
            }
        }
    }

    bool handleWindowEvent(int eventType)
    {
        switch (eventType)
        {
            case SDL_WINDOWEVENT_CLOSE:
                sendCloseRequest();
                break;
            default:
                return false;
        }

        return true;
    }

    void sendCloseRequest()
    {
        closeRequestReceived = true;
    }

    bool shouldClose()
    {
        return closeRequestReceived;
    }

    Vector2 getMousePosition()
    {
        Vector2 position;
        SDL_GetMouseState(position.x, position.y);
        return context.mapFromTargetCoordinates(position);
    }

    void setShowCursor(bool show)
    {
        SDL_ShowCursor(show);
    }

    Vector2 getResolution()
    {
        return getSize() / context.getScale();
    }

    Vector2 getSize()
    {
        Vector2 size;
        SDL_GetWindowSize(windowHandle.get(), size.x, size.y);
        return size;
    }

    int getWidth()
    {
        int width;
        SDL_GetWindowSize(windowHandle.get(), width, null);
        return width;
    }

    int getHeight()
    {
        int height;
        SDL_GetWindowSize(windowHandle.get(), null, height);
        return height;
    }

    string getTitle()
    {
        return SDL_GetWindowTitle(windowHandle.get());
    }

    Vector2 getScreenResolution()
    {
        if (!sdlVideoInitialized)
            initializeSDLVideoSubsystem();

        SDL_DisplayMode mode;

        if (SDL_GetDesktopDisplayMode(0, mode) != 0)
            throw std::runtime_error(SDL_GetError());

        return Vector2(mode.w, mode.h);
    }
}

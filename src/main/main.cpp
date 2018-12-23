#include "action.h"
#include "game.h"
#include "gui.h"
#include "engine/engine.h"
#include "engine/menu.h"
#include "engine/filesystem.h"
#include "engine/geometry.h"
#include "engine/utility.h"
#include "engine/window.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>

static BitmapFont initFont()
{
    BitmapFont font("data/graphics/font-8x10.bmp", Vector2(8, 10));
    font.setDefaultColor(TextColor::White);
    font.setLineSpacing(1);
    return font;
}

class LoadingScreen : public State
{
public:
    LoadingScreen(std::string text) : text(std::move(text)) {}

    void render(Window& window) override
    {
        auto& font = window.getFont();
        auto oldLayout = font.getLayout();
        font.setArea(Rect(Vector2(0, 0), window.getResolution()));
        font.setLayout(TextLayout(HorizontalCenter, VerticalCenter));
        font.print(window, text);
        font.setLayout(oldLayout);
    }

    void execute()
    {
        getEngine().render(getEngine().getWindow());
        getEngine().getWindow().updateScreen();
    }

private:
    std::string text;
};

static const auto preferencesFileName = "prefs.cfg";

static void savePreferencesToFile(double graphicsScale, bool fullscreen)
{
    Config preferences;
    preferences.set("GraphicsScale", graphicsScale);
    preferences.set("Fullscreen", fullscreen);
    saveKeyMap(preferences);
    preferences.writeToFile(preferencesFileName);
}

static void setPrefsMenuCommonOptions(Menu& menu, const Window& window)
{
    menu.addItem(MenuItem(Menu::Exit, "Back", 'q'));
    menu.setItemLayout(Menu::Vertical);
    menu.setItemSize(Tile::getSize());
    menu.setTextLayout(TextLayout(LeftAlign, TopAlign));
    menu.setSecondaryColumnAlignment(RightAlign);
    menu.setArea(window.getResolution() / 4, window.getResolution() / 2);
    menu.setHotkeyStyle(Menu::LetterHotkeys);
}

class KeyMapMenu : public Menu
{
public:
    void execute();
};

void KeyMapMenu::execute()
{
    enum { ResetDefaults };
    auto& window = getEngine().getWindow();

    while (true)
    {
        clear();
        addTitle("Key map");

        for (int i = NoAction + 1; i < LastAction; ++i)
        {
            auto action = static_cast<Action>(i);

            if (auto key = getMappedKey(action))
            {
                auto text = pascalCaseToSentenceCase(toString(action));
                text[0] = std::toupper(text[0]);
                addItem(MenuItem(action, text, toString(key)));
            }
        }

        addItem(MenuItem(ResetDefaults, "Reset defaults"));
        setPrefsMenuCommonOptions(*this, window);

        auto selection = Menu::execute();

        switch (selection)
        {
            case ResetDefaults:
                loadKeyMap(nullptr);
                break;

            case Menu::Exit:
                return;

            default:
                auto event = window.waitForInput();

                if (event.type == Event::KeyDown && getMappedAction(event.key) == NoAction)
                    mapKey(event.key, static_cast<Action>(selection));

                break;
        }
    }
}

class PrefsMenu : public Menu
{
public:
    void execute();
};

void PrefsMenu::execute()
{
    enum { GraphicsScale, Fullscreen, KeyMap };
    auto& window = getEngine().getWindow();

    while (true)
    {
        clear();
        addTitle("Preferences");

        addItem(MenuItem(GraphicsScale, "Graphics scale",
                         toStringAvoidingDecimalPlaces(window.getGraphicsContext().getScale()) + "x"));
        addItem(MenuItem(Fullscreen, "Fullscreen", toOnOffString(window.isFullscreen())));
        addItem(MenuItem(KeyMap, "Key map"));

        setPrefsMenuCommonOptions(*this, window);

        auto selection = Menu::execute();

        switch (selection)
        {
            case GraphicsScale:
                if (window.getGraphicsContext().getScale() >= 2)
                    window.getGraphicsContext().setScale(1);
                else
                    window.getGraphicsContext().setScale(window.getGraphicsContext().getScale() + 0.5);
                break;
            case Fullscreen:
                window.toggleFullscreen();
                break;
            case KeyMap:
            {
                KeyMapMenu keyMapMenu;
                getEngine().execute(keyMapMenu);
                break;
            }
            case Menu::Exit:
                savePreferencesToFile(window.getGraphicsContext().getScale(), window.isFullscreen());
                return;
            default:
                assert(false);
        }
    }
}

class MainMenu : public Menu
{
public:
    void execute();
};

void MainMenu::execute()
{
    enum { NewGame, LoadGame, Preferences };

    std::unique_ptr<Game> game;

    while (true)
    {
        clear();

        if (game || fs::exists(Game::saveFileName))
            addItem(MenuItem(LoadGame, "Load game", 'l'));
        else
            addItem(MenuItem(NewGame, "New game", 'n'));

        addItem(MenuItem(Preferences, "Preferences", 'p'));
        addItem(MenuItem(Menu::Exit, "Quit", 'q'));
        setItemLayout(Menu::Horizontal);
        setItemSpacing(18);
        setTextLayout(TextLayout(HorizontalCenter, VerticalCenter));
        setArea(Vector2(0, 0), getEngine().getWindow().getResolution() / Vector2(1, 6));
        setHotkeyStyle(LetterHotkeys);

        auto selection = Menu::execute();

        switch (selection)
        {
            case NewGame:
            case LoadGame:
                if (!game)
                {
                    if (selection == LoadGame)
                    {
                        LoadingScreen loadingScreen("Loading game...");
                        getEngine().execute(loadingScreen);
                    }

                    rng.seed();
                    game = std::make_unique<Game>(selection == LoadGame);
                }

                try
                {
                    getEngine().execute(*game);
                }
                catch (...)
                {
                    game->save();
                    throw;
                }

                if (game->getPlayer()->isDead())
                {
                    std::remove(Game::saveFileName);
                    game = nullptr;
                }
                break;

            case Preferences:
            {
                PrefsMenu prefsMenu;
                getEngine().execute(prefsMenu);
                break;
            }

            case Menu::Exit:
            case Window::CloseRequest:
                if (game)
                {
                    LoadingScreen loadingScreen("Saving game...");
                    getEngine().execute(loadingScreen);
                    game->save();
                }
                return;
        }
    }
}

int main(int argc, char** argv)
{
    if (argc == 2 && std::string(argv[1]) == "--version")
    {
        std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << std::endl;
        return 0;
    }

    Engine engine;
    auto& window = engine.createWindow(Window::getScreenResolution(), PROJECT_NAME, true);
    window.setAnimationFrameRate(4);

    if (fs::exists(preferencesFileName))
    {
        Config preferences(preferencesFileName);
        window.getGraphicsContext().setScale(preferences.getOptional<double>("GraphicsScale").value_or(1));
        window.setFullscreen(preferences.getOptional<bool>("Fullscreen").value_or(true));
        loadKeyMap(&preferences);
    }
    else
        loadKeyMap(nullptr);

    BitmapFont font = initFont();
    window.setFont(font);
    Menu::setDefaultTextColor(TextColor::Gray);

    MainMenu mainMenu;

#ifdef DEBUG
    engine.execute(mainMenu);
#else
    try
    {
        engine.execute(mainMenu);
    }
    catch (const std::exception& exception)
    {
        engine.reportErrorToUser(std::string("Unhandled exception: ") + exception.what());
    }
#endif

    return 0;
}

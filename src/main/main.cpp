#include "game.h"
#include "gui.h"
#include "engine/engine.h"
#include "engine/menu.h"
#include "engine/geometry.h"
#include "engine/utility.h"
#include "engine/window.h"
#include <boost/filesystem.hpp>
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

static const auto preferencesFileName = "prefs.cfg";

static void savePreferencesToFile(bool asciiGraphics, double graphicsScale)
{
    Config preferences;
    preferences.set("ASCIIGraphics", asciiGraphics);
    preferences.set("GraphicsScale", graphicsScale);
    preferences.writeToFile(preferencesFileName);
}

class PrefsMenu : public Menu
{
public:
    void execute();
};

void PrefsMenu::execute()
{
    enum { AsciiGraphics, GraphicsScale };
    auto& window = getEngine().getWindow();
    int selection = 0;

    while (true)
    {
        clear();
        addTitle("Preferences");
        addItem(MenuItem(AsciiGraphics, "ASCII graphics", toOnOffString(Sprite::useAsciiGraphics())));
        addItem(MenuItem(GraphicsScale, "Graphics scale",
                         toStringAvoidingDecimalPlaces(window.getGraphicsContext().getScale()) + "x"));
        addItem(MenuItem(Menu::Exit, "Back", 'q'));
        setItemLayout(Menu::Vertical);
        setItemSize(Tile::getMaxSize());
        setTextLayout(TextLayout(LeftAlign, TopAlign));
        setSecondaryColumnAlignment(RightAlign);
        setArea(window.getResolution() / 4, window.getResolution() / 2);
        select(selection);
        selection = Menu::execute();

        switch (selection)
        {
            case AsciiGraphics:
                Sprite::useAsciiGraphics(!Sprite::useAsciiGraphics());
                break;
            case GraphicsScale:
                if (window.getGraphicsContext().getScale() >= 2)
                    window.getGraphicsContext().setScale(1);
                else
                    window.getGraphicsContext().setScale(window.getGraphicsContext().getScale() + 0.5);
                break;
            case Menu::Exit:
                savePreferencesToFile(Sprite::useAsciiGraphics(), window.getGraphicsContext().getScale());
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

        if (game || boost::filesystem::exists(Game::saveFileName))
            addItem(MenuItem(LoadGame, "Load game", 'l'));
        else
            addItem(MenuItem(NewGame, "New game", 'n'));

        addItem(MenuItem(Preferences, "Preferences", 'p'));
        addItem(MenuItem(Menu::Exit, "Quit", 'q'));
        setItemLayout(Menu::Horizontal);
        setItemSpacing(18);
        setTextLayout(TextLayout(HorizontalCenter, VerticalCenter));
        setArea(Vector2(0, 0), getEngine().getWindow().getResolution() / Vector2(1, 6));
        
        auto selection = Menu::execute();

        switch (selection)
        {
            case NewGame:
            case LoadGame:
                if (!game)
                {
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
                if (game) game->save();
                return;
        }
    }
}

int main(int argc, const char** argv)
{
    if (argc == 2 && std::string(argv[1]) == "--version")
    {
        std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << std::endl;
        return 0;
    }

    Engine engine;
    auto& window = engine.createWindow(Window::getScreenResolution(), PROJECT_NAME, true);
    window.setAnimationFrameRate(4);

    if (boost::filesystem::exists(preferencesFileName))
    {
        Config preferences(preferencesFileName);
        Sprite::useAsciiGraphics(preferences.getOptional<bool>("ASCIIGraphics").get_value_or(false));
        window.getGraphicsContext().setScale(preferences.getOptional<double>("GraphicsScale").get_value_or(1));
    }

    BitmapFont font = initFont();
    window.setFont(font);
    Sprite::setAsciiGraphicsFont(&font);
    Menu::setDefaultNormalColor(TextColor::Gray);
    Menu::setDefaultSelectionColor(TextColor::White);
    Menu::setDefaultSelectionOffset(Vector2(0, 1));

    MainMenu mainMenu;
    engine.execute(mainMenu);

    return 0;
}

#include "game.h"
#include "gui.h"
#include "engine/engine.h"
#include "engine/menu.h"
#include "engine/geometry.h"
#include "engine/window.h"
#include <boost/filesystem.hpp>
#include <cassert>
#include <iostream>
#include <memory>

static BitmapFont initFont()
{
    BitmapFont font("data/graphics/font.dat", Vector2(8, 10));
    font.setDefaultColor(TextColor::White);
    font.setLineSpacing(1);
    return font;
}

static const auto preferencesFileName = "prefs.cfg";

static void savePreferencesToFile(double graphicsScale)
{
    Config preferences;
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
    enum { GraphicsScale };
    auto& window = getEngine().getWindow();

    while (true)
    {
        clear();
        addTitle("Preferences");
        addItem(MenuItem(GraphicsScale, "Graphics scale",
                         toStringAvoidingDecimalPlaces(window.getGraphicsContext().getScale()) + "x"));
        addItem(MenuItem(Menu::Exit, "Back", 'q'));
        setItemLayout(Menu::Vertical);
        setItemSize(Tile::size);
        setTextLayout(TextLayout(LeftAlign, TopAlign));
        setSecondaryColumnAlignment(RightAlign);
        setArea(window.getResolution() / 4, window.getResolution() / 2);

        switch (Menu::execute())
        {
            case GraphicsScale:
                if (window.getGraphicsContext().getScale() >= 2)
                    window.getGraphicsContext().setScale(1);
                else
                    window.getGraphicsContext().setScale(window.getGraphicsContext().getScale() + 0.5);
                break;
            case Menu::Exit:
                savePreferencesToFile(window.getGraphicsContext().getScale());
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

    while (true)
    {
        clear();

        if (boost::filesystem::exists(Game::saveFileName))
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
            {
                rng.seed();
                Game game(selection == LoadGame);

                try
                {
                    getEngine().execute(game);
                }
                catch (...)
                {
                    game.save();
                    throw;
                }

                game.save();
                break;
            }

            case Preferences:
            {
                PrefsMenu prefsMenu;
                getEngine().execute(prefsMenu);
                break;
            }

            case Menu::Exit:
            case Window::CloseRequest:
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
        window.getGraphicsContext().setScale(preferences.getOptional<double>("GraphicsScale").value_or(1));
    }

    BitmapFont font = initFont();
    window.setFont(font);
    Menu::setDefaultNormalColor(TextColor::Gray);
    Menu::setDefaultSelectionColor(TextColor::White);
    Menu::setDefaultSelectionOffset(Vector2(0, 1));

    MainMenu mainMenu;
    engine.execute(mainMenu);

    return 0;
}

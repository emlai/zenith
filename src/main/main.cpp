#include "game.h"
#include "gui.h"
#include "engine/menu.h"
#include "engine/geometry.h"
#include "engine/window.h"
#include <boost/filesystem.hpp>
#include <cassert>
#include <iostream>

static BitmapFont initFont(Window& window)
{
    BitmapFont font("data/graphics/font.dat", Vector2(8, 10));
    font.setDefaultColor(TextColor::White);
    font.setLineSpacing(1);
    window.setFont(font);
    return font;
}

enum { NewGame, LoadGame, Preferences };

static Menu initMainMenu(Vector2 size)
{
    Menu menu;

    if (boost::filesystem::exists(Game::saveFileName))
        menu.addItem(MenuItem(LoadGame, "Load game", 'l'));
    else
        menu.addItem(MenuItem(NewGame, "New game", 'n'));

    menu.addItem(MenuItem(Preferences, "Preferences", 'p'));
    menu.addItem(MenuItem(Menu::Exit, "Quit", 'q'));
    menu.setItemLayout(Menu::Horizontal);
    menu.setItemSpacing(18);
    menu.setTextLayout(TextLayout(HorizontalCenter, VerticalCenter));
    menu.setArea(Vector2(0, 0), size / Vector2(1, 6));
    return menu;
}

static const auto preferencesFileName = "prefs.cfg";

static void savePreferencesToFile(double graphicsScale)
{
    Config preferences;
    preferences.set("GraphicsScale", graphicsScale);
    preferences.writeToFile(preferencesFileName);
}

static void showPreferences(Window& window)
{
    enum { GraphicsScale };

    while (true)
    {
        Menu menu;
        menu.addTitle("Preferences");
        menu.addItem(MenuItem(GraphicsScale, "Graphics scale",
                     toStringAvoidingDecimalPlaces(window.getGraphicsContext().getScale()) + "x"));
        menu.addItem(MenuItem(Menu::Exit, "Back", 'q'));
        menu.setItemLayout(Menu::Vertical);
        menu.setItemSize(Tile::size);
        menu.setTextLayout(TextLayout(LeftAlign, TopAlign));
        menu.setSecondaryColumnAlignment(RightAlign);
        menu.setArea(window.getResolution() / 4, window.getResolution() / 2);

        switch (menu.getChoice(window, window.getFont()))
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

int main(int argc, const char** argv)
{
    if (argc == 2 && std::string(argv[1]) == "--version")
    {
        std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << std::endl;
        return 0;
    }

    Config preferences(preferencesFileName);

    Window window(GUI::windowSize, PROJECT_NAME, true);
    window.getGraphicsContext().setScale(preferences.getOptional<double>("GraphicsScale").value_or(1));
    window.setAnimationFrameRate(4);
    BitmapFont font = initFont(window);
    Menu::setDefaultNormalColor(TextColor::Gray);
    Menu::setDefaultSelectionColor(TextColor::White);
    Menu::setDefaultSelectionOffset(Vector2(0, 1));

    for (bool exit = false; !exit && !window.shouldClose();)
    {
        Menu mainMenu = initMainMenu(window.getResolution());
        auto choice = mainMenu.getChoice(window, font);

        switch (choice)
        {
            case NewGame:
            case LoadGame:
            {
                rng.seed();
                Game game(window, choice == LoadGame);

                try
                {
                    game.run();
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
                showPreferences(window);
                break;
            case Menu::Exit:
            case Window::CloseRequest:
                exit = true;
                break;
        }
    }

    return 0;
}

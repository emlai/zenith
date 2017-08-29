#include "game.h"
#include "gui.h"
#include "engine/menu.h"
#include "engine/geometry.h"
#include "engine/window.h"
#include <cassert>
#include <iostream>

BitmapFont initFont(Window& window)
{
    BitmapFont font(window, "data/graphics/font.dat", Vector2(8, 10));
    font.setDefaultColor(TextColor::White);
    font.setDrawShadows(true);
    font.setShadowColorMod(0.3);
    font.setLineSpacing(2);
    window.setFont(font);
    return font;
}

enum { NewGame, LoadGame, Preferences };

Menu initMainMenu(Vector2 size)
{
    Menu menu;
    menu.addItem(NewGame, "New game", 'n');
    menu.addItem(LoadGame, "Load game", 'l');
    menu.addItem(Preferences, "Preferences", 'p');
    menu.addItem(Menu::Exit, "Quit", 'q');
    menu.setItemLayout(Menu::Horizontal);
    menu.setItemSpacing(18);
    menu.setTextLayout(TextLayout(HorizontalCenter, VerticalCenter));
    menu.setArea(Vector2(0, 0), size / Vector2(1, 6));
    return menu;
}

static void showPreferences(Window& window)
{
    enum { GraphicsScale };

    while (true)
    {
        Menu menu;
        menu.addTitle("Preferences");
        menu.addItem(GraphicsScale, "Graphics scale",
                     toStringAvoidingDecimalPlaces(window.getGraphicsContext().getScale()) + "x");
        menu.addItem(Menu::Exit, "Back", 'q');
        menu.setItemLayout(Menu::Vertical);
        menu.setItemSize(Tile::size);
        menu.setTextLayout(TextLayout(LeftAlign, TopAlign));
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

    Window window(GUI::windowSize, PROJECT_NAME, true);
    window.getGraphicsContext().setScale(1);
    window.setAnimationFrameRate(24);
    BitmapFont font = initFont(window);
    Menu::setDefaultNormalColor(TextColor::Gray);
    Menu::setDefaultSelectionColor(TextColor::White);
    Menu::setDefaultSelectionOffset(Vector2(0, 1));

    for (bool exit = false; !exit && !window.shouldClose();)
    {
        Menu mainMenu = initMainMenu(window.getResolution());

        switch (mainMenu.getChoice(window, font))
        {
            case NewGame:
            {
                rng.seed();
                Game game(window);
                game.run();
                break;
            }
            case LoadGame:
                break;
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

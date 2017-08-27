#include "game.h"
#include "gui.h"
#include "engine/menu.h"
#include "engine/geometry.h"
#include "engine/window.h"
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
    menu.setSelectionOffset(Vector2(0, 1));
    menu.setArea(Vector2(0, 0), size / Vector2(1, 6));
    return menu;
}

int main(int argc, const char** argv)
{
    if (argc == 2 && std::string(argv[1]) == "--version")
    {
        std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << std::endl;
        return 0;
    }

    Window window(GUI::windowSize, PROJECT_NAME);
    window.setAnimationFrameRate(24);
    BitmapFont font = initFont(window);
    Menu mainMenu = initMainMenu(window.getSize());

    for (bool exit = false; !exit && !window.shouldClose();)
    {
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
                break;
            case Menu::Exit:
            case Window::CloseRequest:
                exit = true;
                break;
        }
    }

    return 0;
}

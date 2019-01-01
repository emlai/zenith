#include "action.h"
#include "game.h"
#include "gui.h"
#include "engine/assert.h"
#include "engine/config.h"
#include "engine/menu.h"
#include "engine/filesystem.h"
#include "engine/geometry.h"
#include "engine/utility.h"
#include "engine/window.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>

using namespace std::literals;

static BitmapFont initFont()
{
    BitmapFont font("data/graphics/font-8x10.bmp", Vector2(8, 10));
    font.setDefaultColor(White);
    font.setLineSpacing(1);
    return font;
}

class LoadingScreen : public State
{
public:
    LoadingScreen(std::string text, std::function<void()> task) : text(std::move(text)), task(std::move(task)) {}

    void render() override
    {
        auto& font = *window->context.font;
        auto oldLayout = font.getLayout();
        font.setArea(Rect(Vector2(0, 0), window->getResolution()));
        font.setLayout(TextLayout(HorizontalCenter, VerticalCenter));
        font.print(*window, text);
        font.setLayout(oldLayout);
    }

    StateChange update() override
    {
        task();
        return StateChange::Pop();
    }

private:
    std::string text;
    std::function<void()> task;
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
    menu.addItem(Menu::Exit, "Back", 'q');
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
    enum { ResetDefaults };

    void render() override;
    StateChange update() override;
};

void KeyMapMenu::render()
{
    clear();
    setTitle("Key map");

    for (int i = NoAction + 1; i < LastAction; ++i)
    {
        auto action = static_cast<Action>(i);

        if (auto key = getMappedKey(action))
        {
            auto text = pascalCaseToSentenceCase(toString(action));
            text[0] = std::toupper(text[0]);
            addItem(action, text, toString(key));
        }
    }

    addItem(ResetDefaults, "Reset defaults");
    setPrefsMenuCommonOptions(*this, *window);

    Menu::render();
}

StateChange KeyMapMenu::update()
{
    auto selection = stateManager->getResult(Menu::update()).getInt();

    switch (selection)
    {
        case ResetDefaults:
            loadKeyMap(nullptr);
            return StateChange::None();

        case Menu::Exit:
            return StateChange::Pop();

        default:
            auto event = window->waitForInput();

            if (event.type == Event::KeyDown && getMappedAction(event.key) == NoAction)
                mapKey(event.key, static_cast<Action>(selection));

            return StateChange::None();
    }
}

class PrefsMenu : public Menu
{
public:
    enum { GraphicsScale, Fullscreen, KeyMap };

    void render() override;
    StateChange update() override;
};

void PrefsMenu::render()
{
    clear();
    setTitle("Preferences");
    addItem(GraphicsScale, "Graphics scale", toStringAvoidingDecimalPlaces(window->context.getScale()) + "x");
    addItem(Fullscreen, "Fullscreen", toOnOffString(window->isFullscreen()));
    addItem(KeyMap, "Key map");
    setPrefsMenuCommonOptions(*this, *window);

    Menu::render();
}

StateChange PrefsMenu::update()
{
    auto selection = stateManager->getResult(Menu::update()).getInt();

    switch (selection)
    {
        case GraphicsScale:
            window->context.setScale(window->context.getScale() >= 2 ? 1 : (window->context.getScale() + 0.5));
            return StateChange::None();

        case Fullscreen:
            window->toggleFullscreen();
            return StateChange::None();

        case KeyMap:
            return StateChange::Push(std::make_unique<KeyMapMenu>());

        case Menu::Exit:
            savePreferencesToFile(window->context.getScale(), window->isFullscreen());
            return StateChange::Pop();

        default:
            ASSERT(false);
    }
}

class MainMenu : public Menu
{
public:
    enum { NewGame, LoadGame, Preferences };

    MainMenu(GameState& gameState) : gameState(gameState) {}
    void render() override;
    StateChange update() override;

private:
    GameState& gameState;
};

void MainMenu::render()
{
    clear();

    if (gameState.isLoaded || fs::exists(Game::saveFileName))
        addItem(LoadGame, "Load game", 'l');
    else
        addItem(NewGame, "New game", 'n');

    addItem(Preferences, "Preferences", 'p');
    addItem(Menu::Exit, "Quit", 'q');
    setItemLayout(Menu::Horizontal);
    setTextLayout(TextLayout(HorizontalCenter, VerticalCenter));
    setArea(Vector2(0, 0), window->getResolution() / Vector2(1, 6));
    setHotkeyStyle(LetterHotkeys);

    Menu::render();
}

StateChange MainMenu::update()
{
    auto selection = stateManager->getResult(Menu::update()).getInt();

    switch (selection)
    {
        case NewGame:
        case LoadGame:
        {
            auto game = std::make_unique<Game>(&gameState);

            if (selection == NewGame)
            {
                gameState.init(&*game);
            }
            else if (!gameState.isLoaded)
            {
                stateManager->pushState(std::make_unique<LoadingScreen>("Loading game...", [&] { gameState.load(&*game); }));
                stateManager->wait();
            }

            return StateChange::Push(std::move(game));
        }
        case Preferences:
            return StateChange::Push(std::make_unique<PrefsMenu>());

        case Menu::Exit:
        case Window::CloseRequest:
            return StateChange::Pop();

        default:
            ASSERT(false);
    }
}

int main(int argc, char** argv)
{
    if (argc == 2 && std::string(argv[1]) == "--version")
    {
        std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << std::endl;
        return 0;
    }

    GameState gameState;
    StateManager stateManager;
    std::optional<Window> window;

    try
    {
        window.emplace(&stateManager, PROJECT_NAME, true);
        window->context.setAnimationFrameRate(4);
        stateManager.window = &*window;

        if (fs::exists(preferencesFileName))
        {
            Config preferences(preferencesFileName);
            window->context.setScale(preferences.getOptional<double>("GraphicsScale").value_or(1));
            window->setFullscreen(preferences.getOptional<bool>("Fullscreen").value_or(true));
            loadKeyMap(&preferences);
        }
        else
            loadKeyMap(nullptr);

        BitmapFont font = initFont();
        window->context.font = &font;
        Menu::setDefaultTextColor(Gray);

        stateManager.pushState(std::make_unique<MainMenu>(gameState));
        stateManager.wait();

        if (gameState.isLoaded)
        {
            stateManager.pushState(std::make_unique<LoadingScreen>("Saving game...", [&] { gameState.save(); }));
            stateManager.wait();
        }
    }
    catch (const std::exception& exception)
    {
        if (gameState.isLoaded)
            gameState.save();

        auto text = "Unhandled exception: "sv + exception.what();

        // TODO: Pass parent window here when it isn't initially made fullscreen.
        if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, PROJECT_NAME, text.c_str(), nullptr) != 0)
            std::cerr << text.c_str() << std::endl;

        throw;
    }

    return 0;
}

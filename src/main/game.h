#pragma once

#include "creature.h"
#include "world.h"
#include "engine/color.h"
#include "engine/keyboard.h"
#include "engine/state.h"
#include "engine/window.h"
#include <optional>
#include <string>

class GameState
{
public:
    void init(Game* game);
    void save();
    void load(Game* game);
    void removeSaveFile();

    World world;
    Creature* player = nullptr;
    int turn = 0;
    bool isLoaded = false;
};

class Game : public State
{
public:
    Game(GameState* state);
    StateChange update() override;
    void stop() { gameIsRunning = false; }
    int showInventory(std::string_view title, bool showNothingAsOption, std::function<bool(const Item&)> itemFilter = nullptr);
    void showEquipmentMenu();
    void lookMode();
    std::string askForString(std::string&& question);
    std::optional<Dir8> askForDirection(std::string&& question);
    Window& getWindow() const;
    World& getWorld() const { return gameState->world; }
    Creature* getPlayer() const { return gameState->player; }
    int getTurn() const { return gameState->turn; }
#ifdef DEBUG
    void enterCommandMode(Window&);
    static const Key commandModeKey = '`';
#endif
    static Tile* hoveredTile;
    bool playerSeesEverything;

    static std::unique_ptr<Config> creatureConfig;
    static std::unique_ptr<Config> objectConfig;
    static std::unique_ptr<Config> itemConfig;
    static std::unique_ptr<Config> groundConfig;
    static std::unique_ptr<Config> materialConfig;
    static std::unique_ptr<Texture> creatureSpriteSheet;
    static std::unique_ptr<Texture> objectSpriteSheet;
    static std::unique_ptr<Texture> itemSpriteSheet;
    static std::unique_ptr<Texture> groundSpriteSheet;
    static std::unique_ptr<Texture> cursorTexture;
    static std::unique_ptr<Texture> fogOfWarTexture;

    static constexpr auto saveFileName = "zenith.sav";

    void renderAtPosition(Window&, Vector2 centerPosition);
    void render() override;
    void renderSidebar(BitmapFont& font) const;
    void printStat(BitmapFont&, std::string_view, double current, double max, Color) const;
    void printAttribute(BitmapFont&, std::string_view, double current) const;

    bool gameIsRunning = true;
    GameState* gameState;

#ifdef DEBUG
    void parseCommand(std::string_view);
    bool showExtraInfo = true;
#endif
};

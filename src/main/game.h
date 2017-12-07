#pragma once

#include "creature.h"
#include "world.h"
#include "engine/config.h"
#include "engine/color.h"
#include "engine/keyboard.h"
#include "engine/state.h"
#include "engine/window.h"
#include <boost/optional.hpp>
#include <string>

class Game : public State
{
public:
    Game(bool loadSavedGame);
    Game(const Game&) = delete;
    Game(Game&&) = default;
    Game& operator=(const Game&) = delete;
    Game& operator=(Game&&) = default;
    void save();
    void load();
    void execute();
    void stop() { gameIsRunning = false; }
    void advanceTurn() { ++turn; }
    int getTurn() const { return turn; }
    int showInventory(boost::string_ref title, bool showNothingAsOption, Item* preselectedItem = nullptr,
                      std::function<bool(const Item&)> itemFilter = nullptr);
    void showEquipmentMenu();
    void lookMode();
    std::string askForString(std::string&& question);
    boost::optional<Dir8> askForDirection(std::string&& question);
    Creature* getPlayer() const { return player; }
    Window& getWindow() const;
#ifdef DEBUG
    void enterCommandMode(Window&);
    static const Key commandModeKey = '`';
#endif
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
    static constexpr auto hotkeySeparator = ") ";

private:
    friend class LookMode;
    void renderAtPosition(Window&, Vector2 centerPosition);
    void render(Window& window) override;
    void printPlayerInformation(BitmapFont&) const;
    void printStat(BitmapFont&, boost::string_ref, double current, double max, Color16) const;
    void printAttribute(BitmapFont&, boost::string_ref, double current) const;

    bool gameIsRunning;
    int turn;
    World world;
    Creature* player;

#ifdef DEBUG
    void parseCommand(boost::string_ref);

    bool showExtraInfo = true;
#endif
};

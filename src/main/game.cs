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
    static boost::optional<Vector2> cursorPosition;
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
#include "game.h"
#include "gui.h"
#include "item.h"
#include "msgsystem.h"
#include "tile.h"
#include "engine/engine.h"
#include "engine/keyboard.h"
#include "engine/math.h"
#include "engine/menu.h"
#include "engine/savefile.h"
#include <cmath>
#include <fstream>
#include <functional>

std::unique_ptr<Config> Game::creatureConfig;
std::unique_ptr<Config> Game::objectConfig;
std::unique_ptr<Config> Game::itemConfig;
std::unique_ptr<Config> Game::groundConfig;
std::unique_ptr<Config> Game::materialConfig;
std::unique_ptr<Texture> Game::creatureSpriteSheet;
std::unique_ptr<Texture> Game::objectSpriteSheet;
std::unique_ptr<Texture> Game::itemSpriteSheet;
std::unique_ptr<Texture> Game::groundSpriteSheet;
std::unique_ptr<Texture> Game::cursorTexture;
std::unique_ptr<Texture> Game::fogOfWarTexture;

static const Color32 transparentColor(0x5A5268FF);
boost::optional<Vector2> Game::cursorPosition;

Game::Game(bool loadSavedGame)
:   playerSeesEverything(false),
    turn(0),
    world(*this)
{
    creatureConfig = std::make_unique<Config>("data/config/creature.cfg");
    objectConfig = std::make_unique<Config>("data/config/object.cfg");
    itemConfig = std::make_unique<Config>("data/config/item.cfg");
    groundConfig = std::make_unique<Config>("data/config/ground.cfg");
    materialConfig = std::make_unique<Config>("data/config/material.cfg");

    creatureSpriteSheet = std::make_unique<Texture>("data/graphics/creature.bmp", transparentColor);
    objectSpriteSheet = std::make_unique<Texture>("data/graphics/object.bmp", transparentColor);
    itemSpriteSheet = std::make_unique<Texture>("data/graphics/item.bmp", transparentColor);
    groundSpriteSheet = std::make_unique<Texture>("data/graphics/ground.bmp");
    cursorTexture = std::make_unique<Texture>("data/graphics/cursor.bmp", transparentColor);
    fogOfWarTexture = std::make_unique<Texture>("data/graphics/fow.bmp", transparentColor);

    if (loadSavedGame)
        load();
    else
    {
        auto* tile = world.getOrCreateTile({0, 0}, 0);
        player = tile->spawnCreature("Human", std::make_unique<PlayerController>(*this));
    }
}

Window& Game::getWindow() const
{
    return getEngine().getWindow();
}

class InventoryMenu : public Menu
{
public:
    InventoryMenu(Window& window, const Creature& player, boost::string_ref title,
                  bool showNothingAsOption, Item* preselectedItem,
                  std::function<bool(const Item&)> itemFilter);
};

InventoryMenu::InventoryMenu(Window& window, const Creature& player, boost::string_ref title,
                             bool showNothingAsOption, Item* preselectedItem,
                             std::function<bool(const Item&)> itemFilter)
{
    addTitle(title);
    setArea(GUI::getInventoryArea(window));
    setItemSize(Tile::getMaxSize());
    setTextLayout(TextLayout(LeftAlign, Sprite::useAsciiGraphics() ? TopAlign : VerticalCenter));
    setTableCellSpacing(Vector2(window.getFont().getColumnWidth(), 0));
    setHotkeyStyle(LetterHotkeys);

    if (showNothingAsOption)
        addItem(MenuItem(-1, "nothing"));

    int id = 0;

    for (auto& item : player.getInventory())
    {
        if (!itemFilter || itemFilter(*item))
            addItem(MenuItem(id, item->getName(), NoKey, &item->getSprite()));

        ++id;
    }
}

int Game::showInventory(boost::string_ref title, bool showNothingAsOption, Item* preselectedItem,
                        std::function<bool(const Item&)> itemFilter)
{
    InventoryMenu inventoryMenu(getWindow(), *player, title, showNothingAsOption,
                                preselectedItem, std::move(itemFilter));
    return getEngine().execute(inventoryMenu);
}

class EquipmentMenu : public Menu
{
public:
    EquipmentMenu(Creature& player) : player(&player) {}
    void execute();

private:
    Creature* player;
};

void EquipmentMenu::execute()
{
    while (true)
    {
        clear();
        addTitle("Equipment");
        setArea(GUI::getInventoryArea(getEngine().getWindow()));
        setItemSize(Tile::getMaxSize());
        setTextLayout(TextLayout(LeftAlign, Sprite::useAsciiGraphics() ? TopAlign : VerticalCenter));
        setTableCellSpacing(Vector2(getEngine().getWindow().getFont().getColumnWidth(), 0));
        setHotkeyStyle(LetterHotkeys);

        for (int i = 0; i < equipmentSlots; ++i)
        {
            auto slot = static_cast<EquipmentSlot>(i);
            auto* image = player->getEquipment(slot) ? &player->getEquipment(slot)->getSprite() : nullptr;
            auto itemName = player->getEquipment(slot) ? player->getEquipment(slot)->getName() : "-";
            addItem(MenuItem(i, toString(slot) + ":", itemName, NoKey, nullptr, image));
        }

        auto choice = Menu::execute();
        if (choice == Menu::Exit)
            break;

        auto selectedSlot = static_cast<EquipmentSlot>(choice);

        InventoryMenu inventoryMenu(getEngine().getWindow(), *player, "", true,
                                    player->getEquipment(selectedSlot), [&](auto& item)
        {
            return item.getEquipmentSlot() == selectedSlot;
        });

        auto selectedItemIndex = getEngine().execute(inventoryMenu);

        if (selectedItemIndex == -1)
            player->equip(selectedSlot, nullptr);
        else if (selectedItemIndex != Menu::Exit)
            player->equip(selectedSlot, &*player->getInventory()[selectedItemIndex]);
    }
}

void Game::showEquipmentMenu()
{
    EquipmentMenu equipmentMenu(*player);
    getEngine().execute(equipmentMenu);
}

class LookMode : public State
{
public:
    LookMode(Game& game) : game(&game), position(game.player->getPosition()) {}
    void execute();

private:
    void render(Window& window) override;

    Game* game;
    Vector2 position;
};

void LookMode::execute()
{
    while (true)
    {
        Event event = getEngine().getWindow().waitForInput();

        if (event.type != Event::KeyDown)
            continue;

        switch (event.key)
        {
            case UpArrow: position += North; break;
            case RightArrow: position += East; break;
            case DownArrow: position += South; break;
            case LeftArrow: position += West; break;
            case Esc: return;
            default: break;
        }
    }
}

void LookMode::render(Window& window)
{
    game->renderAtPosition(window, position);
    window.getFont().setArea(GUI::getQuestionArea(window));
    window.getFont().print(window, "Look mode (arrow keys to move around, esc to exit)");
}

void Game::lookMode()
{
    LookMode lookMode(*this);
    getEngine().execute(lookMode);
}

class StringQuestion : public State
{
public:
    StringQuestion(std::string question) : question(std::move(question)) {}
    std::string execute();

private:
    void render(Window&) override {} // Rendered by keyboard::readLine() in execute().
    bool renderPreviousState() const override { return true; }

    std::string question;
};

std::string StringQuestion::execute()
{
    auto& window = getEngine().getWindow();
    std::string input;

    int result = keyboard::readLine(window, input, GUI::getQuestionArea(window).position,
                                    std::bind(&Engine::render, &getEngine(), std::placeholders::_1),
                                    question);
    if (result == Esc)
        return "";

    return input;
}

std::string Game::askForString(std::string&& question)
{
    StringQuestion stringQuestion(question);
    return getEngine().execute(stringQuestion);
}

class DirectionQuestion : public State
{
public:
    DirectionQuestion(std::string question, Vector2 origin) : question(std::move(question)), origin(origin) {}
    boost::optional<Dir8> execute();

private:
    void render(Window& window) override;
    bool renderPreviousState() const override { return true; }

    std::string question;
    Vector2 origin;
};

boost::optional<Dir8> DirectionQuestion::execute()
{
    Event event = getEngine().getWindow().waitForInput();

    if (auto direction = getDirectionFromEvent(event, origin))
        return direction;

    return boost::none;
}

void DirectionQuestion::render(Window& window)
{
    window.getFont().setArea(GUI::getQuestionArea(window));
    window.getFont().print(window, question);
}

boost::optional<Dir8> Game::askForDirection(std::string&& question)
{
    DirectionQuestion directionQuestion(question, player->getPosition());
    return getEngine().execute(directionQuestion);
}

void Game::execute()
{
    gameIsRunning = true;

    while (gameIsRunning)
    {
        Vector2 updateDistance(64, 64);
        Rect regionToUpdate(player->getPosition() - updateDistance, updateDistance * 2);
        world.exist(regionToUpdate, player->getLevel());
    }
}

void Game::render(Window& window)
{
    renderAtPosition(window, player->getPosition());
}

void Game::renderAtPosition(Window& window, Vector2 centerPosition)
{
    cursorPosition = boost::none;
    printPlayerInformation(window.getFont());
    MessageSystem::drawMessages(window, window.getFont(), player->getMessages(), getTurn());

    Rect worldViewport = GUI::getWorldViewport(getWindow());

    Rect view(centerPosition * Tile::getSize() + Tile::getSize() / 2 - worldViewport.size / 2,
              worldViewport.size);
    window.setView(&view);
    window.setViewport(&worldViewport);

    Rect visibleRegion(centerPosition - worldViewport.size / Tile::getSize() / 2,
                       worldViewport.size / Tile::getSize());
    world.render(window, visibleRegion, player->getLevel(), *player);

    window.setView(nullptr);
    window.setViewport(nullptr);

    bool enableGUIDebugRectangles = false;
    if (enableGUIDebugRectangles)
    {
        window.getGraphicsContext().renderRectangle(GUI::getWorldViewport(window), GUIColor::Gray);
        window.getGraphicsContext().renderRectangle(GUI::getMessageArea(window), GUIColor::Gray);
        window.getGraphicsContext().renderRectangle(GUI::getSidebarArea(window), GUIColor::Gray);
        window.getGraphicsContext().renderRectangle(GUI::getQuestionArea(window), GUIColor::Gray);
        window.getGraphicsContext().renderRectangle(GUI::getInventoryArea(window), GUIColor::Gray);
#ifdef DEBUG
        window.getGraphicsContext().renderRectangle(GUI::getCommandLineArea(window), GUIColor::Gray);
        window.getGraphicsContext().renderRectangle(GUI::getDebugMessageArea(window), GUIColor::Gray);
#endif
    }
}

void Game::printPlayerInformation(BitmapFont& font) const
{
    font.setArea(GUI::getSidebarArea(getWindow()));
    printStat(font, "HP", player->getHP(), player->getMaxHP(), TextColor::Red);
    printStat(font, "MP", player->getMP(), player->getMaxMP(), TextColor::Blue);

    for (auto attribute : player->getDisplayedAttributes())
        printAttribute(font, attributeAbbreviations[attribute], player->getAttribute(attribute));

    if (player->isRunning())
    {
        font.printLine(getWindow(), "");
        font.printLine(getWindow(), "Running");
    }

#ifdef DEBUG
    if (showExtraInfo)
    {
        font.printLine(getWindow(), "");
        font.printLine(getWindow(), "Pos " +
                       std::to_string(player->getPosition().x) + ", " +
                       std::to_string(player->getPosition().y) + ", " +
                       std::to_string(player->getLevel()));
        font.printLine(getWindow(), "Turn " + std::to_string(getTurn()));
    }
#endif
}

void Game::printStat(BitmapFont& font, boost::string_ref statName, double currentValue,
                     double maximumValue, Color16 color) const
{
    int currentValueInt = std::ceil(currentValue);
    int maximumValueInt = std::ceil(maximumValue);
    std::string currentValueString = std::to_string(currentValueInt);
    std::string padding(std::max(0, 4 - int(currentValueString.size())), ' ');
    auto text = statName + padding + currentValueString + "/" + std::to_string(maximumValueInt);

    auto sidebarArea = GUI::getSidebarArea(getWindow());
    auto columns = sidebarArea.getWidth() / font.getColumnWidth();
    auto filledColumns = columns * std::max(currentValueInt, 0) / maximumValueInt;
    text.append(columns - text.size(), ' ');

    font.print(getWindow(), boost::string_ref(text).substr(0, filledColumns), TextColor::White, color, true, PreserveLines);
    font.printLine(getWindow(), boost::string_ref(text).substr(filledColumns), TextColor::White, Color32::none, true, PreserveLines);
}

void Game::printAttribute(BitmapFont& font, boost::string_ref attributeName, double attributeValue) const
{
    std::string padding(5 - attributeName.size(), ' ');
    font.printLine(getWindow(), attributeName + padding + std::to_string(int(attributeValue)),
                   Color32::none, Color32::none, true, PreserveLines);
}

#ifdef DEBUG

void Game::enterCommandMode(Window& window)
{
    for (std::string command;;)
    {
        int result = keyboard::readLine(window, command, GUI::getCommandLineArea(window).position,
                                        std::bind(&Game::render, this, std::placeholders::_1), ">> ");

        if (result == Enter && !command.empty())
        {
            MessageSystem::addToCommandHistory(std::string(command));
            parseCommand(command);
            command.clear();
        }

        if (result == Esc || result == commandModeKey)
            break;

        if (result == UpArrow)
            command = MessageSystem::getPreviousCommand();

        if (result == DownArrow)
            command = MessageSystem::getNextCommand();
    }
}

void Game::parseCommand(boost::string_ref command)
{
    if (command == "respawn")
        *player = Creature(&player->getTileUnder(0), "Human", std::make_unique<PlayerController>(*this));
    else if (command == "clear")
        MessageSystem::clearDebugMessageHistory();
    else if (command == "info")
        showExtraInfo = !showExtraInfo;
    else if (command == "help")
        MessageSystem::addDebugMessage("Available commands: info | respawn | clear | help");
    else
        MessageSystem::addDebugMessage("Unknown command: " + command, Warning);
}

#endif

void Game::save()
{
    SaveFile file(saveFileName, true);
    file.writeInt32(getTurn());
    file.write(player->getPosition());
    file.writeInt32(player->getLevel());
    world.save(file);
}

void Game::load()
{
    SaveFile file(saveFileName, false);
    turn = file.readInt32();
    auto playerPosition = file.readVector2();
    auto playerLevel = file.readInt32();
    world.load(file);

    player = &world.getTile(playerPosition, playerLevel)->getCreature(0);
    player->setController(std::make_unique<PlayerController>(*this));
}
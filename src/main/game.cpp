#include "game.h"
#include "gui.h"
#include "item.h"
#include "msgsystem.h"
#include "tile.h"
#include "engine/keyboard.h"
#include "engine/math.h"
#include "engine/menu.h"
#include "engine/savefile.h"
#include <cmath>
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

static const Color transparentColor(0x5A5268FF);
Tile* Game::hoveredTile;

Game::Game(GameState* state)
:   playerSeesEverything(false),
    gameState(state)
{
    // TODO: Find a better place for loading assets, and make them non-static.
    if (!creatureConfig)
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
    }

    state->world.game = this;
}

Window& Game::getWindow() const
{
    return *window;
}

class InventoryMenu : public Menu
{
public:
    InventoryMenu(Window& window, const Creature& player, std::string_view title,
                  bool showNothingAsOption, std::function<bool(const Item&)> itemFilter);
};

InventoryMenu::InventoryMenu(Window& window, const Creature& player, std::string_view title,
                             bool showNothingAsOption, std::function<bool(const Item&)> itemFilter)
{
    setTitle(title);
    setArea(GUI::getInventoryArea(window));
    setItemSize(Tile::getSize());
    setTextLayout(TextLayout(LeftAlign, VerticalCenter));
    setTableCellSpacing(Vector2(window.context.font->getColumnWidth(), 0));
    setHotkeyStyle(LetterHotkeys);

    if (showNothingAsOption)
        addItem(-1, "nothing");

    int id = 0;

    for (auto& item : player.getInventory())
    {
        if (!itemFilter || itemFilter(*item))
            addItem(id, item->getName(), NoKey, &item->getSprite());

        ++id;
    }
}

int Game::showInventory(std::string_view title, bool showNothingAsOption, std::function<bool(const Item&)> itemFilter)
{
    stateManager->pushState(std::make_unique<InventoryMenu>(*window, *getPlayer(), title, showNothingAsOption, std::move(itemFilter)));
    return stateManager->wait().getInt();
}

class EquipmentMenu : public Menu
{
public:
    EquipmentMenu(Creature& player) : player(&player) {}
    StateChange update() override;
    void render() override;

private:
    Creature* player;
};

void EquipmentMenu::render()
{
    clear();
    setTitle("Equipment");
    setArea(GUI::getInventoryArea(*window));
    setItemSize(Tile::getSize());
    setTextLayout(TextLayout(LeftAlign, VerticalCenter));
    setTableCellSpacing(Vector2(window->context.font->getColumnWidth(), 0));
    setHotkeyStyle(LetterHotkeys);

    for (int i = 0; i < equipmentSlots; ++i)
    {
        auto slot = static_cast<EquipmentSlot>(i);
        auto* image = player->getEquipment(slot) ? &player->getEquipment(slot)->getSprite() : nullptr;
        auto itemName = player->getEquipment(slot) ? player->getEquipment(slot)->getName() : "-";
        addItem(i, toString(slot) + ":", itemName, NoKey, nullptr, image);
    }

    Menu::render();
}

StateChange EquipmentMenu::update()
{
    auto choice = stateManager->getResult(Menu::update()).getInt();

    if (choice == Menu::Exit)
        return StateChange::Pop();

    auto selectedSlot = static_cast<EquipmentSlot>(choice);

    stateManager->pushState(std::make_unique<InventoryMenu>(*window, *player, "", true, [&](auto& item)
    {
        return item.getEquipmentSlot() == selectedSlot;
    }));
    auto selectedItemIndex = stateManager->wait().getInt();

    if (selectedItemIndex == -1)
        player->equip(selectedSlot, nullptr);
    else if (selectedItemIndex != Menu::Exit)
        player->equip(selectedSlot, &*player->getInventory()[selectedItemIndex]);

    return StateChange::None();
}

void Game::showEquipmentMenu()
{
    stateManager->pushState(std::make_unique<EquipmentMenu>(*getPlayer()));
    stateManager->wait();
}

class LookMode : public State
{
public:
    LookMode(Game& game) : game(&game), position(game.getPlayer()->getPosition()) {}
    StateChange onKeyDown(Key key) override;

private:
    void render() override;

    Game* game;
    Vector2 position;
};

StateChange LookMode::onKeyDown(Key key)
{
    switch (key)
    {
        case UpArrow:
            position += North;
            break;

        case RightArrow:
            position += East;
            break;

        case DownArrow:
            position += South;
            break;

        case LeftArrow:
            position += West;
            break;

        case Esc:
            return StateChange::Pop();

        default:
            break;
    }

    return StateChange::None();
}

void LookMode::render()
{
    game->renderAtPosition(*window, position);
    window->context.font->setArea(GUI::getQuestionArea(*window));
    window->context.font->print(*window, "Look mode (arrow keys to move around, esc to exit)");
}

void Game::lookMode()
{
    stateManager->pushState(std::make_unique<LookMode>(*this));
}

class StringQuestion : public State
{
public:
    StringQuestion(std::string question) : question(std::move(question)) {}
    StateChange update() override;

private:
    void render() override {} // Rendered by keyboard::readLine() in update(). (FIXME)
    bool renderPreviousState() const override { return true; }

    std::string question;
};

StateChange StringQuestion::update()
{
    std::string input;
    int result = keyboard::readLine(*window, input, GUI::getQuestionArea(*window).position,
                                    std::bind(&StateManager::render, stateManager), question);

    if (result == Esc)
        return StateChange::Pop();

    return StateChange::Pop(std::move(input));
}

std::string Game::askForString(std::string&& question)
{
    stateManager->pushState(std::make_unique<StringQuestion>(std::move(question)));
    return stateManager->wait().getString();
}

class DirectionQuestion : public State
{
public:
    DirectionQuestion(std::string question, Vector2 origin) : question(std::move(question)), origin(origin) {}
    StateChange onEvent(Event event) override;

private:
    void render() override;
    bool renderPreviousState() const override { return true; }

    std::string question;
    Vector2 origin;
};

StateChange DirectionQuestion::onEvent(Event event)
{
    if (auto direction = getDirectionFromEvent(event, origin))
        return StateChange::Pop(direction);

    return StateChange::None();
}

void DirectionQuestion::render()
{
    window->context.font->setArea(GUI::getQuestionArea(*window));
    window->context.font->print(*window, question);
}

std::optional<Dir8> Game::askForDirection(std::string&& question)
{
    stateManager->pushState(std::make_unique<DirectionQuestion>(question, getPlayer()->getPosition()));

    if (auto result = stateManager->wait())
        return result.getDir();

    return std::nullopt;
}

StateChange Game::update()
{
    if (!gameIsRunning)
    {
        if (getPlayer()->isDead())
            gameState->removeSaveFile();

        return StateChange::Pop();
    }

    Vector2 updateDistance(64, 64);
    Rect regionToUpdate(getPlayer()->getPosition() - updateDistance, updateDistance * 2);
    getWorld().exist(regionToUpdate, getPlayer()->getLevel());

    return StateChange::None();
}

void Game::render()
{
    renderAtPosition(*window, getPlayer()->getPosition());
}

void Game::renderAtPosition(Window& window, Vector2 centerPosition)
{
    Rect worldViewport = GUI::getWorldViewport(getWindow());
    Rect view(centerPosition * Tile::getSize() + Tile::getSize() / 2 - worldViewport.size / 2, worldViewport.size);
    Rect visibleRegion(centerPosition - worldViewport.size / Tile::getSize() / 2, worldViewport.size / Tile::getSize());

    window.context.setView(&view);
    window.context.setViewport(&worldViewport);

    auto cursorPosition = window.getMousePosition().divFloor(Tile::spriteSize);
    hoveredTile = cursorPosition.isWithin(visibleRegion) ? getWorld().getTile(cursorPosition, getPlayer()->getLevel()) : nullptr;
    getWorld().render(window, visibleRegion, getPlayer()->getLevel(), *getPlayer());

    window.context.setView(nullptr);
    window.context.setViewport(nullptr);

    renderSidebar(*window.context.font);
    MessageSystem::drawMessages(window, *window.context.font, getPlayer()->getMessages(), getTurn());

    bool enableGUIDebugRectangles = false;
    if (enableGUIDebugRectangles)
    {
        window.context.renderRectangle(GUI::getWorldViewport(window), Gray);
        window.context.renderRectangle(GUI::getMessageArea(window), Gray);
        window.context.renderRectangle(GUI::getSidebarArea(window), Gray);
        window.context.renderRectangle(GUI::getQuestionArea(window), Gray);
        window.context.renderRectangle(GUI::getInventoryArea(window), Gray);
#ifdef DEBUG
        window.context.renderRectangle(GUI::getCommandLineArea(window), Gray);
        window.context.renderRectangle(GUI::getDebugMessageArea(window), Gray);
#endif
    }
}

void Game::renderSidebar(BitmapFont& font) const
{
    auto* player = getPlayer();

    font.setArea(GUI::getSidebarArea(getWindow()));
    printStat(font, "HP", player->getHP(), player->getMaxHP(), Red);
    printStat(font, "MP", player->getMP(), player->getMaxMP(), Blue);

    for (auto attribute : player->getDisplayedAttributes())
        printAttribute(font, attributeAbbreviations[attribute], player->getAttribute(attribute));

    font.printLine(getWindow(), "");
    font.printLine(getWindow(), "Running", player->isRunning() ? White : DarkGray);

    if (hoveredTile && (player->remembers(*hoveredTile) || playerSeesEverything))
    {
        auto tooltip = hoveredTile->getTooltip();
        if (!tooltip.empty())
        {
            font.printLine(getWindow(), "");
            font.printLine(getWindow(), tooltip, Color::none, Color::none, true, SplitLines);
        }
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

void Game::printStat(BitmapFont& font, std::string_view statName, double currentValue, double maximumValue, Color color) const
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

    font.print(getWindow(), std::string_view(text).substr(0, filledColumns), White, color);
    font.printLine(getWindow(), std::string_view(text).substr(filledColumns), White);
}

void Game::printAttribute(BitmapFont& font, std::string_view attributeName, double attributeValue) const
{
    std::string padding(5 - attributeName.size(), ' ');
    font.printLine(getWindow(), attributeName + padding + std::to_string(int(attributeValue)));
}

#ifdef DEBUG

void Game::enterCommandMode(Window& window)
{
    for (std::string command;;)
    {
        int result = keyboard::readLine(window, command, GUI::getCommandLineArea(window).position,
                                        std::bind(&Game::render, this), ">> ");

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

void Game::parseCommand(std::string_view command)
{
    if (command == "respawn")
        *getPlayer() = Creature(&getPlayer()->getTileUnder(0), "Human", std::make_unique<PlayerController>(*this));
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

void GameState::init(Game* game)
{
    auto* tile = world.getOrCreateTile({0, 0}, 0);
    player = tile->spawnCreature("Human", std::make_unique<PlayerController>(*game));
    isLoaded = true;
}

void GameState::save()
{
    SaveFile file(Game::saveFileName, true);
    file.writeInt32(turn);
    file.write(player->getPosition());
    file.writeInt32(player->getLevel());
    world.save(file);
}

void GameState::load(Game* game)
{
    SaveFile file(Game::saveFileName, false);
    turn = file.readInt32();
    auto playerPosition = file.readVector2();
    auto playerLevel = file.readInt32();
    world.load(file);

    player = world.getTile(playerPosition, playerLevel)->getCreature();
    player->setController(std::make_unique<PlayerController>(*game));
    isLoaded = true;
}

void GameState::removeSaveFile()
{
    std::remove(Game::saveFileName);
    isLoaded = false;
}

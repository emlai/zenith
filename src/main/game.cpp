#include "game.h"
#include "gui.h"
#include "msgsystem.h"
#include "engine/math.h"
#include "engine/menu.h"

const Config Game::creatureConfig("data/config/creature.cfg");
const Config Game::objectConfig("data/config/object.cfg");
const Config Game::itemConfig("data/config/item.cfg");
const Config Game::groundConfig("data/config/ground.cfg");
const Config Game::materialConfig("data/config/material.cfg");
boost::optional<const Texture> Game::creatureSpriteSheet;
boost::optional<const Texture> Game::objectSpriteSheet;
boost::optional<const Texture> Game::itemSpriteSheet;
boost::optional<const Texture> Game::groundSpriteSheet;
boost::optional<const Texture> Game::cursorTexture;
boost::optional<const Texture> Game::fogOfWarTexture;

static const Color32 transparentColor(0x5A5268FF);

Game::Game(Window& window)
:   Engine(window),
    world()
{
    creatureSpriteSheet.emplace(getWindow(), "data/graphics/creature.bmp", transparentColor);
    objectSpriteSheet.emplace(getWindow(), "data/graphics/object.bmp", transparentColor);
    itemSpriteSheet.emplace(getWindow(), "data/graphics/item.bmp", transparentColor);
    groundSpriteSheet.emplace(getWindow(), "data/graphics/ground.bmp");
    cursorTexture.emplace(getWindow(), "data/graphics/cursor.bmp", transparentColor);
    fogOfWarTexture.emplace(getWindow(), "data/graphics/fow.bmp", transparentColor);

    mapKey(Esc, [this] { stop(); });
    mapKey('q', [this] { stop(); });

    mapKey(RightArrow, [this]
    {
        if (player->tryToMoveOrAttack(East))
            advanceTurn();
    });

    mapKey(LeftArrow, [this]
    {
        if (player->tryToMoveOrAttack(West))
            advanceTurn();
    });

    mapKey(DownArrow, [this]
    {
        if (player->tryToMoveOrAttack(South))
            advanceTurn();
    });

    mapKey(UpArrow, [this]
    {
        if (player->tryToMoveOrAttack(North))
            advanceTurn();
    });

    mapKey(Enter, [this]
    {
        if (player->enter())
            advanceTurn();
    });

    mapKey('.', [this]
    {
        advanceTurn();
    });

    mapKey(',', [this]
    {
        if (player->pickUpItem())
            advanceTurn();
    });

    mapKey('i', [this]
    {
        showInventory("Inventory", false);
    });

    mapKey('w', [this]
    {
        int selectedItemIndex = showInventory("What do you want to wield?", true);

        if (selectedItemIndex != Menu::Exit)
        {
            if (selectedItemIndex == -1)
                player->wield(nullptr);
            else
                player->wield(player->getInventory()[selectedItemIndex].get());
        }
    });

    mapKey('d', [this]
    {
        int selectedItemIndex = showInventory("What do you want to drop?", false);

        if (selectedItemIndex != Menu::Exit)
        {
            player->drop(*player->getInventory()[selectedItemIndex]);
            advanceTurn();
        }
    });

    mapKey('c', [this]
    {
        boost::optional<Dir8> direction = askForDirection("What do you want to close?");

        if (direction && player->close(*direction))
            advanceTurn();
    });

#ifdef DEBUG
    mapKey(Tab, [this] { enterCommandMode(getWindow()); });
#endif

    player = world.getOrCreateTile({0, 0}, 0)->spawnCreature("Human", std::make_unique<PlayerController>());
}

int Game::showInventory(boost::string_ref title, bool showNothingAsOption)
{
    Menu menu;
    menu.addTitle(title);
    menu.setArea(GUI::getWorldViewport(getWindow()));
    menu.setItemSize(Tile::size);
    menu.setTextLayout(TextLayout(LeftAlign, VerticalCenter));
    menu.setImageSpacing(Tile::size / 2);
    menu.setSelectionOffset(Vector2(0, 1));

    if (showNothingAsOption)
        menu.addItem(-1, "nothing");

    int id = 0;

    for (auto& item : player->getInventory())
        menu.addItem(id++, item->getName(), NoKey, &item->getSprite());

    return menu.getChoice(getWindow(), getWindow().getFont());
}

boost::optional<Dir8> Game::askForDirection(std::string&& question)
{
    player->addMessage(std::move(question));
    render(getWindow());
    getWindow().updateScreen();

    switch (getWindow().waitForInput())
    {
        case UpArrow: return North;
        case RightArrow: return East;
        case DownArrow: return South;
        case LeftArrow: return West;
        default: return boost::none;
    }
}

void Game::updateLogic()
{
    Vector2 updateDistance(64, 64);
    Rect regionToUpdate(player->getPosition() - updateDistance, updateDistance * 2);
    world.exist(regionToUpdate, player->getLevel());
}

void Game::render(Window& window)
{
    Rect worldViewport = GUI::getWorldViewport(getWindow());

    Rect playerView(player->getPosition() * Tile::size + Tile::sizeVector / 2 - worldViewport.size / 2,
                    worldViewport.size);
    window.setView(&playerView);
    window.setViewport(&worldViewport);

    Rect visibleRegion(player->getPosition() - worldViewport.size / Tile::size / 2,
                       worldViewport.size / Tile::size);
    world.render(window, visibleRegion, player->getLevel(), *player);

    window.setView(nullptr);
    window.setViewport(nullptr);

    printPlayerInformation(window.getFont());
    MessageSystem::drawMessages(window, window.getFont(), player->getMessages());
}

void Game::printPlayerInformation(BitmapFont& font) const
{
    font.setArea(GUI::getSidebarArea(getWindow()));
    printStat(font, "HP", player->getHP(), player->getMaxHP(), TextColor::Red);
    printStat(font, "AP", player->getAP(), player->getMaxAP(), TextColor::Green);
    printStat(font, "MP", player->getMP(), player->getMaxMP(), TextColor::Blue);

    for (auto attribute : player->getDisplayedAttributes())
        printAttribute(font, attributeAbbreviations[attribute], player->getAttribute(attribute));

#ifdef DEBUG
    if (showExtraInfo)
    {
        font.printLine("");
        font.printLine("x: " + std::to_string(player->getPosition().x));
        font.printLine("y: " + std::to_string(player->getPosition().y));
        font.printLine("z: " + std::to_string(player->getLevel()));
        font.printLine("Turn " + std::to_string(getTurn()));
    }
#endif
}

void Game::printStat(BitmapFont& font, boost::string_ref statName, double currentValue,
                     double maximumValue, Color16 color)
{
    std::string currentValueString = std::to_string(int(currentValue));
    std::string padding(std::max(0, 4 - int(currentValueString.size())), ' ');
    font.printLine(statName + padding + currentValueString + '/' + std::to_string(int(maximumValue)), color);
}

void Game::printAttribute(BitmapFont& font, boost::string_ref attributeName, double attributeValue)
{
    std::string padding(5 - attributeName.size(), ' ');
    font.printLine(attributeName + padding + std::to_string(int(attributeValue)));
}

#ifdef DEBUG

void Game::enterCommandMode(Window& window)
{
    for (std::string command;;)
    {
        int result = keyboard::readLine(window, command, GUI::commandLinePosition,
                                        std::bind(&Game::render, this, std::placeholders::_1), ">> ");

        if (result == Enter && !command.empty())
        {
            MessageSystem::addToCommandHistory(std::string(command));
            parseCommand(command);
            command.clear();
        }

        if (result == Esc || result == Tab)
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
        *player = Creature(player->getTileUnder(0), "Human", std::make_unique<PlayerController>());
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

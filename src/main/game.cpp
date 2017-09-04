#include "game.h"
#include "gui.h"
#include "item.h"
#include "msgsystem.h"
#include "tile.h"
#include "engine/math.h"
#include "engine/menu.h"
#include "engine/savefile.h"
#include <cmath>
#include <fstream>

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

Game::Game(Window& window, bool loadSavedGame)
:   Engine(window),
    world(*this)
{
    creatureSpriteSheet.emplace("data/graphics/creature.bmp", transparentColor);
    objectSpriteSheet.emplace("data/graphics/object.bmp", transparentColor);
    itemSpriteSheet.emplace("data/graphics/item.bmp", transparentColor);
    groundSpriteSheet.emplace("data/graphics/ground.bmp");
    cursorTexture.emplace("data/graphics/cursor.bmp", transparentColor);
    fogOfWarTexture.emplace("data/graphics/fow.bmp", transparentColor);

    if (loadSavedGame)
        load();
    else
    {
        auto* tile = world.getOrCreateTile({0, 0}, 0);
        player = tile->spawnCreature("Human", std::make_unique<PlayerController>(*this));
    }
}

int Game::showInventory(boost::string_ref title, bool showNothingAsOption, Item* preselectedItem,
                        std::function<bool(const Item&)> itemFilter)
{
    Menu menu;
    menu.addTitle(title);
    menu.setArea(GUI::getInventoryArea(getWindow()));
    menu.setItemSize(Tile::size);
    menu.setTextLayout(TextLayout(LeftAlign, VerticalCenter));
    menu.setColumnSpacing(Tile::size / 2);

    if (showNothingAsOption)
        menu.addItem(MenuItem(-1, "nothing"));

    int id = 0;
    int preselectedIndex = 0;

    for (auto& item : player->getInventory())
    {
        if (!itemFilter || itemFilter(*item))
        {
            int index = menu.addItem(MenuItem(id, item->getName(), NoKey, &item->getSprite()));

            if (&*item == preselectedItem)
                preselectedIndex = index;
        }

        ++id;
    }

    menu.select(preselectedIndex);
    return menu.getChoice(getWindow(), getWindow().getFont());
}

void Game::showEquipmentMenu()
{
    int selectedMenuItem = 0;

    while (true)
    {
        Menu menu;
        menu.addTitle("Equipment");
        menu.setArea(GUI::getInventoryArea(getWindow()));
        menu.setItemSize(Tile::size);
        menu.setTextLayout(TextLayout(LeftAlign, VerticalCenter));
        menu.setColumnSpacing(Tile::size / 2);

        for (int i = 0; i < equipmentSlots; ++i)
        {
            auto slot = static_cast<EquipmentSlot>(i);
            auto* image = player->getEquipment(slot) ? &player->getEquipment(slot)->getSprite() : nullptr;
            auto itemName = player->getEquipment(slot) ? player->getEquipment(slot)->getName() : "-";
            menu.addItem(MenuItem(i, toString(slot) + ":", itemName, NoKey, nullptr, image));
        }

        menu.select(selectedMenuItem);

        auto choice = menu.getChoice(getWindow(), getWindow().getFont());
        if (choice == Menu::Exit)
            break;

        selectedMenuItem = menu.getSelectedIndex();

        auto selectedSlot = static_cast<EquipmentSlot>(choice);
        auto selectedItemIndex = showInventory("", true, player->getEquipment(selectedSlot), [&](auto& item)
        {
            return item.getEquipmentSlot() == selectedSlot;
        });

        if (selectedItemIndex == -1)
            player->equip(selectedSlot, nullptr);
        else if (selectedItemIndex != Menu::Exit)
            player->equip(selectedSlot, &*player->getInventory()[selectedItemIndex]);
    }
}

void Game::lookMode()
{
    Vector2 position = player->getPosition();

    while (true)
    {
        renderAtPosition(getWindow(), position);
        getWindow().getFont().setArea(GUI::getQuestionArea(getWindow()));
        getWindow().getFont().print(getWindow(), "Look mode (arrow keys to move around, esc to exit)");
        getWindow().updateScreen();

        switch (getWindow().waitForInput())
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

boost::optional<Dir8> Game::askForDirection(std::string&& question)
{
    while (true)
    {
        render(getWindow());
        getWindow().getFont().setArea(GUI::getQuestionArea(getWindow()));
        getWindow().getFont().print(getWindow(), question);
        getWindow().updateScreen();

        switch (getWindow().waitForInput())
        {
            case NoKey: continue; // timeout
            case UpArrow: return North;
            case RightArrow: return East;
            case DownArrow: return South;
            case LeftArrow: return West;
            default: return boost::none;
        }
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
    renderAtPosition(window, player->getPosition());
}

void Game::renderAtPosition(Window& window, Vector2 centerPosition)
{
    Rect worldViewport = GUI::getWorldViewport(getWindow());

    Rect view(centerPosition * Tile::size + Tile::sizeVector / 2 - worldViewport.size / 2,
              worldViewport.size);
    window.setView(&view);
    window.setViewport(&worldViewport);

    Rect visibleRegion(centerPosition - worldViewport.size / Tile::size / 2,
                       worldViewport.size / Tile::size);
    world.render(window, visibleRegion, player->getLevel(), *player);

    window.setView(nullptr);
    window.setViewport(nullptr);

    printPlayerInformation(window.getFont());
    MessageSystem::drawMessages(window, window.getFont(), player->getMessages(), getTurn());

    bool enableGUIDebugRectangles = false;
    if (enableGUIDebugRectangles)
    {
        window.getGraphicsContext().renderRectangle(GUI::getWorldViewport(window), GUIColor::Gray);
        window.getGraphicsContext().renderRectangle(GUI::getMessageArea(window), GUIColor::Gray);
        window.getGraphicsContext().renderRectangle(GUI::getSidebarArea(window), GUIColor::Gray);
        window.getGraphicsContext().renderRectangle(GUI::getQuestionArea(window), GUIColor::Gray);
        window.getGraphicsContext().renderRectangle(GUI::getInventoryArea(window), GUIColor::Gray);
        window.getGraphicsContext().renderRectangle(GUI::getCommandLineArea(window), GUIColor::Gray);
        window.getGraphicsContext().renderRectangle(GUI::getDebugMessageArea(window), GUIColor::Gray);
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
    std::string currentValueString = std::to_string(int(std::ceil(currentValue)));
    std::string padding(std::max(0, 4 - int(currentValueString.size())), ' ');
    font.printLine(getWindow(), statName + padding + currentValueString + '/' +
                   std::to_string(int(std::ceil(maximumValue))), color);
}

void Game::printAttribute(BitmapFont& font, boost::string_ref attributeName, double attributeValue) const
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
    if (player->isDead())
    {
        std::remove(saveFileName);
        return;
    }

    SaveFile file(saveFileName, true);
    file.writeInt32(getTurn());
    file.write(player->getPosition());
    file.writeInt32(player->getLevel());
    world.save(file);
}

void Game::load()
{
    SaveFile file(saveFileName, false);
    setTurn(file.readInt32());
    auto playerPosition = file.readVector2();
    auto playerLevel = file.readInt32();
    world.load(file);
    player = &world.getTile(playerPosition, playerLevel)->getCreature(0);
    player->setController(std::make_unique<PlayerController>(*this));
}

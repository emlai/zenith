#include "game.h"
#include "gui.h"
#include "msgsystem.h"
#include "engine/math.h"

Game::Game(Window& window)
:   Engine(window),
    creatureConfig("data/config/creature.cfg"),
    groundConfig("data/config/ground.cfg"),
    creatureSpriteSheet(getWindow(), "data/graphics/creature.bmp", Color32(0x5A5268FF)),
    groundSpriteSheet(getWindow(), "data/graphics/ground.bmp"),
    currentArea(groundConfig, groundSpriteSheet),
    framesUntilTick(framesPerTick)
{
    mapKey(Esc, [this] { stop(); });
    mapKey('q', [this] { stop(); });
    mapKey(RightArrow, [this] { player->tryToMove(East); });
    mapKey(LeftArrow, [this] { player->tryToMove(West); });
    mapKey(DownArrow, [this] { player->tryToMove(South); });
    mapKey(UpArrow, [this] { player->tryToMove(North); });
#ifdef DEBUG
    mapKey(Tab, [this] { enterCommandMode(getWindow()); });
#endif

    player = getRandomTile(currentArea).spawnCreature("Human", creatureConfig, creatureSpriteSheet);
}

void Game::updateLogic()
{
    --framesUntilTick;
    if (framesUntilTick > 0)
        return;

    advanceTick();
    framesUntilTick = framesPerTick;
    currentArea.exist();
}

void Game::render(Window& window) const
{
    window.setViewport(&GUI::viewport);
    currentArea.render(window, 0);
    currentArea.render(window, 2);
    window.setViewport(nullptr);
    printPlayerInformation(window.getFont());
    MessageSystem::drawMessages(window.getFont());
}

void Game::printPlayerInformation(BitmapFont& font) const
{
    font.setArea(GUI::sidebar);
    printStat(font, "HP", player->getHP(), player->getMaxHP(), TextColor::Red);
    printStat(font, "AP", player->getAP(), player->getMaxAP(), TextColor::Green);
    printStat(font, "MP", player->getMP(), player->getMaxMP(), TextColor::Blue);

    for (auto attribute : player->getDisplayedAttributes())
        printAttribute(font, attributeAbbreviations[attribute], player->getAttribute(attribute));

#ifdef DEBUG
    if (showExtraInfo)
    {
        font.printLine("");
        font.printLine("Pos: " + std::to_string(player->getPosition().x) + "," +
                       std::to_string(player->getPosition().y));
        font.printLine("Frame: #" + std::to_string(framesUntilTick));
    }
#endif
}

void Game::printStat(BitmapFont& font, const std::string& statName, int currentValue, int maximumValue,
                     Color16 color)
{
    std::string currentValueString = std::to_string(currentValue);
    std::string padding(std::max(0, 4 - int(currentValueString.size())), ' ');
    font.printLine(statName + padding + currentValueString + '/' + std::to_string(maximumValue), color);
}

void Game::printAttribute(BitmapFont& font, const std::string& attributeName, int attributeValue)
{
    std::string padding(5 - attributeName.size(), ' ');
    font.printLine(attributeName + padding + std::to_string(attributeValue));
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
            MessageSystem::addToCommandHistory(command);
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

void Game::parseCommand(const std::string& command)
{
    if (command == "respawn")
        *player = Creature(player->getTileUnder(0), "Human", creatureConfig, creatureSpriteSheet);
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

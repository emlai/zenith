#include "msgsystem.h"
#include "gui.h"
#include "engine/color.h"

namespace MessageSystem
{
    static const int recentMessageLimit = 32;
    static std::deque<std::string> recentMessages;

#ifdef DEBUG
    struct DebugMessage
    {
        DebugMessage(const std::string& content, MessageType type = Normal)
        :   content(content), type(type)
        {
        }
        const std::string content;
        const MessageType type;
    };

    static const int debugMessageLimit = 16;
    static const Color16 messageColors[] = { TextColor::White, TextColor::Red };
    static std::deque<DebugMessage> debugMessages;
    static std::vector<std::string> commandHistory;
    static std::vector<std::string>::iterator commandIterator = commandHistory.end();
#endif
}

void MessageSystem::addMessage(const std::string& message)
{
    recentMessages.push_front(message);
}

void MessageSystem::cleanUpOldMessages()
{
    for (int n = recentMessages.size(); n > recentMessageLimit; --n)
        recentMessages.pop_back();
}

void MessageSystem::drawMessages(BitmapFont& font)
{
    font.setArea(GUI::messageArea);
    for (const std::string& message : recentMessages)
        font.printLine(message);

#ifdef DEBUG
    font.setArea(GUI::debugMessageArea);
    for (const DebugMessage& message : debugMessages)
        font.printLine(message.content, messageColors[message.type]);
#endif
}

#ifdef DEBUG

void MessageSystem::addDebugMessage(const std::string& message, MessageType type)
{
    debugMessages.push_front(DebugMessage(message, type));

    if (debugMessages.size() > debugMessageLimit)
        debugMessages.pop_back();
}

void MessageSystem::addToCommandHistory(const std::string& command)
{
    if (commandHistory.empty() || commandHistory.back() != command)
        commandHistory.push_back(command);

    commandIterator = commandHistory.end();
}

std::string MessageSystem::getPreviousCommand()
{
    if (commandHistory.empty())
        return "";

    if (commandIterator != commandHistory.begin())
        --commandIterator;

    return *commandIterator;
}

std::string MessageSystem::getNextCommand()
{
    if (commandIterator == commandHistory.end() || ++commandIterator == commandHistory.end())
        return "";

    return *commandIterator;
}

void MessageSystem::clearDebugMessageHistory()
{
    debugMessages.clear();
}

#endif

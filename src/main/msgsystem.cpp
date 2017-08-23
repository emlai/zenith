#include "msgsystem.h"
#include "gui.h"
#include "engine/color.h"

namespace MessageSystem
{
    static const int maxMessagesToPrint = 6;

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

void MessageSystem::drawMessages(BitmapFont& font, const std::vector<std::string>& messages)
{
    font.setArea(GUI::messageArea);
    for (int end = int(messages.size()), i = std::max(0, end - maxMessagesToPrint); i < end; ++i)
        font.printLine(messages[i]);

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

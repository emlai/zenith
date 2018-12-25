#include "msgsystem.h"
#include "gui.h"
#include "engine/color.h"

void Message::save(SaveFile& file) const
{
    file.write(text);
    file.writeInt32(turn);
}

Message Message::load(const SaveFile& file)
{
    auto text = file.readString();
    auto turn = file.readInt32();
    return Message(std::move(text), turn);
}

namespace MessageSystem
{
    static const int maxMessagesToPrint = 6;

#ifdef DEBUG
    struct DebugMessage
    {
        DebugMessage(std::string_view content, MessageType type = Normal)
        :   content(content), type(type)
        {
        }
        const std::string content;
        const MessageType type;
    };

    static const int debugMessageLimit = 16;
    static const Color messageColors[] = { TextColor::White, TextColor::Red };
    static std::deque<DebugMessage> debugMessages;
    static std::vector<std::string> commandHistory;
    static std::vector<std::string>::iterator commandIterator = commandHistory.end();
#endif
}

void MessageSystem::drawMessages(Window& window, BitmapFont& font,
                                 const std::vector<Message>& messages, int currentTurn)
{
    font.setArea(GUI::getMessageArea(window));
    for (int end = int(messages.size()), i = std::max(0, end - maxMessagesToPrint); i < end; ++i)
    {
        bool isNewMessage = messages[i].turn >= currentTurn - 1;
        auto color = isNewMessage ? TextColor::White : TextColor::Gray;
        font.print(window, "- ", color);
        std::string text = messages[i].text;

        if (messages[i].count > 1)
            text += " (x" + std::to_string(messages[i].count) + ")";

        font.printLine(window, text, color, Color::none, true, SplitLines);
    }

#ifdef DEBUG
    font.setArea(GUI::getDebugMessageArea(window));
    for (const DebugMessage& message : debugMessages)
        font.printLine(window, message.content, messageColors[message.type]);
#endif
}

#ifdef DEBUG

void MessageSystem::addDebugMessage(std::string_view message, MessageType type)
{
    debugMessages.push_front(DebugMessage(message, type));

    if (debugMessages.size() > debugMessageLimit)
        debugMessages.pop_back();
}

void MessageSystem::addToCommandHistory(std::string&& command)
{
    if (commandHistory.empty() || commandHistory.back() != command)
        commandHistory.push_back(std::move(command));

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

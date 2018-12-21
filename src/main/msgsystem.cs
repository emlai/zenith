enum MessageType { Normal, Warning }

class Message
{
    string text;
    int turn;
    int count;

    Message(string text, int turn) : text(text), turn(turn), count(1) {}
    string getText() { return text; }
    int getTurn() { return turn; }
    int getCount() { return count; }
    void increaseCount(int currentTurn) { ++count; turn = currentTurn; }

    void save(SaveFile file)
    {
        file.write(text);
        file.writeInt32(turn);
    }

    Message load(SaveFile file)
    {
        var text = file.readString();
        var turn = file.readInt32();
        return Message(text, turn);
    }
}

static class MessageSystem
{
    void drawMessages(Window window, BitmapFont, List<Message> messages,
                      int currentTurn);

    const int maxMessagesToPrint = 6;

#ifdef DEBUG
    struct DebugMessage
    {
        DebugMessage(string content, MessageType type = Normal)
        :   content(content), type(type)
        {
        }
        const string content;
        const MessageType type;
    }

    const int debugMessageLimit = 16;
    const Color16 messageColors[] = { TextColor::White, TextColor::Red }
    static std::deque<DebugMessage> debugMessages;
    static List<string> commandHistory;
    static List<string>::iterator commandIterator = commandHistory.end();
#endif

    void MessageSystem::drawMessages(Window window, BitmapFont font,
                                     List<Message> messages, int currentTurn)
    {
        font.setArea(GUI::getMessageArea(window));
        for (int end = int(messages.size()), i = std::max(0, end - maxMessagesToPrint); i < end; ++i)
        {
            bool isNewMessage = messages[i].getTurn() >= currentTurn - 1;
            var color = isNewMessage ? TextColor::White : TextColor::Gray;
            font.print(window, "- ", color);
            string text(messages[i].getText());
            if (messages[i].getCount() > 1)
                text += " (x" + std::to_string(messages[i].getCount()) + ")";
            font.printLine(window, text, color);
        }

    #ifdef DEBUG
        font.setArea(GUI::getDebugMessageArea(window));
        foreach (DebugMessage message in debugMessages)
            font.printLine(window, message.content, messageColors[message.type]);
    #endif
    }

    #ifdef DEBUG

    void MessageSystem::addDebugMessage(string message, MessageType type)
    {
        debugMessages.push_front(DebugMessage(message, type));

        if (debugMessages.size() > debugMessageLimit)
            debugMessages.pop_back();
    }

    void MessageSystem::addToCommandHistory(string command)
    {
        if (commandHistory.empty() || commandHistory.back() != command)
            commandHistory.push_back(command);

        commandIterator = commandHistory.end();
    }

    string MessageSystem::getPreviousCommand()
    {
        if (commandHistory.empty())
            return "";

        if (commandIterator != commandHistory.begin())
            --commandIterator;

        return commandIterator;
    }

    string MessageSystem::getNextCommand()
    {
        if (commandIterator == commandHistory.end() || ++commandIterator == commandHistory.end())
            return "";

        return commandIterator;
    }

    void MessageSystem::clearDebugMessageHistory()
    {
        debugMessages.clear();
    }

    #endif
}

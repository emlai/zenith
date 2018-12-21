enum MessageType { Normal, Warning }

class Message
{
public:
    Message(string text, int turn) : text(std::move(text)), turn(turn), count(1) {}
    boost::string_ref getText() const { return text; }
    int getTurn() const { return turn; }
    int getCount() const { return count; }
    void increaseCount(int currentTurn) { ++count; turn = currentTurn; }
    void save(SaveFile file) const;
    static Message load(SaveFile file);

private:
    string text;
    int turn;
    int count;
}

namespace MessageSystem
{
    void drawMessages(Window window, BitmapFont, const List<Message>& messages,
                      int currentTurn);

#ifdef DEBUG
    void addDebugMessage(boost::string_ref message, MessageType = Normal);
    void addToCommandHistory(string command);
    string getPreviousCommand();
    string getNextCommand();
    void clearDebugMessageHistory();
#endif
}
void Message::save(SaveFile file) const
{
    file.write(text);
    file.writeInt32(turn);
}

Message Message::load(SaveFile file)
{
    var text = file.readString();
    var turn = file.readInt32();
    return Message(std::move(text), turn);
}

namespace MessageSystem
{
    static const int maxMessagesToPrint = 6;

#ifdef DEBUG
    struct DebugMessage
    {
        DebugMessage(boost::string_ref content, MessageType type = Normal)
        :   content(content), type(type)
        {
        }
        const string content;
        const MessageType type;
    }

    static const int debugMessageLimit = 16;
    static const Color16 messageColors[] = { TextColor::White, TextColor::Red }
    static std::deque<DebugMessage> debugMessages;
    static List<string> commandHistory;
    static List<string>::iterator commandIterator = commandHistory.end();
#endif
}

void MessageSystem::drawMessages(Window window, BitmapFont font,
                                 const List<Message>& messages, int currentTurn)
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
    for (DebugMessage message : debugMessages)
        font.printLine(window, message.content, messageColors[message.type]);
#endif
}

#ifdef DEBUG

void MessageSystem::addDebugMessage(boost::string_ref message, MessageType type)
{
    debugMessages.push_front(DebugMessage(message, type));

    if (debugMessages.size() > debugMessageLimit)
        debugMessages.pop_back();
}

void MessageSystem::addToCommandHistory(string command)
{
    if (commandHistory.empty() || commandHistory.back() != command)
        commandHistory.push_back(std::move(command));

    commandIterator = commandHistory.end();
}

string MessageSystem::getPreviousCommand()
{
    if (commandHistory.empty())
        return "";

    if (commandIterator != commandHistory.begin())
        --commandIterator;

    return *commandIterator;
}

string MessageSystem::getNextCommand()
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

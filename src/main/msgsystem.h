#pragma once

#include "gui.h"
#include "engine/font.h"
#include "engine/savefile.h"
#include <string_view>
#include <deque>
#include <string>

enum MessageType { Normal, Warning };

class Message
{
public:
    Message(std::string&& text, int turn) : text(std::move(text)), turn(turn), count(1) {}
    std::string_view getText() const { return text; }
    int getTurn() const { return turn; }
    int getCount() const { return count; }
    void increaseCount(int currentTurn) { ++count; turn = currentTurn; }
    void save(SaveFile& file) const;
    static Message load(const SaveFile& file);

private:
    std::string text;
    int turn;
    int count;
};

namespace MessageSystem
{
    void drawMessages(Window& window, BitmapFont&, const std::vector<Message>& messages,
                      int currentTurn);

#ifdef DEBUG
    void addDebugMessage(std::string_view message, MessageType = Normal);
    void addToCommandHistory(std::string&& command);
    std::string getPreviousCommand();
    std::string getNextCommand();
    void clearDebugMessageHistory();
#endif
}

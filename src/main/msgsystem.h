#pragma once

#include "gui.h"
#include "engine/font.h"
#include "engine/savefile.h"
#include <boost/utility/string_ref.hpp>
#include <deque>
#include <string>

enum MessageType { Normal, Warning };

class Message
{
public:
    Message(std::string&& text, int turn) : text(std::move(text)), turn(turn) {}
    boost::string_ref getText() const { return text; }
    int getTurn() const { return turn; }
    void save(SaveFile& file) const;
    static Message load(const SaveFile& file);

private:
    std::string text;
    int turn;
};

namespace MessageSystem
{
    void drawMessages(const Window& window, BitmapFont&, const std::vector<Message>& messages,
                      int currentTurn);

#ifdef DEBUG
    void addDebugMessage(boost::string_ref message, MessageType = Normal);
    void addToCommandHistory(std::string&& command);
    std::string getPreviousCommand();
    std::string getNextCommand();
    void clearDebugMessageHistory();
#endif
}

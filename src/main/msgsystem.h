#pragma once

#include "gui.h"
#include "engine/font.h"
#include <deque>
#include <string>

enum MessageType { Normal, Warning };

namespace MessageSystem
{
    void drawMessages(const Window& window, BitmapFont&, const std::vector<std::string>& messages);

#ifdef DEBUG
    void addDebugMessage(boost::string_ref message, MessageType = Normal);
    void addToCommandHistory(std::string&& command);
    std::string getPreviousCommand();
    std::string getNextCommand();
    void clearDebugMessageHistory();
#endif
}

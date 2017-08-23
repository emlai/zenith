#pragma once

#include "gui.h"
#include "engine/font.h"
#include <deque>
#include <string>

enum MessageType { Normal, Warning };

namespace MessageSystem
{
    void drawMessages(BitmapFont&, const std::vector<std::string>& messages);

#ifdef DEBUG
    void addDebugMessage(const std::string& message, MessageType = Normal);
    void addToCommandHistory(const std::string& command);
    std::string getPreviousCommand();
    std::string getNextCommand();
    void clearDebugMessageHistory();
#endif
}

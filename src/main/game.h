#pragma once

#include "area.h"
#include "creature.h"
#include "engine/keyboard.h"
#include "engine/window.h"
#include "engine/engine.h"
#include "engine/config.h"
#include "engine/color.h"
#include <string>

class Game : public Engine
{
public:
    Game();

private:
    void render(Window&) const override;
    void updateLogic() override;

    void printPlayerInformation(BitmapFont&) const;
    static void printStat(BitmapFont&, const std::string&, int current, int max, Color16);
    static void printAttribute(BitmapFont&, const std::string&, int current);

    Area currentArea;
    Config creatureConfig;
    Creature* player;
    int framesUntilTick;
    static const int framesPerTick = 4;

#ifdef DEBUG
    void enterCommandMode(Window&);
    void parseCommand(const std::string&);

    bool showExtraInfo = true;
#endif
};

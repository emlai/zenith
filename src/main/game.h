#pragma once

#include "creature.h"
#include "world.h"
#include "engine/keyboard.h"
#include "engine/window.h"
#include "engine/engine.h"
#include "engine/config.h"
#include "engine/color.h"
#include <boost/optional.hpp>
#include <string>

class Game : public Engine
{
public:
    Game(Window&);

    static const Config creatureConfig;
    static const Config objectConfig;
    static const Config itemConfig;
    static const Config groundConfig;
    static boost::optional<const Texture> creatureSpriteSheet;
    static boost::optional<const Texture> objectSpriteSheet;
    static boost::optional<const Texture> itemSpriteSheet;
    static boost::optional<const Texture> groundSpriteSheet;
    static boost::optional<const Texture> cursorTexture;

private:
    void render(Window&) override;
    void updateLogic() override;

    void printPlayerInformation(BitmapFont&) const;
    static void printStat(BitmapFont&, const std::string&, int current, int max, Color16);
    static void printAttribute(BitmapFont&, const std::string&, int current);

    World world;
    Creature* player;

#ifdef DEBUG
    void enterCommandMode(Window&);
    void parseCommand(const std::string&);

    bool showExtraInfo = true;
#endif
};

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
    static const Config materialConfig;
    static boost::optional<const Texture> creatureSpriteSheet;
    static boost::optional<const Texture> objectSpriteSheet;
    static boost::optional<const Texture> itemSpriteSheet;
    static boost::optional<const Texture> groundSpriteSheet;
    static boost::optional<const Texture> cursorTexture;
    static boost::optional<const Texture> fogOfWarTexture;

private:
    int showInventory(boost::string_ref title, bool showNothingAsOption);
    boost::optional<Dir8> askForDirection(std::string&& question);
    void render(Window&) override;
    void updateLogic() override;

    void printPlayerInformation(BitmapFont&) const;
    static void printStat(BitmapFont&, boost::string_ref, int current, int max, Color16);
    static void printAttribute(BitmapFont&, boost::string_ref, int current);

    World world;
    Creature* player;

#ifdef DEBUG
    void enterCommandMode(Window&);
    void parseCommand(boost::string_ref);

    bool showExtraInfo = true;
#endif
};

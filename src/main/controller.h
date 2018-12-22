#pragma once

#include "ai.h"
#include "engine/geometry.h"
#include "engine/keyboard.h"
#include <string_view>
#include <memory>

class Config;
class Creature;
class Game;
class Event;
enum Action : int;

class Controller
{
public:
    virtual ~Controller() = 0;
    virtual Action control(Creature& creature) = 0;
};

class AIController : public Controller
{
public:
    AIController(std::unique_ptr<AI> ai) : ai(std::move(ai)) {}
    Action control(Creature& creature) override;
    static std::unique_ptr<AIController> get(std::string_view id, Creature& creature);

private:
    std::unique_ptr<AI> ai;
};

class PlayerController : public Controller
{
public:
    PlayerController(Game& game) : game(game) {}

private:
    Action control(Creature& creature) override;

    Game& game;
};

Action getMappedAction(Key key);
Key getMappedKey(Action action);
void mapKey(Key key, Action action);
// If 'config' is null, loads the default key map.
void loadKeyMap(const Config* config);
void saveKeyMap(Config& config);
Dir8 getDirectionFromEvent(Event event, Vector2 origin);

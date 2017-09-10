#pragma once

#include "ai.h"
#include <boost/utility/string_ref.hpp>
#include <memory>

class Creature;
class Game;
enum Action : int;

class CreatureController
{
public:
    virtual ~CreatureController() = 0;
    virtual Action control(Creature& creature) = 0;
};

class AIController : public CreatureController
{
public:
    AIController(std::unique_ptr<AI> ai) : ai(std::move(ai)) {}
    Action control(Creature& creature) override;
    static std::unique_ptr<AIController> get(boost::string_ref id, Creature& creature);

private:
    std::unique_ptr<AI> ai;
};

class PlayerController : public CreatureController
{
public:
    PlayerController(Game& game) : game(game) {}

private:
    Action control(Creature& creature) override;

    Game& game;
};

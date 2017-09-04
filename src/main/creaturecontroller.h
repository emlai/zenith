#pragma once

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
    Action control(Creature& creature) override;
};

class PlayerController : public CreatureController
{
public:
    PlayerController(Game& game) : game(game) {}

private:
    Action control(Creature& creature) override;

    Game& game;
};

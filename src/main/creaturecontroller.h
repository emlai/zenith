#pragma once

class Creature;

class CreatureController
{
public:
    virtual ~CreatureController() = 0;
    virtual void control(Creature& creature) = 0;
};

class AIController : public CreatureController
{
    void control(Creature& creature) override;
};

class PlayerController : public CreatureController
{
    void control(Creature& creature) override;
};

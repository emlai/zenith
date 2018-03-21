#pragma once

#include <string_view>
#include <memory>

class Creature;
enum Action : int;

class AI
{
public:
    AI(Creature& creature) : creature(&creature) {}
    virtual ~AI() = default;
    virtual Action control() = 0;
    static std::unique_ptr<AI> get(std::string_view id, Creature& creature);

protected:
    Creature* creature;
};

class AttackNearestEnemy : public AI
{
private:
    using AI::AI;
    Action control() override;
};

#pragma once

#include <boost/utility/string_ref.hpp>
#include <memory>

class Creature;
enum Action : int;

class AI
{
public:
    AI(Creature& creature) : creature(&creature) {}
    virtual ~AI() = default;
    virtual Action control() = 0;
    static std::unique_ptr<AI> get(boost::string_ref id, Creature& creature);

protected:
    Creature* creature;
};

class AttackNearestEnemy : public AI
{
private:
    using AI::AI;
    Action control() override;
};

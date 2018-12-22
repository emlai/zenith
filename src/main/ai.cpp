#include "ai.h"
#include "action.h"
#include "creature.h"
#include <stdexcept>

std::unique_ptr<AI> AI::get(std::string_view id, Creature& creature)
{
    if (id == "AttackNearestEnemy") return std::make_unique<AttackNearestEnemy>(creature);

    throw std::runtime_error("Unknown AI '" + id + "'\n");
}

Action AttackNearestEnemy::control()
{
    assert(!creature->isDead());
    Action action;

    if (auto* nearestEnemy = creature->getNearestEnemy())
        action = creature->tryToMoveTowardsOrAttack(*nearestEnemy);
    else
        action = creature->tryToMoveOrAttack(randomDir8());

    if (!action) // TODO: Implement proper AI so action is never NoAction.
        return Wait;

    return action;
}

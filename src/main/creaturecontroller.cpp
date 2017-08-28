#include "creaturecontroller.h"
#include "creature.h"

CreatureController::~CreatureController() {}

void AIController::control(Creature& creature)
{
    if (auto* nearestEnemy = creature.getNearestEnemy())
        creature.tryToMoveTowardsOrAttack(*nearestEnemy);
    else
        creature.tryToMoveOrAttack(randomDir8());
}

void PlayerController::control(Creature& creature)
{
    // TODO: Move player input handling here.
}

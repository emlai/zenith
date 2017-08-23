#include "creaturecontroller.h"
#include "creature.h"

CreatureController::~CreatureController() {}

void AIController::control(Creature& creature)
{
    creature.tryToMoveOrAttack(randomDir8());
}

void PlayerController::control(Creature& creature)
{
    // TODO: Move player input handling here.
}

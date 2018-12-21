class AI
{
    Creature creature;

    AI(Creature creature) : creature(creature) {}
    virtual ~AI() {}
    virtual Action control() = 0;

    AI get(string id, Creature creature)
    {
        if (id == "AttackNearestEnemy")
            return std::make_unique<AttackNearestEnemy>(creature);

        throw std::runtime_error("Unknown AI '" + id + "'\n");
    }
}

class AttackNearestEnemy : AI
{
    Action control()
    {
        assert(!creature.isDead());
        Action action;

        if (var nearestEnemy = creature.getNearestEnemy())
        action = creature.tryToMoveTowardsOrAttack(nearestEnemy);
        else
        action = creature.tryToMoveOrAttack(Random.dir8());

        if (!action) // TODO: Implement proper AI so action is never NoAction.
            return Wait;

        return action;
    }
}


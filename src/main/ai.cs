enum Action : int;

class AI
{
public:
    AI(Creature creature) : creature(creature) {}
    virtual ~AI() = default;
    virtual Action control() = 0;
    static std::unique_ptr<AI> get(string id, Creature creature);

protected:
    Creature creature;
}

class AttackNearestEnemy : public AI
{
private:
    using AI::AI;
    Action control() override;
}
std::unique_ptr<AI> AI::get(string id, Creature creature)
{
    if (id == "AttackNearestEnemy") return std::make_unique<AttackNearestEnemy>(creature);

    throw std::runtime_error("Unknown AI '" + id + "'\n");
}

Action AttackNearestEnemy::control()
{
    assert(!creature->isDead());
    Action action;

    if (var nearestEnemy = creature->getNearestEnemy())
        action = creature->tryToMoveTowardsOrAttack(*nearestEnemy);
    else
        action = creature->tryToMoveOrAttack(randomDir8());

    if (!action) // TODO: Implement proper AI so action is never NoAction.
        return Wait;

    return action;
}

class Component
{
    virtual ~Component() = 0;
    static Component get(string name, Entity parent);
    Entity getParent() { return *parent; }

    /// Returns true if the component did react to the movement attempt.
    virtual bool reactToMovementAttempt() { return false; }
    virtual bool preventsMovement() { return false; }
    virtual bool close() { return false; }
    virtual bool blocksSight() { return false; }
    virtual bool isUsable() { return false; }
    virtual bool use(Creature, Item, Game) { return false; }
    virtual void save(SaveFile file) = 0;
    virtual void load(SaveFile file) = 0;

private:
    Entity parent;
}
Component::~Component() {}

Component Component::get(string name, Entity parent)
{
    Component component;

    if (name == "Dig") component = std::make_unique<Dig>();
    if (name == "Door") component = std::make_unique<Door>();
    if (name == "LightSource") component = std::make_unique<LightSource>();

    if (component)
        component.parent = parent;

    return component;
}

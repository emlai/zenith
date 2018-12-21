class Entity
{
public:
    Entity(string id, Config config);
    Entity(Entity) = delete;
    Entity(Entity) = default;
    Entity operator=(Entity) = delete;
    Entity operator=(Entity) = default;
    virtual ~Entity() = default;

    string getName();
    string getNameIndefinite();
    string getId() { return id; }
    Config getConfig() { return *config; }
    template<typename ComponentType>
    List<ComponentType> getComponentsOfType();

    /// Returns true if the entity reacted to the movement attempt.
    bool reactToMovementAttempt();
    bool preventsMovement();
    bool close();

protected:
    const List<std::unique_ptr<Component>>& getComponents() { return components; }

private:
    virtual string getNameAdjective() { return ""; }

    string id;
    Config config;
    List<std::unique_ptr<Component>> components;
}

template<typename ComponentType>
List<ComponentType> Entity::getComponentsOfType()
{
    List<ComponentType> componentsOfType;

    for (var component : components)
        if (var p = dynamic_cast<ComponentType>(component.get()))
            componentsOfType.push_back(p);

    return componentsOfType;
}
static void reportUnknownComponent(string name)
{
    static boost::unordered_set<string> reportedNames;

    if (reportedNames.insert(name.to_string()).second)
        std::cerr << "Unknown component '" << name << "'\n";
}

Entity::Entity(string id, Config config)
:   id(id),
    config(config)
{
    if (var componentNames = config.getOptional<List<string>>(id, "components"))
    {
        for (var componentName : *componentNames)
        {
            if (var component = Component::get(componentName, *this))
                components.push_back(std::move(component));
            else
                reportUnknownComponent(componentName);
        }
    }
}

string Entity::getName()
{
    string prefix = std::move(getConfig().getOptional<string>(getId(), "NamePrefix").get_value_or(""));

    if (!prefix.empty())
        prefix += ' ';

    string adjective = getNameAdjective();

    if (!adjective.empty())
        adjective += ' ';

    return prefix + adjective + pascalCaseToSentenceCase(id);
}

string Entity::getNameIndefinite()
{
    var name = getName();

    if (isVowel(name[0]))
        name.insert(0, "an ");
    else
        name.insert(0, "a ");

    return name;
}

bool Entity::reactToMovementAttempt()
{
    bool returnValue = false;

    for (var component : components)
        if (component->reactToMovementAttempt())
            returnValue = true;

    return returnValue;
}

bool Entity::preventsMovement()
{
    for (var component : components)
        if (component->preventsMovement())
            return true;

    return config->get<bool>(id, "preventsMovement");
}

class Entity
{
public:
    Entity(boost::string_ref id, const Config& config);
    Entity(const Entity&) = delete;
    Entity(Entity&&) = default;
    Entity& operator=(const Entity&) = delete;
    Entity& operator=(Entity&&) = default;
    virtual ~Entity() = default;

    std::string getName() const;
    std::string getNameIndefinite() const;
    boost::string_ref getId() const { return id; }
    const Config& getConfig() const { return *config; }
    template<typename ComponentType>
    std::vector<ComponentType*> getComponentsOfType() const;

    /// Returns true if the entity reacted to the movement attempt.
    bool reactToMovementAttempt();
    bool preventsMovement() const;
    bool close();

protected:
    const std::vector<std::unique_ptr<Component>>& getComponents() const { return components; }

private:
    virtual std::string getNameAdjective() const { return ""; }

    std::string id;
    const Config* config;
    std::vector<std::unique_ptr<Component>> components;
};

template<typename ComponentType>
std::vector<ComponentType*> Entity::getComponentsOfType() const
{
    std::vector<ComponentType*> componentsOfType;

    for (var component : components)
        if (var p = dynamic_cast<ComponentType*>(component.get()))
            componentsOfType.push_back(p);

    return componentsOfType;
}
static void reportUnknownComponent(boost::string_ref name)
{
    static boost::unordered_set<std::string> reportedNames;

    if (reportedNames.insert(name.to_string()).second)
        std::cerr << "Unknown component '" << name << "'\n";
}

Entity::Entity(boost::string_ref id, const Config& config)
:   id(id),
    config(&config)
{
    if (var componentNames = config.getOptional<std::vector<std::string>>(id, "components"))
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

std::string Entity::getName() const
{
    std::string prefix = std::move(getConfig().getOptional<std::string>(getId(), "NamePrefix").get_value_or(""));

    if (!prefix.empty())
        prefix += ' ';

    std::string adjective = getNameAdjective();

    if (!adjective.empty())
        adjective += ' ';

    return prefix + adjective + pascalCaseToSentenceCase(id);
}

std::string Entity::getNameIndefinite() const
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

bool Entity::preventsMovement() const
{
    for (var component : components)
        if (component->preventsMovement())
            return true;

    return config->get<bool>(id, "preventsMovement");
}

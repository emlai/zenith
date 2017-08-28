#include "entity.h"
#include <iostream>
#include <unordered_set>

static void reportUnknownComponent(boost::string_ref name)
{
    static std::unordered_set<std::string> reportedNames;

    if (reportedNames.insert(name.to_string()).second)
        std::cerr << "Unknown component '" << name << "'\n";
}

Entity::Entity(boost::string_ref id, const Config& config)
:   id(id),
    config(&config)
{
    auto componentNames = config.get<std::vector<std::string>>(id, "components");

    for (auto& componentName : componentNames)
    {
        if (auto component = Component::get(componentName, *this))
            components.push_back(std::move(component));
        else
            reportUnknownComponent(componentName);
    }
}

std::string Entity::getName() const
{
    std::string prefix = getNamePrefix();

    if (!prefix.empty())
        prefix += ' ';

    return prefix + pascalCaseToSentenceCase(id);
}

std::string Entity::getNameIndefinite() const
{
    auto name = getName();

    if (isVowel(name[0]))
        name.insert(0, "an ");
    else
        name.insert(0, "a ");

    return name;
}

bool Entity::reactToMovementAttempt()
{
    bool returnValue = false;

    for (auto& component : components)
        if (component->reactToMovementAttempt())
            returnValue = true;

    return returnValue;
}

bool Entity::preventsMovement() const
{
    for (auto& component : components)
        if (component->preventsMovement())
            return true;

    return config->get<bool>(id, "preventsMovement");
}

#include "entity.h"
#include "engine/config.h"
#include "engine/error.h"
#include "engine/utility.h"

Entity::Entity(std::string_view id, const Config& config)
:   id(id),
    config(&config)
{
    if (auto componentNames = config.getOptional<std::vector<std::string>>(id, "components"))
    {
        for (auto& componentName : *componentNames)
        {
            if (auto component = Component::get(componentName, *this))
                components.push_back(std::move(component));
            else
                warn("Unknown component '" + componentName + "'");
        }
    }
}

std::string Entity::getName() const
{
    std::string prefix = getConfig().getOptional<std::string>(getId(), "NamePrefix").value_or("");

    if (!prefix.empty())
        prefix += ' ';

    std::string adjective = getNameAdjective();

    if (!adjective.empty())
        adjective += ' ';

    return prefix + adjective + pascalCaseToSentenceCase(id);
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

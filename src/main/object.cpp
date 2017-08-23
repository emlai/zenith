#include "object.h"
#include "game.h"
#include "gui.h"
#include <iostream>
#include <unordered_set>

static void reportUnknownComponent(boost::string_ref name)
{
    static std::unordered_set<std::string> reportedNames;

    if (reportedNames.insert(name.to_string()).second)
        std::cerr << "Unknown component '" << name << "'\n";
}

Object::Object(boost::string_ref id)
:   id(id),
    sprite(*Game::objectSpriteSheet, getSpriteTextureRegion(Game::objectConfig, id))
{
    auto componentNames = Game::objectConfig.get<std::vector<std::string>>(id.to_string(), "components");

    for (auto& componentName : componentNames)
    {
        if (auto component = Component::get(componentName, *this))
            components.push_back(std::move(component));
        else
            reportUnknownComponent(componentName);
    }
}

void Object::render(Window& window, Vector2 position) const
{
    sprite.render(window, position);
}

bool Object::reactToMovementAttempt()
{
    bool returnValue = false;

    for (auto& component : components)
        if (component->reactToMovementAttempt())
            returnValue = true;

    return returnValue;
}

bool Object::preventsMovement()
{
    for (auto& component : components)
        if (component->preventsMovement())
            return true;

    return Game::objectConfig.get<bool>(id, "preventsMovement");
}

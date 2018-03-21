#pragma once

#include "component.h"
#include "engine/config.h"
#include <string_view>
#include <memory>
#include <string>
#include <vector>

class Entity
{
public:
    Entity(std::string_view id, const Config& config);
    Entity(const Entity&) = delete;
    Entity(Entity&&) = default;
    Entity& operator=(const Entity&) = delete;
    Entity& operator=(Entity&&) = default;
    virtual ~Entity() = default;

    std::string getName() const;
    std::string getNameIndefinite() const;
    std::string_view getId() const { return id; }
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

    for (auto& component : components)
        if (auto* p = dynamic_cast<ComponentType*>(component.get()))
            componentsOfType.push_back(p);

    return componentsOfType;
}

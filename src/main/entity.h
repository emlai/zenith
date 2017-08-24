#pragma once

#include "component.h"
#include "engine/config.h"
#include <boost/utility/string_ref.hpp>
#include <memory>
#include <string>
#include <vector>

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
    boost::string_ref getId() const { return id; }

    /// Returns true if the entity reacted to the movement attempt.
    bool reactToMovementAttempt();
    bool preventsMovement() const;

private:
    std::string id;
    const Config* config;
    std::vector<std::unique_ptr<Component>> components;
};

#pragma once

class Entity
{
public:
    Entity() = default;
    Entity(const Entity&) = delete;
    Entity(Entity&&) = default;
    Entity& operator=(const Entity&) = delete;
    Entity& operator=(Entity&&) = default;
    virtual ~Entity() = default;
    virtual void exist() {}
};

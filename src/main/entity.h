#pragma once

#include <boost/utility/string_ref.hpp>
#include <string>

class Entity
{
public:
    Entity(boost::string_ref id);
    Entity(const Entity&) = delete;
    Entity(Entity&&) = default;
    Entity& operator=(const Entity&) = delete;
    Entity& operator=(Entity&&) = default;
    virtual ~Entity() = default;

    std::string getName() const;
    boost::string_ref getId() const { return id; }

private:
    std::string id;
};

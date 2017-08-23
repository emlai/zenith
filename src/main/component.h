#pragma once

#include <boost/utility/string_ref.hpp>
#include <cassert>
#include <memory>

class Object;

class Component
{
public:
    virtual ~Component() = 0;
    static std::unique_ptr<Component> get(boost::string_ref name, Object& parent);
    Object& getParent() const { return *parent; }

    /// Returns true if the component did react to the movement attempt.
    virtual bool reactToMovementAttempt() { return false; }
    virtual bool preventsMovement() { return false; }

private:
    Object* parent;
};
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

    virtual void reactToMovementAttempt() {}

private:
    Object* parent;
};

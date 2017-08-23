#include "component.h"
#include "components/door.h"

Component::~Component() {}

std::unique_ptr<Component> Component::get(boost::string_ref name, Object& parent)
{
    std::unique_ptr<Component> component;

    if (name == "Door") component = std::make_unique<Door>();

    if (component)
        component->parent = &parent;

    return component;
}

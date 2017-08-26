#include "component.h"
#include "components/door.h"
#include "components/lightsource.h"

Component::~Component() {}

std::unique_ptr<Component> Component::get(boost::string_ref name, Entity& parent)
{
    std::unique_ptr<Component> component;

    if (name == "Door") component = std::make_unique<Door>();
    if (name == "LightSource") component = std::make_unique<LightSource>();

    if (component)
        component->parent = &parent;

    return component;
}

#include "component.h"
#include "components/dig.h"
#include "components/door.h"
#include "components/lightsource.h"

Component::~Component() {}

std::unique_ptr<Component> Component::get(boost::string_ref name, Entity& parent)
{
    std::unique_ptr<Component> component;

    if (name == "Dig") component = make_unique<Dig>();
    if (name == "Door") component = make_unique<Door>();
    if (name == "LightSource") component = make_unique<LightSource>();

    if (component)
        component->parent = &parent;

    return component;
}

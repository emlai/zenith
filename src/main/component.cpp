#include "component.h"
#include "components/dig.h"
#include "components/door.h"
#include "components/lightsource.h"

Component::~Component() {}

std::unique_ptr<Component> Component::get(std::string_view name, Entity& parent)
{
    std::unique_ptr<Component> component;

    if (name == "Dig") component = std::make_unique<Dig>();
    if (name == "Door") component = std::make_unique<Door>();
    if (name == "LightSource") component = std::make_unique<LightSource>();

    if (component)
        component->parent = &parent;

    return component;
}

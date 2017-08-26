#pragma once

#include "../component.h"
#include "engine/color.h"
#include "engine/geometry.h"

class World;

class LightSource : public Component
{
public:
    void emitLight(World& world, Vector2 position) const;

    static const int maxRadius = 20;
};

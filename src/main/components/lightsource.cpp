#include "lightsource.h"
#include "../tile.h"
#include "../world.h"
#include "engine/assert.h"
#include "engine/raycast.h"
#include "engine/window.h"

struct EmitHandlerData
{
    World& world;
    int level;
    Vector2 targetPosition;
};

static bool emitHandler(Vector2 vector, EmitHandlerData* data)
{
    auto* tile = data->world.getTile(vector, data->level);
    return tile && (tile->getPosition() == data->targetPosition || !tile->blocksSight());
}

void LightSource::emitLight(World& world, Vector2 position, int level) const
{
    position = position.divFloor(Tile::getSize());
    auto color = Color(parent->getConfig().get<uint32_t>(parent->getId(), "LightColor"));
    int radius = parent->getConfig().get<int>(parent->getId(), "LightRadius");
    ASSERT(radius <= maxRadius); // TODO: Convert to warning.
    double radiusSquared = radius * radius;

    for (int dx = -radius; dx <= radius; ++dx)
    {
        for (int dy = -radius; dy <= radius; ++dy)
        {
            double reverseIntensity = Vector2(dx, dy).getLengthSquared() / radiusSquared;

            if (reverseIntensity >= 1.0)
                continue;

            auto targetPosition = position + Vector2(dx, dy);
            EmitHandlerData data { world, level, targetPosition };
            bool isLit = raycast(position, targetPosition, emitHandler, &data);

            if (isLit)
            {
                auto intensity = 1.0 - reverseIntensity;
                world.getTile(targetPosition, level)->addLight(color * intensity);
            }
        }
    }
}

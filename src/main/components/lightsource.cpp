#include "lightsource.h"
#include "../tile.h"
#include "../world.h"
#include "engine/window.h"

void LightSource::emitLight(World& world, Vector2 position) const
{
    position = position.divideRoundingDown(Tile::size);
    Color32 color = getParent().getConfig().get<int>(getParent().getId().to_string(), "LightColor");
    int radius = getParent().getConfig().get<int>(getParent().getId().to_string(), "LightRadius");
    assert(radius <= maxRadius);
    double radiusSquared = radius * radius;

    for (int dx = -radius; dx <= radius; ++dx)
    {
        for (int dy = -radius; dy <= radius; ++dy)
        {
            double reverseIntensity = Vector2(dx, dy).getLengthSquared() / radiusSquared;

            if (reverseIntensity > 1.0)
                continue;

            auto intensity = 1.0 - reverseIntensity;

            if (auto* tile = world.getTile(position + Vector2(dx, dy)))
                tile->addLight(color * intensity);
        }
    }
}

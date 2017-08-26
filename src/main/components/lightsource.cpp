#include "lightsource.h"
#include "../tile.h"
#include "../world.h"
#include "engine/window.h"

void LightSource::emitLight(World& world, Vector2 position, int level) const
{
    position = position.divideRoundingDown(Tile::size);
    Color32 color = getParent().getConfig().get<int>(getParent().getId(), "LightColor");
    int radius = getParent().getConfig().get<int>(getParent().getId(), "LightRadius");
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

            if (auto* tile = world.getTile(position + Vector2(dx, dy), level))
                tile->addLight(color * intensity);
        }
    }
}

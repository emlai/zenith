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

            if (reverseIntensity >= 1.0)
                continue;

            auto targetPosition = position + Vector2(dx, dy);

            bool isLit = raycastIntegerBresenham(position, targetPosition, [&](Vector2 vector)
            {
                auto* tile = world.getTile(vector, level);
                return tile && (tile->getPosition() == targetPosition || !tile->blocksSight());
            });

            if (isLit)
            {
                auto intensity = 1.0 - reverseIntensity;
                world.getTile(targetPosition, level)->addLight(color * intensity);
            }
        }
    }
}

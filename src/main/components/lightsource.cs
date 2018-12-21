class LightSource : public Component
{
public:
    void emitLight(World world, Vector2 position, int level) const;
    void save(SaveFile) const override {}
    void load(SaveFile) override {}

    static const int maxRadius = 20;
}
void LightSource::emitLight(World world, Vector2 position, int level) const
{
    position = position.divideRoundingDown(Tile::getSize());
    Color32 color = Color16(getParent().getConfig().get<ushort>(getParent().getId(), "LightColor"));
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

            var targetPosition = position + Vector2(dx, dy);

            bool isLit = raycastIntegerBresenham(position, targetPosition, [&](Vector2 vector)
            {
                var tile = world.getTile(vector, level);
                return tile && (tile->getPosition() == targetPosition || !tile->blocksSight());
            });

            if (isLit)
            {
                var intensity = 1.0 - reverseIntensity;
                world.getTile(targetPosition, level)->addLight(color * intensity);
            }
        }
    }
}

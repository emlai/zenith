#pragma once

#include "tile.h"
#include <vector>

class Window;

class Area : public Entity
{
public:
    Area(const Config& groundConfig, const Texture& groundSpriteSheet);
    void exist() override;
    void render(Window&, int zIndex) const;
    Tile& getTileAt(Vector2 position) { return tiles[position.x + size * position.y]; }
    const Tile& getTileAt(Vector2 position) const { return tiles[position.x + size * position.y]; }
    static const int size = 64;
    static const Vector2 sizeVector;

private:
    std::vector<Tile> tiles;
};

Tile& getRandomTile(Area& area);
const Tile& getRandomTile(const Area& area);

class Area
{
    List<Tile> tiles;
    World world;
    Vector2 position;
    const int size = 64;
    const Vector2 sizeVector = new Vector2(size, size);

    World getWorld() { return world; }
    Vector2 getPosition() { return position; }

    Area(World world, Vector2 position, int level)
    :   world(world), position(position)
    {
        tiles.reserve(size * size);
        string groundId = level < 0 ? "DirtFloor" : "Grass";

        for (Vector2 pos(0, 0); pos.y < size; ++pos.y)
        {
            for (pos.x = 0; pos.x < size; ++pos.x)
            {
                tiles.emplace_back(world, position * sizeVector + pos, level, groundId);

                if (level < 0)
                    tiles.back().setObject(new Object("Ground"));
            }
        }
    }

    Area(SaveFile file, World world, Vector2 position, int level)
    :   world(world), position(position)
    {
        tiles.reserve(size * size);

        for (Vector2 pos(0, 0); pos.y < size; ++pos.y)
        {
            for (pos.x = 0; pos.x < size; ++pos.x)
                tiles.emplace_back(file, world, position * sizeVector + pos, level);
        }
    }

    void save(SaveFile file)
    {
        foreach (var tile in tiles)
            tile.save(file);
    }

    Tile getTileAt(Vector2 position)
    {
        assert(position.x >= 0 && position.x < size);
        assert(position.y >= 0 && position.y < size);
        return tiles[position.x + size * position.y];
    }

    Tile getTileAt(Vector2 position)
    {
        assert(position.x >= 0 && position.x < size);
        assert(position.y >= 0 && position.y < size);
        return tiles[position.x + size * position.y];
    }

    Tile getRandomTile()
    {
        return getTileAt(makeRandomVector(sizeVector));
    }

    Tile getRandomTile()
    {
        return getTileAt(makeRandomVector(sizeVector));
    }
}

class Area
{
    Area(World world, Vector2 position, int level);
    Area(SaveFile file, World world, Vector2 position, int level);
    void save(SaveFile file);

private:
    Tile getRandomTile();
    Tile getRandomTile();
    World getWorld() { return world; }
    Vector2 getPosition() { return position; }
    const int size = 64;
    const Vector2 sizeVector;

    Tile getTileAt(Vector2 position);
    Tile getTileAt(Vector2 position);

    List<Tile> tiles;
    World world;
    Vector2 position;
}
const Vector2 Area::sizeVector = Vector2(Area::size, Area::size);

Area::Area(World world, Vector2 position, int level)
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

Area::Area(SaveFile file, World world, Vector2 position, int level)
:   world(world), position(position)
{
    tiles.reserve(size * size);

    for (Vector2 pos(0, 0); pos.y < size; ++pos.y)
    {
        for (pos.x = 0; pos.x < size; ++pos.x)
            tiles.emplace_back(file, world, position * sizeVector + pos, level);
    }
}

void Area::save(SaveFile file)
{
    foreach (var tile in tiles)
        tile.save(file);
}

Tile Area::getTileAt(Vector2 position)
{
    assert(position.x >= 0 && position.x < size);
    assert(position.y >= 0 && position.y < size);
    return tiles[position.x + size * position.y];
}

Tile Area::getTileAt(Vector2 position)
{
    assert(position.x >= 0 && position.x < size);
    assert(position.y >= 0 && position.y < size);
    return tiles[position.x + size * position.y];
}

Tile Area::getRandomTile()
{
    return getTileAt(makeRandomVector(sizeVector));
}

Tile Area::getRandomTile()
{
    return getTileAt(makeRandomVector(sizeVector));
}

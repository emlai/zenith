class Area
{
public:
    Area(World& world, Vector2 position, int level);
    Area(const SaveFile& file, World& world, Vector2 position, int level);
    void save(SaveFile& file) const;

private:
    friend class World;

    Tile& getRandomTile();
    const Tile& getRandomTile() const;
    World& getWorld() const { return world; }
    Vector2 getPosition() const { return position; }
    static const int size = 64;
    static const Vector2 sizeVector;

    Tile& getTileAt(Vector2 position);
    const Tile& getTileAt(Vector2 position) const;

    std::vector<Tile> tiles;
    World& world;
    Vector2 position;
}
const Vector2 Area::sizeVector = Vector2(Area::size, Area::size);

Area::Area(World& world, Vector2 position, int level)
:   world(world), position(position)
{
    tiles.reserve(size * size);
    boost::string_ref groundId = level < 0 ? "DirtFloor" : "Grass";

    for (Vector2 pos(0, 0); pos.y < size; ++pos.y)
    {
        for (pos.x = 0; pos.x < size; ++pos.x)
        {
            tiles.emplace_back(world, position * sizeVector + pos, level, groundId);

            if (level < 0)
                tiles.back().setObject(std::make_unique<Object>("Ground"));
        }
    }
}

Area::Area(const SaveFile& file, World& world, Vector2 position, int level)
:   world(world), position(position)
{
    tiles.reserve(size * size);

    for (Vector2 pos(0, 0); pos.y < size; ++pos.y)
    {
        for (pos.x = 0; pos.x < size; ++pos.x)
            tiles.emplace_back(file, world, position * sizeVector + pos, level);
    }
}

void Area::save(SaveFile& file) const
{
    for (var tile : tiles)
        tile.save(file);
}

Tile& Area::getTileAt(Vector2 position)
{
    assert(position.x >= 0 && position.x < size);
    assert(position.y >= 0 && position.y < size);
    return tiles[position.x + size * position.y];
}

const Tile& Area::getTileAt(Vector2 position) const
{
    assert(position.x >= 0 && position.x < size);
    assert(position.y >= 0 && position.y < size);
    return tiles[position.x + size * position.y];
}

const Tile& Area::getRandomTile() const
{
    return getTileAt(makeRandomVector(sizeVector));
}

Tile& Area::getRandomTile()
{
    return getTileAt(makeRandomVector(sizeVector));
}

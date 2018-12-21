class Creature;
class Game;
class SaveFile;
class Tile;

class World
{
public:
    World(const Game& game) : game(&game), sunlight(0x888888FF) {}
    World(const World&) = delete;
    World(World&&) = default;
    World& operator=(const World&) = delete;
    World& operator=(World&&) = default;
    void load(SaveFile& file);
    void save(SaveFile& file) const;
    int getTurn() const;
    void exist(Rect region, int level);
    void render(Window&, Rect region, int level, const Creature& player);
    Tile* getOrCreateTile(Vector2 position, int level);
    Tile* getTile(Vector2 position, int level);
    void forEachTile(Rect region, int level, const std::function<void(Tile&)>&);
    Color32 getSunlight() const { return sunlight; }

private:
    Area* getOrCreateArea(Vector3 position);
    Area* getArea(Vector3 position);
    static Vector3 globalPositionToAreaPosition(Vector2 position, int level);
    static Vector2 globalPositionToTilePosition(Vector2 position);

    const Game* game;
    boost::unordered_map<Vector3, Area> areas;
    boost::unordered_map<Vector3, int64_t> savedAreaOffsets;
    std::unique_ptr<SaveFile> saveFile;
    Color32 sunlight;
};
void World::load(SaveFile& file)
{
    auto areaCount = file.readInt32();
    areas.reserve(size_t(areaCount));
    for (int i = 0; i < areaCount; ++i)
    {
        auto position = file.readVector3();
        auto offset = file.readInt64();
        savedAreaOffsets.emplace(position, offset);
    }

    saveFile = std::make_unique<SaveFile>(file.copyToMemory());
}

void World::save(SaveFile& file) const
{
    file.writeInt32(int32_t(areas.size()));
    auto areaPositionsOffset = file.getOffset();

    for (auto& positionAndArea : areas)
    {
        file.write(positionAndArea.first);
        file.writeInt64(int64_t(0));
    }

    int index = 0;
    for (auto& positionAndArea : areas)
    {
        auto areaOffset = file.getOffset();
        file.seek(areaPositionsOffset + index * (sizeof(Vector3) + sizeof(int64_t)) + sizeof(Vector3));
        file.writeInt64(areaOffset);
        file.seek(areaOffset);
        positionAndArea.second.save(file);
        ++index;
    }
}

int World::getTurn() const
{
    return game->getTurn();
}

void World::exist(Rect region, int level)
{
    // Collect the creatures into a vector to avoid updating the same creature more than once.
    std::vector<Creature*> creaturesToUpdate;

    forEachTile(region, level, [&](Tile& tile)
    {
        tile.exist();

        for (auto& creature : tile.getCreatures())
            creaturesToUpdate.push_back(creature.get());
    });

    // Sort the creatures according to their memory addresses to update them in a consistent order.
    std::sort(creaturesToUpdate.begin(), creaturesToUpdate.end());
    creaturesToUpdate.erase(std::unique(creaturesToUpdate.begin(), creaturesToUpdate.end()),
                            creaturesToUpdate.end());

    for (auto* creature : creaturesToUpdate)
        creature->exist();
}

void World::render(Window& window, Rect region, int level, const Creature& player)
{
    auto lightRegion = region.inset(Vector2(-LightSource::maxRadius, -LightSource::maxRadius));
    forEachTile(lightRegion, level, [&](Tile& tile) { tile.resetLight(); });
    forEachTile(lightRegion, level, [&](Tile& tile) { tile.emitLight(); });

    std::vector<std::pair<const Tile*, bool>> tilesToRender;
    tilesToRender.reserve(region.getArea());

    forEachTile(region, level, [&](const Tile& tile)
    {
        if (game->playerSeesEverything || player.sees(tile))
            tilesToRender.emplace_back(&tile, false);
        else if (player.remembers(tile))
            tilesToRender.emplace_back(&tile, true);
    });

    for (int zIndex = 0; zIndex < 7; ++zIndex)
        for (auto tileAndFogOfWar : tilesToRender)
            tileAndFogOfWar.first->render(window, zIndex, tileAndFogOfWar.second, !game->playerSeesEverything);
}

Area* World::getOrCreateArea(Vector3 position)
{
    if (auto* area = getArea(position))
        return area;

    auto& area = areas.emplace(position, Area(*this, Vector2(position), position.z)).first->second;
    WorldGenerator generator(*this);
    generator.generateRegion(Rect(Vector2(position) * Area::sizeVector, Area::sizeVector), position.z);
    return &area;
}

Area* World::getArea(Vector3 position)
{
    auto it = areas.find(position);
    if (it != areas.end())
        return &it->second;

    auto offset = savedAreaOffsets.find(position);
    if (offset != savedAreaOffsets.end())
    {
        saveFile->seek(offset->second);
        return &areas.emplace(position, Area(*saveFile, *this, Vector2(position), position.z)).first->second;
    }

    return nullptr;
}

Vector3 World::globalPositionToAreaPosition(Vector2 position, int level)
{
    return Vector3(position.divideRoundingDown(Area::size)) + Vector3(0, 0, level);
}

Vector2 World::globalPositionToTilePosition(Vector2 position)
{
    Vector2 tilePosition = position % Area::size;
    if (tilePosition.x < 0) tilePosition.x += Area::size;
    if (tilePosition.y < 0) tilePosition.y += Area::size;
    return tilePosition;
}

Tile* World::getOrCreateTile(Vector2 position, int level)
{
    if (auto* area = getOrCreateArea(globalPositionToAreaPosition(position, level)))
        return &area->getTileAt(globalPositionToTilePosition(position));

    return nullptr;
}

Tile* World::getTile(Vector2 position, int level)
{
    if (auto* area = getArea(globalPositionToAreaPosition(position, level)))
        return &area->getTileAt(globalPositionToTilePosition(position));

    return nullptr;
}

void World::forEachTile(Rect region, int level, const std::function<void(Tile&)>& function)
{
    for (int x = region.getLeft(); x <= region.getRight(); ++x)
        for (int y = region.getTop(); y <= region.getBottom(); ++y)
            if (auto* tile = getOrCreateTile(Vector2(x, y), level))
                function(*tile);
}

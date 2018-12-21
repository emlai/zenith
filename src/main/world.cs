class World
{
    Game game;
    Dictionary<Vector3, Area> areas;
    Dictionary<Vector3, long> savedAreaOffsets;
    SaveFile saveFile;
    Color32 sunlight;

    World(Game game) : game(game), sunlight(0x888888FF) {}
    Color32 getSunlight() { return sunlight; }

    void load(SaveFile file)
    {
        var areaCount = file.readInt32();
        areas.reserve(size_t(areaCount));
        for (int i = 0; i < areaCount; ++i)
        {
            var position = file.readVector3();
            var offset = file.readInt64();
            savedAreaOffsets.emplace(position, offset);
        }

        saveFile = new SaveFile(file.copyToMemory());
    }

    void save(SaveFile file)
    {
        file.writeInt32(int(areas.size()));
        var areaPositionsOffset = file.getOffset();

        foreach (var positionAndArea in areas)
        {
            file.write(positionAndArea.first);
            file.writeInt64(long(0));
        }

        int index = 0;
        foreach (var positionAndArea in areas)
        {
            var areaOffset = file.getOffset();
            file.seek(areaPositionsOffset + index * (sizeof(Vector3) + sizeof(long)) + sizeof(Vector3));
            file.writeInt64(areaOffset);
            file.seek(areaOffset);
            positionAndArea.second.save(file);
            ++index;
        }
    }

    int getTurn()
    {
        return game.getTurn();
    }

    void exist(Rect region, int level)
    {
        // Collect the creatures into a vector to avoid updating the same creature more than once.
        List<Creature> creaturesToUpdate;

        forEachTile(region, level, [&](Tile tile)
        {
            tile.exist();

            foreach (var creature in tile.getCreatures())
                creaturesToUpdate.push_back(creature.get());
        });

        // Sort the creatures according to their memory addresses to update them in a consistent order.
        std::sort(creaturesToUpdate.begin(), creaturesToUpdate.end());
        creaturesToUpdate.erase(std::unique(creaturesToUpdate.begin(), creaturesToUpdate.end()),
                                creaturesToUpdate.end());

        foreach (var creature in creaturesToUpdate)
            creature.exist();
    }

    void render(Window window, Rect region, int level, Creature player)
    {
        var lightRegion = region.inset(Vector2(-LightSource::maxRadius, -LightSource::maxRadius));
        forEachTile(lightRegion, level, [&](Tile tile) { tile.resetLight(); });
        forEachTile(lightRegion, level, [&](Tile tile) { tile.emitLight(); });

        List<std::pair<Tile, bool>> tilesToRender;
        tilesToRender.reserve(region.getArea());

        forEachTile(region, level, [&](Tile tile)
        {
            if (game.playerSeesEverything || player.sees(tile))
                tilesToRender.emplace_back(tile, false);
            else if (player.remembers(tile))
                tilesToRender.emplace_back(tile, true);
        });

        for (int zIndex = 0; zIndex < 7; ++zIndex)
            foreach (var tileAndFogOfWar in tilesToRender)
                tileAndFogOfWar.first.render(window, zIndex, tileAndFogOfWar.second, !game.playerSeesEverything);
    }

    Area getOrCreateArea(Vector3 position)
    {
        if (var area = getArea(position))
            return area;

        var area = areas.emplace(position, Area(this, Vector2(position), position.z)).first.second;
        WorldGenerator generator(this);
        generator.generateRegion(Rect(Vector2(position) * Area::sizeVector, Area::sizeVector), position.z);
        return area;
    }

    Area getArea(Vector3 position)
    {
        var it = areas.find(position);
        if (it != areas.end())
            return it.second;

        var offset = savedAreaOffsets.find(position);
        if (offset != savedAreaOffsets.end())
        {
            saveFile.seek(offset.second);
            return areas.emplace(position, Area(saveFile, this, Vector2(position), position.z)).first.second;
        }

        return null;
    }

    Vector3 globalPositionToAreaPosition(Vector2 position, int level)
    {
        return Vector3(position.divideRoundingDown(Area::size)) + Vector3(0, 0, level);
    }

    Vector2 globalPositionToTilePosition(Vector2 position)
    {
        Vector2 tilePosition = position % Area::size;
        if (tilePosition.x < 0) tilePosition.x += Area::size;
        if (tilePosition.y < 0) tilePosition.y += Area::size;
        return tilePosition;
    }

    Tile getOrCreateTile(Vector2 position, int level)
    {
        if (var area = getOrCreateArea(globalPositionToAreaPosition(position, level)))
            return area.getTileAt(globalPositionToTilePosition(position));

        return null;
    }

    Tile getTile(Vector2 position, int level)
    {
        if (var area = getArea(globalPositionToAreaPosition(position, level)))
            return area.getTileAt(globalPositionToTilePosition(position));

        return null;
    }

    void forEachTile(Rect region, int level, const std::function<void(Tile)>& function)
    {
        for (int x = region.getLeft(); x <= region.getRight(); ++x)
            for (int y = region.getTop(); y <= region.getBottom(); ++y)
                if (var tile = getOrCreateTile(Vector2(x, y), level))
                    function(tile);
    }
}

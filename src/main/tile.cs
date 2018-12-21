class Tile
{
    Tile(World world, Vector2 position, int level, string groundId);
    Tile(SaveFile file, World world, Vector2 position, int level);
    void save(SaveFile file);
    void exist();
    void render(Window window, int zIndex, bool fogOfWar, bool renderLight);
    template<typename... Args>
    Creature spawnCreature(Args...);
    bool hasCreature() { return !creatures.empty(); }
    var getCreatures() { return creatures; }
    Creature getCreature(int index) { return *creatures[index]; }
    void transferCreature(Creature, Tile);
    std::unique_ptr<Creature> removeSingleTileCreature(Creature);
    void removeCreature(Creature);
    bool hasItems() { return !items.empty(); }
    const List<std::unique_ptr<Item>>& getItems() { return items; }
    std::unique_ptr<Item> removeTopmostItem();
    void addItem(std::unique_ptr<Item> item);
    void addLiquid(string materialId);
    bool hasObject() { return bool(object); }
    Object getObject() { return object.get(); }
    Object getObject() { return object.get(); }
    void setObject(std::unique_ptr<Object>);
    string getGroundId() { return groundId; }
    void setGround(string groundId);
    void forEachEntity(const std::function<void(Entity)>& function);
    void forEachLightSource(const std::function<void(LightSource)>& function);
    Color32 getLight() { return light; }
    void emitLight();
    void addLight(Color32 light) { this.light.lighten(light); }
    void resetLight();
    bool blocksSight();
    Tile getAdjacentTile(Dir8);
    Tile getPreExistingAdjacentTile(Dir8);
    Tile getTileBelow();
    Tile getTileAbove();
    World getWorld() { return world; }
    Vector2 getPosition() { return position; }
    Vector3 getPosition3D() { return Vector3(position) + Vector3(0, 0, level); }
    int getLevel() { return level; }
    Vector2 getCenterPosition() { return position * getSize() + getSize() / 2; }
    static Vector2 getSize();
    static Vector2 getMaxSize();
    const Vector2 spriteSize;

private:
    string getTooltip();
    void addCreature(std::unique_ptr<Creature> creature) { creatures.push_back(std::move(creature)); }

    List<std::unique_ptr<Creature>> creatures;
    List<std::unique_ptr<Item>> items;
    List<Liquid> liquids;
    std::unique_ptr<Object> object;
    World world;
    Vector2 position;
    int level;
    string groundId;
    Sprite groundSprite;
    Color32 light;
}

template<typename... Args>
Creature Tile::spawnCreature(Args... creatureArgs)
{
    addCreature(std::make_unique<Creature>(this, std::forward<Args>(creatureArgs)...));
    return creatures.back().get();
}
const Vector2 Tile::spriteSize(20, 20);

Tile::Tile(World world, Vector2 position, int level, string groundId)
:   world(world),
    position(position),
    level(level),
    groundId(groundId),
    groundSprite(getSprite(*Game::groundSpriteSheet, *Game::groundConfig, groundId)),
    light(Color32::black)
{
}

Tile::Tile(SaveFile file, World world, Vector2 position, int level)
:   world(world),
    position(position),
    level(level),
    groundId(file.readString()),
    groundSprite(getSprite(*Game::groundSpriteSheet, *Game::groundConfig, groundId)),
    light(Color32::black)
{
    var creatureCount = file.readInt32();
    creatures.reserve(size_t(creatureCount));
    for (int i = 0; i < creatureCount; ++i)
        creatures.push_back(std::make_unique<Creature>(file, this));

    var itemCount = file.readInt32();
    items.reserve(size_t(itemCount));
    for (int i = 0; i < itemCount; ++i)
        items.push_back(Item::load(file));

    var liquidCount = file.readInt32();
    liquids.reserve(size_t(liquidCount));
    for (int i = 0; i < liquidCount; ++i)
        liquids.push_back(Liquid(file));

    if (file.readBool())
        object = std::make_unique<Object>(file);
}

void Tile::save(SaveFile file)
{
    file.write(groundId);
    file.write(creatures);
    file.write(items);
    file.write(liquids);
    file.write(object != nullptr);
    if (object)
        object.save(file);
}

void Tile::exist()
{
    for (var it = liquids.begin(); it != liquids.end();)
    {
        if (it.exists())
        {
            it.exist();
            ++it;
        }
        else
            it = liquids.erase(it);
    }

    for (var item : items)
        item.exist();
}

void Tile::render(Window window, int zIndex, bool fogOfWar, bool renderLight)
{
    Vector2 renderPosition = position * getSize();

    switch (zIndex)
    {
        case 0:
            groundSprite.render(window, renderPosition);

            for (var liquid : liquids)
                liquid.render(window, renderPosition);
            break;
        case 1:
            for (var item : items)
                item.render(window, renderPosition);
            break;
        case 2:
            if (object)
                object.render(window, renderPosition);
            break;
        case 3:
            if (fogOfWar)
                break;

            for (var creature : creatures)
                creature.render(window, renderPosition);
            break;
        case 4:
            if (fogOfWar || !renderLight)
                break;

            window.getGraphicsContext().renderFilledRectangle(Rect(renderPosition, getSize()),
                                                              light, BlendMode::LinearLight);
            break;
        case 5:
            if (fogOfWar)
                Game::fogOfWarTexture.render(window, renderPosition, getSize());
            break;
        case 6:
        {
            bool showTooltip = true;
            if (showTooltip)
            {
                Rect tileRect(renderPosition, getSize());

                if (window.getMousePosition().isWithin(tileRect))
                {
                    Game::cursorPosition = getPosition();
                    double cursorBreathRateMS = 150.0;
                    double sine = std::sin(SDL_GetTicks() / cursorBreathRateMS);
                    double minAlpha = 0.0;
                    double maxAlpha = 0.5;
                    double currentAlpha = minAlpha + (sine + 1) / 2 * (maxAlpha - minAlpha);
                    Color32 cursorColor = Color32(0xFF, 0xFF, 0xFF, currentAlpha * 0xFF);
                    Game::cursorTexture.setColor(cursorColor);
                    Game::cursorTexture.render(window, tileRect);

                    var tooltip = getTooltip();
                    if (!tooltip.empty())
                    {
                        int lineHeight = 2;
                        Rect lineArea(tileRect.position.x + getSize().x,
                                      tileRect.position.y + getSize().y / 2 - lineHeight / 2,
                                      getSize().x / 2, lineHeight);
                        window.getGraphicsContext().renderFilledRectangle(lineArea, GUIColor::Black);

                        Vector2 inset = Vector2(window.getFont().getColumnWidth(),
                                                window.getFont().getRowHeight() / 2);
                        Rect tooltipArea(Vector2(lineArea.getRight(), tileRect.getTop()),
                                         window.getFont().getTextSize(tooltip) + inset * 2);
                        window.getGraphicsContext().renderFilledRectangle(tooltipArea.inset(Vector2(0, 1)), GUIColor::Black);
                        window.getGraphicsContext().renderFilledRectangle(tooltipArea.inset(Vector2(1, 0)), GUIColor::Black);
                        window.getFont().setArea(tooltipArea.offset(inset));
                        window.getFont().print(window, tooltip, TextColor::White, GUIColor::Black, true, PreserveLines);
                    }
                }
            }
            break;
        }
        default:
            assert(false);
    }

}

string Tile::getTooltip()
{
    string tooltip;

    for (var creature : creatures)
    {
        tooltip += creature.getName();
        tooltip += '\n';
    }

    for (var item : boost::adaptors::reverse(items))
    {
        tooltip += item.getName();
        tooltip += '\n';
    }

    if (object)
    {
        tooltip += object.getName();
        tooltip += '\n';
    }

    return tooltip;
}

void Tile::transferCreature(Creature creature, Tile destination)
{
    for (var it = creatures.begin(); it != creatures.end(); ++it)
    {
        if (it.get() == creature)
        {
            destination.addCreature(std::move(*it));
            creatures.erase(it);
            return;
        }
    }

    assert(false);
}

std::unique_ptr<Creature> Tile::removeSingleTileCreature(Creature creature)
{
    assert(creature.getTilesUnder().size() == 1);

    for (var it = creatures.begin(); it != creatures.end(); ++it)
    {
        if (it.get() == creature)
        {
            var removed = std::move(*it);
            creatures.erase(it);
            return removed;
        }
    }

    assert(false);
}

void Tile::removeCreature(Creature creature)
{
    var newEnd = std::remove_if(creatures.begin(), creatures.end(),
                                 [&](var ptr) { return ptr.get() == creature; });
    creatures.erase(newEnd, creatures.end());
}

std::unique_ptr<Item> Tile::removeTopmostItem()
{
    var item = std::move(items.back());
    items.pop_back();
    return item;
}

void Tile::addItem(std::unique_ptr<Item> item)
{
    items.push_back(std::move(item));
}

void Tile::addLiquid(string materialId)
{
    liquids.push_back(Liquid(materialId));
}

void Tile::setObject(std::unique_ptr<Object> newObject)
{
    object = std::move(newObject);
}

void Tile::setGround(string groundId)
{
    this.groundId = groundId.to_string();
    groundSprite = getSprite(*Game::groundSpriteSheet, *Game::groundConfig, groundId);
}

void Tile::forEachEntity(const std::function<void(Entity)>& function)
{
    for (var creature : creatures)
    {
        function(*creature);

        for (var slotAndItem : creature.getEquipment())
            if (slotAndItem.second)
                function(*slotAndItem.second);
    }

    for (var item : items)
        function(*item);

    if (object)
        function(*object);
}

void Tile::forEachLightSource(const std::function<void(LightSource)>& function)
{
    forEachEntity([&](Entity entity)
    {
        for (var lightSource : entity.getComponentsOfType<LightSource>())
            function(*lightSource);
    });
}

void Tile::emitLight()
{
    forEachLightSource([&](var lightSource)
    {
        lightSource.emitLight(world, this.getCenterPosition(), level);
    });
}

void Tile::resetLight()
{
    if (getLevel() >= 0)
        light = world.getSunlight();
    else
        light = Color32::black;
}

bool Tile::blocksSight()
{
    return hasObject() && getObject().blocksSight();
}

Tile Tile::getAdjacentTile(Dir8 direction)
{
    return getWorld().getOrCreateTile(getPosition() + direction, level);
}

Tile Tile::getPreExistingAdjacentTile(Dir8 direction)
{
    return getWorld().getTile(getPosition() + direction, level);
}

Tile Tile::getTileBelow()
{
    return getWorld().getOrCreateTile(getPosition(), level - 1);
}

Tile Tile::getTileAbove()
{
    return getWorld().getOrCreateTile(getPosition(), level + 1);
}

Vector2 Tile::getSize()
{
    if (Sprite::useAsciiGraphics())
        return Sprite::getAsciiGraphicsFont().getCharSize();
    else
        return spriteSize;
}

Vector2 Tile::getMaxSize()
{
    return Vector2(std::max(Sprite::getAsciiGraphicsFont().getCharSize().x, spriteSize.x),
                   std::max(Sprite::getAsciiGraphicsFont().getCharSize().y, spriteSize.y));
}

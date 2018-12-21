class Object : Entity
{
    Object(string id);
    Object(SaveFile file);
    void save(SaveFile file);
    bool close();
    bool blocksSight();
    void render(Window window, Vector2 position);
    Sprite getSprite() { return sprite; }

private:
    Sprite sprite;
}
Object::Object(string id)
:   Entity(id, Game::objectConfig),
    sprite(::getSprite(Game::objectSpriteSheet, Game::objectConfig, id))
{
}

Object::Object(SaveFile file)
:   Entity(file.readString(), Game::objectConfig),
    sprite(::getSprite(Game::objectSpriteSheet, Game::objectConfig, getId()))
{
    foreach (var component in getComponents())
        component.load(file);
}

void Object::save(SaveFile file)
{
    file.write(getId());

    foreach (var component in getComponents())
        component.save(file);
}

bool Object::close()
{
    bool returnValue = false;

    foreach (var component in getComponents())
        if (component.close())
            returnValue = true;

    return returnValue;
}

bool Object::blocksSight()
{
    foreach (var component in getComponents())
        if (component.blocksSight())
            return true;

    return Game::objectConfig.get<bool>(getId(), "blocksSight");
}

void Object::render(Window window, Vector2 position)
{
    sprite.render(window, position);
}

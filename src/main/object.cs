class Object : public Entity
{
public:
    Object(boost::string_ref id);
    Object(SaveFile file);
    void save(SaveFile file) const;
    bool close();
    bool blocksSight() const;
    void render(Window window, Vector2 position) const;
    Sprite getSprite() { return sprite; }

private:
    Sprite sprite;
}
Object::Object(boost::string_ref id)
:   Entity(id, *Game::objectConfig),
    sprite(::getSprite(*Game::objectSpriteSheet, *Game::objectConfig, id))
{
}

Object::Object(SaveFile file)
:   Entity(file.readString(), *Game::objectConfig),
    sprite(::getSprite(*Game::objectSpriteSheet, *Game::objectConfig, getId()))
{
    for (var component : getComponents())
        component->load(file);
}

void Object::save(SaveFile file) const
{
    file.write(getId());

    for (var component : getComponents())
        component->save(file);
}

bool Object::close()
{
    bool returnValue = false;

    for (var component : getComponents())
        if (component->close())
            returnValue = true;

    return returnValue;
}

bool Object::blocksSight() const
{
    for (var component : getComponents())
        if (component->blocksSight())
            return true;

    return Game::objectConfig->get<bool>(getId(), "blocksSight");
}

void Object::render(Window window, Vector2 position) const
{
    sprite.render(window, position);
}

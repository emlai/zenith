class Object : Entity
{
    Sprite sprite;

    Sprite getSprite() { return sprite; }

    Object(string id)
    :   Entity(id, Game::objectConfig),
        sprite(::getSprite(Game::objectSpriteSheet, Game::objectConfig, id))
    {
    }

    Object(SaveFile file)
    :   Entity(file.readString(), Game::objectConfig),
        sprite(::getSprite(Game::objectSpriteSheet, Game::objectConfig, getId()))
    {
        foreach (var component in getComponents())
            component.load(file);
    }

    void save(SaveFile file)
    {
        file.write(getId());

        foreach (var component in getComponents())
            component.save(file);
    }

    bool close()
    {
        bool returnValue = false;

        foreach (var component in getComponents())
            if (component.close())
                returnValue = true;

        return returnValue;
    }

    bool blocksSight()
    {
        foreach (var component in getComponents())
            if (component.blocksSight())
                return true;

        return Game::objectConfig.get<bool>(getId(), "blocksSight");
    }

    void render(Window window, Vector2 position)
    {
        sprite.render(window, position);
    }
}

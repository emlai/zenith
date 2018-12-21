class Liquid
{
    Liquid(string materialId);
    Liquid(SaveFile file);
    void save(SaveFile file);
    void exist();
    bool exists();
    void render(Window window, Vector2 position);

private:
    constexpr double fadeRate = 0.001;

    string materialId;
    double fadeLevel;
    Texture texture;
}
Liquid::Liquid(string materialId)
:   materialId(materialId),
    fadeLevel(1.0),
    texture(SDL_PIXELFORMAT_RGBA8888, Tile::getSize())
{
    int width = 3;
    int height = 3;
    Vector2 position(randInt(Tile::getSize().x - width), randInt(Tile::getSize().y - height));
    SDL_Rect liquidRectangle = { position.x, position.y, width, height }
    Color32 color = Color16(Game::materialConfig.get<ushort>(materialId, "Color"));
    SDL_FillRect(texture.getSurface(), liquidRectangle, color.value);
}

Liquid::Liquid(SaveFile file)
:   Liquid(file.readString())
{
    fadeLevel = file.readDouble();
}

void Liquid::save(SaveFile file)
{
    file.write(materialId);
    file.write(fadeLevel);
}

void Liquid::exist()
{
    fadeLevel = std::max(0.0, fadeLevel - fadeRate);
}

bool Liquid::exists()
{
    return fadeLevel > 0.0;
}

void Liquid::render(Window window, Vector2 position)
{
    SDL_SetSurfaceAlphaMod(texture.getSurface(), uint_t(fadeLevel * 255));
    texture.render(window, position);
}

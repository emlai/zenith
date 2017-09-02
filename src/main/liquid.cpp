#include "liquid.h"
#include "game.h"
#include "tile.h"

Liquid::Liquid(boost::string_ref materialId)
:   materialId(materialId),
    fadeLevel(1.0),
    texture(SDL_PIXELFORMAT_RGBA8888, Tile::sizeVector)
{
    int width = 3;
    int height = 3;
    Vector2 position(randInt(Tile::size - width), randInt(Tile::size - height));
    SDL_Rect liquidRectangle = { position.x, position.y, width, height };
    Color32 color = Color16(Game::materialConfig.get<uint16_t>(materialId, "Color"));
    SDL_FillRect(texture.getSurface(), &liquidRectangle, color.value);
}

Liquid::Liquid(const SaveFile& file)
:   Liquid(file.readString())
{
    fadeLevel = file.readDouble();
}

void Liquid::save(SaveFile& file) const
{
    file.write(materialId);
    file.write(fadeLevel);
}

void Liquid::exist()
{
    fadeLevel = std::max(0.0, fadeLevel - fadeRate);
}

bool Liquid::exists() const
{
    return fadeLevel > 0.0;
}

void Liquid::render(Window& window, Vector2 position) const
{
    SDL_SetSurfaceAlphaMod(texture.getSurface(), uint8_t(fadeLevel * 255));
    texture.render(window, position);
}

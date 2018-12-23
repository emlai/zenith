#include "liquid.h"
#include "game.h"
#include "tile.h"

Liquid::Liquid(std::string_view materialId)
:   materialId(materialId),
    fadeLevel(1.0),
    texture(SDL_PIXELFORMAT_RGBA8888, Tile::getSize())
{
    int width = 3;
    int height = 3;
    Vector2 position(randInt(Tile::getSize().x - width), randInt(Tile::getSize().y - height));
    SDL_Rect liquidRectangle = { position.x, position.y, width, height };
    auto color = Color(Game::materialConfig->get<uint32_t>(materialId, "Color"));
    SDL_FillRect(texture.surface.get(), &liquidRectangle, color.intValue());
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
    SDL_SetSurfaceAlphaMod(texture.surface.get(), uint8_t(fadeLevel * 255));
    texture.render(window, position);
}

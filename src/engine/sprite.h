#pragma once

#include "geometry.h"

class Texture;
class Window;

class Sprite
{
public:
    Sprite(const Texture& texture, Rect textureRegion);
    void render(Window&, Vector2 position) const;
    void setFrame(int newFrame) { frame = newFrame; }

private:
    const Texture* texture;
    Rect textureRegion;
    int frame;
};

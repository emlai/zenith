#pragma once

#include "color.h"
#include "geometry.h"

class Texture;
class Window;

class Sprite
{
public:
    Sprite(const Texture& texture, Rect textureRegion, Color32 materialColor = Color32::none);
    Vector2 getSize() const { return textureRegion.size; }
    int getWidth() const { return textureRegion.size.x; }
    int getHeight() const { return textureRegion.size.y; }
    void render(Window& window, Vector2 position, Vector2 sourceOffset = Vector2(0, 0)) const;
    Color32 getMaterialColor() const { return materialColor; }
    void setMaterialColor(Color32 color) { materialColor = color; }
    void setFrame(int newFrame) { frame = newFrame; }

private:
    const Texture* texture;
    Rect textureRegion;
    Color32 materialColor;
    int frame;
};

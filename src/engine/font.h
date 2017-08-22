#pragma once

#include "texture.h"
#include "color.h"
#include "geometry.h"
#include <string>
#include <vector>

class Window;

enum HorizontalAlignment { LeftAlign, HorizontalCenter, RightAlign };
enum VerticalAlignment { TopAlign, VerticalCenter, BottomAlign };

struct TextLayout
{
    TextLayout(HorizontalAlignment horizontalAlignment = LeftAlign,
               VerticalAlignment verticalAlignment = TopAlign)
    :   horizontalAlignment(horizontalAlignment), verticalAlignment(verticalAlignment)
    {
    }

    HorizontalAlignment horizontalAlignment;
    VerticalAlignment verticalAlignment;
};

class BitmapFont
{
public:
    BitmapFont(const Window&, const std::string& fileName, Vector2 charSize);
    void print(const std::string&, Color32 = Color32::none);
    void printLine(const std::string&, Color32 = Color32::none);
    void printWithCursor(const std::string&, std::string::const_iterator cursorPosition,
                         Color32 mainColor = Color32::none, Color32 cursorColor = Color32::none);
    void setArea(Rect area) { printArea = area; initCurrentPosition(); }
    void setLayout(TextLayout layout) { this->layout = layout; initCurrentPosition(); }
    void setDefaultColor(Color32 color) { defaultColor = color; }
    void setDrawShadows(bool state) { drawShadows = state; }
    void setShadowColorMod(double mod) { shadowColorMod = mod; }
    void setShadowPosition(Vector2 position) { shadowPosition = position; }
    void setCharSpacing(int amount) { moveVector.x = charSize.x + amount; }
    void setLineSpacing(int amount) { moveVector.y = charSize.y + amount; }
    Vector2 getCharSize() const { return charSize; }
    int getCharWidth() const { return charSize.x; }
    int getCharHeight() const { return charSize.y; }
    int getCharSpacing() const { return moveVector.x - charSize.x; }
    int getLineSpacing() const { return moveVector.y - charSize.y; }
    TextLayout getLayout() const { return layout; }

private:
    using PrintIterator = std::string::const_iterator;
    Vector2 printHelper(const std::string&, Vector2 position) const;
    void printLine(PrintIterator lineBegin, PrintIterator lineEnd, Rect& source, Rect& target) const;
    void initCurrentPosition();
    std::vector<Color32> loadFromFile(const std::string& fileName) const;

    Rect printArea;
    bool lineContinuation;
    TextLayout layout;
    Vector2 currentPosition;
    Color32 defaultColor;
    bool drawShadows;
    double shadowColorMod;
    Vector2 shadowPosition;
    const Vector2 charSize;
    Vector2 moveVector;
    Texture texture;
    static const Vector2 dimensions;
    static const int chars = 96;
};

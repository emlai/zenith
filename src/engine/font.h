#pragma once

#include "texture.h"
#include "color.h"
#include "geometry.h"
#include <string_view>

class Window;

enum HorizontalAlignment { LeftAlign, HorizontalCenter, RightAlign };
enum VerticalAlignment { TopAlign, VerticalCenter, BottomAlign };
enum LineBreakMode { PreserveLines, SplitLines };

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
    BitmapFont(std::string_view fileName, Vector2 charSize);
    void print(Window& window, std::string_view text, Color color = Color::none,
               Color backgroundColor = Color::none, bool blend = true,
               LineBreakMode lineBreakMode = PreserveLines);
    void printLine(Window& window, std::string_view text, Color color = Color::none,
                   Color backgroundColor = Color::none, bool blend = true,
                   LineBreakMode lineBreakMode = PreserveLines);
    void printWithCursor(Window& window, std::string_view, const char* cursorPosition,
                         Color mainColor = Color::none, Color cursorColor = Color::none,
                         Color backgroundColor = Color::none);
    void setArea(Rect area) { printArea = area; initCurrentPosition(); }
    void setLayout(TextLayout layout) { this->layout = layout; initCurrentPosition(); }
    void setDefaultColor(Color color) { defaultColor = color; }
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
    int getColumnWidth() const { return moveVector.x; }
    int getRowHeight() const { return moveVector.y; }
    Vector2 getTextSize(std::string_view text) const;
    TextLayout getLayout() const { return layout; }

private:
    using PrintIterator = std::string_view::const_iterator;
    Vector2 printHelper(Window& window, std::string_view, Vector2 position,
                        Color backgroundColor, LineBreakMode lineBreakMode) const;
    void printLine(Window& window, PrintIterator lineBegin, PrintIterator lineEnd,
                   Rect& source, Rect& target, Color backgroundColor) const;
    void initCurrentPosition();

    Rect printArea;
    bool lineContinuation;
    TextLayout layout;
    Vector2 currentPosition;
    Color defaultColor;
    bool drawShadows;
    double shadowColorMod;
    Vector2 shadowPosition;
    const Vector2 charSize;
    Vector2 moveVector;
    Texture texture;
    static const Vector2 dimensions;
    static const int chars = 96;
};

#pragma once

#include "texture.h"
#include "color.h"
#include "geometry.h"
#include <boost/utility/string_ref.hpp>

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
    BitmapFont(boost::string_ref fileName, Vector2 charSize);
    void print(Window& window, boost::string_ref text, Color32 color = Color32::none,
               Color32 backgroundColor = Color32::none, bool blend = true,
               LineBreakMode lineBreakMode = SplitLines);
    void printLine(Window& window, boost::string_ref text, Color32 color = Color32::none,
                   Color32 backgroundColor = Color32::none);
    void printWithCursor(Window& window, boost::string_ref, const char* cursorPosition,
                         Color32 mainColor = Color32::none, Color32 cursorColor = Color32::none,
                         Color32 backgroundColor = Color32::none);
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
    int getColumnWidth() const { return moveVector.x; }
    int getRowHeight() const { return moveVector.y; }
    Vector2 getTextSize(boost::string_ref text) const;
    TextLayout getLayout() const { return layout; }

private:
    using PrintIterator = const char*;
    Vector2 printHelper(Window& window, boost::string_ref, Vector2 position,
                        Color32 backgroundColor, LineBreakMode lineBreakMode) const;
    void printLine(Window& window, PrintIterator lineBegin, PrintIterator lineEnd,
                   Rect& source, Rect& target, Color32 backgroundColor) const;
    void initCurrentPosition();

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

#pragma once

#include "color.h"
#include "keyboard.h"
#include "geometry.h"
#include <vector>

class BitmapFont;
class Window;

struct MenuItem
{
    MenuItem(int id, boost::string_ref text, Key shortcut = NoKey)
    : id(id), text(text), shortcut(shortcut)
    {
    }

    const int id;
    const std::string text;
    const Key shortcut;
};

class Menu
{
public:
    enum ItemLayout { Vertical, Horizontal };
    enum { Exit = -1 };

    Menu()
    :   wrapEnabled(true),
        showNumbers(false),
        numberSeparator(". "),
        itemLayout(Vertical),
        itemSpacing(1),
        normalColor(Color32::white * 0.6),
        selectionColor(Color32::white),
        selectionOffset(0, 0),
        area(0, 0, 0, 0)
    {
    }
    void addItem(int id, boost::string_ref text, Key shortcut = NoKey);
    int getChoice(Window&, BitmapFont&);
    void setWrap(bool state) { wrapEnabled = state; }
    void setShowNumbers(bool state) { showNumbers = state; }
    void setNumberSeparator(std::string&& string) { numberSeparator = std::move(string); }
    void setTextLayout(TextLayout layout) { textLayout = layout; }
    void setItemLayout(ItemLayout layout) { itemLayout = layout; }
    void setItemSpacing(int amount) { itemSpacing = amount; }
    void setNormalColor(Color32 color) { normalColor = color; }
    void setSelectionColor(Color32 color) { selectionColor = color; }
    void setSelectionOffset(Vector2 offset) { selectionOffset = offset; }
    void setArea(Rect area) { this->area = area; }
    void setArea(Vector2 position, Vector2 size) { area = Rect(position, size); }
    void setArea(int x, int y, int width, int height) { area = Rect(x, y, width, height); }

private:
    bool isValidIndex(unsigned index) const { return index < menuItems.size(); }
    void select(int index);
    void select(std::vector<MenuItem>::iterator newSelection);
    void selectNext();
    void selectPrevious();
    int calculateMaxTextSize();
    void calculateSize();
    void calculateItemPositions();
    void render(BitmapFont&) const;

    std::vector<MenuItem> menuItems;
    std::vector<MenuItem>::iterator selection;
    std::vector<Rect> itemPositions;
    bool wrapEnabled;
    bool showNumbers;
    std::string numberSeparator;
    TextLayout textLayout;
    ItemLayout itemLayout;
    int itemSpacing;
    Color32 normalColor;
    Color32 selectionColor;
    Vector2 selectionOffset;
    Rect area;
    Vector2 size;
};

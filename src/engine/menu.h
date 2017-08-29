#pragma once

#include "color.h"
#include "keyboard.h"
#include "geometry.h"
#include <boost/optional.hpp>
#include <vector>

class BitmapFont;
class Sprite;
class Window;

struct MenuItem
{
    MenuItem(int id, boost::string_ref text, Key shortcut = NoKey, const Sprite* image = nullptr)
    : id(id), mainText(text), shortcut(shortcut), image(image)
    {
    }
    MenuItem(int id, boost::string_ref mainText, boost::string_ref secondaryText,
             Key shortcut = NoKey, const Sprite* image = nullptr)
    : id(id), mainText(mainText), secondaryText(secondaryText), shortcut(shortcut), image(image)
    {
    }

    const int id;
    const std::string mainText;
    const std::string secondaryText;
    const Key shortcut;
    const Sprite* const image;
};

class Menu
{
public:
    enum ItemLayout { Vertical, Horizontal };
    enum { Exit = INT_MIN };

    Menu()
    :   wrapEnabled(true),
        showNumbers(false),
        numberSeparator(". "),
        itemLayout(Vertical),
        itemSpacing(1),
        itemSize(boost::none),
        imageSpacing(0),
        normalColor(defaultNormalColor),
        selectionColor(defaultSelectionColor),
        selectionOffset(defaultSelectionOffset),
        area(0, 0, 0, 0)
    {
    }
    void addTitle(boost::string_ref text);
    void addItem(int id, boost::string_ref text, Key shortcut = NoKey, const Sprite* image = nullptr);
    void addItem(int id, boost::string_ref text, boost::string_ref secondaryText,
                 Key shortcut = NoKey, const Sprite* image = nullptr);
    int getChoice(Window&, BitmapFont&);
    void setWrap(bool state) { wrapEnabled = state; }
    void setShowNumbers(bool state) { showNumbers = state; }
    void setNumberSeparator(std::string&& string) { numberSeparator = std::move(string); }
    void setTextLayout(TextLayout layout) { textLayout = layout; }
    void setItemLayout(ItemLayout layout) { itemLayout = layout; }
    void setItemSpacing(int amount) { itemSpacing = amount; }
    void setItemSize(boost::optional<int> amount) { itemSize = amount; }
    void setImageSpacing(int amount) { imageSpacing = amount; }
    void setNormalColor(Color32 color) { normalColor = color; }
    void setSelectionColor(Color32 color) { selectionColor = color; }
    void setSelectionOffset(Vector2 offset) { selectionOffset = offset; }
    void setArea(Rect area) { this->area = area; }
    void setArea(Vector2 position, Vector2 size) { area = Rect(position, size); }
    void setArea(int x, int y, int width, int height) { area = Rect(x, y, width, height); }
    static void setDefaultNormalColor(Color32 color) { defaultNormalColor = color; }
    static void setDefaultSelectionColor(Color32 color) { defaultSelectionColor = color; }
    static void setDefaultSelectionOffset(Vector2 offset) { defaultSelectionOffset = offset; }

private:
    bool isValidIndex(unsigned index) const { return index < menuItems.size(); }
    void select(int index);
    void select(std::vector<MenuItem>::iterator newSelection);
    void selectNext();
    void selectPrevious();
    int calculateMaxTextSize();
    void calculateSize();
    void calculateItemPositions();
    void render(BitmapFont& font) const;

    std::string title;
    std::vector<MenuItem> menuItems;
    std::vector<MenuItem>::iterator selection;
    std::vector<Rect> itemPositions;
    bool wrapEnabled;
    bool showNumbers;
    std::string numberSeparator;
    TextLayout textLayout;
    ItemLayout itemLayout;
    int itemSpacing;
    boost::optional<int> itemSize;
    int imageSpacing;
    Color32 normalColor;
    Color32 selectionColor;
    Vector2 selectionOffset;
    Rect area;
    Vector2 size;
    static Color32 defaultNormalColor;
    static Color32 defaultSelectionColor;
    static Vector2 defaultSelectionOffset;
};

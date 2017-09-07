#pragma once

#include "color.h"
#include "keyboard.h"
#include "geometry.h"
#include "state.h"
#include <boost/optional.hpp>
#include <vector>

class BitmapFont;
class Sprite;
class Window;

struct MenuItem
{
    MenuItem(int id, boost::string_ref text, Key shortcut = NoKey, const Sprite* image = nullptr)
    :   id(id), mainImage(image), mainText(text), secondaryImage(nullptr), shortcut(shortcut)
    {
    }
    MenuItem(int id, boost::string_ref mainText, boost::string_ref secondaryText, Key shortcut = NoKey,
             const Sprite* mainImage = nullptr, const Sprite* secondaryImage = nullptr)
    :   id(id), mainImage(mainImage), mainText(mainText), secondaryImage(secondaryImage),
        secondaryText(secondaryText), shortcut(shortcut)
    {
    }

    const int id;
    const Sprite* const mainImage;
    const std::string mainText;
    const Sprite* const secondaryImage;
    const std::string secondaryText;
    const Key shortcut;
};

class Menu : public State
{
public:
    enum ItemLayout { Vertical, Horizontal };
    enum { Exit = INT_MIN };

    Menu() { clear(); }
    void addTitle(boost::string_ref text);
    /// Returns the index of the added menu item.
    int addItem(MenuItem&& item);
    void clear();
    int execute();
    void render(Window& window) override;
    void setWrap(bool state) { wrapEnabled = state; }
    void setShowNumbers(bool state) { showNumbers = state; }
    void setNumberSeparator(std::string&& string) { numberSeparator = std::move(string); }
    void setTextLayout(TextLayout layout) { textLayout = layout; }
    void setItemLayout(ItemLayout layout) { itemLayout = layout; }
    void setItemSpacing(int amount) { itemSpacing = amount; }
    void setItemSize(boost::optional<int> amount) { itemSize = amount; }
    void setColumnSpacing(int amount) { columnSpacing = amount; }
    void setSecondaryColumnAlignment(HorizontalAlignment alignment) { secondaryColumnAlignment = alignment; }
    void setNormalColor(Color32 color) { normalColor = color; }
    void setSelectionColor(Color32 color) { selectionColor = color; }
    void setSelectionOffset(Vector2 offset) { selectionOffset = offset; }
    void setArea(Rect area) { this->area = area; }
    void setArea(Vector2 position, Vector2 size) { area = Rect(position, size); }
    void setArea(int x, int y, int width, int height) { area = Rect(x, y, width, height); }
    void select(int index);
    int getSelectedIndex() const { return int(selection - menuItems.begin()); }
    static void setDefaultNormalColor(Color32 color) { defaultNormalColor = color; }
    static void setDefaultSelectionColor(Color32 color) { defaultSelectionColor = color; }
    static void setDefaultSelectionOffset(Vector2 offset) { defaultSelectionOffset = offset; }

private:
    bool isValidIndex(unsigned index) const { return index < menuItems.size(); }
    void select(std::vector<MenuItem>::iterator newSelection);
    void selectNext();
    void selectPrevious();
    int calculateMaxTextSize() const;
    void calculateSize();
    void calculateItemPositions();

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
    int columnSpacing;
    HorizontalAlignment secondaryColumnAlignment;
    Color32 normalColor;
    Color32 selectionColor;
    Vector2 selectionOffset;
    Rect area;
    Vector2 size;
    static Color32 defaultNormalColor;
    static Color32 defaultSelectionColor;
    static Vector2 defaultSelectionOffset;
};

#pragma once

#include "color.h"
#include "font.h"
#include "keyboard.h"
#include "geometry.h"
#include "state.h"
#include <climits>
#include <optional>
#include <vector>

class Sprite;
class Window;

struct MenuItem
{
    MenuItem(int id, std::string_view text, Key shortcut = NoKey, const Sprite* image = nullptr)
    :   id(id), mainImage(image), mainText(text), secondaryImage(nullptr), shortcut(shortcut)
    {
    }
    MenuItem(int id, std::string_view mainText, std::string_view secondaryText, Key shortcut = NoKey,
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
    enum HotkeyStyle { CustomHotkeys, NumberHotkeys, LetterHotkeys };
    enum { Exit = INT_MIN };

    Menu() { clear(); }
    void addTitle(std::string_view text);
    /// Returns the index of the added menu item.
    int addItem(MenuItem&& item);
    void clear();
    int execute();
    void render() override;
    void setWrap(bool state) { wrapEnabled = state; }
    void setHotkeyStyle(HotkeyStyle style) { hotkeyStyle = style; }
    void setTextLayout(TextLayout layout) { textLayout = layout; }
    void setItemLayout(ItemLayout layout) { itemLayout = layout; }
    void setItemSpacing(int amount) { itemSpacing = amount; }
    void setItemSize(std::optional<Vector2> size) { itemSize = size; }
    void setTableCellSpacing(Vector2 spacing) { tableCellSpacing = spacing; }
    void setSecondaryColumnAlignment(HorizontalAlignment alignment) { secondaryColumnAlignment = alignment; }
    void setTextColor(Color color) { textColor = color; }
    void setHoverColor(Color color) { hoverColor = color; }
    void setArea(Rect area) { this->area = area; }
    void setArea(Vector2 position, Vector2 size) { area = Rect(position, size); }
    void setArea(int x, int y, int width, int height) { area = Rect(x, y, width, height); }
    static void setDefaultTextColor(Color color) { defaultTextColor = color; }
    static void setDefaultHoverColor(Color color) { defaultHoverColor = color; }

private:
    bool isValidIndex(unsigned index) const { return index < menuItems.size(); }
    int calculateMaxTextSize() const;
    void calculateSize();
    void calculateItemPositions();
    std::string getHotkeyPrefix(int index) const;
    int calculateMainImageColumnWidth() const;

    std::string title;
    std::vector<MenuItem> menuItems;
    std::vector<Rect> itemPositions;
    bool wrapEnabled;
    HotkeyStyle hotkeyStyle;
    std::string hotkeySuffix;
    TextLayout textLayout;
    ItemLayout itemLayout;
    int itemSpacing;
    std::optional<Vector2> itemSize;
    Vector2 tableCellSpacing;
    HorizontalAlignment secondaryColumnAlignment;
    Color textColor;
    Color hoverColor;
    Rect area;
    Vector2 size;
    static Color defaultTextColor;
    static Color defaultHoverColor;
};

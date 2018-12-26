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

class Menu : public State
{
public:
    enum ItemLayout { Vertical, Horizontal };
    enum HotkeyStyle { CustomHotkeys, NumberHotkeys, LetterHotkeys };
    enum { Exit = INT_MIN };

    Menu() { clear(); }
    void setTitle(std::string_view text);
    void addItem(int id, std::string text, Key shortcut = NoKey, const Sprite* image = nullptr);
    void addItem(int id, std::string mainText, std::string secondaryText, Key shortcut = NoKey,
                 const Sprite* mainImage = nullptr, const Sprite* secondaryImage = nullptr);
    void clear();
    StateChange update() override;
    void render() override;
    void setHotkeyStyle(HotkeyStyle style) { hotkeyStyle = style; }
    void setTextLayout(TextLayout layout) { textLayout = layout; }
    void setItemLayout(ItemLayout layout) { itemLayout = layout; }
    void setItemSize(std::optional<Vector2> size) { itemSize = size; }
    void setTableCellSpacing(Vector2 spacing) { tableCellSpacing = spacing; }
    void setSecondaryColumnAlignment(HorizontalAlignment alignment) { secondaryColumnAlignment = alignment; }
    void setArea(Rect area) { this->area = area; }
    void setArea(Vector2 position, Vector2 size) { area = Rect(position, size); }
    static void setDefaultTextColor(Color color) { defaultTextColor = color; }

private:
    bool isValidIndex(unsigned index) const { return index < menuItems.size(); }
    int calculateMaxTextSize() const;
    void calculateItemPositions();
    std::string getHotkeyPrefix(int index) const;
    int calculateMainImageColumnWidth() const;

    struct MenuItem
    {
        int id;
        const Sprite* mainImage;
        std::string mainText;
        const Sprite* secondaryImage;
        std::string secondaryText;
        Key shortcut;
    };

    std::string title;
    std::vector<MenuItem> menuItems;
    std::vector<Rect> itemPositions;
    HotkeyStyle hotkeyStyle;
    std::string hotkeySuffix;
    TextLayout textLayout;
    ItemLayout itemLayout;
    std::optional<Vector2> itemSize;
    Vector2 tableCellSpacing;
    HorizontalAlignment secondaryColumnAlignment;
    Color textColor;
    Color hoverColor;
    Rect area;
    static Color defaultTextColor;
    static Color defaultHoverColor;
};

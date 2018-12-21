#pragma once

#include "color.h"
#include "font.h"
#include "keyboard.h"
#include "geometry.h"
#include "state.h"
#include <boost/optional.hpp>
#include <vector>

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
    enum HotkeyStyle { CustomHotkeys, NumberHotkeys, LetterHotkeys };
    enum { Exit = INT_MIN };

    Menu() { clear(); }
    void addTitle(boost::string_ref text);
    /// Returns the index of the added menu item.
    int addItem(MenuItem&& item);
    void clear();
    int execute();
    void render(Window& window) override;
    void setWrap(bool state) { wrapEnabled = state; }
    void setHotkeyStyle(HotkeyStyle style) { hotkeyStyle = style; }
    void setHotkeySuffix(std::string string) { hotkeySuffix = std::move(string); }
    void setTextLayout(TextLayout layout) { textLayout = layout; }
    void setItemLayout(ItemLayout layout) { itemLayout = layout; }
    void setItemSpacing(int amount) { itemSpacing = amount; }
    void setItemSize(boost::optional<Vector2> size) { itemSize = size; }
    void setTableCellSpacing(Vector2 spacing) { tableCellSpacing = spacing; }
    void setSecondaryColumnAlignment(HorizontalAlignment alignment) { secondaryColumnAlignment = alignment; }
    void setTextColor(Color32 color) { textColor = color; }
    void setHoverColor(Color32 color) { hoverColor = color; }
    void setArea(Rect area) { this->area = area; }
    void setArea(Vector2 position, Vector2 size) { area = Rect(position, size); }
    void setArea(int x, int y, int width, int height) { area = Rect(x, y, width, height); }
    static void setDefaultTextColor(Color32 color) { defaultTextColor = color; }
    static void setDefaultHoverColor(Color32 color) { defaultHoverColor = color; }

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
    boost::optional<Vector2> itemSize;
    Vector2 tableCellSpacing;
    HorizontalAlignment secondaryColumnAlignment;
    Color32 textColor;
    Color32 hoverColor;
    Rect area;
    Vector2 size;
    static Color32 defaultTextColor;
    static Color32 defaultHoverColor;
};
#include "menu.h"
#include "engine.h"
#include "font.h"
#include "window.h"
#include "sprite.h"
#include <algorithm>
#include <cctype>

Color32 Menu::defaultTextColor = Color32::white * 0.6;
Color32 Menu::defaultHoverColor = Color32::white;

void Menu::addTitle(boost::string_ref text)
{
    title = text.to_string();
}

int Menu::addItem(MenuItem&& item)
{
    menuItems.push_back(std::move(item));
    return int(menuItems.size() - 1);
}

void Menu::clear()
{
    menuItems.clear();
    wrapEnabled = true;
    hotkeyStyle = CustomHotkeys;
    hotkeySuffix = ")";
    itemLayout = Vertical;
    itemSpacing = 1;
    itemSize = boost::none;
    tableCellSpacing = Vector2(0, 0);
    secondaryColumnAlignment = LeftAlign;
    textColor = defaultTextColor;
    hoverColor = defaultHoverColor;
    area = Rect(0, 0, 0, 0);
}

int Menu::execute()
{
    auto& window = getEngine().getWindow();
    auto& font = window.getFont();
    TextLayout oldLayout = font.getLayout();
    font.setLayout(textLayout);
    calculateSize();
    calculateItemPositions();
    int exitCode = -3;

    while (exitCode == -3)
    {
        Event event = window.waitForInput();

        if (event.type == Event::MouseButtonDown)
        {
            for (auto& item : itemPositions)
            {
                if (!title.empty() && &item == &itemPositions[0])
                    continue; // Skip title.

                if (event.mousePosition.isWithin(item))
                {
                    auto index = &item - &itemPositions[title.empty() ? 0 : 1];
                    exitCode = menuItems[index].id;
                    break;
                }
            }
            continue;
        }

        if (window.shouldClose())
        {
            exitCode = Window::CloseRequest;
            break;
        }

        assert(event.type == Event::KeyDown);
        auto input = event.key;

        if (input == Esc)
        {
            exitCode = Exit;
            break;
        }

        switch (hotkeyStyle)
        {
            case NumberHotkeys:
                if (std::isdigit(input) && input != '0' && isValidIndex(input - '1'))
                    exitCode = menuItems[input - '1'].id;
                break;

            case LetterHotkeys:
                if (input >= 'a' && input <= 'z' && isValidIndex(input - 'a'))
                    exitCode = menuItems[input - 'a'].id;
                break;
        }

        // Handle custom shortcuts.
        for (auto item = menuItems.begin(); item != menuItems.end(); ++item)
        {
            if (input == item->shortcut)
            {
                exitCode = item->id;
                break;
            }
        }
    }

    font.setLayout(oldLayout);
    return exitCode;
}

int Menu::calculateMaxTextSize() const
{
    int maxSize = 0;
    int indexOfMax = 0;

    for (const MenuItem& item : menuItems)
    {
        auto index = &item - &menuItems[0];
        auto currentSize = getHotkeyPrefix(index + 1).size() + item.mainText.size();

        if (currentSize > maxSize)
        {
            maxSize = currentSize;
            indexOfMax = index;
        }
    }

    return maxSize;
}

void Menu::calculateSize()
{
    int maxCharsPerLine;
    int lines;

    if (itemLayout == Vertical)
    {
        maxCharsPerLine = calculateMaxTextSize();
        lines = int(menuItems.size() + (menuItems.size() - 1) * (itemSpacing - 1));
    }
    else
    {
        maxCharsPerLine = int((menuItems.size() - 1) * itemSpacing);
        maxCharsPerLine += getHotkeyPrefix(menuItems.size()).size();
        maxCharsPerLine += menuItems.back().mainText.size();
        lines = 1;
    }

    size = Vector2(maxCharsPerLine, lines);
}

void Menu::calculateItemPositions()
{
    int count = int(menuItems.size()) + (title.empty() ? 0 : 1);
    itemPositions.clear();
    itemPositions.reserve(count);
    Vector2 size;

    if (itemLayout == Vertical)
        size = Vector2(area.size.x, itemSize ? itemSize->y : area.size.y / menuItems.size());
    else
        size = Vector2(itemSize ? itemSize->x : area.size.x / menuItems.size(), area.size.y);

    Vector2 position = area.position;

    for (int i = 0; i < count; ++i)
    {
        itemPositions.push_back(Rect(position, size));

        if (itemLayout == Vertical)
            position.y += size.y;
        else
            position.x += size.x;
    }
}

int Menu::calculateMainImageColumnWidth() const
{
    auto width = 0;

    for (auto& item : menuItems)
    {
        if (item.mainImage)
            width = std::max(width, item.mainImage->getWidth());
    }

    return width + tableCellSpacing.x;
}

void Menu::render(Window& window)
{
    auto& font = window.getFont();
    int index = 1;
    auto position = itemPositions.begin();

    if (!title.empty())
    {
        font.setArea(*position);
        font.print(window, title, textColor);
        ++position;
    }

    auto mainImageColumnWidth = calculateMainImageColumnWidth();

    for (auto item = menuItems.begin(); item != menuItems.end(); ++item, ++position, ++index)
    {
        Vector2 itemPosition = position->position;
        bool isHovered = window.getMousePosition().isWithin(Rect(itemPosition, position->size));
        auto color = isHovered ? hoverColor : textColor;

        auto hotkeyPrefix = getHotkeyPrefix(index);
        font.setArea(Rect(itemPosition, position->size));
        font.print(window, hotkeyPrefix, color);
        itemPosition.x += hotkeyPrefix.size() * font.getColumnWidth() + tableCellSpacing.x;

        if (item->mainImage)
            item->mainImage->render(window, itemPosition);

        if (mainImageColumnWidth > 0)
        {
            itemPosition.x += mainImageColumnWidth;
            font.setArea(Rect(itemPosition, position->size));
        }

        font.print(window, item->mainText, color);
        itemPosition.x += calculateMaxTextSize() * font.getColumnWidth() + tableCellSpacing.x;

        if (item->secondaryImage)
        {
            item->secondaryImage->render(window, itemPosition);
            itemPosition.x += item->secondaryImage->getWidth() + tableCellSpacing.x;
        }

        if (!item->secondaryText.empty())
        {
            auto oldLayout = font.getLayout();
            font.setLayout(TextLayout(secondaryColumnAlignment, font.getLayout().verticalAlignment));
            font.setArea(Rect(itemPosition, position->size - Vector2(itemPosition.x - position->position.x, 0)));
            font.print(window, item->secondaryText, color);
            font.setLayout(oldLayout);
        }
    }
}

std::string Menu::getHotkeyPrefix(int index) const
{
    switch (hotkeyStyle)
    {
        case CustomHotkeys: return "";
        case NumberHotkeys: return std::to_string(index) + hotkeySuffix;
        case LetterHotkeys: return char('a' + index - 1) + hotkeySuffix;
    }
}

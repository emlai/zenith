#include "menu.h"
#include "assert.h"
#include "font.h"
#include "window.h"
#include "sprite.h"
#include <algorithm>
#include <cctype>

Color Menu::defaultTextColor = Color::white * 0.6;
Color Menu::defaultHoverColor = Color::white;

void Menu::setTitle(std::string_view text)
{
    title = text;
}

void Menu::addItem(int id, std::string text, Key shortcut, const Sprite* image)
{
    addItem(id, std::move(text), "", shortcut, image, nullptr);
}

void Menu::addItem(int id, std::string mainText, std::string secondaryText, Key shortcut,
                  const Sprite* mainImage, const Sprite* secondaryImage)
{
    menuItems.push_back(MenuItem { id, mainImage, std::move(mainText), secondaryImage, std::move(secondaryText), shortcut });
}

void Menu::clear()
{
    menuItems.clear();
    hotkeyStyle = CustomHotkeys;
    hotkeySuffix = ") ";
    itemLayout = Vertical;
    itemSize = std::nullopt;
    tableCellSpacing = Vector2(0, 0);
    secondaryColumnAlignment = LeftAlign;
    textColor = defaultTextColor;
    hoverColor = defaultHoverColor;
    area = Rect(0, 0, 0, 0);
}

StateChange Menu::update()
{
    while (true)
    {
        Event event = window->waitForInput();

        if (event.type == Event::MouseButtonDown)
        {
            for (auto& item : itemPositions)
            {
                if (!title.empty() && &item == &itemPositions[0])
                    continue; // Skip title.

                if (event.mousePosition.isWithin(item))
                {
                    auto index = &item - &itemPositions[title.empty() ? 0 : 1];
                    return StateChange::Pop(menuItems[index].id);
                }
            }
            continue;
        }

        if (window->shouldClose())
            return StateChange::Pop(Window::CloseRequest);

        ASSERT(event.type == Event::KeyDown);
        auto input = event.key;

        if (input == Esc)
            return StateChange::Pop(Exit);

        switch (hotkeyStyle)
        {
            case NumberHotkeys:
                if (std::isdigit(input) && input != '0' && isValidIndex(input - '1'))
                    return StateChange::Pop(menuItems[input - '1'].id);
                break;

            case LetterHotkeys:
                if (input >= 'a' && input <= 'z' && isValidIndex(input - 'a'))
                    return StateChange::Pop(menuItems[input - 'a'].id);
                break;

            case CustomHotkeys:
                break;
        }

        for (auto& menuItem : menuItems)
        {
            if (input == menuItem.shortcut)
                return StateChange::Pop(menuItem.id);
        }
    }
}

int Menu::calculateMaxTextSize() const
{
    unsigned maxSize = 0;

    for (const MenuItem& item : menuItems)
    {
        auto index = &item - &menuItems[0];
        auto currentSize = getHotkeyPrefix(index + 1).size() + item.mainText.size();

        if (currentSize > maxSize)
            maxSize = currentSize;
    }

    return maxSize;
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

void Menu::render()
{
    auto& font = *window->context.font;

    // TODO: Find a non-ugly way to define the font layout.
    TextLayout oldLayout = font.getLayout();
    font.setLayout(textLayout);
    DEFER { font.setLayout(oldLayout); };

    // TODO: Don't store item positions as member variables.
    calculateItemPositions();

    int index = 1;
    auto position = itemPositions.begin();

    if (!title.empty())
    {
        font.setArea(*position);
        font.print(*window, title, textColor);
        ++position;
    }

    auto mainImageColumnWidth = calculateMainImageColumnWidth();

    for (auto item = menuItems.begin(); item != menuItems.end(); ++item, ++position, ++index)
    {
        Vector2 itemPosition = position->position;
        bool isHovered = window->getMousePosition().isWithin(Rect(itemPosition, position->size));
        auto color = isHovered ? hoverColor : textColor;

        auto hotkeyPrefix = getHotkeyPrefix(index);
        font.setArea(Rect(itemPosition, position->size));
        font.print(*window, hotkeyPrefix, color);
        itemPosition.x += trim(hotkeyPrefix).size() * font.getColumnWidth() + tableCellSpacing.x;

        if (item->mainImage)
            item->mainImage->render(*window, itemPosition);

        if (mainImageColumnWidth > 0)
        {
            itemPosition.x += mainImageColumnWidth;
            font.setArea(Rect(itemPosition, position->size));
        }

        font.print(*window, item->mainText, color);
        itemPosition.x += calculateMaxTextSize() * font.getColumnWidth() + tableCellSpacing.x;

        if (item->secondaryImage)
        {
            item->secondaryImage->render(*window, itemPosition);
            itemPosition.x += item->secondaryImage->getWidth() + tableCellSpacing.x;
        }

        if (!item->secondaryText.empty())
        {
            auto oldLayout = font.getLayout();
            font.setLayout(TextLayout(secondaryColumnAlignment, font.getLayout().verticalAlignment));
            font.setArea(Rect(itemPosition, position->size - Vector2(itemPosition.x - position->position.x, 0)));
            font.print(*window, item->secondaryText, color);
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

    ASSERT(false);
}

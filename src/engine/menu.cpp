#include "menu.h"
#include "engine.h"
#include "font.h"
#include "window.h"
#include "sprite.h"
#include <cctype>

Color32 Menu::defaultTextColor = Color32::white * 0.6;

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
    hotkeySeparator = ". ";
    itemLayout = Vertical;
    itemSpacing = 1;
    itemSize = boost::none;
    tableCellSpacing = Vector2(0, 0);
    secondaryColumnAlignment = LeftAlign;
    textColor = defaultTextColor;
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
        const Key input = window.waitForInput();

        if (window.shouldClose())
        {
            exitCode = Window::CloseRequest;
            break;
        }

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
    itemPositions.clear();
    itemPositions.reserve(menuItems.size());
    Vector2 size;

    if (itemLayout == Vertical)
        size = Vector2(area.size.x, itemSize ? itemSize->y : area.size.y / menuItems.size());
    else
        size = Vector2(itemSize ? itemSize->x : area.size.x / menuItems.size(), area.size.y);

    Vector2 position = area.position;

    for (int i = 0, count = int(menuItems.size()) + (title.empty() ? 0 : 1); i < count; ++i)
    {
        itemPositions.push_back(Rect(position, size));

        if (itemLayout == Vertical)
            position.y += size.y;
        else
            position.x += size.x;
    }
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

    for (auto item = menuItems.begin(); item != menuItems.end(); ++item, ++position, ++index)
    {
        Vector2 itemPosition = position->position;

        if (item->mainImage)
        {
            item->mainImage->render(window, itemPosition);
            itemPosition.x += item->mainImage->getWidth() + tableCellSpacing.x;
        }

        std::string text = getHotkeyPrefix(index) + item->mainText;
        font.setArea(Rect(itemPosition, position->size));
        font.print(window, text, textColor);
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
            font.print(window, item->secondaryText, textColor);
            font.setLayout(oldLayout);
        }
    }
}

std::string Menu::getHotkeyPrefix(int index) const
{
    switch (hotkeyStyle)
    {
        case CustomHotkeys: return "";
        case NumberHotkeys: return std::to_string(index) + hotkeySeparator;
        case LetterHotkeys: return char('a' + index - 1) + hotkeySeparator;
    }
}

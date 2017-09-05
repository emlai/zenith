#include "menu.h"
#include "font.h"
#include "window.h"
#include "sprite.h"
#include <cctype>

Color32 Menu::defaultNormalColor = Color32::white * 0.6;
Color32 Menu::defaultSelectionColor = Color32::white;
Vector2 Menu::defaultSelectionOffset(0, 0);

void Menu::addTitle(boost::string_ref text)
{
    title = text.to_string();
}

int Menu::addItem(MenuItem&& item)
{
    menuItems.push_back(std::move(item));
    selection = menuItems.begin();
    return int(menuItems.size() - 1);
}

int Menu::getChoice(Window& window, BitmapFont& font)
{
    TextLayout oldLayout = font.getLayout();
    font.setLayout(textLayout);
    calculateSize();
    calculateItemPositions();
    int exitCode = -3;

    while (exitCode == -3)
    {
        render(window, font);
        window.updateScreen();
        const Key input = window.waitForInput();

        if (window.shouldClose())
            exitCode = Window::CloseRequest;

        switch (input)
        {
            case NoKey: // timeout
                continue;

            case DownArrow:
                if (itemLayout == Vertical)
                    selectNext();
                break;
            case UpArrow:
                if (itemLayout == Vertical)
                    selectPrevious();
                break;
            case RightArrow:
                if (itemLayout == Horizontal)
                    selectNext();
                break;
            case LeftArrow:
                if (itemLayout == Horizontal)
                    selectPrevious();
                break;
            case Enter:
                exitCode = selection->id;
                break;
            case Esc:
                exitCode = Exit;
                break;
            default:
                // Handle implicit number shortcuts.
                if (showNumbers && isdigit(input) && input != '0' && isValidIndex(input - '1'))
                {
                    select(int(input - '1'));
                    exitCode = selection->id;
                    break;
                }

                // Handle custom shortcuts.
                for (auto item = menuItems.begin(); item != menuItems.end(); ++item)
                {
                    if (input == item->shortcut)
                    {
                        select(item);
                        exitCode = selection->id;
                        break;
                    }
                }

                break;
        }
    }

    font.setLayout(oldLayout);
    return exitCode;
}

void Menu::select(int index)
{
    selection = menuItems.begin() + index;
}

void Menu::select(std::vector<MenuItem>::iterator newSelection)
{
    selection = newSelection;
}

void Menu::selectNext()
{
    if (selection != menuItems.end() - 1)
        ++selection;
    else if (wrapEnabled)
        selection = menuItems.begin();
}

void Menu::selectPrevious()
{
    if (selection != menuItems.begin())
        --selection;
    else if (wrapEnabled)
        selection = menuItems.end() - 1;
}

int Menu::calculateMaxTextSize() const

{
    int maxSize = 0;
    int indexOfMax = 0;

    for (const MenuItem& item : menuItems)
    {
        const int currentSize = int(item.mainText.size());
        if (currentSize > maxSize)
        {
            maxSize = currentSize;
            indexOfMax = int(&item - &menuItems[0]);
        }
    }

    if (showNumbers)
        maxSize += std::to_string(indexOfMax + 1).size() + numberSeparator.size();

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
        if (showNumbers)
            maxCharsPerLine += std::to_string(menuItems.size()).size() + numberSeparator.size();
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
        size = Vector2(area.size.x, itemSize ? *itemSize : area.size.y / menuItems.size());
    else
        size = Vector2(itemSize ? *itemSize : area.size.x / menuItems.size(), area.size.y);

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

void Menu::render(Window& window, BitmapFont& font) const
{
    int index = 1;
    auto position = itemPositions.begin();

    if (!title.empty())
    {
        font.setArea(*position);
        font.print(window, title, normalColor);
        ++position;
    }

    for (auto item = menuItems.begin(); item != menuItems.end(); ++item, ++position)
    {
        Vector2 initialPosition = selection == item ? position->position + selectionOffset : position->position;
        Vector2 itemPosition = initialPosition;

        if (item->mainImage)
        {
            item->mainImage->render(window, itemPosition);
            itemPosition.x += item->mainImage->getWidth() + columnSpacing;
        }

        std::string text =
            showNumbers ? std::to_string(index++) + numberSeparator + item->mainText : item->mainText;

        font.setArea(Rect(itemPosition, position->size));
        font.print(window, text, selection == item ? selectionColor : normalColor);
        itemPosition.x += calculateMaxTextSize() * font.getColumnWidth() + columnSpacing;

        if (item->secondaryImage)
        {
            item->secondaryImage->render(window, itemPosition);
            itemPosition.x += item->secondaryImage->getWidth() + columnSpacing;
        }

        if (!item->secondaryText.empty())
        {
            auto oldLayout = font.getLayout();
            font.setLayout(TextLayout(secondaryColumnAlignment, font.getLayout().verticalAlignment));
            font.setArea(Rect(itemPosition, position->size - Vector2(itemPosition.x - initialPosition.x, 0)));
            font.print(window, item->secondaryText, selection == item ? selectionColor : normalColor);
            font.setLayout(oldLayout);
        }
    }
}

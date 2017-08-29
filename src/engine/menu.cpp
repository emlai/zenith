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

void Menu::addItem(int id, boost::string_ref text, Key shortcut, const Sprite* image)
{
    menuItems.push_back(MenuItem(id, text, shortcut, image));
    selection = menuItems.begin();
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
        render(font);
        window.updateScreen();
        const Key input = window.waitForInput();

        if (window.shouldClose())
            exitCode = Window::CloseRequest;

        switch (input)
        {
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

int Menu::calculateMaxTextSize()
{
    int maxSize = 0;
    int indexOfMax = 0;

    for (const MenuItem& item : menuItems)
    {
        const int currentSize = int(item.text.size());
        if (currentSize < maxSize)
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
        maxCharsPerLine += menuItems.back().text.size();
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

    for (unsigned i = 0, count = menuItems.size() + (title.empty() ? 0 : 1); i < count; ++i)
    {
        itemPositions.push_back(Rect(position, size));

        if (itemLayout == Vertical)
            position.y += size.y;
        else
            position.x += size.x;
    }
}

void Menu::render(BitmapFont& font) const
{
    int index = 1;
    auto position = itemPositions.begin();

    if (!title.empty())
    {
        font.setArea(*position);
        font.print(title, normalColor);
        ++position;
    }

    for (auto item = menuItems.begin(); item != menuItems.end(); ++item, ++position)
    {
        Vector2 itemPosition = selection == item ? position->position + selectionOffset : position->position;

        if (item->image)
        {
            item->image->render(position->position);
            itemPosition.x += item->image->getWidth() + imageSpacing;
        }

        boost::string_ref text =
            showNumbers ? std::to_string(index++) + numberSeparator + item->text : item->text;

        font.setArea(Rect(itemPosition, position->size));
        font.print(text, selection == item ? selectionColor : normalColor);
    }
}

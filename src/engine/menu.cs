struct MenuItem
{
    MenuItem(int id, string text, Key shortcut = NoKey, Sprite image = null)
    :   id(id), mainImage(image), mainText(text), secondaryImage(null), shortcut(shortcut)
    {
    }
    MenuItem(int id, string mainText, string secondaryText, Key shortcut = NoKey,
             Sprite mainImage = null, Sprite secondaryImage = null)
    :   id(id), mainImage(mainImage), mainText(mainText), secondaryImage(secondaryImage),
        secondaryText(secondaryText), shortcut(shortcut)
    {
    }

    const int id;
    Sprite mainImage;
    const string mainText;
    Sprite secondaryImage;
    const string secondaryText;
    const Key shortcut;
}

class Menu : State
{
    enum ItemLayout { Vertical, Horizontal }
    enum HotkeyStyle { CustomHotkeys, NumberHotkeys, LetterHotkeys }
    enum { Exit = INT_MIN }

    string title;
    List<MenuItem> menuItems;
    List<Rect> itemPositions;
    bool wrapEnabled;
    HotkeyStyle hotkeyStyle;
    string hotkeySuffix;
    TextLayout textLayout;
    ItemLayout itemLayout;
    int itemSpacing;
    Vector2? itemSize;
    Vector2 tableCellSpacing;
    HorizontalAlignment secondaryColumnAlignment;
    Color32 textColor;
    Color32 hoverColor;
    Rect area;
    Vector2 size;
    static Color32 defaultTextColor = Color32::white * 0.6;
    static Color32 defaultHoverColor = Color32::white;

    Menu() { clear(); }

    void addTitle(string text)
    {
        title = text;
    }

    int addItem(MenuItem item)
    {
        menuItems.push_back(item);
        return int(menuItems.size() - 1);
    }

    void clear()
    {
        menuItems.clear();
        wrapEnabled = true;
        hotkeyStyle = CustomHotkeys;
        hotkeySuffix = ")";
        itemLayout = Vertical;
        itemSpacing = 1;
        itemSize = null;
        tableCellSpacing = new Vector2(0, 0);
        secondaryColumnAlignment = LeftAlign;
        textColor = defaultTextColor;
        hoverColor = defaultHoverColor;
        area = Rect(0, 0, 0, 0);
    }

    int execute()
    {
        var window = getEngine().getWindow();
        var font = window.getFont();
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
                foreach (var item in itemPositions)
                {
                    if (!title.empty() && item == itemPositions[0])
                        continue; // Skip title.

                    if (event.mousePosition.isWithin(item))
                    {
                        var index = item - itemPositions[title.empty() ? 0 : 1];
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
            var input = event.key;

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
            for (var item = menuItems.begin(); item != menuItems.end(); ++item)
            {
                if (input == item.shortcut)
                {
                    exitCode = item.id;
                    break;
                }
            }
        }

        font.setLayout(oldLayout);
        return exitCode;
    }

    int calculateMaxTextSize()
    {
        int maxSize = 0;
        int indexOfMax = 0;

        foreach (MenuItem item in menuItems)
        {
            var index = item - menuItems[0];
            var currentSize = getHotkeyPrefix(index + 1).size() + item.mainText.size();

            if (currentSize > maxSize)
            {
                maxSize = currentSize;
                indexOfMax = index;
            }
        }

        return maxSize;
    }

    void calculateSize()
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

        size = new Vector2(maxCharsPerLine, lines);
    }

    void calculateItemPositions()
    {
        int count = int(menuItems.size()) + (title.empty() ? 0 : 1);
        itemPositions.clear();
        itemPositions.reserve(count);
        Vector2 size;

        if (itemLayout == Vertical)
            size = new Vector2(area.size.x, itemSize ? itemSize.y : area.size.y / menuItems.size());
        else
            size = new Vector2(itemSize ? itemSize.x : area.size.x / menuItems.size(), area.size.y);

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

    int calculateMainImageColumnWidth()
    {
        var width = 0;

        foreach (var item in menuItems)
        {
            if (item.mainImage)
                width = std::max(width, item.mainImage.getWidth());
        }

        return width + tableCellSpacing.x;
    }

    void render(Window window)
    {
        var font = window.getFont();
        int index = 1;
        var position = itemPositions.begin();

        if (!title.empty())
        {
            font.setArea(position);
            font.print(window, title, textColor);
            ++position;
        }

        var mainImageColumnWidth = calculateMainImageColumnWidth();

        for (var item = menuItems.begin(); item != menuItems.end(); ++item, ++position, ++index)
        {
            Vector2 itemPosition = position.position;
            bool isHovered = window.getMousePosition().isWithin(Rect(itemPosition, position.size));
            var color = isHovered ? hoverColor : textColor;

            var hotkeyPrefix = getHotkeyPrefix(index);
            font.setArea(Rect(itemPosition, position.size));
            font.print(window, hotkeyPrefix, color);
            itemPosition.x += hotkeyPrefix.size() * font.getColumnWidth() + tableCellSpacing.x;

            if (item.mainImage)
                item.mainImage.render(window, itemPosition);

            if (mainImageColumnWidth > 0)
            {
                itemPosition.x += mainImageColumnWidth;
                font.setArea(Rect(itemPosition, position.size));
            }

            font.print(window, item.mainText, color);
            itemPosition.x += calculateMaxTextSize() * font.getColumnWidth() + tableCellSpacing.x;

            if (item.secondaryImage)
            {
                item.secondaryImage.render(window, itemPosition);
                itemPosition.x += item.secondaryImage.getWidth() + tableCellSpacing.x;
            }

            if (!item.secondaryText.empty())
            {
                var oldLayout = font.getLayout();
                font.setLayout(TextLayout(secondaryColumnAlignment, font.getLayout().verticalAlignment));
                font.setArea(Rect(itemPosition, position.size - Vector2(itemPosition.x - position.position.x, 0)));
                font.print(window, item.secondaryText, color);
                font.setLayout(oldLayout);
            }
        }
    }

    string getHotkeyPrefix(int index)
    {
        switch (hotkeyStyle)
        {
            case CustomHotkeys: return "";
            case NumberHotkeys: return std::to_string(index) + hotkeySuffix;
            case LetterHotkeys: return char('a' + index - 1) + hotkeySuffix;
        }
    }

    void setWrap(bool state) { wrapEnabled = state; }
    void setHotkeyStyle(HotkeyStyle style) { hotkeyStyle = style; }
    void setHotkeySuffix(string string) { hotkeySuffix = string; }
    void setTextLayout(TextLayout layout) { textLayout = layout; }
    void setItemLayout(ItemLayout layout) { itemLayout = layout; }
    void setItemSpacing(int amount) { itemSpacing = amount; }
    void setItemSize(Vector2? size) { itemSize = size; }
    void setTableCellSpacing(Vector2 spacing) { tableCellSpacing = spacing; }
    void setSecondaryColumnAlignment(HorizontalAlignment alignment) { secondaryColumnAlignment = alignment; }
    void setTextColor(Color32 color) { textColor = color; }
    void setHoverColor(Color32 color) { hoverColor = color; }
    void setArea(Rect area) { this.area = area; }
    void setArea(Vector2 position, Vector2 size) { area = Rect(position, size); }
    void setArea(int x, int y, int width, int height) { area = Rect(x, y, width, height); }
    static void setDefaultTextColor(Color32 color) { defaultTextColor = color; }
    static void setDefaultHoverColor(Color32 color) { defaultHoverColor = color; }

    bool isValidIndex(unsigned index) { return index < menuItems.size(); }
    int calculateMaxTextSize();
    void calculateSize();
    void calculateItemPositions();
    string getHotkeyPrefix(int index);
    int calculateMainImageColumnWidth();
}

enum HorizontalAlignment { LeftAlign, HorizontalCenter, RightAlign }
enum VerticalAlignment { TopAlign, VerticalCenter, BottomAlign }
enum LineBreakMode { PreserveLines, SplitLines }

struct TextLayout
{
    TextLayout(HorizontalAlignment horizontalAlignment = LeftAlign,
               VerticalAlignment verticalAlignment = TopAlign)
    :   horizontalAlignment(horizontalAlignment), verticalAlignment(verticalAlignment)
    {
    }

    HorizontalAlignment horizontalAlignment;
    VerticalAlignment verticalAlignment;
}

class BitmapFont
{
public:
    BitmapFont(boost::string_ref fileName, Vector2 charSize);
    void print(Window window, boost::string_ref text, Color32 color = Color32::none,
               Color32 backgroundColor = Color32::none, bool blend = true,
               LineBreakMode lineBreakMode = SplitLines);
    void printLine(Window window, boost::string_ref text, Color32 color = Color32::none,
                   Color32 backgroundColor = Color32::none, bool blend = true,
                   LineBreakMode lineBreakMode = SplitLines);
    void printWithCursor(Window window, boost::string_ref, const char cursorPosition,
                         Color32 mainColor = Color32::none, Color32 cursorColor = Color32::none,
                         Color32 backgroundColor = Color32::none);
    void setArea(Rect area) { printArea = area; initCurrentPosition(); }
    void setLayout(TextLayout layout) { this->layout = layout; initCurrentPosition(); }
    void setDefaultColor(Color32 color) { defaultColor = color; }
    void setDrawShadows(bool state) { drawShadows = state; }
    void setShadowColorMod(double mod) { shadowColorMod = mod; }
    void setShadowPosition(Vector2 position) { shadowPosition = position; }
    void setCharSpacing(int amount) { moveVector.x = charSize.x + amount; }
    void setLineSpacing(int amount) { moveVector.y = charSize.y + amount; }
    Vector2 getCharSize() const { return charSize; }
    int getCharWidth() const { return charSize.x; }
    int getCharHeight() const { return charSize.y; }
    int getCharSpacing() const { return moveVector.x - charSize.x; }
    int getLineSpacing() const { return moveVector.y - charSize.y; }
    int getColumnWidth() const { return moveVector.x; }
    int getRowHeight() const { return moveVector.y; }
    Vector2 getTextSize(boost::string_ref text) const;
    TextLayout getLayout() const { return layout; }

private:
    using PrintIterator = const char;
    Vector2 printHelper(Window window, boost::string_ref, Vector2 position,
                        Color32 backgroundColor, LineBreakMode lineBreakMode) const;
    void printLine(Window window, PrintIterator lineBegin, PrintIterator lineEnd,
                   Rect source, Rect target, Color32 backgroundColor) const;
    void initCurrentPosition();

    Rect printArea;
    bool lineContinuation;
    TextLayout layout;
    Vector2 currentPosition;
    Color32 defaultColor;
    bool drawShadows;
    double shadowColorMod;
    Vector2 shadowPosition;
    const Vector2 charSize;
    Vector2 moveVector;
    Texture texture;
    static const Vector2 dimensions;
    static const int chars = 96;
}
const Vector2 BitmapFont::dimensions = Vector2(16, 6);

BitmapFont::BitmapFont(boost::string_ref fileName, Vector2 charSize)
:   printArea(0, 0, 0, 0),
    lineContinuation(false),
    currentPosition(0, 0),
    defaultColor(Color32::white),
    drawShadows(false),
    shadowColorMod(0.5),
    shadowPosition(1, 1),
    charSize(charSize),
    moveVector(charSize),
    texture(fileName, Color32::black)
{
}

void BitmapFont::print(Window window, boost::string_ref text, Color32 color, Color32 backgroundColor,
                       bool blend, LineBreakMode lineBreakMode)
{
    if (!color)
        color = defaultColor;

    texture.setBlendMode(blend);

    if (drawShadows)
    {
        texture.setColor(color * shadowColorMod);
        printHelper(window, text, currentPosition + shadowPosition, backgroundColor, lineBreakMode);
    }

    texture.setColor(color);
    currentPosition = printHelper(window, text, currentPosition, backgroundColor, lineBreakMode);
    lineContinuation = true;
}

void BitmapFont::printLine(Window window, boost::string_ref text, Color32 color, Color32 backgroundColor,
                           bool blend, LineBreakMode lineBreakMode)
{
    print(window, text, color, backgroundColor, blend, lineBreakMode);
    currentPosition.x = printArea.position.x;
    currentPosition.y += moveVector.y;
}

void BitmapFont::printWithCursor(Window window, boost::string_ref text, const char cursor,
                                 Color32 mainColor, Color32 cursorColor, Color32 backgroundColor)
{
    Vector2 cursorPosition = currentPosition + Vector2(int((cursor - text.begin()) * moveVector.x), 0);
    print(window, text, mainColor);

    if (cursorColor)
        texture.setColor(cursorColor);

    printHelper(window, "_", cursorPosition, backgroundColor, PreserveLines);
}

Vector2 BitmapFont::printHelper(Window window, boost::string_ref text, Vector2 position,
                                Color32 backgroundColor, LineBreakMode lineBreakMode) const
{
    Rect source;
    source.size = charSize;
    Rect target(position, charSize);

    std::string splitText;

    if (lineBreakMode == SplitLines)
    {
        std::istringstream stream(text.to_string());
        int currentLineSize = 0;
        int maxLineSize = printArea.getRight() - position.x;

        for (std::string word; stream >> word;)
        {
            if ((currentLineSize + int(word.size())) * moveVector.x > maxLineSize)
            {
                if (splitText.empty())
                {
                    splitText = text.to_string();
                    break;
                }

                assert(splitText.back() == ' ');
                splitText.back() = '\n';
                currentLineSize = 0;
            }

            splitText += word;
            splitText += ' ';
            currentLineSize += word.size() + 1;
        }

        text = splitText;
    }

    if (!lineContinuation)
    {
        var lineCount = 1 + std::count(text.begin(), text.end(), '\n');
        var textHeight = lineCount * moveVector.y;

        switch (layout.verticalAlignment)
        {
            case TopAlign: break;
            case VerticalCenter: target.position.y -= textHeight / 2; break;
            case BottomAlign: target.position.y -= textHeight; break;
        }
    }

    for (var character = text.begin(), lineBegin = text.begin();; ++character)
    {
        if (*character == '\n' || character == text.end())
        {
            printLine(window, lineBegin, character, source, target, backgroundColor);

            if (character == text.end())
                return target.position;
            else
            {
                target.position.x = position.x;
                target.position.y += moveVector.y;
                lineBegin = character + 1;
            }
        }
    }
}

void BitmapFont::printLine(Window window, PrintIterator lineBegin, PrintIterator lineEnd,
                           Rect source, Rect target, Color32 backgroundColor) const
{
    const int lineWidth = int((lineEnd - lineBegin) * moveVector.x);

    if (!lineContinuation)
    {
        switch (layout.horizontalAlignment)
        {
            case LeftAlign: target.position.x = printArea.getLeft(); break;
            case HorizontalCenter: target.position.x = printArea.getCenter().x - lineWidth / 2; break;
            case RightAlign: target.position.x = printArea.getRight() - lineWidth; break;
        }
    }

    for (var character = lineBegin; character != lineEnd; ++character)
    {
        if (*character >= ' ')
        {
            var index = *character - ' ';
            source.position = charSize * Vector2(index % dimensions.x, index / dimensions.x);
            window.getGraphicsContext().renderFilledRectangle(target, backgroundColor);
            texture.render(window, source, target);
        }

        target.position.x += moveVector.x;
    }
}

Vector2 BitmapFont::getTextSize(boost::string_ref text) const
{
    int lengthOfLongestLine = 0;
    int currentLineLength = 0;
    int lineCount = 0;

    for (var character : text)
    {
        if (character != '\n')
        {
            ++currentLineLength;

            if (&character != &text.back())
                continue;
        }

        if (currentLineLength > lengthOfLongestLine)
            lengthOfLongestLine = currentLineLength;

        currentLineLength = 0;
        ++lineCount;
    }

    return Vector2(getColumnWidth() * lengthOfLongestLine, getRowHeight() * lineCount);
}

void BitmapFont::initCurrentPosition()
{
    lineContinuation = false;

    switch (layout.verticalAlignment)
    {
        case TopAlign:
            currentPosition = Vector2(printArea.position.x, printArea.getTop());
            break;
        case VerticalCenter:
            currentPosition = Vector2(printArea.position.x, printArea.getCenter().y);
            break;
        case BottomAlign:
            currentPosition = Vector2(printArea.position.x, printArea.getBottom());
            break;
    }
}

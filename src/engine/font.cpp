#include "font.h"
#include "geometry.h"
#include <algorithm>
#include <sstream>

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
    texture(fileName)
{
}

void BitmapFont::print(Window& window, boost::string_ref text, Color32 color, bool blend,
                       LineBreakMode lineBreakMode)
{
    if (!color)
        color = defaultColor;

    texture.setBlendMode(blend);

    if (drawShadows)
    {
        texture.setColor(color * shadowColorMod);
        printHelper(window, text, currentPosition + shadowPosition, lineBreakMode);
    }

    texture.setColor(color);
    currentPosition = printHelper(window, text, currentPosition, lineBreakMode);
    lineContinuation = true;
}

void BitmapFont::printLine(Window& window, boost::string_ref text, Color32 color)
{
    print(window, text, color, PreserveLines);
    currentPosition.x = printArea.position.x;
    currentPosition.y += moveVector.y;
}

void BitmapFont::printWithCursor(Window& window, boost::string_ref text, const char* cursor,
                                 Color32 mainColor, Color32 cursorColor)
{
    Vector2 cursorPosition = currentPosition + Vector2(int((cursor - text.begin()) * moveVector.x), 0);
    print(window, text, mainColor);

    if (cursorColor)
        texture.setColor(cursorColor);

    printHelper(window, "_", cursorPosition, PreserveLines);
}

Vector2 BitmapFont::printHelper(Window& window, boost::string_ref text, Vector2 position,
                                LineBreakMode lineBreakMode) const
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

    const auto lineCount = 1 + std::count(text.begin(), text.end(), '\n');
    const auto textHeight = lineCount * moveVector.y;

    switch (layout.verticalAlignment)
    {
        case TopAlign: break;
        case VerticalCenter: target.position.y -= textHeight / 2; break;
        case BottomAlign: target.position.y -= textHeight; break;
    }

    for (auto character = text.begin(), lineBegin = text.begin();; ++character)
    {
        if (*character == '\n' || character == text.end())
        {
            printLine(window, lineBegin, character, source, target);

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

void BitmapFont::printLine(Window& window, PrintIterator lineBegin, PrintIterator lineEnd,
                           Rect& source, Rect& target) const
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

    for (auto character = lineBegin; character != lineEnd; ++character)
    {
        if (*character >= ' ')
        {
            const auto index = *character - ' ';
            source.position = charSize * Vector2(index % dimensions.x, index / dimensions.x);
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

    for (auto& character : text)
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

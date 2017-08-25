#include "font.h"
#include "geometry.h"
#include <algorithm>
#include <fstream>
#include <stdexcept>

const Vector2 BitmapFont::dimensions = Vector2(16, 6);

BitmapFont::BitmapFont(const Window& window, const std::string& fileName, Vector2 charSize)
:   printArea(0, 0, 0, 0),
    lineContinuation(false),
    currentPosition(0, 0),
    defaultColor(Color32::white),
    drawShadows(false),
    shadowColorMod(0.5),
    shadowPosition(1, 1),
    charSize(charSize),
    moveVector(charSize),
    texture(window, loadFromFile(fileName), SDL_PIXELFORMAT_RGBA8888, dimensions * charSize)
{
    texture.setBlendMode(true);
}

std::vector<Color32> BitmapFont::loadFromFile(const std::string& fileName) const
{
    std::ifstream inputFile(fileName, std::ios::binary);

    if (!inputFile)
        throw std::runtime_error("Unable to open " + fileName);

    const int pixels = dimensions.getArea() * charSize.getArea();
    std::vector<Color32> pixelData;
    pixelData.reserve(pixels);

    for (int i = 0; i < pixels / CHAR_BIT; ++i)
    {
        char input;
        inputFile.read(&input, 1);

        for (int bit = 0; bit < CHAR_BIT; ++bit)
            pixelData.push_back(input >> bit & 1 ? UINT32_MAX : 0);
    }

    return pixelData;
}

void BitmapFont::print(const std::string& text, Color32 color)
{
    if (!color)
        color = defaultColor;

    if (drawShadows)
    {
        texture.setColor(color * shadowColorMod);
        printHelper(text, currentPosition + shadowPosition);
    }

    texture.setColor(color);
    currentPosition = printHelper(text, currentPosition);
    lineContinuation = true;
}

void BitmapFont::printLine(const std::string& text, Color32 color)
{
    int startX = currentPosition.x;
    print(text, color);
    currentPosition.x = startX;
    currentPosition.y += moveVector.y;
}

void BitmapFont::printWithCursor(const std::string& text, std::string::const_iterator cursor,
                                 Color32 mainColor, Color32 cursorColor)
{
    Vector2 cursorPosition = currentPosition + Vector2(int((cursor - text.begin()) * moveVector.x), 0);
    print(text, mainColor);

    if (cursorColor)
        texture.setColor(cursorColor);

    printHelper("_", cursorPosition);
}

Vector2 BitmapFont::printHelper(const std::string& text, Vector2 position) const
{
    Rect source;
    source.size = charSize;
    Rect target(position, charSize);
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
            printLine(lineBegin, character, source, target);

            if (character == text.end())
                return target.position;
            else
            {
                target.position.y += moveVector.y;
                lineBegin = character + 1;
            }
        }
    }
}

void BitmapFont::printLine(PrintIterator lineBegin, PrintIterator lineEnd, Rect& source,
                           Rect& target) const
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
        if (*character > ' ')
        {
            const auto index = *character - ' ';
            source.position = charSize * Vector2(index % dimensions.x, index / dimensions.x);
            texture.render(source, target);
        }

        target.position.x += moveVector.x;
    }
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

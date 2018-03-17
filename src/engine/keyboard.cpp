#include "keyboard.h"
#include "font.h"
#include "geometry.h"
#include "window.h"
#include <SDL.h>
#include <cctype>
#include <cassert>
#include <climits>

std::string toString(Key key)
{
    if (key > 0 && key <= UCHAR_MAX && std::islower(key))
        return std::string(1, char(key));

    return SDL_GetKeyName(key);
}

namespace keyboard
{
    int readLineProcessKey(const SDL_Event&, std::string& line, std::string::iterator& cursor);
}

int keyboard::readLine(Window& window, std::string& line, Vector2 position, const std::function<void(Window&)>& render,
                       boost::string_ref prefix)
{
    BitmapFont& font = window.getFont();
    std::string::iterator cursor = line.end();
    SDL_Event event;

    while (true)
    {
        render(window);
        font.setArea(Rect(position, window.getResolution() - position));
        font.print(window, prefix);
        font.printWithCursor(window, line, cursor == line.end() ? nullptr : &*cursor);
        window.updateScreen();
        SDL_WaitEvent(&event);

        if (auto key = readLineProcessKey(event, line, cursor))
            return key;
    }
}

int keyboard::readLineProcessKey(const SDL_Event& event, std::string& line, std::string::iterator& cursor)
{
    static const int maxBufferSize = 4096;

    if (event.type != SDL_KEYDOWN)
        return 0;

    auto key = event.key.keysym.sym;

    switch (key)
    {
        case Backspace:
            if (cursor != line.begin())
            {
                cursor = line.erase(cursor - 1);
                return 0;
            }
            break;

        case Delete:
            if (cursor != line.end())
            {
                cursor = line.erase(cursor);
                return 0;
            }
            break;

        case LeftArrow:
            if (cursor-- == line.begin())
            {
                cursor = line.end();
                return 0;
            }
            break;

        case RightArrow:
            if (cursor++ == line.end())
            {
                cursor = line.begin();
                return 0;
            }
            break;

        case 'v':
            if (event.key.keysym.mod & Ctrl)
            {
                std::unique_ptr<char[], decltype(SDL_free)&> clipboardText(SDL_GetClipboardText(),
                                                                           SDL_free);
                auto clipboardTextSize = strlen(clipboardText.get());
                auto maxToAdd = maxBufferSize - line.size();
                char* endPosition = std::min(clipboardText.get() + maxToAdd,
                                             clipboardText.get() + clipboardTextSize);
                line.insert(cursor, clipboardText.get(), endPosition);
                cursor = line.end();
                return 0;
            }
            break;

        case 'c':
            if (event.key.keysym.mod & Ctrl && !line.empty())
            {
                SDL_SetClipboardText(line.c_str());
                return 0;
            }
            break;

        case 'x':
            if (event.key.keysym.mod & Ctrl && !line.empty())
            {
                SDL_SetClipboardText(line.c_str());
                line.clear();
                cursor = line.begin();
                return 0;
            }
            break;
    }

    if (key >= 0 && key <= UCHAR_MAX && std::isprint(key) && line.size() < maxBufferSize)
        cursor = 1 + line.insert(cursor, key);

    return key;
}

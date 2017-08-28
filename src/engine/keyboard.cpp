#include "keyboard.h"
#include "geometry.h"
#include "window.h"
#include <SDL.h>
#include <cctype>
#include <cassert>

namespace keyboard
{
    int readLineProcessKey(const SDL_Event&, std::string& line, std::string::iterator& cursor);
    static CommandFunction keyMap[128][4];
    static CommandFunction specialKeyMap[226][4];
}

void keyboard::mapKey(Key key, Mod mod, CommandFunction callback)
{
    if (isupper(key))
    {
        key = tolower(key);
        mod |= Shift;
    }

    switch (mod)
    {
        case NoMod: break;
        case Shift: mod = 1; break;
        case Ctrl: mod = 2; break;
        case Alt: mod = 3; break;
        default: assert(false);
    }

    getMappedCommand(key, mod) = callback;
}

CommandFunction& keyboard::getMappedCommand(Key key, Mod mod)
{
    if (!(key & SDLK_SCANCODE_MASK))
        return keyMap[key][mod];
    else
        return specialKeyMap[key - SDLK_SCANCODE_MASK - SDL_SCANCODE_CAPSLOCK][mod];
}

int keyboard::readLine(Window& window, std::string& line, Vector2 position, RenderFunction render,
                       boost::string_ref prefix)
{
    BitmapFont& font = window.getFont();
    std::string::iterator cursor = line.end();
    int exitCode = 0;
    SDL_Event event;
    SDL_StartTextInput();

    while (!exitCode)
    {
        render(window);
        font.setArea(Rect(position, window.getResolution() - position));
        font.print(prefix);
        font.printWithCursor(line, &*cursor);
        window.updateScreen();
        SDL_WaitEvent(&event);
        exitCode = readLineProcessKey(event, line, cursor);
    }

    SDL_StopTextInput();
    return exitCode;
}

int keyboard::readLineProcessKey(const SDL_Event& event, std::string& line, std::string::iterator& cursor)
{
    static const int maxBufferSize = 4096;

    if (event.type == SDL_TEXTINPUT)
    {
        if (std::isprint(event.text.text[0]) && event.text.text[1] == '\0' &&
            line.size() < maxBufferSize)
            cursor = 1 + line.insert(cursor, event.text.text[0]);
        return 0;
    }

    if (event.type != SDL_KEYDOWN)
        return 0;

    switch (event.key.keysym.sym)
    {
        case Enter:
        case Esc:
        case Tab:
        case UpArrow:
        case DownArrow:
            return event.key.keysym.sym;
        case Backspace:
            if (cursor != line.begin())
                cursor = line.erase(cursor - 1);
            break;
        case Delete:
            if (cursor != line.end())
                cursor = line.erase(cursor);
            break;
        case LeftArrow:
            if (cursor-- == line.begin())
                cursor = line.end();
            break;
        case RightArrow:
            if (cursor++ == line.end())
                cursor = line.begin();
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
                cursor = clipboardTextSize + line.insert(cursor, clipboardText.get(), endPosition);
            }
            break;
        case 'c':
            if (event.key.keysym.mod & Ctrl && !line.empty())
                SDL_SetClipboardText(line.c_str());
            break;
        case 'x':
            if (event.key.keysym.mod & Ctrl && !line.empty())
            {
                SDL_SetClipboardText(line.c_str());
                line.clear();
                cursor = line.begin();
            }
            break;
    }

    return 0;
}

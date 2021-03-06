#pragma once

#include "geometry.h"
#include <SDL.h>
#include <string_view>
#include <functional>
#include <string>

class Window;

using Key = decltype(SDL_Keysym::sym);
using Mod = decltype(SDL_Keysym::mod);

enum : Key
{
    NoKey = 0,
    Backspace = SDLK_BACKSPACE,
    Tab = SDLK_TAB,
    Enter = SDLK_RETURN,
    Esc = SDLK_ESCAPE,
    Delete = SDLK_DELETE,
    RightArrow = SDLK_RIGHT,
    LeftArrow = SDLK_LEFT,
    DownArrow = SDLK_DOWN,
    UpArrow = SDLK_UP,
    F2 = SDLK_F2,
    F3 = SDLK_F3
};

std::string toString(Key key);

enum : Mod
{
    NoMod = 0,
    LeftShift = KMOD_LSHIFT,
    RightShift = KMOD_RSHIFT,
#ifndef __APPLE__
    LeftCtrl = KMOD_LCTRL,
    RightCtrl = KMOD_RCTRL,
#else
    LeftCtrl = KMOD_LGUI,
    RightCtrl = KMOD_RGUI,
#endif
    LeftAlt = KMOD_LALT,
    RightAlt = KMOD_RALT,
    CapsLock = KMOD_CAPS,
    Shift = LeftShift | RightShift,
    Ctrl = LeftCtrl | RightCtrl,
    Alt = LeftAlt | RightAlt
};

namespace keyboard
{
    int readLine(Window&, std::string& lineContent, Vector2 position,
                 const std::function<void()>&, std::string_view prefix = "");
}

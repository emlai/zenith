#pragma once

#include "font.h"
#include <SDL.h>
#include <functional>

using Key = decltype(SDL_Keysym::sym);
using Mod = decltype(SDL_Keysym::mod);
using KeyDownCallback = std::function<void(Window&, Key, Mod)>;
using RenderFunction = std::function<void(Window&)>;
using CommandFunction = std::function<void()>;

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
    Comma = SDLK_COMMA,
    C = SDLK_c
};

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
    int readLine(Window&, std::string& lineContent, Vector2 position, RenderFunction,
                 boost::string_ref prefix = "");
    void mapKey(Key, Mod, CommandFunction);
    CommandFunction& getMappedCommand(Key, Mod);
}

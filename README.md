# Zenith

Zenith is an upcoming open-world roguelike-like game currently in development.
It is written in C++17 using the SDL2 library.

![](https://forum.freegamedev.net/download/file.php?id=10844&mode=view)
![](https://forum.freegamedev.net/download/file.php?id=10843&mode=view)

## Building from source

1.  Install the dependencies: CMake and SDL2.

    Ubuntu:

        sudo apt-get install cmake libsdl2-dev

    macOS (with [Homebrew](https://brew.sh/)):

        brew install cmake sdl2

    Windows:

    - Download CMake installer: https://cmake.org/download/
    - Download SDL2 development libraries: https://libsdl.org/download-2.0.php

2.  Generate a build system:

        cmake .

    You may need to define the path to SDL2:

        cmake . -DSDL2_PATH="/path/to/SDL2"

3.  Build the project using the generated build system, or directly from the command line using:

        cmake --build .

    Or in release mode:

        cmake --build . --config Release

    This creates the game executable in the project root directory.
    On Windows you may need to copy SDL2.dll to this directory to run the game.

Feel free to open an issue if you run into any problems.

## License

The Zenith source code is licensed under the GNU General Public License. See the
file [LICENSE.txt](LICENSE.txt) for the full license text.

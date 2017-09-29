# Zenith [![Build Status](https://travis-ci.org/emlai/zenith.svg?branch=master)](https://travis-ci.org/emlai/zenith)

Zenith is an upcoming open-world roguelike-like game currently in development.

It is written in C++14 using the Boost and SDL2 libraries.

## Building from source

Install the dependencies: CMake, Boost, and SDL2.

Ubuntu:

    sudo apt-get install cmake libboost-all-dev libsdl2-dev

macOS (with [Homebrew](https://brew.sh/)):

    brew install cmake boost sdl2

Then, generate a build system, and build the project:

    cmake .
    make -j

This creates the game executable in the project root directory.

Feel free to open an issue if you run into any problems.

## License

The Zenith source code is licensed under the GNU General Public License. See the
file [LICENSE.txt](LICENSE.txt) for the full license text.
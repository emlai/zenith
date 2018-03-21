# Zenith [![Build Status](https://travis-ci.org/emlai/zenith.svg?branch=master)](https://travis-ci.org/emlai/zenith) [![Build Status](https://ci.appveyor.com/api/projects/status/avyab8dh0d35dakh/branch/master?svg=true)](https://ci.appveyor.com/project/emlai/zenith/branch/master)

Zenith is an upcoming open-world roguelike-like game currently in development.
It is written in C++17 using the SDL2 library.

## Building from source

Install the dependencies: CMake and SDL2.

Ubuntu:

    sudo apt-get install cmake libsdl2-dev

macOS (with [Homebrew](https://brew.sh/)):

    brew install cmake sdl2

Then, generate a build system, and build the project:

    cmake .
    make -j

This creates the game executable in the project root directory.

Feel free to open an issue if you run into any problems.

## License

The Zenith source code is licensed under the GNU General Public License. See the
file [LICENSE.txt](LICENSE.txt) for the full license text.

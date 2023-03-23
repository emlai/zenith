cmake_minimum_required(VERSION 3.1)
project(zenith VERSION 0.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}" -DPROJECT_VERSION="${PROJECT_VERSION}" -D_USE_MATH_DEFINES)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.h ${PROJECT_SOURCE_DIR}/src/*.cpp)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Weverything HAS_WEVERYTHING)
if(HAS_WEVERYTHING)
    set(CMAKE_CXX_FLAGS "-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic \
        -Wno-padded -Wno-global-constructors -Wno-exit-time-destructors -Wno-weak-vtables \
        -Wno-shadow -Wno-sign-conversion -Wno-shorten-64-to-32 -Wno-conversion ${CMAKE_CXX_FLAGS}")
elseif(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wold-style-cast ${CMAKE_CXX_FLAGS}")
endif()

foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_BINARY_DIR})
endforeach()

add_executable(zenith WIN32 ${SOURCES})
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
target_include_directories(zenith PUBLIC "${PROJECT_SOURCE_DIR}/src")
set_target_properties(zenith PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/cmake-modules")

if(NOT EXISTS "${CMAKE_BINARY_DIR}/cmake-modules/FindSDL2.cmake")
    file(DOWNLOAD "https://github.com/tcbrindle/sdl2-cmake-scripts/raw/master/FindSDL2.cmake" "${CMAKE_BINARY_DIR}/cmake-modules/FindSDL2.cmake")
endif()

if(NOT EXISTS "${CMAKE_BINARY_DIR}/cmake-modules/cotire.cmake")
    file(DOWNLOAD "https://github.com/sakra/cotire/raw/master/CMake/cotire.cmake" "${CMAKE_BINARY_DIR}/cmake-modules/cotire.cmake")
endif()

find_package(SDL2 REQUIRED)
target_include_directories(zenith SYSTEM PUBLIC ${SDL2_INCLUDE_DIR})
target_link_libraries(zenith ${SDL2_LIBRARY})

add_subdirectory(llama.cpp)
target_compile_options(llama PRIVATE -O3)
target_include_directories(zenith SYSTEM PUBLIC llama.cpp)
target_link_libraries(zenith llama)

include(cotire)
cotire(zenith)

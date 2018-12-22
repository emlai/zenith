cmake_minimum_required(VERSION 3.10)
project(zenith VERSION 0.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}" -DPROJECT_VERSION="${PROJECT_VERSION}")

file(GLOB_RECURSE ENGINE_SOURCES ${PROJECT_SOURCE_DIR}/src/engine/*.h ${PROJECT_SOURCE_DIR}/src/engine/*.cpp)
file(GLOB_RECURSE MAIN_SOURCES ${PROJECT_SOURCE_DIR}/src/main/*.h ${PROJECT_SOURCE_DIR}/src/main/*.cpp)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Weverything HAS_WEVERYTHING)
if(HAS_WEVERYTHING)
    set(CMAKE_CXX_FLAGS "-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic \
        -Wno-padded -Wno-global-constructors -Wno-exit-time-destructors -Wno-weak-vtables \
        -Wno-shadow -Wno-sign-conversion ${CMAKE_CXX_FLAGS}")
elseif(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wold-style-cast ${CMAKE_CXX_FLAGS}")
endif()

foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_BINARY_DIR})
endforeach()

add_library(zenithEngine ${ENGINE_SOURCES})
add_executable(zenith WIN32 ${MAIN_SOURCES})
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
target_include_directories(zenith PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(zenith zenithEngine)
set_target_properties(zenith PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/cmake-modules)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/cmake-modules/FindSDL2.cmake")
    file(DOWNLOAD "https://github.com/tcbrindle/sdl2-cmake-scripts/raw/master/FindSDL2.cmake"
         "${CMAKE_BINARY_DIR}/cmake-modules/FindSDL2.cmake")
endif()
find_package(SDL2 REQUIRED)
target_include_directories(zenithEngine SYSTEM PUBLIC ${SDL2_INCLUDE_DIR})
target_link_libraries(zenithEngine ${SDL2_LIBRARY})

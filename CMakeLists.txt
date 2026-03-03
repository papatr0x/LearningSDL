cmake_minimum_required(VERSION 4.1)
project(LetsLearnSDL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG        "release-3.4.2"
)
FetchContent_MakeAvailable(SDL3)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(LetsLearnSDL ${SOURCES})
target_link_libraries(LetsLearnSDL PUBLIC SDL3::SDL3)

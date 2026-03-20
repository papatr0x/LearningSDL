cmake_minimum_required(VERSION 4.1)
project(LetsLearnSDL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# --- SDL3 ---
set(SDL_TESTS    OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
    GIT_TAG        "release-3.4.2"
)
FetchContent_MakeAvailable(SDL3)

# --- SDL3_ttf ---
set(SDLTTF_FREETYPE  OFF CACHE BOOL "" FORCE)
set(SDLTTF_VENDORED  ON  CACHE BOOL "" FORCE)
set(SDLTTF_HARFBUZZ  OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf.git"
    GIT_TAG        "release-3.2.2"
)
FetchContent_MakeAvailable(SDL3_ttf)

# --- SDL3_mixer ---
set(SDLMIXER_VENDORED ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    SDL3_mixer
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL_mixer.git"
    GIT_TAG        "release-3.2.0"
)
FetchContent_MakeAvailable(SDL3_mixer)

# Fix for missing headers when compiling static version on Windows
if (WIN32)
    if (TARGET libmpg123)
        target_include_directories(SDL3_mixer-static PRIVATE
            "${SDL3_mixer_SOURCE_DIR}/external/mpg123/src/include"
            "${SDL3_mixer_BINARY_DIR}/external/libmpg123-build/ports/cmake/src/libmpg123"
        )
    endif()
endif()

add_subdirectory(engine)
add_subdirectory(demo1)
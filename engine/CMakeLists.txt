file(GLOB_RECURSE ENGINE_SOURCES "*.cpp")

add_library(engine STATIC ${ENGINE_SOURCES})

target_link_libraries(engine PUBLIC
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    SDL3_mixer::SDL3_mixer
)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(demo3_space ${SOURCES})

target_include_directories(demo3_space PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(demo3_space PRIVATE engine)

add_custom_command(
    OUTPUT  "${CMAKE_CURRENT_SOURCE_DIR}/BuildInfo.h"
    COMMAND python3 "${CMAKE_SOURCE_DIR}/bump_build.py" "${CMAKE_CURRENT_LIST_DIR}"
    DEPENDS ${SOURCES}
    COMMENT "Bumping build version for demo3_space"
    VERBATIM
)

add_custom_target(bump_build_number_demo3
    DEPENDS "${CMAKE_CURRENT_LIST_DIR}/BuildInfo.h"
)
add_dependencies(demo3_space bump_build_number_demo3)
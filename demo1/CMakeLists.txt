file(GLOB_RECURSE DEMO1_SOURCES "*.cpp")

add_executable(demo1 ${DEMO1_SOURCES})

target_include_directories(demo1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(demo1 PRIVATE engine)
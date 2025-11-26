project(GDCave)

# Core cave library
set(CAVE_LIB_NAME cave)
file(GLOB_RECURSE CAVE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp")

add_library(${CAVE_LIB_NAME} SHARED ${CAVE_SOURCES})
target_include_directories(${CAVE_LIB_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_BINARY_DIR}/_deps/libs-src/lib/MathStuff"
    "${CMAKE_BINARY_DIR}/_deps/libs-src/lib/Algo"
    "${CMAKE_BINARY_DIR}/_deps/libs-src/lib/Util")

if(NOT MSVC)
    set_target_properties(${CAVE_LIB_NAME} PROPERTIES PREFIX "")
endif()

target_link_libraries(${CAVE_LIB_NAME}
    PRIVATE Algo
    PRIVATE PCG
    PRIVATE Random
    PRIVATE MathStuff)

# Godot wrapper library
set(GDCAVE_LIB_NAME GDCave)
file(GLOB_RECURSE GDCAVE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/godot/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/godot/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/godot/*.cpp")

add_library(${GDCAVE_LIB_NAME} SHARED ${GDCAVE_SOURCES})
target_include_directories(${GDCAVE_LIB_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_BINARY_DIR}/_deps/libs-src/lib/Util")

if(NOT MSVC)
    set_target_properties(${GDCAVE_LIB_NAME} PROPERTIES PREFIX "")
endif()

target_link_libraries(${GDCAVE_LIB_NAME}
    PRIVATE ${CAVE_LIB_NAME}
    PUBLIC godot::cpp)

add_custom_command(
    TARGET ${GDCAVE_LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${GDCAVE_LIB_NAME}>
        "D:/git/the-lott/addons/gdcave/bin")

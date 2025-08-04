project(GDCave)

SET(LIB_NAME GDCave)

# Automatically pick up files added to src
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.C"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${LIB_NAME} SHARED ${SOURCES})

target_include_directories(${LIB_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(${LIB_NAME} 
	PRIVATE Algo
	PRIVATE PCG
	PRIVATE Random
	PRIVATE MathStuff
	PUBLIC godot::cpp)

add_custom_command(
        TARGET ${LIB_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_FILE:${LIB_NAME}>
			    "D:/git/the-lott/addons/gdcave/bin")
			    

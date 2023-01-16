cmake_minimum_required(VERSION 3.13)
project(source/amogpu)
set(CMAKE_CXX_STANDARD 17)

if(WIN32)
	set(LIBRARY_OUTPUT_PATH ../build/libs/win/)
else()
	set(LIBRARY_OUTPUT_PATH ../build/libs/linux/)
endif()

file(GLOB_RECURSE HEADER_FILES "source/amogpu/*.hpp")
file(GLOB_RECURSE SRC_FILES "source/src/*.cpp")

include_directories(source/)
add_library(amogpu STATIC "${SRC_FILES}")
add_compile_options(-O3)

set_target_properties(amogpu PROPERTIES VERSION 1.0 SOVERSION 1)
set_target_properties(amogpu PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
target_include_directories(amogpu PUBLIC .)

set(ENABLE_TESTING true)

if (ENABLE_TESTING)
	project(amogputesting)

	set(CURRENT_OS "")
	set(CMAKE_CXX_STANDARD 17)
	set(LINKER_OPTIONS "")

	if(WIN32)
		set(CURRENT_OS "win")
		set(LINKER_OPTIONS mingw32 opengl32 glew32 SDL2main SDL2 freetype)
	else()
		set(CURRENT_OS "linux")
		set(LINKER_OPTIONS GLEW SDL2main SDL2 freetype)
	endif()

	set(EXECUTABLE_OUTPUT_PATH ../build/tests/${CURRENT_OS}/)

	add_compile_options(-O3)
	add_executable(amogputesting "tests/main.cpp")
	target_link_libraries(amogputesting "${CMAKE_CURRENT_SOURCE_DIR}/build/libs/${CURRENT_OS}/libamogpu.a" ${LINKER_OPTIONS})
endif()
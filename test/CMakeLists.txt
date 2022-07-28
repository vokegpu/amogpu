cmake_minimum_required(VERSION 3.13)
project(testfun)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ../build)

get_filename_component(BACKWARD_DIR ../ ABSOLUTE)
file(GLOB_RECURSE DYNAMIC_BATCHING_STATIC_LIB "${BACKWARD_DIR}/lib/libamogpu.a")
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# The dynamic batching library.
include_directories("${BACKWARD_DIR}/include")
include_directories(src)
message("-- Creating executable!")

add_executable(testfun ${SRC_FILES})
target_link_libraries(testfun "${DYNAMIC_BATCHING_STATIC_LIB}" mingw32 SDL2main SDL2 opengl32 glew32 freetype)
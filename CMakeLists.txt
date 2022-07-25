cmake_minimum_required(VERSION 3.13)
project(jogodaforcagpu)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ../build)
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

include_directories(src)

add_executable(jogodaforcagpu ${SRC_FILES})
target_link_libraries(jogodaforcagpu mingw32 glew32 opengl32 SDL2main SDL2 freetype)
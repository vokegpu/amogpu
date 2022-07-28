cmake_minimum_required(VERSION 3.13)
project(amogpu)

set(CMAKE_CXX_STANDARD 11)
set(LIBRARY_OUTPUT_PATH ../lib)

file(GLOB_RECURSE HEADER_FILES "include/*.hpp")
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

include_directories(include)
add_library(amogpu STATIC "${SRC_FILES}")

set_target_properties(amogpu PROPERTIES VERSION 1.0 SOVERSION 1)
set_target_properties(amogpu PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
target_include_directories(amogpu PUBLIC .)
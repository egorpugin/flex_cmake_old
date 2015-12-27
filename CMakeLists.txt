#
# flex
#

cmake_minimum_required(VERSION 2.8.11)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")

project(flex CXX)

find_package(BISON REQUIRED 3.0)

add_definitions(-DHAVE_CONFIG_H)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0 /MP")

add_subdirectory(src)

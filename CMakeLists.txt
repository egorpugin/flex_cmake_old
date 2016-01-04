#
# flex
#

cmake_minimum_required(VERSION 2.8.11)

# Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")

project(flex C CXX)

enable_testing()

find_package(BISON REQUIRED 3.0)
find_package(FLEX REQUIRED 2.5.35)

add_definitions(-DHAVE_CONFIG_H)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0 /MP")
elseif (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fpermissive")
endif()

if (WIN32)
    set(winsock_lib Ws2_32)
endif()

add_subdirectory(src)
add_subdirectory(tests)

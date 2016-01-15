#
# flex
#

###############################################################################
#
# cmake settings
#
###############################################################################

cmake_minimum_required(VERSION 2.8.11)

# In-source builds are disabled.
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR
        "CMake generation is not possible within the source directory!"
        "\n Remove the CMakeCache.txt file and try again from another folder, e.g.:"
        "\n "
        "\n rm CMakeCache.txt"
        "\n mkdir build"
        "\n cd build"
        "\n cmake .."
    )
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake")

cmake_policy(SET CMP0053 NEW)

# Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")

###############################################################################
#
# project settings
#
###############################################################################

project(flex C CXX)

if (WITH_TESTS)
    enable_testing()
endif()

set(VERSION_MAJOR 3)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION_PLAIN ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

find_package(BISON REQUIRED 3.0)
find_package(FLEX REQUIRED 2.5.35)

###############################################################################
#
# compiler and linker
#
###############################################################################

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

###############################################################################
#
# configure
#
###############################################################################

set(AUTOCONFIG_SRC ${CMAKE_BINARY_DIR}/config.h.in)
set(AUTOCONFIG ${CMAKE_BINARY_DIR}/config.h)

include(Configure)

configure_file(${AUTOCONFIG_SRC} ${AUTOCONFIG} @ONLY)

###############################################################################
#
# build
#
###############################################################################

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(doc)
add_subdirectory(src)

if (WITH_TESTS)
    add_subdirectory(tests)
endif()

###############################################################################

#
# flex
#

cmake_minimum_required(VERSION 2.8.11)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")

project(flex C CXX)

find_package(BISON REQUIRED 3.0)

file(GLOB flex_src "src/*.cpp")
file(GLOB flex_hdr "src/*.h")
set_source_files_properties(${flex_src} PROPERTIES LANGUAGE CXX)

add_definitions(-DHAVE_CONFIG_H)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include_directories(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0 /MP")

BISON_TARGET(parse
    "src/parse.y"
    "${CMAKE_CURRENT_BINARY_DIR}/parse.cpp"
    DEFINES_FILE "src/parse.h"
)

add_executable(flex
    ${BISON_parse_OUTPUTS}
    ${flex_src}
    ${flex_hdr}
    src/parse.y
    src/flex.skl
    src/scan.l
)
target_link_libraries(flex Ws2_32)
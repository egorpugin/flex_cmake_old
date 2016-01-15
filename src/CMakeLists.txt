#
# flex
#

########################################

set(BSRC ${CMAKE_BINARY_DIR})

set(_CPP ".*\\.cpp")
set(CPP "${_CPP}$")

set(_H ".*\\.h")
set(H "${_H}$")

set(H_CPP "(${H}|${CPP})")

source_group("Generated Files" "${BSRC}/${H_CPP}")

########################################

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

bison_target(parse
    "parse.y"
    "${CMAKE_CURRENT_BINARY_DIR}/parse.cpp"
    COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/parse.h"
    DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/parse.h"
)
flex_target(scan
    "${CMAKE_CURRENT_SOURCE_DIR}/scan.l"
    "${CMAKE_CURRENT_BINARY_DIR}/stage1scan.cpp"
)

set(context_src
    context.cpp
    context.h
)

add_library(context ${context_src})

set(simple_m4_src
    simple_m4.cpp
    simple_m4.h
)

add_library(simple_m4 ${simple_m4_src})
target_link_libraries(simple_m4 context)

add_executable(prepare_skel prepare_skel.cpp)
target_link_libraries(prepare_skel simple_m4)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/skel.cpp
    COMMAND prepare_skel "${CMAKE_CURRENT_SOURCE_DIR}/flex.skl" "${CMAKE_CURRENT_BINARY_DIR}/skel.cpp"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS prepare_skel flex.skl
)

set(common_flex_src
    ccl.cpp
    ccl.h
    common.h
    dfa.cpp
    dfa.h
    ecs.cpp
    ecs.h
    flexdef.h
    flexint.h
    gen.cpp
    gen.h
    gettext.h
    main.cpp
    misc.cpp
    misc.h
    nfa.cpp
    nfa.h
    options.cpp
    options.h
    scanflags.cpp
    scanflags.h
    scanopt.cpp
    scanopt.h
    sym.cpp
    sym.h
    tables.cpp
    tables.h
    tables_shared.cpp
    tables_shared.h
    tblcmp.cpp
    tblcmp.h
    yylex.cpp
    
    ${CMAKE_CURRENT_BINARY_DIR}/skel.cpp

    ${CMAKE_BINARY_DIR}/config.h

    FlexLexer.h

    parse.y
    flex.skl
    scan.l

    ${BISON_parse_OUTPUTS}
)

add_library(common ${common_flex_src})
target_link_libraries(common simple_m4 ${winsock_lib})

set(stage1flex_src
    ${FLEX_scan_OUTPUTS}
)

add_executable(stage1flex ${stage1flex_src})
target_link_libraries(stage1flex common)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/scan.cpp
    COMMAND stage1flex -o "${CMAKE_CURRENT_BINARY_DIR}/scan.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/scan.l"
    DEPENDS stage1flex scan.l
)

set(flex_src
    "${CMAKE_CURRENT_BINARY_DIR}/scan.cpp"
)

add_executable(flex ${flex_src})
target_link_libraries(flex common)
install(TARGETS flex RUNTIME DESTINATION bin)


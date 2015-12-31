#
# flex
#

if (WIN32)
add_definitions(-DYY_NO_UNISTD_H=1)
add_definitions(-Disatty=_isatty)
endif()

add_definitions(-DDEFINE_ME)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(dir ${CMAKE_CURRENT_BINARY_DIR})
set(SOLUTION_FOLDER Tests)

########################################

macro(test_project_group target)
    if (SOLUTION_FOLDER)
        set_target_properties(${target} PROPERTIES FOLDER ${SOLUTION_FOLDER})
    endif()
endmacro(test_project_group)

########################################

macro(test_simple_m4 s e)
    foreach(T RANGE ${s} ${e})
        add_test(NAME simple_m4_${T} COMMAND simple_m4_test ${T})
    endforeach()
endmacro(test_simple_m4)

########################################

function(add_flex_test_generate name lang out)
    set(test_file ${CMAKE_CURRENT_BINARY_DIR}/${name}.${lang})
    add_test(NAME ${name}_generate
        COMMAND flex -o ${test_file} ${CMAKE_CURRENT_SOURCE_DIR}/${name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set(${out} ${test_file} PARENT_SCOPE)
endfunction(add_flex_test_generate)

########################################

function(add_flex_test name lang)
    list(GET name 0 test_name)

    set(test_files)
    foreach(N ${name})
        add_flex_test_generate(${N} ${lang} out)
        set(test_files "${test_files};${out}")
    endforeach()
    
    set_source_files_properties(${test_files} PROPERTIES GENERATED True)

    add_executable(${test_name} ${test_files} ${name} ${ARGN})
    set_target_properties(${test_name} PROPERTIES EXCLUDE_FROM_ALL True)
    set_target_properties(${test_name} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD True)
    test_project_group(${test_name})

    add_test(NAME ${test_name}_build
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${test_name}
    )
endfunction(add_flex_test)

########################################

function(add_bison_target name)
    bison_target(${name} "${name}_parser.y"
        "${CMAKE_CURRENT_BINARY_DIR}/${name}_parser.c"
        COMPILE_FLAGS "--name-prefix=\"test\""
        DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/${name}_parser.h"
    )
    set(BISON_${name}_OUTPUTS ${BISON_${name}_OUTPUTS} PARENT_SCOPE)
endfunction(add_bison_target)

########################################

add_executable(simple_m4_test simple_m4_test.cpp)
target_link_libraries(simple_m4_test simple_m4)
test_project_group(simple_m4_test)

########################################

test_simple_m4(1 13)

set(flex_tests_simple
	alloc_extra.l
	array_nr.l
	array_r.l
	basic_nr.l
	basic_r.l
	c_cxx_nr.lll
	c_cxx_r.lll
	ccl.l
	debug_nr.l
	debug_r.l
	extended.l
	mem_nr.l
	mem_r.l
	noansi_nr.l
	noansi_r.l
	posix.l
	posixly_correct.l
	prefix_nr.l
	prefix_r.l
	quotes.l
	string_nr.l
	string_r.l
	yyextra.l
)

foreach(T ${flex_tests_simple})
    add_flex_test(${T} c)
endforeach()

add_flex_test(cxx_basic.ll cc)
add_flex_test("cxx_multiple_scanners_1.ll;cxx_multiple_scanners_2.ll" cc cxx_multiple_scanners_main.cc)
add_flex_test(header_nr_scanner.l c header_nr_main.c)
add_flex_test(header_r_scanner.l c header_r_main.c)
add_flex_test("multiple_scanners_nr_1.l;multiple_scanners_nr_2.l" c multiple_scanners_nr_main.c)
add_flex_test("multiple_scanners_r_1.l;multiple_scanners_r_2.l" c multiple_scanners_r_main.c)
add_flex_test(top.l c top_main.c)

add_bison_target(bison_nr)
add_flex_test(bison_nr_scanner.l c
    bison_nr_main.c
    ${BISON_bison_nr_OUTPUTS}
    bison_nr_parser.y
)

add_bison_target(bison_yylloc)
add_flex_test(bison_yylloc_scanner.l c
    bison_yylloc_main.c
    ${BISON_bison_yylloc_OUTPUTS}
    bison_yylloc_parser.y
)

add_bison_target(bison_yylval)
add_flex_test(bison_yylval_scanner.l c
    bison_yylval_main.c
    ${BISON_bison_yylval_OUTPUTS}
    bison_yylval_parser.y
)






#reject_tests
	#reject_nr.reject
	#reject_r.reject

#TABLE_TESTS
	#reject_ver.table
	#reject_ser.table

#DIRECT_TESTS
	#include_by_buffer.direct
	#include_by_push.direct
	#include_by_reentrant.direct
	#rescan_nr.direct
	#rescan_r.direct

#I3_TESTS
	#cxx_yywrap.i3

#PTHREAD_TESTS
	#pthread.pthread

#ONE_TESTS
	#lineno_nr.one
	#lineno_r.one
	#lineno_trailing.one

#
# flex
#

################################################################################

if (WIN32)
    add_definitions(-DYY_NO_UNISTD_H=1)
    add_definitions(-Disatty=_isatty)
endif()

add_definitions(-DDEFINE_ME)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(dir ${CMAKE_CURRENT_BINARY_DIR})
set(SOLUTION_FOLDER Tests)

set(generate_suffix _generate)
set(build_suffix _build)
set(run_suffix _run)

################################################################################

########################################
# MACRO test_project_group
########################################

macro(test_project_group target)
    if (SOLUTION_FOLDER)
        set_target_properties(${target} PROPERTIES FOLDER ${SOLUTION_FOLDER})
    endif()
endmacro(test_project_group)

########################################
# MACRO test_depends
########################################

macro(test_depends test1 test2)
    set_tests_properties(${test1} PROPERTIES DEPENDS ${test2})
endmacro(test_depends)

########################################
# MACRO test_simple_m4
########################################

macro(test_simple_m4 s e)
    add_test(NAME simple_m4${build_suffix}
        COMMAND ${CMAKE_COMMAND}
            --build ${CMAKE_BINARY_DIR}
            --target simple_m4_test
            --config $<CONFIGURATION>
    )

    foreach(T RANGE ${s} ${e})
        add_test(NAME simple_m4_${T} COMMAND simple_m4_test ${T})
        test_depends(simple_m4_${T} simple_m4${build_suffix})
    endforeach()
endmacro(test_simple_m4)

################################################################################

########################################
# FUNCTION add_test_run
########################################

function(add_test_run test_name directory)
    if ("${directory}" STREQUAL "")
        set(directory ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    add_test(
        NAME ${test_name}${run_suffix}
        COMMAND ${test_name} ${ARGN}
        WORKING_DIRECTORY ${directory}
    )
    set_tests_properties(${test_name}${run_suffix} PROPERTIES DEPENDS ${test_name}${build_suffix})
endfunction(add_test_run)

########################################
# FUNCTION add_test_run_with_input
########################################

function(add_test_run_with_input test_name input_file)
    add_test(NAME ${test_name}${run_suffix}
        COMMAND ${CMAKE_COMMAND}
            -DEXE=$<TARGET_FILE:${test_name}>
            -DTXT=${CMAKE_CURRENT_SOURCE_DIR}/${input_file}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/test.cmake
    )
    set_tests_properties(${test_name}${run_suffix} PROPERTIES DEPENDS ${test_name}${build_suffix})
endfunction(add_test_run_with_input)

########################################
# FUNCTION add_test_flex_raw
########################################

function(add_test_flex_raw test_name generated_file source_file options)
    add_test(NAME ${test_name}${generate_suffix}
        COMMAND flex -o ${generated_file} ${options} ${source_file}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_source_files_properties(${generated_file} PROPERTIES GENERATED True)
endfunction(add_test_flex_raw)

########################################
# FUNCTION add_test_flex
########################################

function(add_test_flex name lang out_source_filename)
    set(generated_file ${CMAKE_CURRENT_BINARY_DIR}/${name}.${lang})
    add_test_flex_raw(${name} ${generated_file} ${CMAKE_CURRENT_SOURCE_DIR}/${name} "")
    set(${out_source_filename} ${generated_file} PARENT_SCOPE)
endfunction(add_test_flex)

########################################
# FUNCTION add_test_build_raw
########################################

function(add_test_build_raw test_name test_files definitions compile_options link_libraries)
    # add test target
    add_executable(${test_name} ${test_files})
    target_link_libraries(${test_name} ${link_libraries})
    set_target_properties(${test_name} PROPERTIES EXCLUDE_FROM_ALL True)
    set_target_properties(${test_name} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD True)
    test_project_group(${test_name})

    foreach(D ${definitions})
        target_compile_definitions(${test_name} PRIVATE ${D})
    endforeach()

    # build test
    add_test(NAME ${test_name}${build_suffix}
        COMMAND ${CMAKE_COMMAND}
            --build ${CMAKE_BINARY_DIR}
            --target ${test_name}
            --config $<CONFIGURATION>
    )
    set_tests_properties(${test_name}${build_suffix} PROPERTIES DEPENDS ${test_name}${generate_suffix})
endfunction(add_test_build_raw)

########################################
# FUNCTION add_test_flex_build
########################################

function(add_test_flex_build name lang)
    list(GET name 0 test_name)

    string(REGEX MATCH "^(.*)\\.[^.]*$" dummy ${test_name})
    set(test_name ${CMAKE_MATCH_1})

    set(test_name ${test_name} PARENT_SCOPE)

    set(test_files)
    foreach(N ${name})
        # generate test
        add_test_flex(${N} ${lang} out)
        set(test_files "${N};${test_files};${out}")
    endforeach()
    
    add_test_build_raw(${test_name} "${test_files};${ARGN}" "" "" "")
endfunction(add_test_flex_build)

########################################
# FUNCTION add_test_flex_build_run_with_default_input
########################################

function(add_test_flex_build_run_with_default_input name lang)
    add_test_flex_build(${name} ${lang} ${ARGN})

    # run test
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${test_name}.txt)
        add_test_run_with_input(${test_name} ${test_name}.txt)
    else()
        message(FATAL_ERROR "input file is missing: ${test_name}.txt")
    endif()
endfunction(add_test_flex_build_run_with_default_input)

########################################
# FUNCTION add_test_flex_build_run
########################################

function(add_test_flex_build_run name lang)
    add_test_flex_build("${name}" ${lang} ${ARGN})
    add_test_run(${test_name} "")
endfunction(add_test_flex_build_run)

########################################
# FUNCTION add_test_flex_build_run_with_custom_input
########################################

function(add_test_flex_build_run_with_custom_input name lang build_files input_file)
    add_test_flex_build("${name}" ${lang} "${build_files}")
    add_test_run_with_input(${test_name} ${input_file})
endfunction(add_test_flex_build_run_with_custom_input)

########################################
# FUNCTION add_bison_target
########################################

function(add_bison_target name)
    bison_target(${name} "${name}_parser.y"
        "${CMAKE_CURRENT_BINARY_DIR}/${name}_parser.c"
        COMPILE_FLAGS "-d -p test --defines=${CMAKE_CURRENT_BINARY_DIR}/${name}_parser.h"
    )
    set(BISON_${name}_OUTPUTS ${BISON_${name}_OUTPUTS} PARENT_SCOPE)
endfunction(add_bison_target)

########################################
# FUNCTION add_test_compare
########################################

function(add_test_compare name lang)
    add_test_flex_build(${name} ${lang} ${ARGN})

    add_test(NAME ${test_name}${run_suffix}_1
        COMMAND ${CMAKE_COMMAND}
            -DEXE=$<TARGET_FILE:${test_name}>
            -DTXT=${CMAKE_CURRENT_SOURCE_DIR}/${test_name}.txt
            -DOUT=${CMAKE_CURRENT_BINARY_DIR}/${test_name}.txt.1
            -P ${CMAKE_CURRENT_SOURCE_DIR}/test.cmake
    )
    add_test(NAME ${test_name}${run_suffix}_2
        COMMAND ${CMAKE_COMMAND}
            -DEXE=$<TARGET_FILE:${test_name}>
            -DARGS=1
            -DTXT=${CMAKE_CURRENT_SOURCE_DIR}/${test_name}.txt
            -DOUT=${CMAKE_CURRENT_BINARY_DIR}/${test_name}.txt.2
            -P ${CMAKE_CURRENT_SOURCE_DIR}/test.cmake
    )
    add_test(NAME ${test_name}${run_suffix}_compare
        COMMAND ${CMAKE_COMMAND} -E compare_files
            ${CMAKE_CURRENT_BINARY_DIR}/${test_name}.txt.1
            ${CMAKE_CURRENT_BINARY_DIR}/${test_name}.txt.2
    )
endfunction(add_test_compare)

########################################
# EXECUTABLE simple_m4_test
########################################

add_executable(simple_m4_test simple_m4_test.cpp)
target_link_libraries(simple_m4_test simple_m4)
set_target_properties(simple_m4_test PROPERTIES EXCLUDE_FROM_ALL True)
set_target_properties(simple_m4_test PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD True)
test_project_group(simple_m4_test)

################################################################################
# TESTS
################################################################################

########################################
# TESTS simple m4
########################################

test_simple_m4(1 13)

########################################
# TESTS flex simple
########################################

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
	prefix_nr.l
	prefix_r.l
	quotes.l
	yyextra.l
)

foreach(T ${flex_tests_simple})
    add_test_flex_build_run_with_default_input(${T} c)
endforeach()

add_test_flex_build_run(posix.l c)
add_test_flex_build_run(posixly_correct.l c)
add_test_flex_build_run(string_nr.l c)
add_test_flex_build_run(string_r.l c)

add_test_flex_build_run_with_default_input(cxx_basic.ll cc)
add_test_flex_build_run_with_custom_input("cxx_multiple_scanners_1.ll;cxx_multiple_scanners_2.ll" cc cxx_multiple_scanners_main.cc cxx_multiple_scanners.txt)
add_test_flex_build_run_with_custom_input(header_nr_scanner.l c header_nr_main.c header_nr.txt)
add_test_flex_build_run_with_custom_input(header_r_scanner.l c header_r_main.c header_nr.txt)
add_test_flex_build_run("multiple_scanners_nr_1.l;multiple_scanners_nr_2.l" c multiple_scanners_nr_main.c)
add_test_flex_build_run("multiple_scanners_r_1.l;multiple_scanners_r_2.l" c multiple_scanners_r_main.c)
add_test_flex_build_run_with_default_input(top.l c top_main.c)

add_bison_target(bison_nr)
add_test_flex_build(bison_nr_scanner.l c
    bison_nr_main.c
    ${BISON_bison_nr_OUTPUTS}
    bison_nr_parser.y
)
add_test_run_with_input(bison_nr_scanner bison_nr.txt)

add_bison_target(bison_yylloc)
add_test_flex_build(bison_yylloc_scanner.l c
    bison_yylloc_main.c
    ${BISON_bison_yylloc_OUTPUTS}
    bison_yylloc_parser.y
)
add_test_run_with_input(bison_yylloc_scanner bison_yylloc.txt)

add_bison_target(bison_yylval)
add_test_flex_build(bison_yylval_scanner.l c
    bison_yylval_main.c
    ${BISON_bison_yylval_OUTPUTS}
    bison_yylval_parser.y
)
add_test_run_with_input(bison_yylval_scanner bison_yylval.txt)

########################################

# reject
add_test_flex_raw(reject_nr
    ${CMAKE_CURRENT_BINARY_DIR}/reject_nr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/reject.l4 "")
add_test_build_raw(reject_nr "${CMAKE_CURRENT_BINARY_DIR}/reject_nr.c;${CMAKE_CURRENT_SOURCE_DIR}/reject.l4" "" "" "")
add_test_run_with_input(reject_nr reject.txt)

add_test_flex_raw(reject_r
    ${CMAKE_CURRENT_BINARY_DIR}/reject_r.c
    ${CMAKE_CURRENT_SOURCE_DIR}/reject.l4 "--reentrant")
add_test_build_raw(reject_r "${CMAKE_CURRENT_BINARY_DIR}/reject_r.c;${CMAKE_CURRENT_SOURCE_DIR}/reject.l4" "-DTEST_IS_REENTRANT" "" "")
add_test_run_with_input(reject_r reject.txt)

# table
add_test_flex_raw(reject_ser
    ${CMAKE_CURRENT_BINARY_DIR}/reject_ser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/reject.l4 "--tables-file=reject_ser.tables")
add_test_build_raw(reject_ser "${CMAKE_CURRENT_BINARY_DIR}/reject_ser.c;${CMAKE_CURRENT_SOURCE_DIR}/reject.l4"
    "-DTEST_HAS_TABLES_EXTERNAL" "" "${winsock_lib}")
add_test_run(reject_ser "" ${CMAKE_CURRENT_BINARY_DIR}/reject_ser.tables ${CMAKE_CURRENT_SOURCE_DIR}/reject.txt)

add_test_flex_raw(reject_ver
    ${CMAKE_CURRENT_BINARY_DIR}/reject_ver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/reject.l4 "--tables-verify;--tables-file=reject_ver.tables")
add_test_build_raw(reject_ver "${CMAKE_CURRENT_BINARY_DIR}/reject_ver.c;${CMAKE_CURRENT_SOURCE_DIR}/reject.l4"
    "-DTEST_HAS_TABLES_EXTERNAL" "" "${winsock_lib}")
add_test_run(reject_ver "" ${CMAKE_CURRENT_BINARY_DIR}/reject_ver.tables ${CMAKE_CURRENT_SOURCE_DIR}/reject.txt)

########################################

# direct
add_test_flex_build(include_by_buffer.direct.l c)
add_test_run(include_by_buffer.direct ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include_by_buffer.direct.txt)

add_test_flex_build(include_by_push.direct.l c)
add_test_run(include_by_push.direct ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include_by_push.direct.txt)

add_test_flex_build(include_by_reentrant.direct.l c)
add_test_run(include_by_reentrant.direct ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include_by_reentrant.direct.txt)

add_test_flex_build(rescan_nr.direct.l c)
add_test_run(rescan_nr.direct "" ${CMAKE_CURRENT_SOURCE_DIR}/rescan_nr.direct.txt)

add_test_flex_build(rescan_r.direct.l c)
add_test_run(rescan_r.direct "" ${CMAKE_CURRENT_SOURCE_DIR}/rescan_r.direct.txt)

# options_cn
add_test(NAME options_cn COMMAND ${CMAKE_COMMAND} -DEXE=$<TARGET_FILE:flex> -P ${CMAKE_CURRENT_SOURCE_DIR}/options.cmake)

# i3
add_test_flex_build(cxx_yywrap.ll cc)
add_test_run(cxx_yywrap "${CMAKE_CURRENT_SOURCE_DIR}" cxx_yywrap.txt cxx_yywrap.txt cxx_yywrap.txt)

# one
add_test_compare(lineno_nr.l c)
add_test_compare(lineno_r.l c)
add_test_compare(lineno_trailing.l c)
    
# pthread
if (NOT MSVC)
    add_test_flex_raw(pthreadf
        ${CMAKE_CURRENT_BINARY_DIR}/pthread.c
        ${CMAKE_CURRENT_SOURCE_DIR}/pthread.l "")
    add_test_build_raw(pthreadf "${CMAKE_CURRENT_BINARY_DIR}/pthread.c;${CMAKE_CURRENT_SOURCE_DIR}/pthread.l"
        "-DHAVE_PTHREAD_H" "" "pthread")
    add_test_run(pthreadf "${CMAKE_CURRENT_SOURCE_DIR}" pthread_1.txt pthread_2.txt pthread_3.txt pthread_4.txt pthread_5.txt)
endif()

########################################

foreach(kind opt ser ver)
    foreach(threading r nr)
        foreach(opt -Ca -Ce -Cf -CF -Cm -Cem -Cae -Caef -CaeF -Cam -Caem)
            set(name_opt ${opt})
            if (${opt} STREQUAL -CF OR
                ${opt} STREQUAL -CaeF)
                set(name_opt ${name_opt}_)
            endif()

            set(name tableopts_${kind}_${threading}_${name_opt})
            string(REPLACE "-" "" name ${name})
            set(l_file ${CMAKE_CURRENT_SOURCE_DIR}/tableopts.l4)
            set(c_file ${CMAKE_CURRENT_BINARY_DIR}/${name}.c)
            
            set(libs)
            set(tables)
            set(tables_def)
            if (${kind} STREQUAL ser OR ${kind} STREQUAL ver)
                set(tables --tables-file=${CMAKE_CURRENT_BINARY_DIR}/${name}.tables) # need quotes?
                set(tables_def -DTEST_HAS_TABLES_EXTERNAL)
                set(libs ${winsock_lib})
            endif()

            set(verify)
            if (${kind} STREQUAL ver)
                set(verify --tables-verify)
            endif()

            set(reentrant)
            set(reentrant_def)
            if (${threading} STREQUAL r)
                set(reentrant --reentrant)
                set(reentrant_def -DTEST_IS_REENTRANT)
            endif()

            string(REPLACE "-" "_" prefix ${opt})

            add_test_flex_raw(${name} ${c_file} ${l_file} "${verify};${reentrant};${tables};${opt};-P;${prefix}")
            add_test_build_raw(${name} "${c_file};${l_file}" "${reentrant_def};${tables_def}" "" "${libs}")
            test_depends(${name}${build_suffix} ${name}${generate_suffix})
            
            if (${kind} STREQUAL opt)
                add_test_run_with_input(${name} tableopts.txt)
            else()
                add_test(NAME ${name}${run_suffix}
                    COMMAND ${CMAKE_COMMAND}
                        -DEXE=$<TARGET_FILE:${name}>
                        -DARGS=${CMAKE_CURRENT_BINARY_DIR}/${name}.tables
                        -DTXT=${CMAKE_CURRENT_SOURCE_DIR}/tableopts.txt
                        -P ${CMAKE_CURRENT_SOURCE_DIR}/test.cmake
                )
                test_depends(${name}${run_suffix} ${name}${build_suffix})
            endif()
        endforeach()
    endforeach()
endforeach()

################################################################################
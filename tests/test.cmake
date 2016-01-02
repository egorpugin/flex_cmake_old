#
# flex
#

execute_process(
    COMMAND "${EXE}"
    INPUT_FILE "${TXT}"
    RESULT_VARIABLE r
)

if (NOT ${r} EQUAL 0)
    message(FATAL_ERROR "Error: ${r}")
endif()

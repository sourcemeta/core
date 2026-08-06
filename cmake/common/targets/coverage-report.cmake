# Script executed by the `coverage` target after the test suite runs, as
# the raw profile files are only known at that point
file(GLOB COVERAGE_PROFILE_FILES "${COVERAGE_PROFILE_DIRECTORY}/*.profraw")
if(NOT COVERAGE_PROFILE_FILES)
  message(FATAL_ERROR "No profile data found in ${COVERAGE_PROFILE_DIRECTORY}")
endif()

set(COVERAGE_PROFILE_DATA "${COVERAGE_OUTPUT_DIRECTORY}/coverage.profdata")
execute_process(COMMAND "${COVERAGE_LLVM_PROFDATA}" merge -sparse
  ${COVERAGE_PROFILE_FILES} -o "${COVERAGE_PROFILE_DATA}"
  COMMAND_ERROR_IS_FATAL ANY)

string(REPLACE "|" ";" COVERAGE_OBJECTS "${COVERAGE_OBJECTS}")
list(POP_FRONT COVERAGE_OBJECTS COVERAGE_MAIN_OBJECT)
set(COVERAGE_OBJECT_ARGUMENTS)
foreach(coverage_object IN LISTS COVERAGE_OBJECTS)
  list(APPEND COVERAGE_OBJECT_ARGUMENTS -object "${coverage_object}")
endforeach()

execute_process(COMMAND "${COVERAGE_LLVM_COV}" show
  "${COVERAGE_MAIN_OBJECT}" ${COVERAGE_OBJECT_ARGUMENTS}
  "-instr-profile=${COVERAGE_PROFILE_DATA}"
  -format=html "-output-dir=${COVERAGE_OUTPUT_DIRECTORY}/html"
  "-ignore-filename-regex=${COVERAGE_EXCLUDE}"
  -show-branches=count
  COMMAND_ERROR_IS_FATAL ANY)

message(STATUS "Coverage report: ${COVERAGE_OUTPUT_DIRECTORY}/html/index.html")

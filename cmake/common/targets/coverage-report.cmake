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

# Passing every binary to a single `llvm-cov` invocation misattributes
# header-inline and constexpr functions: when the compiler constant folds or
# re-hashes a definition in one translation unit, the shared record can shadow
# the executed one and report covered code as untouched. Exporting one LCOV
# trace per binary and keeping the highest execution count per line sidesteps
# the collision entirely, so a line counts as covered when any binary truly
# ran it
set(COVERAGE_LCOV_DIRECTORY "${COVERAGE_OUTPUT_DIRECTORY}/lcov")
file(REMOVE_RECURSE "${COVERAGE_LCOV_DIRECTORY}")
file(MAKE_DIRECTORY "${COVERAGE_LCOV_DIRECTORY}")
set(COVERAGE_TRACE_INDEX 0)
foreach(coverage_object IN LISTS COVERAGE_OBJECTS)
  execute_process(COMMAND "${COVERAGE_LLVM_COV}" export
    "${coverage_object}"
    "-instr-profile=${COVERAGE_PROFILE_DATA}"
    -format=lcov
    "-ignore-filename-regex=${COVERAGE_EXCLUDE}"
    OUTPUT_FILE "${COVERAGE_LCOV_DIRECTORY}/${COVERAGE_TRACE_INDEX}.lcov"
    COMMAND_ERROR_IS_FATAL ANY)
  math(EXPR COVERAGE_TRACE_INDEX "${COVERAGE_TRACE_INDEX} + 1")
endforeach()

# Merge the traces line by line, keeping the highest count observed for every
# line and branch, then emit a merged LCOV trace plus a per file summary
set(COVERAGE_MERGE_PROGRAM "${COVERAGE_OUTPUT_DIRECTORY}/merge.awk")
file(WRITE "${COVERAGE_MERGE_PROGRAM}" [==[
/^SF:/ { source = substr($0, 4); files[source] = 1; next }
/^DA:/ {
  split(substr($0, 4), record, ",")
  key = source SUBSEP record[1]
  if (!(key in lines) || record[2] + 0 > lines[key] + 0) {
    lines[key] = record[2] + 0
  }
  next
}
/^BRDA:/ {
  split(substr($0, 6), record, ",")
  key = source SUBSEP record[1] SUBSEP record[2] SUBSEP record[3]
  count = record[4] == "-" ? 0 : record[4] + 0
  if (!(key in branches) || count > branches[key] + 0) {
    branches[key] = count
  }
  next
}
END {
  total_lines = 0
  total_covered = 0
  total_branches = 0
  total_branches_covered = 0
  for (key in lines) {
    split(key, parts, SUBSEP)
    file_lines[parts[1]] += 1
    total_lines += 1
    if (lines[key] > 0) {
      file_covered[parts[1]] += 1
      total_covered += 1
    }
  }
  for (key in branches) {
    split(key, parts, SUBSEP)
    file_branches[parts[1]] += 1
    total_branches += 1
    if (branches[key] > 0) {
      file_branches_covered[parts[1]] += 1
      total_branches_covered += 1
    }
  }
  for (source in files) {
    printf "SF:%s\n", source > merged
    for (key in lines) {
      split(key, parts, SUBSEP)
      if (parts[1] == source) {
        printf "DA:%s,%s\n", parts[2], lines[key] > merged
      }
    }
    printf "end_of_record\n" > merged
    covered = file_covered[source] + 0
    percentage = file_lines[source] > 0 \
      ? (covered * 100.0) / file_lines[source] : 100
    printf "%8.2f%% %6d/%-6d %s\n", percentage, covered,
      file_lines[source], source | "sort -k4"
  }
  close("sort -k4")
  line_percentage = total_lines > 0 \
    ? (total_covered * 100.0) / total_lines : 100
  branch_percentage = total_branches > 0 \
    ? (total_branches_covered * 100.0) / total_branches : 100
  printf "%8.2f%% %6d/%-6d TOTAL lines\n", line_percentage,
    total_covered, total_lines
  printf "%8.2f%% %6d/%-6d TOTAL branches\n", branch_percentage,
    total_branches_covered, total_branches
}
]==])

file(GLOB COVERAGE_TRACES "${COVERAGE_LCOV_DIRECTORY}/*.lcov")
execute_process(COMMAND awk
  -v "merged=${COVERAGE_OUTPUT_DIRECTORY}/coverage.lcov"
  -f "${COVERAGE_MERGE_PROGRAM}" ${COVERAGE_TRACES}
  OUTPUT_FILE "${COVERAGE_OUTPUT_DIRECTORY}/summary.txt"
  COMMAND_ERROR_IS_FATAL ANY)

# The browsable report keeps the combined view. Its annotated sources can
# still under count the header-inline cases described above, so the summary
# file carries the authoritative numbers
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

file(STRINGS "${COVERAGE_OUTPUT_DIRECTORY}/summary.txt" COVERAGE_TOTALS
  REGEX "TOTAL")
foreach(coverage_total IN LISTS COVERAGE_TOTALS)
  message(STATUS "Coverage:${coverage_total}")
endforeach()
message(STATUS "Coverage summary: ${COVERAGE_OUTPUT_DIRECTORY}/summary.txt")
message(STATUS "Coverage trace: ${COVERAGE_OUTPUT_DIRECTORY}/coverage.lcov")
message(STATUS "Coverage report: ${COVERAGE_OUTPUT_DIRECTORY}/html/index.html")

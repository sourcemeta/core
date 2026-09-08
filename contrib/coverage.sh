#!/bin/sh

# Produces an LLVM source based code coverage report for the project
# See https://clang.llvm.org/docs/SourceBasedCodeCoverage.html
#
# The build tree lives inside the given output directory, so pointing this
# script at a scratch location makes the whole run disposable, while pointing
# it at the same location twice reuses the previous build
#
# Instrumentation is observable from inside the suite, as the profile runtime
# adds a variable to the environment of every child process and writes a
# profile into the working directory of a program that was started without one.
# Suites that assert on either of those will fail here while passing in an
# ordinary build, so a failing suite still produces a report

set -o errexit
set -o nounset

SOURCE_DIRECTORY="$(cd "$(dirname "$0")/.." && pwd)"
mkdir -p "${1:-$SOURCE_DIRECTORY/build/coverage}"
OUTPUT_DIRECTORY="$(cd "${1:-$SOURCE_DIRECTORY/build/coverage}" && pwd)"
BUILD_DIRECTORY="$OUTPUT_DIRECTORY/build"

# On Apple platforms, the LLVM tools that understand the profile format
# emitted by the system compiler are only reachable through Xcode
if [ "$(uname)" = "Darwin" ]
then
  LLVM_PROFDATA="$(xcrun --find llvm-profdata)"
  LLVM_COV="$(xcrun --find llvm-cov)"
else
  LLVM_PROFDATA="$(command -v llvm-profdata)"
  LLVM_COV="$(command -v llvm-cov)"
fi

# Spelled out rather than left to the build tool, as the Makefile generator
# reads a bare parallel option as a licence for unbounded parallelism
if command -v nproc > /dev/null 2>&1
then
  JOBS="$(nproc)"
else
  JOBS="$(sysctl -n hw.ncpu)"
fi

# Instrumentation is injected through the standard CMake flag variables so that
# the project build system does not need to know about coverage at all. Static
# linking keeps every library under measurement inside the test binaries
cmake -S "$SOURCE_DIRECTORY" -B "$BUILD_DIRECTORY" \
  -DCMAKE_BUILD_TYPE:STRING=Debug \
  -DCMAKE_COMPILE_WARNING_AS_ERROR:BOOL=ON \
  -DSOURCEMETA_CORE_TESTS:BOOL=ON \
  -DBUILD_SHARED_LIBS:BOOL=OFF \
  -DCMAKE_C_FLAGS:STRING="-fprofile-instr-generate -fcoverage-mapping" \
  -DCMAKE_CXX_FLAGS:STRING="-fprofile-instr-generate -fcoverage-mapping" \
  -DCMAKE_EXE_LINKER_FLAGS:STRING="-fprofile-instr-generate" \
  -DCMAKE_SHARED_LINKER_FLAGS:STRING="-fprofile-instr-generate"

cmake --build "$BUILD_DIRECTORY" --config Debug --parallel "$JOBS"

PROFILE_DIRECTORY="$OUTPUT_DIRECTORY/profile"
rm -rf "$PROFILE_DIRECTORY"
mkdir -p "$PROFILE_DIRECTORY"

if LLVM_PROFILE_FILE="$PROFILE_DIRECTORY/%p.profraw" \
  ctest --test-dir "$BUILD_DIRECTORY" --build-config Debug \
    --output-on-failure --parallel "$JOBS"
then
  TESTS_PASSED="yes"
else
  TESTS_PASSED="no"
fi

PROFILE_DATA="$OUTPUT_DIRECTORY/coverage.profdata"
"$LLVM_PROFDATA" merge -sparse -o "$PROFILE_DATA" "$PROFILE_DIRECTORY"/*.profraw

# CTest already knows every binary the suite runs, including the ones that the
# shell script based tests take as arguments, which removes the need for the
# build system to keep a registry of test targets
OBJECT_LIST="$OUTPUT_DIRECTORY/objects.txt"
: > "$OBJECT_LIST"
ctest --test-dir "$BUILD_DIRECTORY" --show-only=json-v1 | awk '
/"command" : *$/ { collecting = 1; next }
collecting && /^[[:space:]]*\[[[:space:]]*$/ { next }
collecting && /^[[:space:]]*\],?[[:space:]]*$/ { collecting = 0; next }
collecting && match($0, /"[^"]*"/) {
  print substr($0, RSTART + 1, RLENGTH - 2)
}
' | sort -u | while IFS= read -r CANDIDATE
do
  case "$CANDIDATE" in
    "$BUILD_DIRECTORY"/*) ;;
    *) continue ;;
  esac

  if [ -f "$CANDIDATE" ] && [ -x "$CANDIDATE" ]
  then
    echo "$CANDIDATE" >> "$OBJECT_LIST"
  fi
done

if [ ! -s "$OBJECT_LIST" ]
then
  echo "No instrumented test binaries found in $BUILD_DIRECTORY" >&2
  exit 1
fi

EXCLUDE="$BUILD_DIRECTORY|$SOURCE_DIRECTORY/vendor|$SOURCE_DIRECTORY/test"

# Passing every binary to a single report invocation misattributes header
# inline and constexpr functions: when the compiler constant folds or re-hashes
# a definition in one translation unit, the shared record can shadow the
# executed one and report covered code as untouched. Exporting one LCOV trace
# per binary and keeping the highest execution count per line sidesteps the
# collision entirely, so a line counts as covered when any binary truly ran it
LCOV_DIRECTORY="$OUTPUT_DIRECTORY/lcov"
rm -rf "$LCOV_DIRECTORY"
mkdir -p "$LCOV_DIRECTORY"
TRACE_INDEX=0
while IFS= read -r OBJECT
do
  "$LLVM_COV" export "$OBJECT" \
    "-instr-profile=$PROFILE_DATA" \
    -format=lcov \
    "-ignore-filename-regex=$EXCLUDE" > "$LCOV_DIRECTORY/$TRACE_INDEX.lcov"
  TRACE_INDEX=$((TRACE_INDEX + 1))
done < "$OBJECT_LIST"

# Merge the traces line by line, keeping the highest count observed for every
# line and branch, then emit a merged LCOV trace plus a per file summary
MERGE_PROGRAM="$OUTPUT_DIRECTORY/merge.awk"
cat > "$MERGE_PROGRAM" <<'AWK'
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
AWK

awk -v "merged=$OUTPUT_DIRECTORY/coverage.lcov" -f "$MERGE_PROGRAM" \
  "$LCOV_DIRECTORY"/*.lcov > "$OUTPUT_DIRECTORY/summary.txt"

# The browsable report keeps the combined view. Its annotated sources can still
# under count the header inline cases described above, so the summary file
# carries the authoritative numbers
MAIN_OBJECT="$(head -n 1 "$OBJECT_LIST")"
set --
while IFS= read -r OBJECT
do
  if [ "$OBJECT" != "$MAIN_OBJECT" ]
  then
    set -- "$@" -object "$OBJECT"
  fi
done < "$OBJECT_LIST"

"$LLVM_COV" show "$MAIN_OBJECT" "$@" \
  "-instr-profile=$PROFILE_DATA" \
  -format=html "-output-dir=$OUTPUT_DIRECTORY/html" \
  "-ignore-filename-regex=$EXCLUDE" \
  -show-branches=count

grep TOTAL "$OUTPUT_DIRECTORY/summary.txt"
echo "Coverage summary: $OUTPUT_DIRECTORY/summary.txt"
echo "Coverage trace: $OUTPUT_DIRECTORY/coverage.lcov"
echo "Coverage report: $OUTPUT_DIRECTORY/html/index.html"

if [ "$TESTS_PASSED" = "no" ]
then
  echo "Some tests failed under instrumentation. See the CTest output above" >&2
fi

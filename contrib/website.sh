#!/bin/sh

# Assembles the published website out of a single build tree: the API
# reference, plus the LLVM source based coverage report that sits alongside it
# See https://clang.llvm.org/docs/SourceBasedCodeCoverage.html
#
# Takes the directory to build in. The site is left in a subdirectory of it,
# which is where the reference generator writes on its own. Naming the same
# build directory twice reuses the previous build, while naming a scratch one
# makes the whole run disposable
#
# Instrumentation is observable from inside the suite, as the profile runtime
# adds a variable to the environment of every child process and writes a
# profile into the working directory of a program that was started without one.
# The suite accounts for both, so a failure here is a real one

set -o errexit
set -o nounset

if [ "$#" -ne 1 ]
then
  echo "Usage: $0 <build-directory>" >&2
  exit 1
fi

SOURCE_DIRECTORY="$(cd "$(dirname "$0")/.." && pwd)"
mkdir -p "$1"
BUILD_DIRECTORY="$(cd "$1" && pwd)"
SITE_DIRECTORY="$BUILD_DIRECTORY/website"
REPORT_DIRECTORY="$SITE_DIRECTORY/coverage"

# Everything the report is derived from stays next to the build it came from,
# so that the published pages sit on their own
WORK_DIRECTORY="$BUILD_DIRECTORY/coverage"
mkdir -p "$REPORT_DIRECTORY" "$WORK_DIRECTORY"

# The kind of coverage this produces is an LLVM feature, so the toolchain is
# not a choice the caller gets to make
CC=clang
CXX=clang++
export CC
export CXX

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

# A count is spelled out rather than left to the build tool, as the Makefile
# generator reads a bare parallel option as a licence for unbounded
# parallelism. These are the documented way of asking for it, so a caller that
# already exports either one keeps the last word
if command -v nproc > /dev/null 2>&1
then
  JOBS="$(nproc)"
else
  JOBS="$(sysctl -n hw.ncpu)"
fi

CMAKE_BUILD_PARALLEL_LEVEL="${CMAKE_BUILD_PARALLEL_LEVEL:-$JOBS}"
CTEST_PARALLEL_LEVEL="${CTEST_PARALLEL_LEVEL:-$JOBS}"
export CMAKE_BUILD_PARALLEL_LEVEL
export CTEST_PARALLEL_LEVEL

# Counters are written out when each program exits. Keeping them up to date as
# they change instead, so that a program dying on a fatal signal still reports
# what it ran, costs the compiler cache entirely, as the option that arranges it
# is one the cache does not recognise, and it refuses every compilation carrying
# it. Asking for it through the profile file name alone is not an alternative,
# as a program built without the option then fails to honour the request and
# says so on its error stream, which the tests that compare output then read
PROFILE_FLAGS="-fprofile-instr-generate -fcoverage-mapping"

# Assertions are compiled out, which is a measurement decision rather than an
# instrumentation one. The branch an assertion takes when it fails is
# unreachable for as long as the suite passes, so each of the many in this
# project would otherwise sit in the report as a region nothing reaches, and
# adding a precondition check to a function would lower what it scores. Nothing
# here depends on one firing, and the release build already compiles this same
# code with them out, so neither their conditions nor anything they name can be
# load bearing
ASSERTION_FLAGS="-DNDEBUG"

# Instrumentation is injected through the standard CMake flag variables so that
# the project build system does not need to know about coverage at all. Static
# linking keeps every library under measurement inside the test binaries. The
# reference is read out of the sources rather than out of anything the build
# produces, so one tree can carry both
cmake -S "$SOURCE_DIRECTORY" -B "$BUILD_DIRECTORY" \
  -DCMAKE_BUILD_TYPE:STRING=Debug \
  -DCMAKE_COMPILE_WARNING_AS_ERROR:BOOL=ON \
  -DSOURCEMETA_CORE_TESTS:BOOL=ON \
  -DSOURCEMETA_CORE_DOCS:BOOL=ON \
  -DSOURCEMETA_CORE_CRYPTO_USE_REFERENCE:BOOL=ON \
  -DBUILD_SHARED_LIBS:BOOL=OFF \
  -DCMAKE_C_FLAGS:STRING="$PROFILE_FLAGS $ASSERTION_FLAGS" \
  -DCMAKE_CXX_FLAGS:STRING="$PROFILE_FLAGS $ASSERTION_FLAGS" \
  -DCMAKE_EXE_LINKER_FLAGS:STRING="-fprofile-instr-generate" \
  -DCMAKE_SHARED_LINKER_FLAGS:STRING="-fprofile-instr-generate"

cmake --build "$BUILD_DIRECTORY" --config Debug

PROFILE_DIRECTORY="$WORK_DIRECTORY/profile"
rm -rf "$PROFILE_DIRECTORY"
mkdir -p "$PROFILE_DIRECTORY"

# The packaging tests drive a separate build of a consuming project, which
# carries no instrumentation and contributes no coverage, and which expects an
# installation that this script has no reason to produce
LLVM_PROFILE_FILE="$PROFILE_DIRECTORY/%p.profraw" \
  ctest --test-dir "$BUILD_DIRECTORY" --build-config Debug \
    --output-on-failure --exclude-regex find_package

PROFILE_DATA="$WORK_DIRECTORY/coverage.profdata"
"$LLVM_PROFDATA" merge -sparse -o "$PROFILE_DATA" "$PROFILE_DIRECTORY"/*.profraw

# CTest already knows every binary the suite runs, including the ones that the
# shell script based tests take as arguments, which removes the need for the
# build system to keep a registry of test targets
OBJECT_LIST="$WORK_DIRECTORY/objects.txt"
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
LCOV_DIRECTORY="$WORK_DIRECTORY/lcov"
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

# All four metrics are counted in one place, from one set of exceptions. The
# traces carry the report's own verdict on each line and branch, and the JSON
# export carries the functions and regions with their columns, which is how the
# report itself counts every instantiation of a template as a single function.
# The highest count across the binaries is kept for the same reason as above
EXPORT_PROGRAM="$WORK_DIRECTORY/export.py"
cat > "$EXPORT_PROGRAM" <<'PYTHON'
import json
import os
import re
import subprocess
import sys
from collections import defaultdict

llvm_cov, profile_data, exclude, object_list, uncovered, exceptions, \
    traces, merged = sys.argv[1:]
excluded = re.compile(exclude)

# Functions the suite does exercise but cannot measure, each paired with the
# file it lives in, so that an entry cannot silently start excusing a function
# of the same name elsewhere
permitted = set()
with open(exceptions, encoding="utf-8") as listing:
    for entry in listing:
        text = entry.split("#", 1)[0].strip()
        if text:
            suffix, _, name = text.partition(" ")
            permitted.add((suffix, name.strip()))

# Lines and branches are read from the traces rather than derived from the
# regions below. A line counts as covered when the report says it does, and the
# report settles that from segments that carry which part of a line is code,
# which the regions do not: taking a region to cover every line it spans counts
# blank and declaration lines as code and reports a line as run whenever any
# region spanning it ran. Keeping the highest count seen for each line is what
# makes one count as covered when any binary truly ran it
lines = defaultdict(dict)
branches = defaultdict(dict)
for entry in sorted(os.listdir(traces)):
    source = None
    with open(os.path.join(traces, entry), encoding="utf-8") as trace:
        for row in trace:
            if row.startswith("SF:"):
                source = row[3:].strip()
                lines.setdefault(source, {})
            elif row.startswith("DA:"):
                number, _, count = row[3:].strip().partition(",")
                held = lines[source]
                number = int(number)
                held[number] = max(held.get(number, 0), int(count))
            elif row.startswith("BRDA:"):
                number, block, edge, count = row[5:].strip().split(",")
                held = branches[source]
                key = (int(number), block, edge)
                taken = 0 if count == "-" else int(count)
                held[key] = max(held.get(key, 0), taken)

# The eighth element of a region says what it is, and only a code region counts
# towards region coverage, which the report agrees with file by file. The sixth
# names which of the function's files it belongs to, since a region can sit in a
# header the function was expanded from
CODE_REGION = 0
REGION_FILE = 5
REGION_KIND = 7

counts = {}
names = {}
owned = {}
spans = {}
with open(object_list, encoding="utf-8") as objects:
    for binary in objects.read().splitlines():
        export = subprocess.run(
            [llvm_cov, "export", binary, f"-instr-profile={profile_data}",
             "-format=text", "-skip-expansions",
             f"-ignore-filename-regex={exclude}"],
            check=True, stdout=subprocess.PIPE)
        for data in json.loads(export.stdout)["data"]:
            for function in data["functions"]:
                filename = function["filenames"][0]
                if excluded.search(filename):
                    continue
                start = function["regions"][0]
                key = (filename, start[0], start[1])
                counts[key] = max(counts.get(key, 0), function["count"])
                names.setdefault(key, set()).add(function["name"])
                for region in function["regions"]:
                    if region[REGION_KIND] != CODE_REGION:
                        continue
                    source = function["filenames"][region[REGION_FILE]]
                    if excluded.search(source):
                        continue
                    where = (source, region[0], region[1], region[2],
                             region[3])
                    mine = owned.setdefault(key, {})
                    mine[where] = max(mine.get(where, 0), region[4])
                    reach = spans.setdefault(key, set())
                    for number in range(region[0], region[2] + 1):
                        reach.add((source, number))

def excused(key):
    filename = key[0]
    return any(filename.endswith(suffix) and name in names[key]
               for suffix, name in permitted)


missed = sorted(key for key, count in counts.items()
                if count == 0 and not excused(key))
with open(uncovered, "w", encoding="utf-8") as output:
    for key in missed:
        filename, line, column = key
        for name in sorted(names[key]):
            output.write(f"{filename}:{line}:{column} {name}\n")

# An excused function is left out of the region, line and branch counts as
# well, so that the four metrics answer to the same exceptions rather than some
# of them holding a function to a standard the others have already set aside.
# Applied once the whole export has been read, since which names a function
# goes by is only settled then, and a place an excused function shares with one
# that is measurable stays in through the latter
regions = {}
forgiven = set()
measurable = set()
for key, mine in owned.items():
    if excused(key):
        forgiven.update(spans.get(key, ()))
        continue

    measurable.update(spans.get(key, ()))
    for where, count in mine.items():
        regions[where] = max(regions.get(where, 0), count)

forgiven -= measurable

# The merged trace the browsable report is built from carries every line the
# binaries reported, excused or not, since the report is there to be read rather
# than to be met
with open(merged, "w", encoding="utf-8") as output:
    for source in sorted(lines):
        output.write(f"SF:{source}\n")
        for number, count in sorted(lines[source].items()):
            output.write(f"DA:{number},{count}\n")
        for (number, block, edge), count in sorted(branches[source].items()):
            output.write(f"BRDA:{number},{block},{edge},{count}\n")
        output.write("end_of_record\n")

total_lines = 0
total_covered = 0
# A file every one of whose lines is excused stays on the list, reading as no
# lines out of no lines, because the list is the authoritative record and a file
# that quietly stopped appearing on it would be the hardest kind of change to
# notice
for source in sorted(lines):
    held = {number: count for number, count in lines[source].items()
            if (source, number) not in forgiven}
    covered = sum(1 for count in held.values() if count > 0)
    total_lines += len(held)
    total_covered += covered
    share = covered * 100 / len(held) if held else 100
    print(f"{share:8.2f}% {covered:6d}/{len(held):<6d} {source}")

share = total_covered * 100 / total_lines if total_lines else 100
print(f"{share:8.2f}% {total_covered:6d}/{total_lines:<6d} TOTAL lines")

taken = 0
total_branches = 0
for source in branches:
    for (number, block, edge), count in branches[source].items():
        if (source, number) in forgiven:
            continue

        total_branches += 1
        if count > 0:
            taken += 1

share = taken * 100 / total_branches if total_branches else 100
print(f"{share:8.2f}% {taken:6d}/{total_branches:<6d} TOTAL branches")

reached = sum(1 for count in regions.values() if count > 0)
share = reached * 100 / len(regions) if regions else 100
print(f"{share:8.2f}% {reached:6d}/{len(regions):<6d} TOTAL regions")

covered = len(counts) - len(missed)
percentage = covered * 100 / len(counts) if counts else 100
print(f"{percentage:8.2f}% {covered:6d}/{len(counts):<6d} TOTAL functions")
PYTHON

UNCOVERED_FUNCTIONS="$WORK_DIRECTORY/uncovered.txt"
python3 "$EXPORT_PROGRAM" "$LLVM_COV" "$PROFILE_DATA" "$EXCLUDE" \
  "$OBJECT_LIST" "$UNCOVERED_FUNCTIONS" \
  "$SOURCE_DIRECTORY/contrib/coverage-exceptions.txt" \
  "$LCOV_DIRECTORY" "$WORK_DIRECTORY/coverage.lcov" \
  > "$WORK_DIRECTORY/summary.txt"

# Optionally require every function under measurement to be reached by the
# suite. Only the platform that the report is published from is held to it, as
# a report produced elsewhere measures a different set of code
if [ -n "${REQUIRE_FULL_FUNCTION_COVERAGE:-}" ] && [ -s "$UNCOVERED_FUNCTIONS" ]
then
  echo "The test suite never calls the functions starting at:" >&2
  cat "$UNCOVERED_FUNCTIONS" >&2
  exit 1
fi

# Optionally hold the line and region shares to a floor, given as a percentage.
# Branches are deliberately not held to one: an exhaustive switch over an
# enumeration carries an edge out of it that nothing can take, so the branch
# share has a ceiling below a hundred that the other two do not. Gated on the
# same platform argument as the function requirement above
if [ -n "${REQUIRE_MINIMUM_COVERAGE:-}" ]
then
  awk -v "minimum=$REQUIRE_MINIMUM_COVERAGE" '
BEGIN { failed = 0 }
$3 == "TOTAL" && ($4 == "lines" || $4 == "regions") {
  # Compared as the counts behind the share rather than as the share itself,
  # which is already rounded to two places by the time it is printed and would
  # let a total just under the floor round up into passing
  split($2, tally, "/")
  if (100 * tally[1] < minimum * tally[2]) {
    printf "The %s coverage is %s, below the required %s%%\n", $4, $1,
      minimum > "/dev/stderr"
    failed = 1
  }
  seen += 1
}
END {
  if (seen != 2) {
    printf "Expected a line and a region total, found %d\n", seen \
      > "/dev/stderr"
    exit 1
  }

  exit failed
}
' "$WORK_DIRECTORY/summary.txt"
fi

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
  -format=html "-output-dir=$WORK_DIRECTORY/html" \
  "-ignore-filename-regex=$EXCLUDE" \
  -show-branches=count

# Whatever the report generator emitted is taken as is rather than named entry
# by entry, and only the entries about to be written are cleared, so that the
# destination is never removed wholesale
for ENTRY in "$WORK_DIRECTORY"/html/*
do
  TARGET="$REPORT_DIRECTORY/$(basename "$ENTRY")"
  rm -rf "$TARGET"
  cp -R "$ENTRY" "$TARGET"
done

# Runs last because the reference only adds to its output directory, whereas
# the coverage report replaces the tree it is given
cmake --build "$BUILD_DIRECTORY" --config Debug --target doxygen

# The pages a visitor lands on, confirmed rather than assumed, as each of the
# steps above reports success on its own terms without knowing what the ones
# after it expect to find
for PAGE in "$SITE_DIRECTORY/index.html" "$REPORT_DIRECTORY/index.html"
do
  if [ ! -f "$PAGE" ]
  then
    echo "Missing from the assembled website: $PAGE" >&2
    exit 1
  fi
done

grep TOTAL "$WORK_DIRECTORY/summary.txt"
echo "Coverage summary: $WORK_DIRECTORY/summary.txt"
echo "Coverage trace: $WORK_DIRECTORY/coverage.lcov"
echo "Website: $SITE_DIRECTORY/index.html"

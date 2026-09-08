#!/bin/sh

# Assembles the published website out of a single build tree: the API
# reference, plus the coverage report that sits alongside it
#
# Takes the directory to build in. The site is left in a subdirectory of it,
# which is where the reference generator writes on its own

set -o errexit
set -o nounset

if [ "$#" -ne 1 ]
then
  echo "Usage: $0 <build-directory>" >&2
  exit 1
fi

CONTRIB_DIRECTORY="$(cd "$(dirname "$0")" && pwd)"
mkdir -p "$1"
BUILD_DIRECTORY="$(cd "$1" && pwd)"
SITE_DIRECTORY="$BUILD_DIRECTORY/website"

# The reference is read out of the sources rather than out of anything the
# build produces, so one tree can carry both it and the instrumented binaries
# the coverage report is derived from
"$CONTRIB_DIRECTORY/coverage.sh" "$BUILD_DIRECTORY" "$SITE_DIRECTORY/coverage" \
  -DSOURCEMETA_CORE_DOCS:BOOL=ON

# Runs last because the reference only adds to its output directory, whereas
# the coverage report replaces the tree it is given
cmake --build "$BUILD_DIRECTORY" --config Debug --target doxygen

echo "Website: $SITE_DIRECTORY/index.html"

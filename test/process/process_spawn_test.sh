#!/bin/sh

set -o errexit
set -o nounset

PROCESS_SPAWN_MAIN="$1"

OUTPUT=$("$PROCESS_SPAWN_MAIN" /tmp /bin/sh -c 'echo "$(pwd)"' 2>&1)
EXIT_CODE=$?

if [ "$EXIT_CODE" -ne 0 ]
then
  echo "FAIL: Expected exit code 0, got $EXIT_CODE" >&2
  exit 1
fi

EXPECTED=$(realpath /tmp)

if [ "$OUTPUT" != "$EXPECTED" ]
then
  echo "FAIL: Output mismatch" >&2
  echo "Expected: $EXPECTED" >&2
  echo "Got: $OUTPUT" >&2
  exit 1
fi

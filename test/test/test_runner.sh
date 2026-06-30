#!/bin/sh

set -o errexit
set -o nounset

DRIVER="$1"
EXPECTED_EXIT="$2"
EXPECTED_OUTPUT="$3"
shift 3

EXIT_CODE=0
OUTPUT=$("$DRIVER" "$@" 2>&1) || EXIT_CODE=$?

if [ "$EXIT_CODE" -ne "$EXPECTED_EXIT" ]
then
  echo "FAIL: Expected exit code $EXPECTED_EXIT, got $EXIT_CODE" >&2
  echo "$OUTPUT" >&2
  exit 1
fi

if ! printf '%s\n' "$OUTPUT" | diff -u "$EXPECTED_OUTPUT" - >&2
then
  echo "FAIL: Output did not match $EXPECTED_OUTPUT" >&2
  exit 1
fi

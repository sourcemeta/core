#!/bin/sh

set -o errexit
set -o nounset

if [ $# -lt 4 ] || [ $((($# - 2) % 2)) -ne 0 ]
then
  echo "Usage: $0 <maker> <output.tgz> <file1> <content1> [file2] [content2] ..." >&2
  exit 1
fi

maker="$1"
output="$2"
shift 2

"$maker" "$output" "$@"

file "$output" | grep "gzip compressed"

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

tar -xzf "$output" -C "$TMP"

while [ $# -ge 2 ]
do
  expected_file="$1"
  expected_content="$2"
  shift 2
  printf "%s" "$expected_content" > "$TMP/$expected_file.expected"
  diff "$TMP/$expected_file" "$TMP/$expected_file.expected"
done

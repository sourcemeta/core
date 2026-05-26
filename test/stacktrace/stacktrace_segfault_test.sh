#!/bin/sh

set -o errexit
set -o nounset

STACKTRACE_SEGFAULT_MAIN="$1"
WORKDIR="$2"
STACKTRACE_LIBRARY="$3"
SELF="$(basename "$STACKTRACE_SEGFAULT_MAIN")"

case "$STACKTRACE_LIBRARY" in
  *.a|*.lib) LIBRARY_PATH="$STACKTRACE_SEGFAULT_MAIN" ;;
  *)         LIBRARY_PATH="$STACKTRACE_LIBRARY" ;;
esac

"$STACKTRACE_SEGFAULT_MAIN" > "$WORKDIR/$SELF.actual.txt" 2>&1 \
  && EXIT_CODE="$?" || EXIT_CODE="$?"
# Crashed by a fatal signal
test "$EXIT_CODE" -ne 0

cat "$WORKDIR/$SELF.actual.txt"

sed -E \
  -e 's/0x[0-9a-fA-F]+/0xADDR/g' \
  -e 's/\+0xADDR/+0xOFFSET/g' \
  -e 's/^pid:[[:space:]]+[0-9]+/pid:     <PID>/' \
  -e 's/^#[0-9]+ /# /' \
  -e '/^# /{N;/_sigtramp|__restore_rt|libsystem_|libdyld|\/dyld| _?start \+/d;}' \
  "$WORKDIR/$SELF.actual.txt" \
  > "$WORKDIR/$SELF.normalized.txt"

cat << EOF > "$WORKDIR/$SELF.expected.txt"

================================================================================
signal:  11 (SIGSEGV)
pid:     <PID>

# 0xADDR _ZN31sourcemeta_core_stacktrace_test13crash_deepestEv +0xOFFSET
  in $STACKTRACE_SEGFAULT_MAIN
# 0xADDR crash_handler +0xOFFSET
  in $LIBRARY_PATH
# 0xADDR _ZN31sourcemeta_core_stacktrace_test12crash_middleEv +0xOFFSET
  in $STACKTRACE_SEGFAULT_MAIN
# 0xADDR main +0xOFFSET
  in $STACKTRACE_SEGFAULT_MAIN
================================================================================
EOF

diff "$WORKDIR/$SELF.normalized.txt" "$WORKDIR/$SELF.expected.txt"

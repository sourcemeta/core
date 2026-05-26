#!/bin/sh

set -o errexit
set -o nounset

STACKTRACE_ON_DEMAND_MAIN="$1"
WORKDIR="$2"
STACKTRACE_LIBRARY="$3"
SELF="$(basename "$STACKTRACE_ON_DEMAND_MAIN")"

case "$STACKTRACE_LIBRARY" in
  *.a|*.lib) LIBRARY_PATH="$STACKTRACE_ON_DEMAND_MAIN" ;;
  *)         LIBRARY_PATH="$STACKTRACE_LIBRARY" ;;
esac

"$STACKTRACE_ON_DEMAND_MAIN" > "$WORKDIR/$SELF.actual.txt" 2>&1 \
  && EXIT_CODE="$?" || EXIT_CODE="$?"
# Exited cleanly after printing the on-demand trace
test "$EXIT_CODE" -eq 0

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
pid:     <PID>

# 0xADDR _ZN10sourcemeta4core10stacktraceEv +0xOFFSET
  in $LIBRARY_PATH
# 0xADDR _ZN31sourcemeta_core_stacktrace_test13print_deepestEv +0xOFFSET
  in $STACKTRACE_ON_DEMAND_MAIN
# 0xADDR _ZN31sourcemeta_core_stacktrace_test12print_middleEv +0xOFFSET
  in $STACKTRACE_ON_DEMAND_MAIN
# 0xADDR main +0xOFFSET
  in $STACKTRACE_ON_DEMAND_MAIN
================================================================================
EOF

diff "$WORKDIR/$SELF.normalized.txt" "$WORKDIR/$SELF.expected.txt"

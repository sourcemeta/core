#!/bin/sh

set -o errexit
set -o nounset

# A program that dies on a fatal signal never reaches the point where counters
# are written out, so ask for the ones that stay up to date as they change.
# Only a build that set this up puts a profile path in the environment, and the
# programs that path applies to are built to honour the request
if [ -n "${LLVM_PROFILE_FILE:-}" ]
then
  LLVM_PROFILE_FILE="$(dirname "$LLVM_PROFILE_FILE")/%c$(basename "$LLVM_PROFILE_FILE")"
  export LLVM_PROFILE_FILE
fi

STACKTRACE_ON_DEMAND_MAIN="$1"
WORKDIR="$2"
STACKTRACE_LIBRARY="$3"
SELF="$(basename "$STACKTRACE_ON_DEMAND_MAIN")"

case "$STACKTRACE_LIBRARY" in
  *.a|*.lib)
    LIBRARY_PATH="$STACKTRACE_ON_DEMAND_MAIN"
    ;;
  *)
    LIBRARY_PATH="$(echo "$STACKTRACE_LIBRARY" | sed -E \
      -e 's|\.so\.[0-9.]+|.so|g' \
      -e 's|\.[0-9.]+\.dylib|.dylib|g')"
    ;;
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
  -e '/^# /{
        N
        /_sigtramp|__restore_rt|__kernel_rt_sigreturn/d
        /__libc_start_main| _?start \+/d
        /libsystem_|libdyld|\/dyld|linux-vdso|libc\.so/d
        / pthread_kill | gsignal | raise /d
      }' \
  -e 's|\.so\.[0-9.]+|.so|g' \
  -e 's|\.[0-9.]+\.dylib|.dylib|g' \
  -e '/^Aborted/d' \
  -e '/^Segmentation fault/d' \
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

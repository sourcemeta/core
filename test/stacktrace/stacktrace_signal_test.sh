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

STACKTRACE_SIGNAL_MAIN="$1"
WORKDIR="$2"
STACKTRACE_LIBRARY="$3"
SIGNAL_NAME="$4"
SELF="$(basename "$STACKTRACE_SIGNAL_MAIN")"

case "$STACKTRACE_LIBRARY" in
  *.a|*.lib)
    LIBRARY_PATH="$STACKTRACE_SIGNAL_MAIN"
    ;;
  *)
    LIBRARY_PATH="$(echo "$STACKTRACE_LIBRARY" | sed -E \
      -e 's|\.so\.[0-9.]+|.so|g' \
      -e 's|\.[0-9.]+\.dylib|.dylib|g')"
    ;;
esac

"$STACKTRACE_SIGNAL_MAIN" > "$WORKDIR/$SELF.actual.txt" 2>&1 \
  && EXIT_CODE="$?" || EXIT_CODE="$?"
# Crashed by a fatal signal
test "$EXIT_CODE" -ne 0

cat "$WORKDIR/$SELF.actual.txt"

# The number a signal answers to is the one the system assigns it, which is not
# the same everywhere, so what is read back here is the name the handler maps it
# to rather than the number it printed alongside
sed -E \
  -e 's/0x[0-9a-fA-F]+/0xADDR/g' \
  -e 's/\+0xADDR/+0xOFFSET/g' \
  -e 's/^signal:[[:space:]]+[0-9]+ \(/signal:  <NUMBER> (/' \
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
  -e '/^Floating point exception/d' \
  -e '/^Bus error/d' \
  -e '/^Illegal instruction/d' \
  "$WORKDIR/$SELF.actual.txt" \
  > "$WORKDIR/$SELF.normalized.txt"

cat << EOF > "$WORKDIR/$SELF.expected.txt"

================================================================================
signal:  <NUMBER> ($SIGNAL_NAME)
pid:     <PID>

# 0xADDR sourcemeta_core_stacktrace_crash_handler +0xOFFSET
  in $LIBRARY_PATH
# 0xADDR _ZN31sourcemeta_core_stacktrace_test13crash_deepestEv +0xOFFSET
  in $STACKTRACE_SIGNAL_MAIN
# 0xADDR _ZN31sourcemeta_core_stacktrace_test12crash_middleEv +0xOFFSET
  in $STACKTRACE_SIGNAL_MAIN
# 0xADDR main +0xOFFSET
  in $STACKTRACE_SIGNAL_MAIN
================================================================================
EOF

diff "$WORKDIR/$SELF.normalized.txt" "$WORKDIR/$SELF.expected.txt"

function(sourcemeta_add_default_options visibility target)
  if(SOURCEMETA_COMPILER_MSVC)
    # See https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category
    target_compile_options("${target}" ${visibility}
      /options:strict
      /permissive-
      /W4
      /WL
      /MP
      /sdl)
  elseif(SOURCEMETA_COMPILER_LLVM OR SOURCEMETA_COMPILER_GCC)
    target_compile_options("${target}" ${visibility}
      -Wall
      -Wextra
      -Wpedantic
      -Wshadow
      -Wdouble-promotion
      -Wconversion
      -Wunused-parameter
      -Wtrigraphs
      -Wunreachable-code
      -Wmissing-braces
      -Wparentheses
      -Wswitch
      -Wunused-function
      -Wunused-label
      -Wunused-parameter
      -Wunused-variable
      -Wunused-value
      -Wempty-body
      -Wuninitialized
      -Wshadow
      -Wconversion
      -Wenum-conversion
      -Wfloat-conversion
      -Wimplicit-fallthrough
      -Wsign-compare
      -Wsign-conversion
      -Wunknown-pragmas
      -Wnon-virtual-dtor
      -Woverloaded-virtual
      -Winvalid-offsetof
      -funroll-loops
      -ftree-vectorize

      # To improve how much GCC/Clang will vectorize
      -fno-math-errno

      # Assume that signed arithmetic overflow of addition, subtraction and
      # multiplication wraps around using twos-complement representation
      # See https://users.cs.utah.edu/~regehr/papers/overflow12.pdf
      # See https://www.postgresql.org/message-id/1689.1134422394@sss.pgh.pa.us
      -fwrapv

      # See https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html
      -Wformat
      -Wformat=2
      -Werror=format-security
      -fstack-protector-strong)

    # Control-flow protection: requires hardware and OS support
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
      # -fcf-protection uses Intel CET (Control-flow Enforcement Technology)
      # Requires OS kernel support, primarily available on Linux
      if(LINUX)
        target_compile_options("${target}" ${visibility} -fcf-protection=full)
      endif()
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
      # -mbranch-protection uses ARM BTI/PAC, requires Linux kernel 5.8+
      if(LINUX)
        target_compile_options("${target}" ${visibility} -mbranch-protection=standard)
      endif()
    endif()

    # _FORTIFY_SOURCE requires optimization (-O1 or higher), so only enable in Release builds
    # First undefine to avoid conflicts, then define
    target_compile_options("${target}" ${visibility}
      $<$<CONFIG:Release>:-U_FORTIFY_SOURCE>
      $<$<CONFIG:RelWithDebInfo>:-U_FORTIFY_SOURCE>)
    target_compile_definitions("${target}" ${visibility}
      $<$<CONFIG:Release>:_FORTIFY_SOURCE=3>
      $<$<CONFIG:RelWithDebInfo>:_FORTIFY_SOURCE=3>)

    # _GLIBCXX_ASSERTIONS is libstdc++ (GNU) specific, not applicable to libc++ (LLVM/macOS)
    if(NOT APPLE AND SOURCEMETA_COMPILER_GCC)
      target_compile_definitions("${target}" ${visibility} $<$<CONFIG:Debug>:_GLIBCXX_ASSERTIONS>)
    endif()
  endif()

  if(SOURCEMETA_COMPILER_LLVM)
    target_compile_options("${target}" ${visibility}
      -Wbool-conversion
      -Wint-conversion
      -Wpointer-sign
      -Wconditional-uninitialized
      -Wconstant-conversion
      -Wnon-literal-null-conversion
      -Wshorten-64-to-32
      -Wdeprecated-implementations
      -Winfinite-recursion
      -Wnewline-eof
      -Wfour-char-constants
      -Wselector
      -Wundeclared-selector
      -Wdocumentation
      -Wmove
      -Wc++11-extensions
      -Wcomma
      -Wno-exit-time-destructors
      -Wrange-loop-analysis

      # Enable loop vectorization for performance reasons
      -fvectorize
      # Enable vectorization of straight-line code for performance
      -fslp-vectorize)

    # See https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html
    target_compile_options("${target}" ${visibility}
      $<$<CONFIG:Release>:-fno-delete-null-pointer-checks -fno-strict-aliasing -ftrivial-auto-var-init=zero>
      $<$<CONFIG:RelWithDebInfo>:-fno-delete-null-pointer-checks -fno-strict-aliasing -ftrivial-auto-var-init=zero>)
  elseif(SOURCEMETA_COMPILER_GCC)
    target_compile_options("${target}" ${visibility}
      -fno-trapping-math
      # Newer versions of GCC (i.e. 14) seem to print a lot of false-positives here
      -Wno-dangling-reference
      # GCC seems to print a lot of false-positives here
      -Wno-free-nonheap-object
      # Disables runtime type information
      -fno-rtti

      # See https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html
      -Wtrampolines
      -Wbidi-chars=any
      -fstack-clash-protection
      -fstrict-flex-arrays=3)

    # See https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html
    target_compile_options("${target}" ${visibility}
      $<$<CONFIG:Release>:-fno-delete-null-pointer-checks -fno-strict-overflow -fno-strict-aliasing -ftrivial-auto-var-init=zero>
      $<$<CONFIG:RelWithDebInfo>:-fno-delete-null-pointer-checks -fno-strict-overflow -fno-strict-aliasing -ftrivial-auto-var-init=zero>)
  endif()
endfunction()

# For studying failed vectorization results
# - On Clang , seems to only take effect on release shared builds
# - On GCC, seems to only take effect on release shared builds
function(sourcemeta_add_vectorization_diagnostics target)
  if(SOURCEMETA_COMPILER_LLVM)
    # See https://llvm.org/docs/Vectorizers.html#id6
    target_compile_options("${target}" PRIVATE
      -Rpass-analysis=loop-vectorize
      -Rpass-missed=loop-vectorize)
  elseif(SOURCEMETA_COMPILER_GCC)
    target_compile_options("${target}" PRIVATE
      -fopt-info-vec-missed
      -fopt-info-loop-missed)
  endif()
endfunction()

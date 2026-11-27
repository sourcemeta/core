#ifndef SOURCEMETA_CORE_JOSE_SWITCH_H_
#define SOURCEMETA_CORE_JOSE_SWITCH_H_

// Clang reads an exhaustive switch as carrying an arm for the value that never
// arrives, which this fold closes there while the compilers that read case
// labels keep holding the switch to naming every one of them
#if defined(__clang__)
#define SOURCEMETA_CORE_JOSE_FIRST_CASE(value) default:
#else
#define SOURCEMETA_CORE_JOSE_FIRST_CASE(value) case value:
#endif

// The fold above leaves clang nothing to reach past such a switch, while the
// compilers that read case labels still have to be told the end of the
// function is never reached
#if defined(__clang__)
#define SOURCEMETA_CORE_JOSE_NO_OTHER_VALUE
#else
#define SOURCEMETA_CORE_JOSE_NO_OTHER_VALUE std::unreachable();
#endif

#endif

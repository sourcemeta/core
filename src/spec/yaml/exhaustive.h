#ifndef SOURCEMETA_CORE_YAML_EXHAUSTIVE_H_
#define SOURCEMETA_CORE_YAML_EXHAUSTIVE_H_

// A switch that names every enumerator of its type needs no default, and the
// compilers that cannot see that the end of such a function is unreachable are
// told so after it. Clang keeps an arm for the value that never arrives and
// counts it as one that nothing took, so there the last enumerator is written
// as the default instead, which leaves the switch no arm to spare. The
// enumerator stays named in the macro so that the compilers still reading a
// case label are the ones holding the switch to naming them all, which is what
// `-Wswitch` does for the builds that are not measuring coverage
// The label carries its own colon so that nothing reading the source has a
// stray one to make sense of
#if defined(__clang__)
#define SOURCEMETA_CORE_YAML_LAST_CASE(enumerator) default:
#else
#define SOURCEMETA_CORE_YAML_LAST_CASE(enumerator) case enumerator:
#endif

#endif

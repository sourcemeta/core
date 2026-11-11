#ifndef SOURCEMETA_CORE_YAML_EXHAUSTIVE_H_
#define SOURCEMETA_CORE_YAML_EXHAUSTIVE_H_

// A switch that names every enumerator of its type needs no default. Clang
// keeps an arm for the value that never arrives all the same, and counts it as
// one that nothing took, so there the last enumerator is written as the
// default instead, which leaves the switch no arm to spare.
//
// That costs `-Wswitch` on clang, which no longer has a case label to find
// missing, so an enumerator added to one of these types is caught by the GCC
// and MSVC builds rather than by a local one. Both read the case label this
// expands to for them and turn the warning into an error, which the
// enumerator named here is what gives them to miss. Telling the two apart by
// anything narrower would mean the sources knowing which build is measuring
// coverage, which the build deliberately keeps to itself.
//
// The label carries its own colon so that nothing reading the source has a
// stray one to make sense of
#if defined(__clang__)
#define SOURCEMETA_CORE_YAML_LAST_CASE(enumerator) default:
#else
#define SOURCEMETA_CORE_YAML_LAST_CASE(enumerator) case enumerator:
#endif

#endif

#include <sourcemeta/core/test.h>

// Not derived from the standard exception type, so the runner has nothing to
// ask for a description
struct Alien {};

TEST(unknown_throw) { throw Alien{}; }

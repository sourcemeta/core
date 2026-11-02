#include <sourcemeta/core/test.h>

#include <stdexcept> // std::runtime_error

TEST(unexpected_throw) { throw std::runtime_error{"something went wrong"}; }

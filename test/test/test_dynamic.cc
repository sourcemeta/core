#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {
auto expect_length(const std::string_view value, const std::size_t length)
    -> void {
  EXPECT_EQ(value.size(), length);
}
} // namespace

auto main(int argc, char **argv) -> int {
  constexpr std::array<std::string_view, 3> values{"a", "bb", "ccc"};
  std::size_t index{0};
  for (const std::string_view value : values) {
    index += 1;
    sourcemeta::core::test_register(
        "length_" + std::to_string(index),
        [value]() -> void { expect_length(value, value.size()); });
  }

  return sourcemeta::core::test_run(argc, argv);
}

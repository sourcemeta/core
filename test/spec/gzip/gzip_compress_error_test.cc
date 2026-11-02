#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/test.h>

#include <cstdint> // std::uint8_t
#include <limits>  // std::numeric_limits
#include <string>  // std::string

namespace {

auto compress_error(const int level) -> std::string {
  const std::string input{"hello world"};
  try {
    sourcemeta::core::gzip(reinterpret_cast<const std::uint8_t *>(input.data()),
                           input.size(), level);
  } catch (const sourcemeta::core::GZIPError &error) {
    return error.what();
  }

  FAIL();
}

} // namespace

TEST(compress_level_above_maximum) {
  EXPECT_EQ(compress_error(13), "Invalid compression level");
}

TEST(compress_negative_level) {
  EXPECT_EQ(compress_error(-2), "Invalid compression level");
}

TEST(compress_minus_one_level) {
  EXPECT_EQ(compress_error(-1), "Invalid compression level");
}

TEST(compress_largest_integer_level) {
  EXPECT_EQ(compress_error(std::numeric_limits<int>::max()),
            "Invalid compression level");
}

TEST(compress_smallest_integer_level) {
  EXPECT_EQ(compress_error(std::numeric_limits<int>::min()),
            "Invalid compression level");
}

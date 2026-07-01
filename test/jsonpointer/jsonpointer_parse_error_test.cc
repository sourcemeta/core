#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <exception>
#include <string>

#define EXPECT_POINTER_PARSE_ERROR(input, expected_column)                     \
  try {                                                                        \
    sourcemeta::core::to_pointer(input);                                       \
    FAIL();                                                                    \
  } catch (const sourcemeta::core::PointerParseError &error) {                 \
    EXPECT_EQ(error.column(), expected_column);                                \
  } catch (const std::exception &) {                                           \
    FAIL();                                                                    \
  }

TEST(missing_initial_slash) {
  const std::string input{"1"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(tilde) {
  const std::string input{"/~"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(tilde_2) {
  const std::string input{"/~2"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(tilde_tilde) {
  const std::string input{"/~~"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(foo_tilde) {
  const std::string input{"/foo~"};
  EXPECT_POINTER_PARSE_ERROR(input, 6);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(index_foo_tilde) {
  const std::string input{"/123/foo~"};
  EXPECT_POINTER_PARSE_ERROR(input, 10);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(zero_index_foo_tilde) {
  const std::string input{"/0/foo~"};
  EXPECT_POINTER_PARSE_ERROR(input, 8);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(slash_slash_foo_tilde) {
  const std::string input{"//foo~"};
  EXPECT_POINTER_PARSE_ERROR(input, 7);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(foo_tilde_2) {
  const std::string input{"/foo~2"};
  EXPECT_POINTER_PARSE_ERROR(input, 6);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(foo_tilde_tilde) {
  const std::string input{"/foo~~"};
  EXPECT_POINTER_PARSE_ERROR(input, 6);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

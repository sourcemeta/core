#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <exception>
#include <string>

#define EXPECT_POINTER_PARSE_ERROR(input, expected_column)                     \
  try {                                                                        \
    sourcemeta::core::to_pointer(input);                                       \
    FAIL() << "The to_pointer function was expected to throw";                 \
  } catch (const sourcemeta::core::PointerParseError &error) {                 \
    EXPECT_EQ(error.column(), expected_column);                                \
    SUCCEED();                                                                 \
  } catch (const std::exception &) {                                           \
    FAIL() << "The to_pointer function threw an unexpected error";             \
  }

TEST(JSONPointer_parse_error, missing_initial_slash) {
  const std::string input{"1"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(JSONPointer_parse_error, tilde) {
  const std::string input{"/~"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(JSONPointer_parse_error, tilde_2) {
  const std::string input{"/~2"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(JSONPointer_parse_error, tilde_tilde) {
  const std::string input{"/~~"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(JSONPointer_parse_error, foo_tilde) {
  const std::string input{"/foo~"};
  EXPECT_POINTER_PARSE_ERROR(input, 6);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(JSONPointer_parse_error, index_foo_tilde) {
  const std::string input{"/123/foo~"};
  EXPECT_POINTER_PARSE_ERROR(input, 10);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(JSONPointer_parse_error, zero_index_foo_tilde) {
  const std::string input{"/0/foo~"};
  EXPECT_POINTER_PARSE_ERROR(input, 8);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(JSONPointer_parse_error, slash_slash_foo_tilde) {
  const std::string input{"//foo~"};
  EXPECT_POINTER_PARSE_ERROR(input, 7);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(JSONPointer_parse_error, foo_tilde_2) {
  const std::string input{"/foo~2"};
  EXPECT_POINTER_PARSE_ERROR(input, 6);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(JSONPointer_parse_error, foo_tilde_tilde) {
  const std::string input{"/foo~~"};
  EXPECT_POINTER_PARSE_ERROR(input, 6);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <exception>
#include <string>
#include <string_view>

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

TEST(invalid_tilde_3) {
  const std::string input{"/~3"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_tilde_9) {
  const std::string input{"/~9"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_tilde_letter) {
  const std::string input{"/~a"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_tilde_dash) {
  const std::string input{"/~-1"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_tilde_slash) {
  const std::string input{"/~/"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_partial_escape_1) {
  const std::string input{"/~0~"};
  EXPECT_POINTER_PARSE_ERROR(input, 5);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_partial_escape_2) {
  const std::string input{"/~0/~"};
  EXPECT_POINTER_PARSE_ERROR(input, 6);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_interior_dangling) {
  const std::string input{"/a~"};
  EXPECT_POINTER_PARSE_ERROR(input, 4);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_multitoken_dangling) {
  const std::string input{"/foo/bar~"};
  EXPECT_POINTER_PARSE_ERROR(input, 10);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_tilde_before_space) {
  const std::string input{"/~ "};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_tilde_run) {
  const std::string input{"/foo~~~~~~~~"};
  EXPECT_POINTER_PARSE_ERROR(input, 6);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_no_slash_letter) {
  const std::string input{"a"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_no_slash_digit) {
  const std::string input{"0"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_no_slash_path) {
  const std::string input{"a/a"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_no_slash_word) {
  const std::string input{"foo"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_leading_space) {
  const std::string input{" /foo"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_leading_tab) {
  const std::string input{"\011/foo"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_leading_newline) {
  const std::string input{"\012/foo"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_leading_letter_slash) {
  const std::string input{"x/foo"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_fragment_hash) {
  const std::string input{"#"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_fragment_hash_slash) {
  const std::string input{"#/"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_fragment_hash_a) {
  const std::string input{"#a"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_fragment_tail) {
  const std::string input{"#/foo"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_fragment_tail_index) {
  const std::string input{"#/foo/0"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(invalid_fragment_tail_escape) {
  const std::string input{"#/a~1b"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(nul_then_dangling_tilde) {
  const std::string input{"/foo\000~", 6};
  EXPECT_POINTER_PARSE_ERROR(input, 7);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(nul_then_bad_escape) {
  const std::string input{"/foo\000~2", 7};
  EXPECT_POINTER_PARSE_ERROR(input, 7);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(nul_first_token_dangling) {
  const std::string input{"/\000~", 3};
  EXPECT_POINTER_PARSE_ERROR(input, 4);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(nul_valid_escape_then_dangling) {
  const std::string input{"/a~1\000~", 6};
  EXPECT_POINTER_PARSE_ERROR(input, 7);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(leading_nul) {
  const std::string input{"\000/foo", 5};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(lone_nul) {
  const std::string input{"\000", 1};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(tilde_fullwidth_zero) {
  const std::string input{"/~\357\274\220"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

TEST(fullwidth_solidus_leading) {
  const std::string input{"\357\274\217foo"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
  EXPECT_FALSE(sourcemeta::core::is_pointer(input));
}

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

TEST(invalid_bare_tilde_eof) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("/~"));
}

TEST(invalid_tilde_2) { EXPECT_FALSE(sourcemeta::core::is_pointer("/~2")); }

TEST(invalid_tilde_3) { EXPECT_FALSE(sourcemeta::core::is_pointer("/~3")); }

TEST(invalid_tilde_9) { EXPECT_FALSE(sourcemeta::core::is_pointer("/~9")); }

TEST(invalid_tilde_letter) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("/~a"));
}

TEST(invalid_tilde_dash) { EXPECT_FALSE(sourcemeta::core::is_pointer("/~-1")); }

TEST(invalid_double_tilde) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("/~~"));
}

TEST(invalid_tilde_slash) { EXPECT_FALSE(sourcemeta::core::is_pointer("/~/")); }

TEST(invalid_trailing_tilde) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("/foo~"));
}

TEST(invalid_partial_escape_1) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("/~0~"));
}

TEST(invalid_partial_escape_2) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("/~0/~"));
}

TEST(invalid_interior_dangling) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("/a~"));
}

TEST(invalid_multitoken_dangling) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("/foo/bar~"));
}

TEST(invalid_tilde_before_space) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("/~ "));
}

TEST(invalid_tilde_run) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("/foo~~~~~~~~"));
}

TEST(invalid_no_slash_letter) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("a"));
}

TEST(invalid_no_slash_digit) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("0"));
}

TEST(invalid_no_slash_path) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("a/a"));
}

TEST(invalid_no_slash_word) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("foo"));
}

TEST(invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_pointer(" /foo"));
}

TEST(invalid_leading_tab) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("\011/foo"));
}

TEST(invalid_leading_newline) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("\012/foo"));
}

TEST(invalid_leading_letter_slash) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("x/foo"));
}

TEST(invalid_fragment_hash) { EXPECT_FALSE(sourcemeta::core::is_pointer("#")); }

TEST(invalid_fragment_hash_slash) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("#/"));
}

TEST(invalid_fragment_hash_a) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("#a"));
}

TEST(invalid_fragment_tail) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("#/foo"));
}

TEST(invalid_fragment_tail_index) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("#/foo/0"));
}

TEST(invalid_fragment_tail_escape) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("#/a~1b"));
}

TEST(nul_then_dangling_tilde) {
  EXPECT_FALSE(sourcemeta::core::is_pointer(std::string_view{"/foo\000~", 6}));
}

TEST(nul_then_bad_escape) {
  EXPECT_FALSE(sourcemeta::core::is_pointer(std::string_view{"/foo\000~2", 7}));
}

TEST(nul_first_token_dangling) {
  EXPECT_FALSE(sourcemeta::core::is_pointer(std::string_view{"/\000~", 3}));
}

TEST(nul_valid_escape_then_dangling) {
  EXPECT_FALSE(sourcemeta::core::is_pointer(std::string_view{"/a~1\000~", 6}));
}

TEST(leading_nul) {
  EXPECT_FALSE(sourcemeta::core::is_pointer(std::string_view{"\000/foo", 5}));
}

TEST(lone_nul) {
  EXPECT_FALSE(sourcemeta::core::is_pointer(std::string_view{"\000", 1}));
}

TEST(tilde_fullwidth_zero) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("/~\357\274\220"));
}

TEST(fullwidth_solidus_leading) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("\357\274\217foo"));
}

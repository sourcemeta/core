#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <string_view>

// Exhaustive is_pointer coverage against the RFC 6901 section 3 string grammar:
//   json-pointer    = *( "/" reference-token )
//   reference-token = *( unescaped / escaped )
//   unescaped       = %x00-2E / %x30-7D / %x7F-10FFFF
//   escaped         = "~" ( "0" / "1" )
// Core already passes every case, so this is coverage, not a fix.

TEST(empty_whole_document) { EXPECT_TRUE(sourcemeta::core::is_pointer("")); }

TEST(root_single_empty_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/"));
}

TEST(two_empty_tokens) { EXPECT_TRUE(sourcemeta::core::is_pointer("//")); }

TEST(three_empty_tokens) { EXPECT_TRUE(sourcemeta::core::is_pointer("///")); }

TEST(four_empty_tokens) { EXPECT_TRUE(sourcemeta::core::is_pointer("////")); }

TEST(empty_interior_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo//bar"));
}

TEST(trailing_empty_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/bar/"));
}

TEST(leading_then_empty) { EXPECT_TRUE(sourcemeta::core::is_pointer("//foo")); }

TEST(single_char_token) { EXPECT_TRUE(sourcemeta::core::is_pointer("/a")); }

TEST(single_token) { EXPECT_TRUE(sourcemeta::core::is_pointer("/foo")); }

TEST(two_tokens) { EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/bar")); }

TEST(deep_six_tokens) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a/b/c/d/e/f"));
}

TEST(deep_twelve_tokens) {
  EXPECT_TRUE(
      sourcemeta::core::is_pointer("/t0/t1/t2/t3/t4/t5/t6/t7/t8/t9/t10/t11"));
}

TEST(long_single_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer(
      "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
}

TEST(escape_tilde0) { EXPECT_TRUE(sourcemeta::core::is_pointer("/~0")); }

TEST(escape_tilde1) { EXPECT_TRUE(sourcemeta::core::is_pointer("/~1")); }

TEST(escape_tilde0_tilde1) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0~1"));
}

TEST(escape_tilde1_tilde0) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~1~0"));
}

TEST(escape_tilde0_tilde0) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0~0"));
}

TEST(escape_tilde1_tilde1) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~1~1"));
}

TEST(escape_tilde00) { EXPECT_TRUE(sourcemeta::core::is_pointer("/~00")); }

TEST(escape_tilde11) { EXPECT_TRUE(sourcemeta::core::is_pointer("/~11")); }

TEST(escape_tilde01) { EXPECT_TRUE(sourcemeta::core::is_pointer("/~01")); }

TEST(escape_tilde10) { EXPECT_TRUE(sourcemeta::core::is_pointer("/~10")); }

TEST(escape_interior_tilde1) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a~1b"));
}

TEST(escape_interior_tilde0) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/m~0n"));
}

TEST(escape_run) { EXPECT_TRUE(sourcemeta::core::is_pointer("/~1~0~0~1~1")); }

TEST(escape_then_fraction_1) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~1.1"));
}

TEST(escape_then_fraction_0) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0.1"));
}

TEST(escape_only_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0~1~0~1"));
}

TEST(escape_each_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0/~1/~0"));
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

TEST(unescaped_nul) {
  EXPECT_TRUE(sourcemeta::core::is_pointer(std::string_view{"/\000", 2}));
}

TEST(unescaped_tab) { EXPECT_TRUE(sourcemeta::core::is_pointer("/\011")); }

TEST(unescaped_lf) { EXPECT_TRUE(sourcemeta::core::is_pointer("/\012")); }

TEST(unescaped_cr) { EXPECT_TRUE(sourcemeta::core::is_pointer("/\015")); }

TEST(unescaped_us) { EXPECT_TRUE(sourcemeta::core::is_pointer("/\037")); }

TEST(unescaped_space) { EXPECT_TRUE(sourcemeta::core::is_pointer("/ ")); }

TEST(unescaped_dot_upper) { EXPECT_TRUE(sourcemeta::core::is_pointer("/.")); }

TEST(unescaped_zero_lower) { EXPECT_TRUE(sourcemeta::core::is_pointer("/0")); }

TEST(unescaped_brace_upper) { EXPECT_TRUE(sourcemeta::core::is_pointer("/}")); }

TEST(unescaped_del) { EXPECT_TRUE(sourcemeta::core::is_pointer("/\177")); }

TEST(unescaped_high_0x80) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\302\200"));
}

TEST(unescaped_latin1) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\303\251"));
}

TEST(unescaped_astral_face) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/bar/\360\237\230\216"));
}

TEST(unescaped_astral_gclef) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\360\235\204\236"));
}

TEST(unescaped_control_run) {
  EXPECT_TRUE(sourcemeta::core::is_pointer(
      std::string_view{"/foo\000bar\012\011baz", 13}));
}

TEST(special_caret) { EXPECT_TRUE(sourcemeta::core::is_pointer("/e^f")); }

TEST(special_pipe) { EXPECT_TRUE(sourcemeta::core::is_pointer("/g|h")); }

TEST(special_backslash) { EXPECT_TRUE(sourcemeta::core::is_pointer("/i\\j")); }

TEST(special_quote) { EXPECT_TRUE(sourcemeta::core::is_pointer("/k\"l")); }

TEST(special_percent) { EXPECT_TRUE(sourcemeta::core::is_pointer("/c%d")); }

TEST(literal_pct_slash) { EXPECT_TRUE(sourcemeta::core::is_pointer("/a%2Fb")); }

TEST(suite_mixed_valid) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/bar~0/baz~1/%a"));
}

TEST(index_double_zero) { EXPECT_TRUE(sourcemeta::core::is_pointer("/00")); }

TEST(index_zero_one) { EXPECT_TRUE(sourcemeta::core::is_pointer("/01")); }

TEST(index_zero_zero_seven) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/007"));
}

TEST(index_leading_zero_num) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/0123"));
}

TEST(index_ten) { EXPECT_TRUE(sourcemeta::core::is_pointer("/10")); }

TEST(index_dash) { EXPECT_TRUE(sourcemeta::core::is_pointer("/-")); }

TEST(index_dash_after) { EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/-")); }

TEST(index_dash_member) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/-/bar"));
}

TEST(index_exponent_like) { EXPECT_TRUE(sourcemeta::core::is_pointer("/1e0")); }

TEST(index_negative_like) { EXPECT_TRUE(sourcemeta::core::is_pointer("/-1")); }

TEST(index_long_numeric) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1234567890"));
}

TEST(index_huge_numeric) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/99999999999999999999"));
}

TEST(realworld_openapi_ref) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/paths/~1p~1{}/get"));
}

TEST(realworld_components) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/components/schemas/Foo"));
}

TEST(realworld_escaped_slash_key) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/definitions/~1etc~1passwd"));
}

TEST(trailing_lf) { EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\012")); }

TEST(trailing_two_lf) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\012\012"));
}

TEST(trailing_cr) { EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\015")); }

TEST(trailing_crlf) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\015\012"));
}

TEST(space_in_token) { EXPECT_TRUE(sourcemeta::core::is_pointer("/foo bar")); }

TEST(escape_then_trailing_lf) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0\012"));
}

TEST(nul_interior_valid) {
  EXPECT_TRUE(sourcemeta::core::is_pointer(std::string_view{"/foo\000bar", 8}));
}

TEST(nul_trailing_valid) {
  EXPECT_TRUE(sourcemeta::core::is_pointer(std::string_view{"/foo\000", 5}));
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

TEST(boundary_brace_interior) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a}b"));
}

TEST(fullwidth_tilde_alone) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\357\275\236"));
}

TEST(fullwidth_tilde_interior) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a\357\275\236b"));
}

TEST(fullwidth_tilde_then_2) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\357\275\2362"));
}

TEST(tilde_fullwidth_zero) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("/~\357\274\220"));
}

TEST(fullwidth_solidus_leading) {
  EXPECT_FALSE(sourcemeta::core::is_pointer("\357\274\217foo"));
}

TEST(fullwidth_solidus_interior) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a\357\274\217b"));
}

TEST(percent_7e) { EXPECT_TRUE(sourcemeta::core::is_pointer("/%7E")); }

TEST(percent_7e_foo) { EXPECT_TRUE(sourcemeta::core::is_pointer("/%7Efoo")); }

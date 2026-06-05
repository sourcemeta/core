#include <gtest/gtest.h>

#include <string>

#include <sourcemeta/core/http.h>

TEST(HTTP_field_list_contains_any, single_match) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any("a", {"a"}));
}

TEST(HTTP_field_list_contains_any, single_no_match) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("a", {"b"}));
}

TEST(HTTP_field_list_contains_any, empty_header_returns_false) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("", {"a"}));
}

TEST(HTTP_field_list_contains_any, whitespace_only_header_returns_false) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("   ", {"a"}));
}

TEST(HTTP_field_list_contains_any, empty_token_list_returns_false) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("a", {}));
}

TEST(HTTP_field_list_contains_any, comma_separated_match_first) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any("a, b, c", {"a"}));
}

TEST(HTTP_field_list_contains_any, comma_separated_match_middle) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any("a, b, c", {"b"}));
}

TEST(HTTP_field_list_contains_any, comma_separated_match_last) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any("a, b, c", {"c"}));
}

TEST(HTTP_field_list_contains_any, comma_separated_no_match) {
  EXPECT_FALSE(
      sourcemeta::core::http_field_list_contains_any("a, b, c", {"d"}));
}

TEST(HTTP_field_list_contains_any, multiple_tokens_any_match) {
  EXPECT_TRUE(
      sourcemeta::core::http_field_list_contains_any("c", {"a", "b", "c"}));
}

TEST(HTTP_field_list_contains_any, multiple_tokens_none_match) {
  EXPECT_FALSE(
      sourcemeta::core::http_field_list_contains_any("z", {"a", "b", "c"}));
}

TEST(HTTP_field_list_contains_any, strips_parameters_before_comparison) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any("a;p=1", {"a"}));
}

TEST(HTTP_field_list_contains_any, whitespace_around_entries_tolerated) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "  a  ,  b  ,  c  ", {"b"}));
}

TEST(HTTP_field_list_contains_any, exact_match_required_not_substring) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("abcd", {"abc"}));
}

TEST(HTTP_field_list_contains_any, case_sensitive_comparison) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("A", {"a"}));
}

TEST(HTTP_field_list_contains_any, if_none_match_wildcard) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "*", {"*", "\"abc\"", "W/\"abc\""}));
}

TEST(HTTP_field_list_contains_any, if_none_match_strong_etag) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "\"abc\"", {"*", "\"abc\"", "W/\"abc\""}));
}

TEST(HTTP_field_list_contains_any, if_none_match_weak_etag) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "W/\"abc\"", {"*", "\"abc\"", "W/\"abc\""}));
}

TEST(HTTP_field_list_contains_any, if_none_match_etag_list_with_match) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "\"xyz\", \"abc\", \"def\"", {"*", "\"abc\"", "W/\"abc\""}));
}

TEST(HTTP_field_list_contains_any, if_none_match_etag_list_without_match) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any(
      "\"xyz\", \"def\", \"ghi\"", {"*", "\"abc\"", "W/\"abc\""}));
}

TEST(HTTP_field_list_contains_any, runtime_constructed_strings_safe) {
  const std::string checksum{"abc123"};
  const std::string strong_etag{std::string{"\""} + checksum + "\""};
  const std::string weak_etag{std::string{"W/\""} + checksum + "\""};
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "\"abc123\"", {"*", strong_etag, weak_etag}));
}

TEST(HTTP_field_list_contains_any, stops_at_first_match_no_false_positive) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("x, y, z",
                                                              {"a", "b", "c"}));
}

TEST(HTTP_field_list_contains_any, single_comma_only_header) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any(",", {"a"}));
}

TEST(HTTP_field_list_contains_any, leading_comma_with_value) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(", a", {"a"}));
}

TEST(HTTP_field_list_contains_any, trailing_comma_with_value) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any("a,", {"a"}));
}

TEST(HTTP_field_list_contains_any, vary_style_header) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "Accept-Encoding, User-Agent", {"Accept-Encoding"}));
}

TEST(HTTP_field_list_contains_any, cache_control_no_cache_predicate) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "no-cache, no-store, max-age=0", {"no-cache"}));
}

TEST(HTTP_field_list_contains_any, etag_with_comma_inside_quotes_kept_whole) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      R"("abc,def", "ghi")", {R"("abc,def")"}));
}

TEST(HTTP_field_list_contains_any,
     etag_with_comma_inside_quotes_not_split_into_pieces) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any(R"("abc,def")",
                                                              {R"("abc)"}));
}

TEST(HTTP_field_list_contains_any, escaped_quote_in_quoted_string_handled) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      R"X("a\"b,c", "xyz")X", {R"X("a\"b,c")X"}));
}

TEST(HTTP_field_list_contains_any,
     weak_etag_with_comma_in_opaque_tag_kept_whole) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      R"(W/"abc,def", "ghi")", {R"(W/"abc,def")"}));
}

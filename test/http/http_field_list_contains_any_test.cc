#include <sourcemeta/core/test.h>

#include <string>

#include <sourcemeta/core/http.h>

TEST(single_match) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any("a", {"a"}));
}

TEST(single_no_match) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("a", {"b"}));
}

TEST(empty_header_returns_false) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("", {"a"}));
}

TEST(whitespace_only_header_returns_false) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("   ", {"a"}));
}

TEST(empty_token_list_returns_false) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("a", {}));
}

TEST(comma_separated_match_first) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any("a, b, c", {"a"}));
}

TEST(comma_separated_match_middle) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any("a, b, c", {"b"}));
}

TEST(comma_separated_match_last) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any("a, b, c", {"c"}));
}

TEST(comma_separated_no_match) {
  EXPECT_FALSE(
      sourcemeta::core::http_field_list_contains_any("a, b, c", {"d"}));
}

TEST(multiple_tokens_any_match) {
  EXPECT_TRUE(
      sourcemeta::core::http_field_list_contains_any("c", {"a", "b", "c"}));
}

TEST(multiple_tokens_none_match) {
  EXPECT_FALSE(
      sourcemeta::core::http_field_list_contains_any("z", {"a", "b", "c"}));
}

TEST(strips_parameters_before_comparison) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any("a;p=1", {"a"}));
}

TEST(whitespace_around_entries_tolerated) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "  a  ,  b  ,  c  ", {"b"}));
}

TEST(exact_match_required_not_substring) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("abcd", {"abc"}));
}

TEST(case_sensitive_comparison) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("A", {"a"}));
}

TEST(if_none_match_wildcard) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "*", {"*", "\"abc\"", "W/\"abc\""}));
}

TEST(if_none_match_strong_etag) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "\"abc\"", {"*", "\"abc\"", "W/\"abc\""}));
}

TEST(if_none_match_weak_etag) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "W/\"abc\"", {"*", "\"abc\"", "W/\"abc\""}));
}

TEST(if_none_match_etag_list_with_match) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "\"xyz\", \"abc\", \"def\"", {"*", "\"abc\"", "W/\"abc\""}));
}

TEST(if_none_match_etag_list_without_match) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any(
      "\"xyz\", \"def\", \"ghi\"", {"*", "\"abc\"", "W/\"abc\""}));
}

TEST(runtime_constructed_strings_safe) {
  const std::string checksum{"abc123"};
  const std::string strong_etag{std::string{"\""} + checksum + "\""};
  const std::string weak_etag{std::string{"W/\""} + checksum + "\""};
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "\"abc123\"", {"*", strong_etag, weak_etag}));
}

TEST(stops_at_first_match_no_false_positive) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any("x, y, z",
                                                              {"a", "b", "c"}));
}

TEST(single_comma_only_header) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any(",", {"a"}));
}

TEST(leading_comma_with_value) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(", a", {"a"}));
}

TEST(trailing_comma_with_value) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any("a,", {"a"}));
}

TEST(vary_style_header) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "Accept-Encoding, User-Agent", {"Accept-Encoding"}));
}

TEST(cache_control_no_cache_predicate) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      "no-cache, no-store, max-age=0", {"no-cache"}));
}

TEST(etag_with_comma_inside_quotes_kept_whole) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      R"("abc,def", "ghi")", {R"("abc,def")"}));
}

TEST(etag_with_comma_inside_quotes_not_split_into_pieces) {
  EXPECT_FALSE(sourcemeta::core::http_field_list_contains_any(R"("abc,def")",
                                                              {R"("abc)"}));
}

TEST(escaped_quote_in_quoted_string_handled) {
  const std::string_view header{"\"a\\\"b,c\", \"xyz\""};
  const std::string_view token{"\"a\\\"b,c\""};
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(header, {token}));
}

TEST(weak_etag_with_comma_in_opaque_tag_kept_whole) {
  EXPECT_TRUE(sourcemeta::core::http_field_list_contains_any(
      R"(W/"abc,def", "ghi")", {R"(W/"abc,def")"}));
}

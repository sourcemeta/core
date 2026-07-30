#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

TEST(empty_header_returns_true) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "", {"text/html", "application/json"}));
}

TEST(whitespace_only_header_returns_true) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all("   ", {"text/html"}));
}

TEST(empty_media_types_returns_true) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all("text/html", {}));
}

TEST(single_exact_match) {
  EXPECT_TRUE(
      sourcemeta::core::http_accept_includes_all("text/html", {"text/html"}));
}

TEST(single_no_match) {
  EXPECT_FALSE(
      sourcemeta::core::http_accept_includes_all("text/html", {"image/png"}));
}

TEST(two_media_types_both_present) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "text/html, application/json", {"text/html", "application/json"}));
}

TEST(two_media_types_only_first_present) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all(
      "text/html", {"text/html", "application/json"}));
}

TEST(two_media_types_only_second_present) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all(
      "application/json", {"text/html", "application/json"}));
}

TEST(full_wildcard_covers_every_type) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "*/*", {"text/html", "application/json", "image/png"}));
}

TEST(type_wildcard_covers_matching_subtypes) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "text/*", {"text/html", "text/plain"}));
}

TEST(type_wildcard_does_not_cover_other_type) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all(
      "text/*", {"text/html", "application/json"}));
}

TEST(q_zero_excludes_type) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all(
      "text/html;q=0, application/json", {"text/html"}));
}

TEST(q_zero_on_specific_excludes_even_when_wildcard_present) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all("text/html;q=0, */*",
                                                          {"text/html"}));
}

TEST(q_zero_on_wildcard_does_not_exclude_specific) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all("text/html, */*;q=0",
                                                         {"text/html"}));
}

TEST(case_insensitive_match) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "TEXT/HTML, Application/JSON", {"text/html", "application/json"}));
}

// RFC 9110 §12.5.1: a parameterized media range applies only to a type that
// carries the parameter, so a bare candidate without it is not included
TEST(parameterized_range_does_not_include_bare_type) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all(
      "text/html;charset=UTF-8", {"text/html"}));
}

// RFC 9110 §12.5.1: the same range does include a candidate that carries the
// parameter
TEST(parameterized_range_includes_type_with_parameter) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "text/html;charset=UTF-8", {"text/html;charset=UTF-8"}));
}

// RFC 9110 §12.5.1: a bare media range still includes a candidate that carries
// extra parameters
TEST(bare_range_includes_parameterized_candidate) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "text/plain", {"text/plain;format=flowed"}));
}

// RFC 9110 §12.5.1: any parameter after q is an accept-ext and does not affect
// matching
TEST(accept_ext_after_q_does_not_affect_inclusion) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "text/html;q=0.5;extra=foo", {"text/html"}));
}

TEST(browser_accept_covers_text_html) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
      {"text/html"}));
}

TEST(three_types_one_missing) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all(
      "text/html, application/json",
      {"text/html", "application/json", "image/png"}));
}

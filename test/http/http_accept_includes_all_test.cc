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

TEST(parameters_on_entry_ignored) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "text/html;charset=UTF-8", {"text/html"}));
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

#include <gtest/gtest.h>

#include <sourcemeta/core/http.h>

TEST(HTTP_accept_includes_all, empty_header_returns_true) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "", {"text/html", "application/json"}));
}

TEST(HTTP_accept_includes_all, whitespace_only_header_returns_true) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all("   ", {"text/html"}));
}

TEST(HTTP_accept_includes_all, empty_media_types_returns_true) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all("text/html", {}));
}

TEST(HTTP_accept_includes_all, single_exact_match) {
  EXPECT_TRUE(
      sourcemeta::core::http_accept_includes_all("text/html", {"text/html"}));
}

TEST(HTTP_accept_includes_all, single_no_match) {
  EXPECT_FALSE(
      sourcemeta::core::http_accept_includes_all("text/html", {"image/png"}));
}

TEST(HTTP_accept_includes_all, two_media_types_both_present) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "text/html, application/json", {"text/html", "application/json"}));
}

TEST(HTTP_accept_includes_all, two_media_types_only_first_present) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all(
      "text/html", {"text/html", "application/json"}));
}

TEST(HTTP_accept_includes_all, two_media_types_only_second_present) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all(
      "application/json", {"text/html", "application/json"}));
}

TEST(HTTP_accept_includes_all, full_wildcard_covers_every_type) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "*/*", {"text/html", "application/json", "image/png"}));
}

TEST(HTTP_accept_includes_all, type_wildcard_covers_matching_subtypes) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "text/*", {"text/html", "text/plain"}));
}

TEST(HTTP_accept_includes_all, type_wildcard_does_not_cover_other_type) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all(
      "text/*", {"text/html", "application/json"}));
}

TEST(HTTP_accept_includes_all, q_zero_excludes_type) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all(
      "text/html;q=0, application/json", {"text/html"}));
}

TEST(HTTP_accept_includes_all,
     q_zero_on_specific_excludes_even_when_wildcard_present) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all("text/html;q=0, */*",
                                                          {"text/html"}));
}

TEST(HTTP_accept_includes_all, q_zero_on_wildcard_does_not_exclude_specific) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all("text/html, */*;q=0",
                                                         {"text/html"}));
}

TEST(HTTP_accept_includes_all, case_insensitive_match) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "TEXT/HTML, Application/JSON", {"text/html", "application/json"}));
}

TEST(HTTP_accept_includes_all, parameters_on_entry_ignored) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "text/html;charset=UTF-8", {"text/html"}));
}

TEST(HTTP_accept_includes_all, browser_accept_covers_text_html) {
  EXPECT_TRUE(sourcemeta::core::http_accept_includes_all(
      "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
      {"text/html"}));
}

TEST(HTTP_accept_includes_all, three_types_one_missing) {
  EXPECT_FALSE(sourcemeta::core::http_accept_includes_all(
      "text/html, application/json",
      {"text/html", "application/json", "image/png"}));
}

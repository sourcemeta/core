#include <gtest/gtest.h>

#include <sourcemeta/core/http.h>

TEST(HTTP_match_accept, exact_match) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html", {"text/html", "application/json"}),
            "text/html");
}

TEST(HTTP_match_accept, exact_match_second_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "application/json", {"text/html", "application/json"}),
            "application/json");
}

TEST(HTTP_match_accept, no_match_returns_empty) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "image/png", {"text/html", "application/json"}),
            "");
}

TEST(HTTP_match_accept, empty_header_returns_first_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "", {"text/html", "application/json"}),
            "text/html");
}

TEST(HTTP_match_accept, type_wildcard_matches_subtype) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/*", {"text/html", "application/json"}),
            "text/html");
}

TEST(HTTP_match_accept, type_wildcard_does_not_match_other_type) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("image/*", {"text/html"}), "");
}

TEST(HTTP_match_accept, full_wildcard_matches_first_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "*/*", {"text/html", "application/json"}),
            "text/html");
}

TEST(HTTP_match_accept, q_value_orders_candidates) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("application/json, text/html;q=0.5",
                                          {"text/html", "application/json"}),
      "application/json");
}

TEST(HTTP_match_accept, q_value_zero_excludes_entry) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0, application/json;q=0.5",
                {"text/html", "application/json"}),
            "application/json");
}

TEST(HTTP_match_accept, q_value_zero_on_all_returns_empty) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/html;q=0, application/json;q=0",
                                          {"text/html", "application/json"}),
      "");
}

TEST(HTTP_match_accept, specificity_prefers_exact_over_wildcard) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/*;q=0.5, application/json",
                                          {"text/html", "application/json"}),
      "application/json");
}

TEST(HTTP_match_accept, candidate_specificity_breaks_q_tie_between_candidates) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("*/*;q=0.5, application/json;q=0.5",
                                          {"text/html", "application/json"}),
      "application/json");
}

TEST(HTTP_match_accept,
     candidate_specificity_breaks_q_tie_even_when_candidate_order_reversed) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("*/*;q=0.5, application/json;q=0.5",
                                          {"application/json", "text/html"}),
      "application/json");
}

TEST(HTTP_match_accept, specificity_prefers_type_wildcard_over_full_wildcard) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/*;q=0.5, */*;q=0.5", {"text/html", "application/json"}),
            "text/html");
}

TEST(HTTP_match_accept, q_value_tie_broken_by_candidate_order) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0.5, application/json;q=0.5",
                {"text/html", "application/json"}),
            "text/html");
}

TEST(HTTP_match_accept, q_value_tie_broken_by_candidate_order_reversed) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0.5, application/json;q=0.5",
                {"application/json", "text/html"}),
            "application/json");
}

TEST(HTTP_match_accept, ignores_non_q_parameters) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;level=2;q=0.4, application/json;q=0.3",
                {"text/html", "application/json"}),
            "text/html");
}

TEST(HTTP_match_accept, q_value_default_is_one) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/html, application/json;q=0.9",
                                          {"application/json", "text/html"}),
      "text/html");
}

TEST(HTTP_match_accept, empty_candidate_list_returns_empty) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/html", {}), "");
}

TEST(HTTP_match_accept, whitespace_around_entries_tolerated) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/html , application/json",
                                                {"text/html"}),
            "text/html");
}

TEST(HTTP_match_accept, whitespace_around_q_parameter_tolerated) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html; q=0.5, application/json; q=0.9",
                {"text/html", "application/json"}),
            "application/json");
}

TEST(HTTP_match_accept, three_candidates_picks_best_q) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0.3, application/json;q=0.9, text/plain;q=0.5",
                {"text/html", "application/json", "text/plain"}),
            "application/json");
}

TEST(HTTP_match_accept, malformed_q_treated_as_one) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/html;q=abc", {"text/html"}),
      "text/html");
}

TEST(HTTP_match_accept, html_or_json_returns_html_for_html_accept) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html", {"text/html", "application/json"}),
            "text/html");
}

TEST(HTTP_match_accept, html_or_json_returns_html_for_browser_accept) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept(
          "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
          {"text/html", "application/json"}),
      "text/html");
}

TEST(HTTP_match_accept, html_or_json_returns_json_for_json_accept) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "application/json", {"text/html", "application/json"}),
            "application/json");
}

TEST(HTTP_match_accept, whitespace_only_header_returns_first_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("   ", {"text/html"}),
            "text/html");
}

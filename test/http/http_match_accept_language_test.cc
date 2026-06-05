#include <gtest/gtest.h>

#include <sourcemeta/core/http.h>

TEST(HTTP_match_accept_language, exact_match) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("en", {"en", "fr"}),
            "en");
}

TEST(HTTP_match_accept_language, prefix_match_at_subtag_boundary) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("en-US", {"en", "fr"}),
            "en");
}

TEST(HTTP_match_accept_language, longer_prefix_match) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("en-US-x-foo", {"en"}),
            "en");
}

TEST(HTTP_match_accept_language, candidate_more_specific_than_range_no_match) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("en", {"en-US"}), "");
}

TEST(HTTP_match_accept_language, wildcard_returns_first_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("*", {"en", "fr"}),
            "en");
}

TEST(HTTP_match_accept_language, empty_header_returns_first_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("", {"en", "fr"}),
            "en");
}

TEST(HTTP_match_accept_language, q_value_prefers_higher) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("en;q=0.5, fr;q=0.9",
                                                         {"en", "fr"}),
            "fr");
}

TEST(HTTP_match_accept_language, q_value_zero_excludes_entry) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept_language("en;q=0, fr", {"en", "fr"}),
      "fr");
}

TEST(HTTP_match_accept_language, q_value_default_is_one) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("fr-CA;q=0.9, en",
                                                         {"en", "fr"}),
            "en");
}

TEST(HTTP_match_accept_language, prefix_match_with_q_value) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept_language("fr-CA;q=0.9", {"en", "fr"}),
      "fr");
}

TEST(HTTP_match_accept_language, no_match_returns_empty) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("de", {"en", "fr"}),
            "");
}

TEST(HTTP_match_accept_language, empty_candidate_list_returns_empty) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("en", {}), "");
}

TEST(HTTP_match_accept_language, candidate_order_breaks_q_tie) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("en;q=0.5, fr;q=0.5",
                                                         {"fr", "en"}),
            "fr");
}

TEST(HTTP_match_accept_language, exact_match_outranks_prefix_at_same_q) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("en-US, fr",
                                                         {"en-US", "fr"}),
            "en-US");
}

TEST(HTTP_match_accept_language, wildcard_loses_to_exact_match) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("*, fr", {"en", "fr"}),
            "fr");
}

TEST(HTTP_match_accept_language, hyphen_not_at_boundary_does_not_match) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("enx-US", {"en"}), "");
}

TEST(HTTP_match_accept_language, multi_token_range_truncates_correctly) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("zh-Hant-TW", {"zh"}),
            "zh");
}

TEST(HTTP_match_accept_language, three_candidates_picks_best) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language(
                "en;q=0.3, de;q=0.7, fr;q=0.9", {"en", "de", "fr"}),
            "fr");
}

TEST(HTTP_match_accept_language,
     whitespace_only_header_returns_first_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept_language("   ", {"en"}), "en");
}

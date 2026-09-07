#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <string_view> // std::string_view

TEST(exact_beats_every_wildcard) {
  const std::array<std::string_view, 3> ranges{{"*/*", "text/*", "text/plain"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range("text/plain", ranges),
            "text/plain");
}

TEST(subtype_wildcard_beats_full_wildcard) {
  const std::array<std::string_view, 2> ranges{{"*/*", "text/*"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range("text/plain", ranges),
            "text/*");
}

TEST(full_wildcard_alone) {
  const std::array<std::string_view, 1> ranges{{"*/*"}};
  EXPECT_EQ(
      sourcemeta::core::http_match_media_range("application/json", ranges),
      "*/*");
}

TEST(order_does_not_affect_specificity) {
  const std::array<std::string_view, 3> ranges{{"text/plain", "text/*", "*/*"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range("text/plain", ranges),
            "text/plain");
}

TEST(no_match) {
  const std::array<std::string_view, 2> ranges{{"text/*", "image/png"}};
  EXPECT_EQ(
      sourcemeta::core::http_match_media_range("application/json", ranges), "");
}

TEST(no_ranges) {
  const std::array<std::string_view, 0> ranges{};
  EXPECT_EQ(
      sourcemeta::core::http_match_media_range("application/json", ranges), "");
}

TEST(type_and_subtype_are_case_insensitive) {
  const std::array<std::string_view, 1> ranges{{"TEXT/PLAIN"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range("text/plain", ranges),
            "TEXT/PLAIN");
}

TEST(wildcard_type_is_case_insensitive) {
  const std::array<std::string_view, 1> ranges{{"TEXT/*"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range("text/plain", ranges),
            "TEXT/*");
}

TEST(candidate_parameters_do_not_prevent_a_bare_range) {
  const std::array<std::string_view, 1> ranges{{"text/plain"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range(
                "text/plain; charset=UTF-8", ranges),
            "text/plain");
}

TEST(range_parameter_is_more_specific) {
  const std::array<std::string_view, 2> ranges{
      {"text/plain", "text/plain; charset=UTF-8"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range(
                "text/plain; charset=UTF-8", ranges),
            "text/plain; charset=UTF-8");
}

TEST(range_parameter_the_candidate_lacks_does_not_match) {
  const std::array<std::string_view, 1> ranges{{"text/plain; charset=UTF-8"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range("text/plain", ranges), "");
}

TEST(quoted_and_unquoted_parameter_values_are_equivalent) {
  const std::array<std::string_view, 1> ranges{
      {"text/plain; charset=\"utf-8\""}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range(
                "text/plain; charset=utf-8", ranges),
            "text/plain; charset=\"utf-8\"");
}

TEST(first_of_equally_specific_ranges_wins) {
  const std::array<std::string_view, 2> ranges{{"text/*", "text/*"}};
  EXPECT_EQ(
      sourcemeta::core::http_match_media_range("text/plain", ranges).data(),
      ranges.at(0).data());
}

TEST(structured_syntax_suffix_matches_a_type_wildcard) {
  const std::array<std::string_view, 2> ranges{{"*/*", "application/*"}};
  EXPECT_EQ(
      sourcemeta::core::http_match_media_range("application/geo+json", ranges),
      "application/*");
}

TEST(invalid_media_type) {
  const std::array<std::string_view, 1> ranges{{"*/*"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range("application", ranges),
            "");
}

TEST(invalid_media_type_with_a_comment) {
  const std::array<std::string_view, 1> ranges{{"*/*"}};
  EXPECT_EQ(
      sourcemeta::core::http_match_media_range("text/plain(comment)", ranges),
      "");
}

TEST(invalid_range_is_ignored) {
  const std::array<std::string_view, 2> ranges{{"garbage", "text/plain"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range("text/plain", ranges),
            "text/plain");
}

TEST(the_range_is_returned_as_given) {
  const std::array<std::string_view, 1> ranges{{"  text/plain  "}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range("text/plain", ranges),
            "  text/plain  ");
}

TEST(a_weight_on_a_range_is_not_a_media_type_parameter) {
  const std::array<std::string_view, 1> ranges{{"text/plain;q=0.8"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range("text/plain", ranges),
            "text/plain;q=0.8");
}

TEST(a_weight_after_a_media_type_parameter) {
  const std::array<std::string_view, 1> ranges{
      {"text/plain;charset=utf-8;q=0.8"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range(
                "text/plain; charset=utf-8", ranges),
            "text/plain;charset=utf-8;q=0.8");
}

TEST(parameters_after_a_weight_are_not_media_type_parameters) {
  const std::array<std::string_view, 1> ranges{
      {"text/plain;q=0.8;charset=utf-8"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range("text/plain", ranges),
            "text/plain;q=0.8;charset=utf-8");
}

TEST(a_weight_does_not_affect_specificity) {
  const std::array<std::string_view, 2> ranges{
      {"text/*;q=1", "text/plain;q=0.1"}};
  EXPECT_EQ(sourcemeta::core::http_match_media_range("text/plain", ranges),
            "text/plain;q=0.1");
}

TEST(malformed_parameters_do_not_prevent_matching) {
  const std::array<std::string_view, 1> ranges{{"text/plain"}};
  EXPECT_EQ(
      sourcemeta::core::http_match_media_range("text/plain; charset", ranges),
      "text/plain");
}

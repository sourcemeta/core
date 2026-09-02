#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

// RFC 9110 §12.5.1 Table 5 example Accept header
static constexpr std::string_view RFC9110_TABLE5_ACCEPT{
    "text/*;q=0.3, text/plain;q=0.7, text/plain;format=flowed, "
    "text/plain;format=fixed;q=0.4, */*;q=0.5"};

// RFC 9110 §12.5.1 media-range precedence example
static constexpr std::string_view RFC9110_PRECEDENCE_ACCEPT{
    "text/*, text/plain, text/plain;format=flowed, */*"};

// Historic RFC 2616 §14.1 level example, retained by RFC 9110 §12.5.1 semantics
static constexpr std::string_view RFC9110_LEVEL_ACCEPT{
    "text/*;q=0.3, text/html;q=0.7, text/html;level=1, "
    "text/html;level=2;q=0.4, */*;q=0.5"};

TEST(exact_match) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html", {"text/html", "application/json"}),
            "text/html");
}

TEST(exact_match_second_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "application/json", {"text/html", "application/json"}),
            "application/json");
}

TEST(no_match_returns_empty) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "image/png", {"text/html", "application/json"}),
            "");
}

TEST(empty_header_returns_first_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "", {"text/html", "application/json"}),
            "text/html");
}

TEST(type_wildcard_matches_subtype) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/*", {"text/html", "application/json"}),
            "text/html");
}

TEST(type_wildcard_does_not_match_other_type) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("image/*", {"text/html"}), "");
}

TEST(full_wildcard_matches_first_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "*/*", {"text/html", "application/json"}),
            "text/html");
}

TEST(q_value_orders_candidates) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("application/json, text/html;q=0.5",
                                          {"text/html", "application/json"}),
      "application/json");
}

TEST(q_value_zero_excludes_entry) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0, application/json;q=0.5",
                {"text/html", "application/json"}),
            "application/json");
}

TEST(q_value_zero_on_all_returns_empty) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/html;q=0, application/json;q=0",
                                          {"text/html", "application/json"}),
      "");
}

TEST(specificity_prefers_exact_over_wildcard) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/*;q=0.5, application/json",
                                          {"text/html", "application/json"}),
      "application/json");
}

TEST(candidate_specificity_breaks_q_tie_between_candidates) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("*/*;q=0.5, application/json;q=0.5",
                                          {"text/html", "application/json"}),
      "application/json");
}

TEST(candidate_specificity_breaks_q_tie_even_when_candidate_order_reversed) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("*/*;q=0.5, application/json;q=0.5",
                                          {"application/json", "text/html"}),
      "application/json");
}

TEST(specificity_prefers_type_wildcard_over_full_wildcard) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/*;q=0.5, */*;q=0.5", {"text/html", "application/json"}),
            "text/html");
}

TEST(q_value_tie_broken_by_candidate_order) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0.5, application/json;q=0.5",
                {"text/html", "application/json"}),
            "text/html");
}

TEST(q_value_tie_broken_by_candidate_order_reversed) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0.5, application/json;q=0.5",
                {"application/json", "text/html"}),
            "application/json");
}

// RFC 9110 §12.5.1: a media range that carries a media-type parameter only
// applies to a type that also carries it, so a parameterized range does not
// lend its weight to a bare candidate that lacks the parameter
TEST(media_type_parameter_must_be_carried_by_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;level=2;q=0.4, application/json;q=0.3",
                {"text/html", "application/json"}),
            "application/json");
}

TEST(q_value_default_is_one) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/html, application/json;q=0.9",
                                          {"application/json", "text/html"}),
      "text/html");
}

TEST(empty_candidate_list_returns_empty) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/html", {}), "");
}

TEST(whitespace_around_entries_tolerated) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/html , application/json",
                                                {"text/html"}),
            "text/html");
}

TEST(whitespace_around_q_parameter_tolerated) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html; q=0.5, application/json; q=0.9",
                {"text/html", "application/json"}),
            "application/json");
}

TEST(three_candidates_picks_best_q) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0.3, application/json;q=0.9, text/plain;q=0.5",
                {"text/html", "application/json", "text/plain"}),
            "application/json");
}

// RFC 9110 §12.4.2: a malformed weight is a fail-safe refusal, treated as 0
TEST(malformed_q_treated_as_zero) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/html;q=abc", {"text/html"}),
      "");
}

// RFC 9110 §5.6.6: a semicolon inside a quoted string does not separate
// parameters, so the quoted content does not synthesise a phantom q parameter.
// The text/html range therefore keeps its default weight of 1.0 and, matching
// the candidate that carries the same parameter, outranks application/json
TEST(quoted_parameter_semicolon_not_split) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;foo=\"a;q=0.1\", application/json;q=0.5",
                {"text/html;foo=\"a;q=0.1\"", "application/json"}),
            "text/html;foo=\"a;q=0.1\"");
}

TEST(html_or_json_returns_html_for_html_accept) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html", {"text/html", "application/json"}),
            "text/html");
}

TEST(html_or_json_returns_html_for_browser_accept) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept(
          "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
          {"text/html", "application/json"}),
      "text/html");
}

TEST(html_or_json_returns_json_for_json_accept) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "application/json", {"text/html", "application/json"}),
            "application/json");
}

TEST(whitespace_only_header_returns_first_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("   ", {"text/html"}),
            "text/html");
}

TEST(case_insensitive_exact_match) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("Text/HTML", {"text/html"}),
            "text/html");
}

TEST(case_insensitive_uppercase_request) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "TEXT/HTML", {"text/html", "application/json"}),
            "text/html");
}

TEST(case_insensitive_type_wildcard) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("TEXT/*", {"text/html"}),
            "text/html");
}

TEST(case_insensitive_mixed_case_subtype) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("application/JSON, text/html;q=0.5",
                                          {"text/html", "application/json"}),
      "application/json");
}

TEST(q_value_boundary_at_zero_point_one) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0.1, application/json;q=0.2",
                {"text/html", "application/json"}),
            "application/json");
}

TEST(q_value_three_decimal_digits) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0.999, application/json;q=0.998",
                {"text/html", "application/json"}),
            "text/html");
}

TEST(q_value_one_no_decimal) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=1, application/json;q=0.999",
                {"text/html", "application/json"}),
            "text/html");
}

TEST(leading_and_trailing_comma_tolerated) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(",text/html,", {"text/html"}),
            "text/html");
}

TEST(many_empty_entries_tolerated) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept(", , text/html , ,", {"text/html"}),
      "text/html");
}

// RFC 9110 §12.5.1: a parameterized media range applies only to a type that
// carries the parameter, so a bare candidate without it is not acceptable
TEST(parameterized_range_does_not_match_bare_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/html;charset=UTF-8",
                                                {"text/html"}),
            "");
}

// RFC 9110 §12.5.1: the same range does match a candidate that carries the
// parameter
TEST(parameterized_range_matches_candidate_with_parameter) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/html;charset=UTF-8",
                                                {"text/html;charset=UTF-8"}),
            "text/html;charset=UTF-8");
}

TEST(type_wildcard_specificity_beats_full_wildcard) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/*, */*", {"text/html"}),
            "text/html");
}

TEST(candidate_with_charset_in_name_treated_verbatim) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("image/png",
                                                {"image/png", "image/jpeg"}),
            "image/png");
}

TEST(parameter_after_q_is_ignored) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0.5;extra=foo, application/json;q=0.4",
                {"text/html", "application/json"}),
            "text/html");
}

TEST(more_specific_match_overrides_higher_q_wildcard_for_same_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/html;q=0.5, text/*;q=1.0",
                                                {"text/html", "text/plain"}),
            "text/plain");
}

TEST(specific_low_q_wins_over_wildcard_high_q_within_candidate) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/html;q=0.5, */*;q=1.0",
                                          {"text/html", "application/json"}),
      "application/json");
}

// RFC 9110 §5.6.6: a comma inside a quoted string does not split the list, so
// the whole quoted parameter stays with the text/html range and matches the
// candidate that carries it
TEST(quoted_string_with_comma_does_not_split_entry) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                R"(text/html;profile="urn:a,b", application/json;q=0.5)",
                {R"(text/html;profile="urn:a,b")", "application/json"}),
            R"(text/html;profile="urn:a,b")");
}

// RFC 9110 §5.6.4: an escaped quote inside a quoted string is content, so the
// quoted parameter is preserved intact and matches the candidate that carries
// it
TEST(escaped_quote_inside_quoted_string_handled) {
  const std::string_view header{
      R"(text/html;profile="a\"b,c", application/json;q=0.5)"};
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                header, {"text/html;profile=\"a\\\"b,c\"", "application/json"}),
            "text/html;profile=\"a\\\"b,c\"");
}

TEST(q_value_zero_dot_no_digits_is_zero) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0., application/json;q=0.5",
                {"text/html", "application/json"}),
            "application/json");
}

TEST(q_value_one_dot_no_digits_is_one) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=1., application/json;q=0.5",
                {"text/html", "application/json"}),
            "text/html");
}

// RFC 9110 §12.4.2: qvalue allows at most three fractional digits, so a
// four-digit fraction is malformed and is a fail-safe refusal treated as 0
TEST(q_value_four_decimal_digits_treated_as_zero) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;q=0.1234, application/json;q=0.5",
                {"text/html", "application/json"}),
            "application/json");
}

// RFC 9110 §12.5.1 Table 5 associates text/plain;format=flowed with 1.0, so it
// outranks text/plain at 0.7
TEST(table5_flowed_beats_plain) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept(
          RFC9110_TABLE5_ACCEPT, {"text/plain;format=flowed", "text/plain"}),
      "text/plain;format=flowed");
}

// RFC 9110 §12.5.1 Table 5 associates text/plain with 0.7, so it outranks
// image/jpeg at 0.5
TEST(table5_plain_beats_jpeg) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(RFC9110_TABLE5_ACCEPT,
                                                {"text/plain", "image/jpeg"}),
            "text/plain");
}

// RFC 9110 §12.5.1 Table 5 associates image/jpeg with 0.5, so it outranks
// text/plain;format=fixed at 0.4
TEST(table5_jpeg_beats_fixed) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept(
          RFC9110_TABLE5_ACCEPT, {"image/jpeg", "text/plain;format=fixed"}),
      "image/jpeg");
}

// RFC 9110 §12.5.1 Table 5 associates text/plain;format=fixed with 0.4, so it
// outranks text/html at 0.3
TEST(table5_fixed_beats_html) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept(
          RFC9110_TABLE5_ACCEPT, {"text/plain;format=fixed", "text/html"}),
      "text/plain;format=fixed");
}

// RFC 9110 §12.5.1 Table 5 associates both text/html and text/html;level=1 with
// 0.3 through the text/* range, so the tie falls to candidate order
TEST(table5_html_and_level1_tie_on_candidate_order) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                RFC9110_TABLE5_ACCEPT, {"text/html", "text/html;level=1"}),
            "text/html");
}

// RFC 9110 §12.5.1 Table 5: text/plain;format=flowed at 1.0 is the overall best
TEST(table5_flowed_is_overall_best) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                RFC9110_TABLE5_ACCEPT,
                {"text/html", "image/jpeg", "text/plain;format=fixed",
                 "text/plain", "text/plain;format=flowed"}),
            "text/plain;format=flowed");
}

// RFC 9110 §12.5.1 Table 5: with the flowed candidate removed, text/plain at
// 0.7 is the best remaining
TEST(table5_plain_is_best_without_flowed) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept(
          RFC9110_TABLE5_ACCEPT,
          {"text/html", "image/jpeg", "text/plain;format=fixed", "text/plain"}),
      "text/plain");
}

// RFC 9110 §12.5.1: with equal weights the most specific media range has
// precedence, so text/plain;format=flowed outranks text/plain
TEST(precedence_flowed_over_plain) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                RFC9110_PRECEDENCE_ACCEPT,
                {"text/plain", "text/plain;format=flowed"}),
            "text/plain;format=flowed");
}

// RFC 9110 §12.5.1: text/plain (matched by the type/subtype range) outranks
// image/jpeg (matched only by */*)
TEST(precedence_plain_over_wildcard_only) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(RFC9110_PRECEDENCE_ACCEPT,
                                                {"text/plain", "image/jpeg"}),
            "text/plain");
}

// RFC 9110 §12.5.1 level example: text/html;level=1 at 1.0 outranks text/html
// at 0.7
TEST(level_example_level1_beats_html) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                RFC9110_LEVEL_ACCEPT, {"text/html;level=1", "text/html"}),
            "text/html;level=1");
}

// RFC 9110 §12.5.1 level example: text/html at 0.7 outranks image/jpeg at 0.5
TEST(level_example_html_beats_jpeg) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(RFC9110_LEVEL_ACCEPT,
                                                {"text/html", "image/jpeg"}),
            "text/html");
}

// RFC 9110 §12.5.1 level example: image/jpeg at 0.5 outranks text/html;level=2
// at 0.4
TEST(level_example_jpeg_beats_level2) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                RFC9110_LEVEL_ACCEPT, {"image/jpeg", "text/html;level=2"}),
            "image/jpeg");
}

// RFC 9110 §12.5.1 level example: text/html;level=2 at 0.4 outranks text/plain
// at 0.3
TEST(level_example_level2_beats_plain) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                RFC9110_LEVEL_ACCEPT, {"text/html;level=2", "text/plain"}),
            "text/html;level=2");
}

// RFC 9110 §12.5.1 level example: text/html;level=3 falls back to text/html at
// 0.7, tying text/html so candidate order decides
TEST(level_example_level3_matches_html_weight) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                RFC9110_LEVEL_ACCEPT, {"text/html;level=3", "text/html"}),
            "text/html;level=3");
}

TEST(range_without_slash_is_ignored) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("foo, application/json",
                                                {"application/json"}),
            "application/json");
}

TEST(range_without_slash_matches_nothing) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("foo", {"application/json"}),
            "");
}

TEST(type_wildcard_with_different_slash_position_matches_nothing) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/*", {"ab/cd"}), "");
}

TEST(type_wildcard_with_different_type_of_same_length_matches_nothing) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/*", {"abcd/efg"}), "");
}

TEST(empty_q_value_excludes_the_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/html;q=", {"text/html"}),
            "");
}

TEST(q_parameter_without_a_value_excludes_the_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/html;q", {"text/html"}),
            "");
}

TEST(q_value_above_one_excludes_the_candidate) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/html;q=1.5", {"text/html"}),
      "");
}

TEST(q_value_with_leading_digit_above_one_excludes_the_candidate) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/html;q=2", {"text/html"}),
            "");
}

TEST(q_value_without_a_dot_separator_excludes_the_candidate) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/html;q=0x5", {"text/html"}),
      "");
}

TEST(q_value_with_a_non_numeric_fraction_excludes_the_candidate) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/html;q=0.a", {"text/html"}),
      "");
}

TEST(q_value_with_too_many_digits_excludes_the_candidate) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/html;q=0.1234", {"text/html"}),
      "");
}

TEST(uppercase_q_value_is_recognised) {
  EXPECT_EQ(sourcemeta::core::http_match_accept("text/html;Q=0", {"text/html"}),
            "");
}

TEST(empty_parameter_before_the_q_value_is_skipped) {
  EXPECT_EQ(
      sourcemeta::core::http_match_accept("text/html;;q=0.5", {"text/html"}),
      "text/html");
}

TEST(range_with_multiple_parameters_matches_a_candidate_with_them) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;a=1;b=2", {"text/html;a=1;b=2", "text/html"}),
            "text/html;a=1;b=2");
}

TEST(range_with_a_quoted_escaped_parameter_value) {
  const std::string_view header{R"(text/html;title="a\"b")"};
  const std::string_view candidate{R"(text/html;title="a\"b")"};
  EXPECT_EQ(sourcemeta::core::http_match_accept(header, {candidate}),
            candidate);
}

TEST(range_with_a_flag_parameter_matches_a_candidate_with_it) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;flag", {"text/html;flag", "text/plain"}),
            "text/html;flag");
}

TEST(range_parameters_with_whitespace_after_semicolon) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(
                "text/html;a=1; b=2", {"text/html;b=2;a=1", "text/plain"}),
            "text/html;b=2;a=1");
}

TEST(range_with_an_empty_type_is_ignored) {
  EXPECT_EQ(sourcemeta::core::http_match_accept(";q=0.5, application/json",
                                                {"application/json"}),
            "application/json");
}

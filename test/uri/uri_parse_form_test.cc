#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <string>  // std::string
#include <utility> // std::pair
#include <vector>  // std::vector

using Parameters = std::vector<std::pair<std::string, std::string>>;

TEST(two_pairs) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a=1&b=2", parameters);
  EXPECT_EQ(parameters.size(), 2);
  EXPECT_EQ(parameters.at(0).first, "a");
  EXPECT_EQ(parameters.at(0).second, "1");
  EXPECT_EQ(parameters.at(1).first, "b");
  EXPECT_EQ(parameters.at(1).second, "2");
}

TEST(plus_becomes_space) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a+b=c+d", parameters);
  EXPECT_EQ(parameters.size(), 1);
  EXPECT_EQ(parameters.at(0).first, "a b");
  EXPECT_EQ(parameters.at(0).second, "c d");
}

TEST(percent_escapes_are_decoded) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a%2Fb=c%26d", parameters);
  EXPECT_EQ(parameters.size(), 1);
  EXPECT_EQ(parameters.at(0).first, "a/b");
  EXPECT_EQ(parameters.at(0).second, "c&d");
}

TEST(lowercase_hexadecimal_escapes_are_decoded) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a=%c2%a3", parameters);
  EXPECT_EQ(parameters.size(), 1);
  EXPECT_EQ(parameters.at(0).first, "a");
  EXPECT_EQ(parameters.at(0).second, "\xC2\xA3");
}

TEST(a_sequence_without_an_equals_has_an_empty_value) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a&b=2", parameters);
  EXPECT_EQ(parameters.size(), 2);
  EXPECT_EQ(parameters.at(0).first, "a");
  EXPECT_EQ(parameters.at(0).second, "");
  EXPECT_EQ(parameters.at(1).first, "b");
  EXPECT_EQ(parameters.at(1).second, "2");
}

TEST(a_leading_equals_gives_an_empty_name) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("=1", parameters);
  EXPECT_EQ(parameters.size(), 1);
  EXPECT_EQ(parameters.at(0).first, "");
  EXPECT_EQ(parameters.at(0).second, "1");
}

TEST(a_trailing_equals_gives_an_empty_value) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a=", parameters);
  EXPECT_EQ(parameters.size(), 1);
  EXPECT_EQ(parameters.at(0).first, "a");
  EXPECT_EQ(parameters.at(0).second, "");
}

TEST(only_the_first_equals_splits) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a=b=c", parameters);
  EXPECT_EQ(parameters.size(), 1);
  EXPECT_EQ(parameters.at(0).first, "a");
  EXPECT_EQ(parameters.at(0).second, "b=c");
}

TEST(empty_sequences_are_skipped) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a=1&&b=2", parameters);
  EXPECT_EQ(parameters.size(), 2);
  EXPECT_EQ(parameters.at(0).first, "a");
  EXPECT_EQ(parameters.at(0).second, "1");
  EXPECT_EQ(parameters.at(1).first, "b");
  EXPECT_EQ(parameters.at(1).second, "2");
}

TEST(only_ampersands_yield_no_pairs) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("&&&", parameters);
  EXPECT_TRUE(parameters.empty());
}

TEST(a_lone_equals_is_a_pair) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a=1&=&b=2", parameters);
  EXPECT_EQ(parameters.size(), 3);
  EXPECT_EQ(parameters.at(0).first, "a");
  EXPECT_EQ(parameters.at(0).second, "1");
  EXPECT_EQ(parameters.at(1).first, "");
  EXPECT_EQ(parameters.at(1).second, "");
  EXPECT_EQ(parameters.at(2).first, "b");
  EXPECT_EQ(parameters.at(2).second, "2");
}

TEST(repeats_are_kept_in_order) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a=1&a=2&a=3", parameters);
  EXPECT_EQ(parameters.size(), 3);
  EXPECT_EQ(parameters.at(0).first, "a");
  EXPECT_EQ(parameters.at(0).second, "1");
  EXPECT_EQ(parameters.at(1).first, "a");
  EXPECT_EQ(parameters.at(1).second, "2");
  EXPECT_EQ(parameters.at(2).first, "a");
  EXPECT_EQ(parameters.at(2).second, "3");
}

TEST(a_trailing_ampersand_yields_no_extra_pair) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a=1&", parameters);
  EXPECT_EQ(parameters.size(), 1);
  EXPECT_EQ(parameters.at(0).first, "a");
  EXPECT_EQ(parameters.at(0).second, "1");
}

TEST(empty_input_yields_no_pairs) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("", parameters);
  EXPECT_TRUE(parameters.empty());
}

TEST(an_incomplete_escape_is_kept_as_it_appears) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a=%zz", parameters);
  EXPECT_EQ(parameters.size(), 1);
  EXPECT_EQ(parameters.at(0).first, "a");
  EXPECT_EQ(parameters.at(0).second, "%zz");
}

TEST(a_trailing_percent_is_kept_as_it_appears) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a=%", parameters);
  EXPECT_EQ(parameters.size(), 1);
  EXPECT_EQ(parameters.at(0).first, "a");
  EXPECT_EQ(parameters.at(0).second, "%");
}

TEST(a_truncated_escape_is_kept_as_it_appears) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("a=%4", parameters);
  EXPECT_EQ(parameters.size(), 1);
  EXPECT_EQ(parameters.at(0).first, "a");
  EXPECT_EQ(parameters.at(0).second, "%4");
}

TEST(appends_to_an_existing_container) {
  Parameters parameters;
  parameters.emplace_back("existing", "value");
  sourcemeta::core::URI::parse_form("a=1", parameters);
  EXPECT_EQ(parameters.size(), 2);
  EXPECT_EQ(parameters.at(0).first, "existing");
  EXPECT_EQ(parameters.at(0).second, "value");
  EXPECT_EQ(parameters.at(1).first, "a");
  EXPECT_EQ(parameters.at(1).second, "1");
}

TEST(rfc6749_appendix_b_example) {
  Parameters parameters;
  sourcemeta::core::URI::parse_form("value=+%25%26%2B%C2%A3%E2%82%AC",
                                    parameters);
  EXPECT_EQ(parameters.size(), 1);
  EXPECT_EQ(parameters.at(0).first, "value");
  EXPECT_EQ(parameters.at(0).second, " %&+\xC2\xA3\xE2\x82\xAC");
}

TEST(round_trip_through_the_serializer) {
  std::string body;
  sourcemeta::core::URI::append_form_parameter(body, "a b", "c&d=e");
  sourcemeta::core::URI::append_form_parameter(body, "~", "*");
  Parameters parameters;
  sourcemeta::core::URI::parse_form(body, parameters);
  EXPECT_EQ(parameters.size(), 2);
  EXPECT_EQ(parameters.at(0).first, "a b");
  EXPECT_EQ(parameters.at(0).second, "c&d=e");
  EXPECT_EQ(parameters.at(1).first, "~");
  EXPECT_EQ(parameters.at(1).second, "*");
}

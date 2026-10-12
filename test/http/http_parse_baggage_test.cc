#include <sourcemeta/core/http_baggage.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

TEST(the_example_from_the_specification) {
  std::vector<std::string> flat;
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage(
      "userId=alice,serverNode=DF%2028,isProduction=false",
      [&flat](const auto key, const auto value, const auto properties) {
        flat.push_back(std::string{key} + "=" + std::string{value} + "[" +
                       std::string{properties} + "]");
      }));
  EXPECT_EQ(flat.size(), 3);
  EXPECT_EQ(flat.at(0), "userId=alice[]");
  EXPECT_EQ(flat.at(1), "serverNode=DF%2028[]");
  EXPECT_EQ(flat.at(2), "isProduction=false[]");
}

// A key may repeat and the order of members is significant, so arrival order
// is what is reported
// https://www.w3.org/TR/baggage/#baggage-string
TEST(duplicate_keys_are_reported_in_order) {
  std::vector<std::string> flat;
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage(
      "a=1,a=2,a=3", [&flat](const auto key, const auto value, const auto) {
        flat.push_back(std::string{key} + "=" + std::string{value});
      }));
  EXPECT_EQ(flat.size(), 3);
  EXPECT_EQ(flat.at(0), "a=1");
  EXPECT_EQ(flat.at(1), "a=2");
  EXPECT_EQ(flat.at(2), "a=3");
}

// The property set is handed over whole, so a caller that does not care about
// it pays nothing, and one that does can walk it
// https://www.w3.org/TR/baggage/#property
TEST(the_property_set_is_reported_whole) {
  std::string properties;
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage(
      "key1=value1;property1;property2",
      [&properties](const auto, const auto, const auto trailing) {
        properties = std::string{trailing};
      }));
  EXPECT_EQ(properties, "property1;property2");
}

TEST(a_member_without_properties_reports_an_empty_set) {
  std::string properties{"unset"};
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage(
      "a=1", [&properties](const auto, const auto, const auto trailing) {
        properties = std::string{trailing};
      }));
  EXPECT_TRUE(properties.empty());
}

// Whitespace around a key or a value is not part of it, so what is reported is
// the member itself
// https://www.w3.org/TR/baggage/#key
TEST(whitespace_is_not_reported) {
  std::string flat;
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage(
      "  userId = alice  ",
      [&flat](const auto key, const auto value, const auto) {
        flat = std::string{key} + "=" + std::string{value};
      }));
  EXPECT_EQ(flat, "userId=alice");
}

TEST(an_empty_value_is_reported_as_empty) {
  std::string flat{"unset"};
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage(
      "a=", [&flat](const auto, const auto value, const auto) {
        flat = std::string{value};
      }));
  EXPECT_TRUE(flat.empty());
}

// Only the first equals sign separates, so the rest belongs to the value
// https://www.w3.org/TR/baggage/#value
TEST(a_value_carrying_an_equals_sign_is_reported_whole) {
  std::string flat;
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage(
      "a=b=c", [&flat](const auto, const auto value, const auto) {
        flat = std::string{value};
      }));
  EXPECT_EQ(flat, "b=c");
}

// The whole input is checked before anything is visited, so a caller never
// propagates a pair out of a value it then discards
TEST(a_malformed_value_visits_nothing) {
  bool visited{false};
  EXPECT_FALSE(sourcemeta::core::http_parse_baggage(
      "a=1,novalue",
      [&visited](const auto, const auto, const auto) { visited = true; }));
  EXPECT_FALSE(visited);
}

TEST(an_empty_field_value_visits_nothing) {
  bool visited{false};
  EXPECT_FALSE(sourcemeta::core::http_parse_baggage(
      "", [&visited](const auto, const auto, const auto) { visited = true; }));
  EXPECT_FALSE(visited);
}

// The views borrow the input rather than copying it
TEST(the_members_borrow_the_input) {
  const std::string value{"userId=alice"};
  std::string_view key;
  std::string_view reported;
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage(
      value,
      [&key, &reported](const auto member, const auto content, const auto) {
        key = member;
        reported = content;
      }));
  EXPECT_EQ(key.data(), value.data());
  EXPECT_EQ(reported.data(), value.data() + 7);
}

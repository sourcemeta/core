#include <sourcemeta/core/http_baggage.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

TEST(a_bare_entry_and_an_entry_with_a_value) {
  std::vector<std::string> flat;
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage_properties(
      "property1;propertyKey=propertyValue",
      [&flat](const auto key, const auto value) {
        flat.push_back(std::string{key} +
                       (value.has_value() ? "=" + std::string{*value} : ""));
      }));
  EXPECT_EQ(flat.size(), 2);
  EXPECT_EQ(flat.at(0), "property1");
  EXPECT_EQ(flat.at(1), "propertyKey=propertyValue");
}

// An entry may be a bare key, which is not the same as one whose value is
// empty, so the value is absent rather than empty in the first case
// https://www.w3.org/TR/baggage/#property
TEST(a_bare_key_reports_no_value) {
  bool present{true};
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage_properties(
      "flag", [&present](const auto, const auto value) {
        present = value.has_value();
      }));
  EXPECT_FALSE(present);
}

TEST(an_empty_value_reports_a_present_empty_value) {
  bool present{false};
  bool empty{false};
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage_properties(
      "flag=", [&present, &empty](const auto, const auto value) {
        present = value.has_value();
        empty = value.has_value() && value->empty();
      }));
  EXPECT_TRUE(present);
  EXPECT_TRUE(empty);
}

// Whitespace around a property key or value is not part of it
// https://www.w3.org/TR/baggage/#property
TEST(whitespace_is_not_reported) {
  std::string flat;
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage_properties(
      " k = v ", [&flat](const auto key, const auto value) {
        flat = std::string{key} +
               (value.has_value() ? "=" + std::string{*value} : "");
      }));
  EXPECT_EQ(flat, "k=v");
}

TEST(several_entries_are_reported_in_order) {
  std::vector<std::string> flat;
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage_properties(
      "one;two=2;three", [&flat](const auto key, const auto) {
        flat.push_back(std::string{key});
      }));
  EXPECT_EQ(flat.size(), 3);
  EXPECT_EQ(flat.at(0), "one");
  EXPECT_EQ(flat.at(1), "two");
  EXPECT_EQ(flat.at(2), "three");
}

// The set is checked before anything is visited, as elsewhere in this header
TEST(a_malformed_key_visits_nothing) {
  bool visited{false};
  EXPECT_FALSE(sourcemeta::core::http_parse_baggage_properties(
      "one;k(x)=v", [&visited](const auto, const auto) { visited = true; }));
  EXPECT_FALSE(visited);
}

TEST(a_malformed_value_visits_nothing) {
  bool visited{false};
  EXPECT_FALSE(sourcemeta::core::http_parse_baggage_properties(
      "k=%ZZ", [&visited](const auto, const auto) { visited = true; }));
  EXPECT_FALSE(visited);
}

TEST(an_empty_entry_visits_nothing) {
  bool visited{false};
  EXPECT_FALSE(sourcemeta::core::http_parse_baggage_properties(
      "one;;two", [&visited](const auto, const auto) { visited = true; }));
  EXPECT_FALSE(visited);
}

// The views borrow the input rather than copying it
TEST(the_entries_borrow_the_input) {
  const std::string properties{"k=v"};
  std::string_view key;
  std::string_view reported;
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage_properties(
      properties, [&key, &reported](const auto entry, const auto value) {
        key = entry;
        reported = value.value_or(std::string_view{});
      }));
  EXPECT_EQ(key.data(), properties.data());
  EXPECT_EQ(reported.data(), properties.data() + 2);
}

// The repetition that carries properties may run zero times, so the empty set
// a member without any hands over is a set of no entries and parses
// https://www.w3.org/TR/baggage/#definition
TEST(an_empty_set_parses_and_visits_nothing) {
  bool visited{false};
  EXPECT_TRUE(sourcemeta::core::http_parse_baggage_properties(
      "", [&visited](const auto, const auto) { visited = true; }));
  EXPECT_FALSE(visited);
}

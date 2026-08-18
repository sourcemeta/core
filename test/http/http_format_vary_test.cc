#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <string>      // std::string
#include <string_view> // std::string_view

#include <sourcemeta/core/http.h>

// RFC 9110 §12.5.5: "a response that contains Vary: accept-encoding,
// accept-language indicates that the origin server might have used the
// request's Accept-Encoding and Accept-Language header fields"
TEST(format_vary_two_field_names) {
  const std::array<std::string_view, 2> names{
      {"accept-encoding", "accept-language"}};
  const auto value{sourcemeta::core::http_format_vary(names)};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "accept-encoding, accept-language");
}

TEST(format_vary_single_field_name) {
  const std::array<std::string_view, 1> names{{"Accept"}};
  const auto value{sourcemeta::core::http_format_vary(names)};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Accept");
}

TEST(format_vary_three_field_names) {
  const std::array<std::string_view, 3> names{
      {"User-Agent", "Accept-Encoding", "Cookie"}};
  const auto value{sourcemeta::core::http_format_vary(names)};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "User-Agent, Accept-Encoding, Cookie");
}

// RFC 9110 §5.1: field names are case-insensitive, so the spelling the caller
// chose is emitted as given
TEST(format_vary_preserves_the_spelling_of_a_field_name) {
  const std::array<std::string_view, 1> names{{"AcCePt"}};
  const auto value{sourcemeta::core::http_format_vary(names)};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "AcCePt");
}

// RFC 9110 §12.5.5: "Vary = #( "*" / field-name )", so the wildcard member is
// a legal alternative to a field name
TEST(format_vary_wildcard) {
  const std::array<std::string_view, 1> names{{"*"}};
  const auto value{sourcemeta::core::http_format_vary(names)};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "*");
}

// RFC 9110 §5.6.1.1: "In any production that uses the list construct, a sender
// MUST NOT generate empty list elements"
TEST(format_vary_rejects_an_empty_field_name) {
  const std::array<std::string_view, 2> names{{"Accept", ""}};
  EXPECT_FALSE(sourcemeta::core::http_format_vary(names).has_value());
}

// RFC 9110 §5.1: "field-name = token", and §5.6.2 excludes the space
TEST(format_vary_rejects_a_field_name_with_a_space) {
  const std::array<std::string_view, 1> names{{"Accept Encoding"}};
  EXPECT_FALSE(sourcemeta::core::http_format_vary(names).has_value());
}

TEST(format_vary_rejects_a_field_name_carrying_the_list_separator) {
  const std::array<std::string_view, 1> names{{"Accept,Accept-Encoding"}};
  EXPECT_FALSE(sourcemeta::core::http_format_vary(names).has_value());
}

TEST(format_vary_rejects_a_field_name_with_a_line_break) {
  const std::array<std::string_view, 1> names{{"Accept\r\nX-Injected: 1"}};
  EXPECT_FALSE(sourcemeta::core::http_format_vary(names).has_value());
}

TEST(format_vary_rejects_no_field_names) {
  const std::array<std::string_view, 0> names{};
  EXPECT_FALSE(sourcemeta::core::http_format_vary(names).has_value());
}

TEST(format_vary_sink_overload_appends) {
  const std::array<std::string_view, 2> names{{"Accept", "Accept-Encoding"}};
  std::string buffer{"Vary: "};
  EXPECT_TRUE(sourcemeta::core::http_format_vary(names, buffer));
  EXPECT_EQ(buffer, "Vary: Accept, Accept-Encoding");
}

TEST(format_vary_sink_overload_leaves_the_buffer_alone_on_failure) {
  const std::array<std::string_view, 2> names{{"Accept", "bad header"}};
  std::string buffer{"Vary: "};
  EXPECT_FALSE(sourcemeta::core::http_format_vary(names, buffer));
  EXPECT_EQ(buffer, "Vary: ");
}

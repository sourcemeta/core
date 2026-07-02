#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <cstddef>     // std::size_t
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

TEST(single_pair) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("session=abc123", cookies);
  EXPECT_EQ(cookies.size(), 1);
  EXPECT_EQ(cookies.at(0).first, "session");
  EXPECT_EQ(cookies.at(0).second, "abc123");
}

TEST(multiple_pairs) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("session=abc123; theme=dark; lang=en",
                                       cookies);
  EXPECT_EQ(cookies.size(), 3);
  EXPECT_EQ(cookies.at(0).first, "session");
  EXPECT_EQ(cookies.at(0).second, "abc123");
  EXPECT_EQ(cookies.at(1).first, "theme");
  EXPECT_EQ(cookies.at(1).second, "dark");
  EXPECT_EQ(cookies.at(2).first, "lang");
  EXPECT_EQ(cookies.at(2).second, "en");
}

TEST(no_space_after_semicolon) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("a=1;b=2", cookies);
  EXPECT_EQ(cookies.size(), 2);
  EXPECT_EQ(cookies.at(0).first, "a");
  EXPECT_EQ(cookies.at(0).second, "1");
  EXPECT_EQ(cookies.at(1).first, "b");
  EXPECT_EQ(cookies.at(1).second, "2");
}

TEST(surrounding_whitespace_is_trimmed) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("  a = 1 ;  b = 2  ", cookies);
  EXPECT_EQ(cookies.size(), 2);
  EXPECT_EQ(cookies.at(0).first, "a");
  EXPECT_EQ(cookies.at(0).second, "1");
  EXPECT_EQ(cookies.at(1).first, "b");
  EXPECT_EQ(cookies.at(1).second, "2");
}

TEST(empty_value) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("name=", cookies);
  EXPECT_EQ(cookies.size(), 1);
  EXPECT_EQ(cookies.at(0).first, "name");
  EXPECT_EQ(cookies.at(0).second, "");
}

TEST(value_containing_equals_is_preserved) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("token=abc=def", cookies);
  EXPECT_EQ(cookies.size(), 1);
  EXPECT_EQ(cookies.at(0).first, "token");
  EXPECT_EQ(cookies.at(0).second, "abc=def");
}

TEST(base64url_value_is_preserved_verbatim) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("sig=aGVsbG8-_x.YWJj", cookies);
  EXPECT_EQ(cookies.size(), 1);
  EXPECT_EQ(cookies.at(0).first, "sig");
  EXPECT_EQ(cookies.at(0).second, "aGVsbG8-_x.YWJj");
}

TEST(surrounding_quotes_are_preserved) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("session=\"abc\"", cookies);
  EXPECT_EQ(cookies.size(), 1);
  EXPECT_EQ(cookies.at(0).first, "session");
  EXPECT_EQ(cookies.at(0).second, "\"abc\"");
}

TEST(empty_input) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("", cookies);
  EXPECT_TRUE(cookies.empty());
}

TEST(trailing_semicolon_is_ignored) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("a=1;", cookies);
  EXPECT_EQ(cookies.size(), 1);
  EXPECT_EQ(cookies.at(0).first, "a");
  EXPECT_EQ(cookies.at(0).second, "1");
}

TEST(segment_without_equals_is_ignored) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("a=1; garbage; b=2", cookies);
  EXPECT_EQ(cookies.size(), 2);
  EXPECT_EQ(cookies.at(0).first, "a");
  EXPECT_EQ(cookies.at(0).second, "1");
  EXPECT_EQ(cookies.at(1).first, "b");
  EXPECT_EQ(cookies.at(1).second, "2");
}

TEST(empty_name_is_ignored) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("=value; a=1", cookies);
  EXPECT_EQ(cookies.size(), 1);
  EXPECT_EQ(cookies.at(0).first, "a");
  EXPECT_EQ(cookies.at(0).second, "1");
}

TEST(callback_form_is_invoked_per_pair) {
  std::size_t count{0};
  sourcemeta::core::http_parse_cookies(
      "a=1; b=2; c=3",
      [&count](const std::string_view, const std::string_view) -> void {
        count += 1;
      });
  EXPECT_EQ(count, 3);
}

TEST(string_view_container_borrows_from_input_without_copying) {
  const std::string input{"session=abc; theme=dark"};
  std::vector<std::pair<std::string_view, std::string_view>> cookies;
  sourcemeta::core::http_parse_cookies(input, cookies);
  EXPECT_EQ(cookies.size(), 2);
  EXPECT_EQ(cookies.at(0).first, "session");
  EXPECT_EQ(cookies.at(0).second, "abc");
  EXPECT_EQ(cookies.at(1).first, "theme");
  EXPECT_EQ(cookies.at(1).second, "dark");
  // The collected views point into the original buffer, proving no copy
  EXPECT_EQ(cookies.at(0).first.data(), input.data());
  EXPECT_EQ(cookies.at(0).second.data(), input.data() + 8);
}

TEST(cookie_name_case_is_preserved) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("SessionID=Value", cookies);
  EXPECT_EQ(cookies.size(), 1);
  EXPECT_EQ(cookies.at(0).first, "SessionID");
  EXPECT_EQ(cookies.at(0).second, "Value");
}

TEST(duplicate_names_are_all_reported) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("a=1; a=2", cookies);
  EXPECT_EQ(cookies.size(), 2);
  EXPECT_EQ(cookies.at(0).second, "1");
  EXPECT_EQ(cookies.at(1).second, "2");
}

TEST(empty_segments_are_skipped) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("a=1;;b=2", cookies);
  EXPECT_EQ(cookies.size(), 2);
  EXPECT_EQ(cookies.at(0).first, "a");
  EXPECT_EQ(cookies.at(1).first, "b");
}

TEST(leading_semicolon_is_ignored) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("; a=1", cookies);
  EXPECT_EQ(cookies.size(), 1);
  EXPECT_EQ(cookies.at(0).first, "a");
  EXPECT_EQ(cookies.at(0).second, "1");
}

TEST(whitespace_only_input_yields_nothing) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("   ", cookies);
  EXPECT_TRUE(cookies.empty());
}

TEST(semicolon_within_quotes_still_splits) {
  std::vector<std::pair<std::string, std::string>> cookies;
  sourcemeta::core::http_parse_cookies("a=\"1;2\"", cookies);
  EXPECT_EQ(cookies.size(), 1);
  EXPECT_EQ(cookies.at(0).first, "a");
  EXPECT_EQ(cookies.at(0).second, "\"1");
}

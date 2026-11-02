#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

TEST(collects_a_single_value) {
  std::vector<std::string_view> values;
  sourcemeta::core::http_cookie_values("session=abc; theme=dark", "session",
                                       values);
  EXPECT_EQ(values.size(), std::size_t{1});
  EXPECT_EQ(values.at(0), "abc");
}

TEST(collects_every_value_under_one_name_in_order) {
  // The shadowing case: a parent domain and the host each set one, and the
  // header presents both with no indication of origin
  std::vector<std::string_view> values;
  sourcemeta::core::http_cookie_values("session=abc; theme=dark; session=xyz",
                                       "session", values);
  EXPECT_EQ(values.size(), std::size_t{2});
  EXPECT_EQ(values.at(0), "abc");
  EXPECT_EQ(values.at(1), "xyz");
}

TEST(collects_three_values_under_one_name) {
  std::vector<std::string_view> values;
  sourcemeta::core::http_cookie_values("a=1; a=2; a=3", "a", values);
  EXPECT_EQ(values.size(), std::size_t{3});
  EXPECT_EQ(values.at(0), "1");
  EXPECT_EQ(values.at(1), "2");
  EXPECT_EQ(values.at(2), "3");
}

TEST(collects_nothing_for_an_absent_name) {
  std::vector<std::string_view> values;
  sourcemeta::core::http_cookie_values("session=abc; theme=dark", "missing",
                                       values);
  EXPECT_TRUE(values.empty());
}

TEST(collects_nothing_from_an_empty_header) {
  std::vector<std::string_view> values;
  sourcemeta::core::http_cookie_values("", "session", values);
  EXPECT_TRUE(values.empty());
}

TEST(matches_the_name_exactly) {
  // RFC 6265 cookie names are case-sensitive, unlike field names
  std::vector<std::string_view> values;
  sourcemeta::core::http_cookie_values("Session=abc; session=xyz", "session",
                                       values);
  EXPECT_EQ(values.size(), std::size_t{1});
  EXPECT_EQ(values.at(0), "xyz");
}

TEST(does_not_match_a_name_prefix) {
  std::vector<std::string_view> values;
  sourcemeta::core::http_cookie_values("session_id=abc", "session", values);
  EXPECT_TRUE(values.empty());
}

TEST(trims_surrounding_whitespace) {
  std::vector<std::string_view> values;
  sourcemeta::core::http_cookie_values("  session = abc ;  session = xyz ",
                                       "session", values);
  EXPECT_EQ(values.size(), std::size_t{2});
  EXPECT_EQ(values.at(0), "abc");
  EXPECT_EQ(values.at(1), "xyz");
}

TEST(collects_an_empty_value) {
  std::vector<std::string_view> values;
  sourcemeta::core::http_cookie_values("session=; session=xyz", "session",
                                       values);
  EXPECT_EQ(values.size(), std::size_t{2});
  EXPECT_EQ(values.at(0), "");
  EXPECT_EQ(values.at(1), "xyz");
}

TEST(collects_into_an_owning_container) {
  std::vector<std::string> values;
  sourcemeta::core::http_cookie_values("session=abc; session=xyz", "session",
                                       values);
  EXPECT_EQ(values.size(), std::size_t{2});
  EXPECT_EQ(values.at(0), "abc");
  EXPECT_EQ(values.at(1), "xyz");
}

TEST(appends_to_an_existing_container) {
  std::vector<std::string_view> values;
  values.emplace_back("existing");
  sourcemeta::core::http_cookie_values("session=abc", "session", values);
  EXPECT_EQ(values.size(), std::size_t{2});
  EXPECT_EQ(values.at(0), "existing");
  EXPECT_EQ(values.at(1), "abc");
}

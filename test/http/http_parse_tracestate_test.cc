#include <sourcemeta/core/http_tracecontext.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

TEST(the_example_from_the_specification) {
  std::vector<std::pair<std::string_view, std::string_view>> members;
  EXPECT_TRUE(sourcemeta::core::http_parse_tracestate(
      "rojo=00f067aa0ba902b7,congo=t61rcWkgMzE", members));
  EXPECT_EQ(members.size(), 2);
  EXPECT_EQ(members.at(0).first, "rojo");
  EXPECT_EQ(members.at(0).second, "00f067aa0ba902b7");
  EXPECT_EQ(members.at(1).first, "congo");
  EXPECT_EQ(members.at(1).second, "t61rcWkgMzE");
}

// The leftmost member is the most recent position in the trace, so order is
// reported as it arrives
// https://www.w3.org/TR/trace-context/#combined-header-value
TEST(order_is_preserved) {
  std::vector<std::pair<std::string_view, std::string_view>> members;
  EXPECT_TRUE(sourcemeta::core::http_parse_tracestate("a=1,b=2,c=3", members));
  EXPECT_EQ(members.size(), 3);
  EXPECT_EQ(members.at(0).first, "a");
  EXPECT_EQ(members.at(1).first, "b");
  EXPECT_EQ(members.at(2).first, "c");
}

// An empty member is legal and carries nothing, so it is skipped rather than
// reported as a pair of empty views
// https://www.w3.org/TR/trace-context/#list
TEST(empty_members_are_skipped) {
  std::vector<std::pair<std::string_view, std::string_view>> members;
  EXPECT_TRUE(
      sourcemeta::core::http_parse_tracestate("rojo=1 , , congo=2", members));
  EXPECT_EQ(members.size(), 2);
  EXPECT_EQ(members.at(0).first, "rojo");
  EXPECT_EQ(members.at(1).first, "congo");
}

TEST(an_empty_value_yields_no_members) {
  std::vector<std::pair<std::string_view, std::string_view>> members;
  EXPECT_TRUE(sourcemeta::core::http_parse_tracestate("", members));
  EXPECT_TRUE(members.empty());
}

TEST(a_multi_tenant_key_is_reported_whole) {
  std::vector<std::pair<std::string_view, std::string_view>> members;
  EXPECT_TRUE(
      sourcemeta::core::http_parse_tracestate("1tenant@system=v", members));
  EXPECT_EQ(members.size(), 1);
  EXPECT_EQ(members.at(0).first, "1tenant@system");
}

TEST(a_value_containing_a_space_is_reported_whole) {
  std::vector<std::pair<std::string_view, std::string_view>> members;
  EXPECT_TRUE(sourcemeta::core::http_parse_tracestate("foo=bar baz", members));
  EXPECT_EQ(members.size(), 1);
  EXPECT_EQ(members.at(0).second, "bar baz");
}

// The whole value is checked before anything is visited, so a caller never
// acts on part of a value it then has to discard
TEST(a_malformed_value_visits_nothing) {
  std::vector<std::pair<std::string_view, std::string_view>> members;
  EXPECT_FALSE(sourcemeta::core::http_parse_tracestate("a=1,FOO=2", members));
  EXPECT_TRUE(members.empty());
}

TEST(a_duplicate_key_visits_nothing) {
  std::vector<std::pair<std::string_view, std::string_view>> members;
  EXPECT_FALSE(sourcemeta::core::http_parse_tracestate("a=1,a=2", members));
  EXPECT_TRUE(members.empty());
}

TEST(the_callback_overload_reports_each_member) {
  std::string flat;
  EXPECT_TRUE(sourcemeta::core::http_parse_tracestate(
      "rojo=1,congo=2", [&flat](const auto key, const auto value) {
        flat += std::string{key} + "=" + std::string{value} + ";";
      }));
  EXPECT_EQ(flat, "rojo=1;congo=2;");
}

TEST(the_callback_overload_is_not_invoked_on_failure) {
  std::string flat;
  EXPECT_FALSE(sourcemeta::core::http_parse_tracestate(
      "foobarbaz", [&flat](const auto key, const auto value) {
        flat += std::string{key} + "=" + std::string{value} + ";";
      }));
  EXPECT_TRUE(flat.empty());
}

// The views borrow the input rather than copying it
TEST(the_members_borrow_the_input) {
  const std::string value{"rojo=00f067aa0ba902b7"};
  std::vector<std::pair<std::string_view, std::string_view>> members;
  EXPECT_TRUE(sourcemeta::core::http_parse_tracestate(value, members));
  EXPECT_EQ(members.size(), 1);
  EXPECT_EQ(members.at(0).first.data(), value.data());
}

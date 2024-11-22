#include <gtest/gtest.h>

#include <sourcemeta/jsontoolkit/regex.h>

TEST(Regex_matches, match_true_1) {
  const auto regex{sourcemeta::jsontoolkit::to_regex("^foo")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::jsontoolkit::matches(regex.value(), "foobar"));
}

TEST(Regex_matches, match_true_2) {
  const auto regex{sourcemeta::jsontoolkit::to_regex("^.*$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::jsontoolkit::matches(regex.value(), "foobar"));
}

TEST(Regex_matches, match_true_3) {
  const auto regex{sourcemeta::jsontoolkit::to_regex("^(.*)$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::jsontoolkit::matches(regex.value(), "foobar"));
}

TEST(Regex_matches, match_true_4) {
  const auto regex{sourcemeta::jsontoolkit::to_regex(".*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::jsontoolkit::matches(regex.value(), "foobar"));
}

TEST(Regex_matches, match_true_5) {
  const auto regex{sourcemeta::jsontoolkit::to_regex(".+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::jsontoolkit::matches(regex.value(), "foobar"));
}

TEST(Regex_matches, match_true_6) {
  const auto regex{sourcemeta::jsontoolkit::to_regex("^/.*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::jsontoolkit::matches(regex.value(), "/foo/bar"));
}

TEST(Regex_matches, match_true_7) {
  const auto regex{sourcemeta::jsontoolkit::to_regex("^/.*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::jsontoolkit::matches(regex.value(), "/"));
}

TEST(Regex_matches, match_true_8) {
  const auto regex{sourcemeta::jsontoolkit::to_regex("^.{1,256}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::jsontoolkit::matches(regex.value(), "foo"));
}

TEST(Regex_matches, match_true_9) {
  const auto regex{sourcemeta::jsontoolkit::to_regex("^.{1,256}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::jsontoolkit::matches(regex.value(), "f"));
}

TEST(Regex_matches, match_true_10) {
  const auto regex{sourcemeta::jsontoolkit::to_regex("^.{1,256}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(
      sourcemeta::jsontoolkit::matches(regex.value(), std::string(256, 'x')));
}

TEST(Regex_matches, match_false_1) {
  const auto regex{sourcemeta::jsontoolkit::to_regex("^foo")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::jsontoolkit::matches(regex.value(), "bar"));
}

TEST(Regex_matches, match_false_2) {
  const auto regex{sourcemeta::jsontoolkit::to_regex(".+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::jsontoolkit::matches(regex.value(), ""));
}

TEST(Regex_matches, match_false_3) {
  const auto regex{sourcemeta::jsontoolkit::to_regex("^/.*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::jsontoolkit::matches(regex.value(), "foo/bar"));
}

TEST(Regex_matches, match_false_4) {
  const auto regex{sourcemeta::jsontoolkit::to_regex("^.{1,256}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(
      sourcemeta::jsontoolkit::matches(regex.value(), std::string(257, 'x')));
}

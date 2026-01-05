#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <map>

TEST(URI, default_constructor) {
  const sourcemeta::core::URI uri;
  EXPECT_TRUE(uri.empty());
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI, default_constructor_equals_empty_string) {
  const sourcemeta::core::URI uri_default;
  const sourcemeta::core::URI uri_empty{""};
  EXPECT_EQ(uri_default, uri_empty);
}

TEST(URI, copy_constructor) {
  const sourcemeta::core::URI uri_1{"https://example.com"};
  const sourcemeta::core::URI uri_2{uri_1};
  EXPECT_EQ(uri_1.recompose(), uri_2.recompose());
}

TEST(URI, copy_constructor_empty) {
  const sourcemeta::core::URI uri_1;
  const sourcemeta::core::URI uri_2{uri_1};
  EXPECT_TRUE(uri_2.empty());
  EXPECT_EQ(uri_1, uri_2);
}

TEST(URI, move_constructor) {
  sourcemeta::core::URI uri_1{"https://example.com"};
  const sourcemeta::core::URI uri_2{std::move(uri_1)};
  EXPECT_EQ(uri_2.recompose(), "https://example.com");
}

TEST(URI, move_constructor_empty) {
  sourcemeta::core::URI uri_1;
  const sourcemeta::core::URI uri_2{std::move(uri_1)};
  EXPECT_TRUE(uri_2.empty());
}

TEST(URI, copy_assignment) {
  const sourcemeta::core::URI uri_1{"https://example.com"};
  sourcemeta::core::URI uri_2{"https://other.com"};
  uri_2 = uri_1;
  EXPECT_EQ(uri_1.recompose(), uri_2.recompose());
  EXPECT_EQ(uri_2.recompose(), "https://example.com");
}

TEST(URI, copy_assignment_empty) {
  const sourcemeta::core::URI uri_1;
  sourcemeta::core::URI uri_2{"https://example.com"};
  uri_2 = uri_1;
  EXPECT_TRUE(uri_2.empty());
  EXPECT_EQ(uri_1, uri_2);
}

TEST(URI, move_assignment) {
  sourcemeta::core::URI uri_1{"https://example.com"};
  sourcemeta::core::URI uri_2{"https://other.com"};
  uri_2 = std::move(uri_1);
  EXPECT_EQ(uri_2.recompose(), "https://example.com");
}

TEST(URI, move_assignment_empty) {
  sourcemeta::core::URI uri_1;
  sourcemeta::core::URI uri_2{"https://example.com"};
  uri_2 = std::move(uri_1);
  EXPECT_TRUE(uri_2.empty());
}

TEST(URI, from_fragment) {
  const auto uri{sourcemeta::core::URI::from_fragment("foo")};
  const auto fragment{uri.fragment()};
  EXPECT_TRUE(fragment.has_value());
  EXPECT_EQ(fragment.value(), "foo");
  EXPECT_EQ(uri.recompose(), "#foo");
}

TEST(URI, from_fragment_at) {
  const auto uri{sourcemeta::core::URI::from_fragment("/@foo")};
  const auto fragment{uri.fragment()};
  EXPECT_TRUE(fragment.has_value());
  EXPECT_EQ(fragment.value(), "/@foo");
  EXPECT_EQ(uri.recompose(), "#/@foo");
}

TEST(URI, from_fragment_empty) {
  const auto uri{sourcemeta::core::URI::from_fragment("")};
  const auto fragment{uri.fragment()};
  EXPECT_TRUE(fragment.has_value());
  EXPECT_EQ(fragment.value(), "");
  EXPECT_EQ(uri.recompose(), "#");
}

TEST(URI, from_fragment_with_hash) {
  const auto uri{sourcemeta::core::URI::from_fragment("#foo")};
  const auto fragment{uri.fragment()};
  EXPECT_TRUE(fragment.has_value());
  EXPECT_EQ(fragment.value(), "foo");
  EXPECT_EQ(uri.recompose(), "#foo");
}

TEST(URI, from_fragment_with_hash_empty) {
  const auto uri{sourcemeta::core::URI::from_fragment("#")};
  const auto fragment{uri.fragment()};
  EXPECT_TRUE(fragment.has_value());
  EXPECT_EQ(fragment.value(), "");
  EXPECT_EQ(uri.recompose(), "#");
}

TEST(URI, using_istream) {
  std::istringstream input{"https://example.com"};
  const sourcemeta::core::URI uri{input};
  EXPECT_EQ(uri.recompose(), "https://example.com");
}

TEST(URI, equality) {
  EXPECT_EQ(sourcemeta::core::URI{"https://example.com"},
            sourcemeta::core::URI{"https://example.com"});
  EXPECT_NE(sourcemeta::core::URI{"https://example.com"},
            sourcemeta::core::URI{"http://example.com"});
  const sourcemeta::core::URI self{"https://example.com"};
  EXPECT_EQ(self, self);
}

TEST(URI, map) {
  std::map<sourcemeta::core::URI, bool> map;
  map.emplace("https://example.com", true);
  map.emplace("https://foo.com", false);

  EXPECT_EQ(map.size(), 2);
  EXPECT_TRUE(map.contains(sourcemeta::core::URI{"https://example.com"}));
  EXPECT_TRUE(map.contains(sourcemeta::core::URI{"https://foo.com"}));
  EXPECT_FALSE(map.contains(sourcemeta::core::URI{"https://example.org"}));
  EXPECT_FALSE(map.contains(sourcemeta::core::URI{"https://foo.org"}));
  EXPECT_TRUE(map.at(sourcemeta::core::URI{"https://example.com"}));
  EXPECT_FALSE(map.at(sourcemeta::core::URI{"https://foo.com"}));
}

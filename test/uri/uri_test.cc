#include <gtest/gtest.h>
#include <sourcemeta/core/uri.h>

TEST(URI, copy_constructor) {
  const sourcemeta::core::URI uri_1{"https://example.com"};
  const sourcemeta::core::URI uri_2{uri_1};
  EXPECT_EQ(uri_1.recompose(), uri_2.recompose());
}

TEST(URI, move_constructor) {
  sourcemeta::core::URI uri_1{"https://example.com"};
  const sourcemeta::core::URI uri_2{std::move(uri_1)};
  EXPECT_EQ(uri_2.recompose(), "https://example.com");
}

TEST(URI, from_fragment) {
  const auto uri{sourcemeta::core::URI::from_fragment("foo")};
  const auto fragment{uri.fragment()};
  EXPECT_TRUE(fragment.has_value());
  EXPECT_EQ(fragment.value(), "foo");
  EXPECT_EQ(uri.recompose(), "#foo");
}

TEST(URI, using_istream) {
  std::istringstream input{"https://example.com"};
  const sourcemeta::core::URI uri{input};
  EXPECT_EQ(uri.recompose(), "https://example.com");
}

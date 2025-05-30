#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_path_to_uri, unix_absolute) {
  const std::filesystem::path example{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::to_uri(example)};
  EXPECT_EQ(uri.recompose(), "file:///foo/bar/baz");
}

TEST(URI_path_to_uri, unix_absolute_other_characters) {
  const std::filesystem::path example{"/foo bar/bar?"};
  const auto uri{sourcemeta::core::to_uri(example)};
  // TODO: Escape
  EXPECT_EQ(uri.recompose(), "file:///foo bar/bar?");
}

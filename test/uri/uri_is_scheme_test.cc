#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(common_schemes) {
  EXPECT_TRUE(sourcemeta::core::URI::is_scheme("http"));
  EXPECT_TRUE(sourcemeta::core::URI::is_scheme("https"));
  EXPECT_TRUE(sourcemeta::core::URI::is_scheme("urn"));
  EXPECT_TRUE(sourcemeta::core::URI::is_scheme("file"));
  EXPECT_TRUE(sourcemeta::core::URI::is_scheme("mailto"));
}

TEST(single_letter) {
  EXPECT_TRUE(sourcemeta::core::URI::is_scheme("a"));
  EXPECT_TRUE(sourcemeta::core::URI::is_scheme("Z"));
}

TEST(allowed_non_alpha_after_first) {
  EXPECT_TRUE(sourcemeta::core::URI::is_scheme("h123"));
  EXPECT_TRUE(sourcemeta::core::URI::is_scheme("foo+bar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_scheme("foo-bar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_scheme("foo.bar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_scheme("a1+2-3.4"));
}

TEST(empty_is_not_a_scheme) {
  EXPECT_FALSE(sourcemeta::core::URI::is_scheme(""));
}

TEST(must_start_with_alpha) {
  EXPECT_FALSE(sourcemeta::core::URI::is_scheme("1http"));
  EXPECT_FALSE(sourcemeta::core::URI::is_scheme("+http"));
  EXPECT_FALSE(sourcemeta::core::URI::is_scheme("-http"));
  EXPECT_FALSE(sourcemeta::core::URI::is_scheme(".http"));
}

TEST(rejects_disallowed_characters) {
  EXPECT_FALSE(sourcemeta::core::URI::is_scheme("http:"));
  EXPECT_FALSE(sourcemeta::core::URI::is_scheme("ht tp"));
  EXPECT_FALSE(sourcemeta::core::URI::is_scheme("foo/bar"));
  EXPECT_FALSE(sourcemeta::core::URI::is_scheme("foo_bar"));
  EXPECT_FALSE(sourcemeta::core::URI::is_scheme("caf\xC3\xA9"));
}

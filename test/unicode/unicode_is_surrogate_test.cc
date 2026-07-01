#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

TEST(null) { EXPECT_FALSE(sourcemeta::core::is_surrogate(0x0000)); }

TEST(ascii_letter) { EXPECT_FALSE(sourcemeta::core::is_surrogate(0x0041)); }

TEST(just_below_low_surrogate) {
  EXPECT_FALSE(sourcemeta::core::is_surrogate(0xD7FF));
}

TEST(low_surrogate_low_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_surrogate(0xD800));
}

TEST(low_surrogate_mid) { EXPECT_TRUE(sourcemeta::core::is_surrogate(0xDA00)); }

TEST(low_surrogate_high_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_surrogate(0xDBFF));
}

TEST(high_surrogate_low_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_surrogate(0xDC00));
}

TEST(high_surrogate_mid) {
  EXPECT_TRUE(sourcemeta::core::is_surrogate(0xDE00));
}

TEST(high_surrogate_high_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_surrogate(0xDFFF));
}

TEST(just_above_high_surrogate) {
  EXPECT_FALSE(sourcemeta::core::is_surrogate(0xE000));
}

TEST(max_bmp) { EXPECT_FALSE(sourcemeta::core::is_surrogate(0xFFFF)); }

TEST(emoji_grinning_face) {
  EXPECT_FALSE(sourcemeta::core::is_surrogate(0x1F600));
}

TEST(max_codepoint) { EXPECT_FALSE(sourcemeta::core::is_surrogate(0x10FFFF)); }

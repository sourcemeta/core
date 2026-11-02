#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

TEST(ascii_uppercase_letter) {
  EXPECT_TRUE(sourcemeta::core::is_id_start(U'A'));
}

TEST(ascii_lowercase_letter) {
  EXPECT_TRUE(sourcemeta::core::is_id_start(U'z'));
}

TEST(ascii_digit) { EXPECT_FALSE(sourcemeta::core::is_id_start(U'0')); }

TEST(underscore) { EXPECT_FALSE(sourcemeta::core::is_id_start(U'_')); }

TEST(dollar_sign) { EXPECT_FALSE(sourcemeta::core::is_id_start(U'$')); }

TEST(hyphen) { EXPECT_FALSE(sourcemeta::core::is_id_start(U'-')); }

TEST(space) { EXPECT_FALSE(sourcemeta::core::is_id_start(U' ')); }

TEST(null) { EXPECT_FALSE(sourcemeta::core::is_id_start(0x0000)); }

TEST(latin_small_letter_e_with_acute) {
  EXPECT_TRUE(sourcemeta::core::is_id_start(0x00E9));
}

TEST(greek_small_letter_alpha) {
  EXPECT_TRUE(sourcemeta::core::is_id_start(0x03B1));
}

TEST(cjk_unified_ideograph) {
  EXPECT_TRUE(sourcemeta::core::is_id_start(0x4E2D));
}

TEST(letter_number) { EXPECT_TRUE(sourcemeta::core::is_id_start(0x2160)); }

TEST(combining_acute_accent) {
  EXPECT_FALSE(sourcemeta::core::is_id_start(0x0301));
}

TEST(snowman) { EXPECT_FALSE(sourcemeta::core::is_id_start(0x2603)); }

TEST(emoji_outside_basic_plane) {
  EXPECT_FALSE(sourcemeta::core::is_id_start(0x1F600));
}

TEST(past_the_unicode_range) {
  EXPECT_FALSE(sourcemeta::core::is_id_start(0x110000));
}

#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

TEST(ascii_letter) { EXPECT_TRUE(sourcemeta::core::is_id_continue(U'A')); }

TEST(ascii_digit) { EXPECT_TRUE(sourcemeta::core::is_id_continue(U'0')); }

TEST(underscore) { EXPECT_TRUE(sourcemeta::core::is_id_continue(U'_')); }

TEST(dollar_sign) { EXPECT_FALSE(sourcemeta::core::is_id_continue(U'$')); }

TEST(hyphen) { EXPECT_FALSE(sourcemeta::core::is_id_continue(U'-')); }

TEST(space) { EXPECT_FALSE(sourcemeta::core::is_id_continue(U' ')); }

TEST(null) { EXPECT_FALSE(sourcemeta::core::is_id_continue(0x0000)); }

TEST(combining_acute_accent) {
  EXPECT_TRUE(sourcemeta::core::is_id_continue(0x0301));
}

TEST(latin_small_letter_e_with_acute) {
  EXPECT_TRUE(sourcemeta::core::is_id_continue(0x00E9));
}

TEST(cjk_unified_ideograph) {
  EXPECT_TRUE(sourcemeta::core::is_id_continue(0x4E2D));
}

TEST(snowman) { EXPECT_FALSE(sourcemeta::core::is_id_continue(0x2603)); }

TEST(past_the_unicode_range) {
  EXPECT_FALSE(sourcemeta::core::is_id_continue(0x110000));
}

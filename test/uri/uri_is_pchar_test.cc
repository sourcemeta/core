#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(unreserved_letters) {
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('a'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('Z'));
}

TEST(unreserved_digits) {
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('0'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('9'));
}

TEST(unreserved_marks) {
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('-'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('.'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('_'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('~'));
}

TEST(all_sub_delimiters) {
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('!'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('$'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('&'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('\''));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('('));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar(')'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('*'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('+'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar(','));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar(';'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('='));
}

TEST(the_two_extra_characters) {
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar(':'));
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('@'));
}

TEST(a_percent_sign_begins_a_triplet) {
  EXPECT_TRUE(sourcemeta::core::URI::is_pchar('%'));
}

TEST(generic_delimiters_that_are_not_path_characters) {
  EXPECT_FALSE(sourcemeta::core::URI::is_pchar('/'));
  EXPECT_FALSE(sourcemeta::core::URI::is_pchar('?'));
  EXPECT_FALSE(sourcemeta::core::URI::is_pchar('#'));
  EXPECT_FALSE(sourcemeta::core::URI::is_pchar('['));
  EXPECT_FALSE(sourcemeta::core::URI::is_pchar(']'));
}

TEST(a_space_is_not_a_path_character) {
  EXPECT_FALSE(sourcemeta::core::URI::is_pchar(' '));
}

TEST(a_brace_is_not_a_path_character) {
  EXPECT_FALSE(sourcemeta::core::URI::is_pchar('{'));
  EXPECT_FALSE(sourcemeta::core::URI::is_pchar('}'));
}

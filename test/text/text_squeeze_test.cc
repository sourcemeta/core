#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

TEST(empty) { EXPECT_EQ(sourcemeta::core::squeeze("", '/'), ""); }

TEST(no_runs) { EXPECT_EQ(sourcemeta::core::squeeze("a/b/c", '/'), "a/b/c"); }

TEST(collapses_runs) {
  EXPECT_EQ(sourcemeta::core::squeeze("a//b///c", '/'), "a/b/c");
}

TEST(leading_run) { EXPECT_EQ(sourcemeta::core::squeeze("///a", '/'), "/a"); }

TEST(trailing_run) { EXPECT_EQ(sourcemeta::core::squeeze("a///", '/'), "a/"); }

TEST(only_the_run_character) {
  EXPECT_EQ(sourcemeta::core::squeeze("/////", '/'), "/");
}

TEST(single_occurrences_untouched) {
  EXPECT_EQ(sourcemeta::core::squeeze("/", '/'), "/");
}

TEST(spaces) {
  EXPECT_EQ(sourcemeta::core::squeeze("a   b   c", ' '), "a b c");
}

TEST(other_characters_untouched) {
  EXPECT_EQ(sourcemeta::core::squeeze("aabbcc", '/'), "aabbcc");
}

TEST(collapses_only_the_given_character) {
  EXPECT_EQ(sourcemeta::core::squeeze("a..//b", '/'), "a../b");
}

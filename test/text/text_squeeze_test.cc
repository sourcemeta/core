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

TEST(append_preserves_existing_output) {
  std::string output{"path="};
  sourcemeta::core::squeeze("a//b", '/', output);
  EXPECT_EQ(output, "path=a/b");
}

TEST(append_empty_leaves_output_untouched) {
  std::string output{"unchanged"};
  sourcemeta::core::squeeze("", '/', output);
  EXPECT_EQ(output, "unchanged");
}

TEST(append_matches_returning_form) {
  std::string output;
  sourcemeta::core::squeeze("a   b", ' ', output);
  EXPECT_EQ(output, sourcemeta::core::squeeze("a   b", ' '));
}

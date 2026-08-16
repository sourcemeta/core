#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

TEST(no_occurrence) {
  EXPECT_EQ(sourcemeta::core::replace("alpha", "x", "y"), "alpha");
}

TEST(a_single_occurrence) {
  EXPECT_EQ(sourcemeta::core::replace("a.b", ".", "/"), "a/b");
}

TEST(every_occurrence) {
  EXPECT_EQ(sourcemeta::core::replace("a.b.c", ".", "/"), "a/b/c");
}

TEST(an_empty_input) { EXPECT_EQ(sourcemeta::core::replace("", "a", "b"), ""); }

TEST(an_empty_target_matches_nothing) {
  EXPECT_EQ(sourcemeta::core::replace("alpha", "", "x"), "alpha");
}

TEST(an_empty_replacement_removes) {
  EXPECT_EQ(sourcemeta::core::replace("a.b.c", ".", ""), "abc");
}

TEST(a_target_longer_than_one_character) {
  EXPECT_EQ(sourcemeta::core::replace("a\r\nb\r\n", "\r\n", "\n"), "a\nb\n");
}

TEST(a_replacement_longer_than_the_target) {
  EXPECT_EQ(sourcemeta::core::replace("a-b", "-", " to "), "a to b");
}

TEST(a_replacement_holding_the_target_does_not_recurse) {
  EXPECT_EQ(sourcemeta::core::replace("aaa", "a", "aa"), "aaaaaa");
}

TEST(the_whole_input_is_the_target) {
  EXPECT_EQ(sourcemeta::core::replace("alpha", "alpha", "beta"), "beta");
}

TEST(overlapping_candidates_are_taken_left_to_right) {
  EXPECT_EQ(sourcemeta::core::replace("aaaa", "aa", "b"), "bb");
}

TEST(a_target_at_the_boundaries) {
  EXPECT_EQ(sourcemeta::core::replace("xaax", "x", "-"), "-aa-");
}

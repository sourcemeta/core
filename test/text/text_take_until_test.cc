#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

TEST(Text_take_until, marker_in_middle) {
  EXPECT_EQ(sourcemeta::core::take_until("foo # bar", '#'), "foo ");
}

TEST(Text_take_until, marker_absent_returns_input) {
  EXPECT_EQ(sourcemeta::core::take_until("no marker here", '#'),
            "no marker here");
}

TEST(Text_take_until, marker_at_start_returns_empty) {
  EXPECT_EQ(sourcemeta::core::take_until("#leading", '#'), "");
}

TEST(Text_take_until, marker_at_end) {
  EXPECT_EQ(sourcemeta::core::take_until("trailing#", '#'), "trailing");
}

TEST(Text_take_until, only_first_occurrence) {
  EXPECT_EQ(sourcemeta::core::take_until("a#b#c", '#'), "a");
}

TEST(Text_take_until, empty_input) {
  EXPECT_EQ(sourcemeta::core::take_until("", '#'), "");
}

TEST(Text_take_until, marker_only) {
  EXPECT_EQ(sourcemeta::core::take_until("#", '#'), "");
}

TEST(Text_take_until, marker_null_byte) {
  EXPECT_EQ(sourcemeta::core::take_until(std::string_view{"ab\0cd", 5}, '\0'),
            "ab");
}

TEST(Text_take_until, non_ascii_passthrough) {
  EXPECT_EQ(sourcemeta::core::take_until("caf\xC3\xA9 # x", '#'),
            "caf\xC3\xA9 ");
}

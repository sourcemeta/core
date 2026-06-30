#include <sourcemeta/core/test.h>

#include <sourcemeta/core/text.h>

TEST(marker_in_middle) {
  EXPECT_EQ(sourcemeta::core::take_until("foo # bar", '#'), "foo ");
}

TEST(marker_absent_returns_input) {
  EXPECT_EQ(sourcemeta::core::take_until("no marker here", '#'),
            "no marker here");
}

TEST(marker_at_start_returns_empty) {
  EXPECT_EQ(sourcemeta::core::take_until("#leading", '#'), "");
}

TEST(marker_at_end) {
  EXPECT_EQ(sourcemeta::core::take_until("trailing#", '#'), "trailing");
}

TEST(only_first_occurrence) {
  EXPECT_EQ(sourcemeta::core::take_until("a#b#c", '#'), "a");
}

TEST(empty_input) { EXPECT_EQ(sourcemeta::core::take_until("", '#'), ""); }

TEST(marker_only) { EXPECT_EQ(sourcemeta::core::take_until("#", '#'), ""); }

TEST(marker_null_byte) {
  EXPECT_EQ(sourcemeta::core::take_until(std::string_view{"ab\0cd", 5}, '\0'),
            "ab");
}

TEST(non_ascii_passthrough) {
  EXPECT_EQ(sourcemeta::core::take_until("caf\xC3\xA9 # x", '#'),
            "caf\xC3\xA9 ");
}

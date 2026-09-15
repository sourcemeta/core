#include <sourcemeta/core/html.h>
#include <sourcemeta/core/test.h>

TEST(entity_with_semicolon) {
  EXPECT_EQ(sourcemeta::core::html_entity("amp;"), "&");
}

TEST(entity_uppercase_name) {
  EXPECT_EQ(sourcemeta::core::html_entity("AMP;"), "&");
}

TEST(entity_mixed_case_name_is_unknown) {
  EXPECT_TRUE(sourcemeta::core::html_entity("Amp;").empty());
}

TEST(entity_legacy_name_without_semicolon) {
  EXPECT_EQ(sourcemeta::core::html_entity("copy"), "\xC2\xA9");
}

TEST(entity_legacy_name_with_semicolon) {
  EXPECT_EQ(sourcemeta::core::html_entity("copy;"), "\xC2\xA9");
}

TEST(entity_without_legacy_form_needs_semicolon) {
  EXPECT_TRUE(sourcemeta::core::html_entity("ne").empty());
}

TEST(entity_without_legacy_form_with_semicolon) {
  EXPECT_EQ(sourcemeta::core::html_entity("ne;"), "\xE2\x89\xA0");
}

TEST(entity_no_break_space) {
  EXPECT_EQ(sourcemeta::core::html_entity("nbsp;"), "\xC2\xA0");
}

TEST(entity_with_two_code_points) {
  EXPECT_EQ(sourcemeta::core::html_entity("NotEqualTilde;"),
            "\xE2\x89\x82\xCC\xB8");
}

TEST(entity_outside_basic_multilingual_plane) {
  EXPECT_EQ(sourcemeta::core::html_entity("Afr;"), "\xF0\x9D\x94\x84");
}

TEST(entity_first_in_order) {
  EXPECT_EQ(sourcemeta::core::html_entity("AElig"), "\xC3\x86");
}

TEST(entity_last_in_order) {
  EXPECT_EQ(sourcemeta::core::html_entity("zwnj;"), "\xE2\x80\x8C");
}

TEST(entity_longest_name) {
  EXPECT_EQ(sourcemeta::core::html_entity("CounterClockwiseContourIntegral;"),
            "\xE2\x88\xB3");
}

TEST(entity_empty_name) {
  EXPECT_TRUE(sourcemeta::core::html_entity("").empty());
}

TEST(entity_name_with_ampersand) {
  EXPECT_TRUE(sourcemeta::core::html_entity("&amp;").empty());
}

TEST(entity_unknown_name) {
  EXPECT_TRUE(sourcemeta::core::html_entity("notreal;").empty());
}

TEST(entity_name_with_trailing_characters) {
  EXPECT_TRUE(sourcemeta::core::html_entity("amp;x").empty());
}

#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

#include <string_view> // std::string_view

TEST(Text_remove_suffix_ignore_case, matches_lowercase) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("schema.json", ".json"),
            "schema");
}

TEST(Text_remove_suffix_ignore_case, matches_uppercase) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("schema.JSON", ".json"),
            "schema");
}

TEST(Text_remove_suffix_ignore_case, matches_mixed_case) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("schema.JsOn", ".json"),
            "schema");
}

TEST(Text_remove_suffix_ignore_case, matches_uppercase_suffix_argument) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("schema.json", ".JSON"),
            "schema");
}

TEST(Text_remove_suffix_ignore_case, exact_case_match) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("Schema.json", ".json"),
            "Schema");
}

TEST(Text_remove_suffix_ignore_case, no_match_returns_input_unchanged) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("schema.txt", ".json"),
            "schema.txt");
}

TEST(Text_remove_suffix_ignore_case, suffix_longer_than_input) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("x", ".json"), "x");
}

TEST(Text_remove_suffix_ignore_case, empty_suffix) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("file.json", ""),
            "file.json");
}

TEST(Text_remove_suffix_ignore_case, empty_input) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("", ".json"), "");
}

TEST(Text_remove_suffix_ignore_case, empty_input_and_empty_suffix) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("", ""), "");
}

TEST(Text_remove_suffix_ignore_case, suffix_equals_input) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case(".json", ".json"), "");
}

TEST(Text_remove_suffix_ignore_case, suffix_equals_input_different_case) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case(".JSON", ".json"), "");
}

TEST(Text_remove_suffix_ignore_case, partial_overlap_does_not_match) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("son", ".json"), "son");
}

TEST(Text_remove_suffix_ignore_case, multiple_extensions_only_last_removed) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("file.tar.gz", ".gz"),
            "file.tar");
}

TEST(Text_remove_suffix_ignore_case, non_alphabetic_suffix_chars) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("data-123", "-123"),
            "data");
}

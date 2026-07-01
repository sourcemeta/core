#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string_view> // std::string_view

TEST(matches_lowercase) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("schema.json", ".json"),
            "schema");
}

TEST(matches_uppercase) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("schema.JSON", ".json"),
            "schema");
}

TEST(matches_mixed_case) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("schema.JsOn", ".json"),
            "schema");
}

TEST(matches_uppercase_suffix_argument) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("schema.json", ".JSON"),
            "schema");
}

TEST(exact_case_match) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("Schema.json", ".json"),
            "Schema");
}

TEST(no_match_returns_input_unchanged) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("schema.txt", ".json"),
            "schema.txt");
}

TEST(suffix_longer_than_input) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("x", ".json"), "x");
}

TEST(empty_suffix) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("file.json", ""),
            "file.json");
}

TEST(empty_input) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("", ".json"), "");
}

TEST(empty_input_and_empty_suffix) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("", ""), "");
}

TEST(suffix_equals_input) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case(".json", ".json"), "");
}

TEST(suffix_equals_input_different_case) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case(".JSON", ".json"), "");
}

TEST(partial_overlap_does_not_match) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("son", ".json"), "son");
}

TEST(multiple_extensions_only_last_removed) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("file.tar.gz", ".gz"),
            "file.tar");
}

TEST(non_alphabetic_suffix_chars) {
  EXPECT_EQ(sourcemeta::core::remove_suffix_ignore_case("data-123", "-123"),
            "data");
}

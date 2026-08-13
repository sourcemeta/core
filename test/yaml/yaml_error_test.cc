#include <sourcemeta/core/test.h>
#include <sourcemeta/core/yaml.h>

#include <filesystem> // std::filesystem::path

TEST(general_error_message) {
  const sourcemeta::core::YAMLError error{"Something went wrong"};
  EXPECT_STREQ(error.what(), "Something went wrong");
}

TEST(parse_error_default_message) {
  const sourcemeta::core::YAMLParseError error{4, 7};
  EXPECT_STREQ(error.what(), "Failed to parse the YAML document");
}

TEST(parse_error_default_message_line) {
  const sourcemeta::core::YAMLParseError error{4, 7};
  EXPECT_EQ(error.line(), 4);
}

TEST(parse_error_default_message_column) {
  const sourcemeta::core::YAMLParseError error{4, 7};
  EXPECT_EQ(error.column(), 7);
}

TEST(file_parse_error_message) {
  const sourcemeta::core::YAMLFileParseError error{"/foo/bar.yaml", 4, 7,
                                                   "Unexpected scalar"};
  EXPECT_STREQ(error.what(), "Unexpected scalar");
}

TEST(file_parse_error_path) {
  const sourcemeta::core::YAMLFileParseError error{"/foo/bar.yaml", 4, 7,
                                                   "Unexpected scalar"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar.yaml"});
}

TEST(file_parse_error_line) {
  const sourcemeta::core::YAMLFileParseError error{"/foo/bar.yaml", 4, 7,
                                                   "Unexpected scalar"};
  EXPECT_EQ(error.line(), 4);
}

TEST(file_parse_error_column) {
  const sourcemeta::core::YAMLFileParseError error{"/foo/bar.yaml", 4, 7,
                                                   "Unexpected scalar"};
  EXPECT_EQ(error.column(), 7);
}

TEST(duplicate_key_error_message) {
  const sourcemeta::core::YAMLDuplicateKeyError error{"foo", 4, 7};
  EXPECT_STREQ(error.what(), "Duplicate key in YAML mapping");
}

TEST(duplicate_key_error_key) {
  const sourcemeta::core::YAMLDuplicateKeyError error{"foo", 4, 7};
  EXPECT_EQ(error.key(), "foo");
}

TEST(duplicate_key_error_empty_key) {
  const sourcemeta::core::YAMLDuplicateKeyError error{"", 4, 7};
  EXPECT_EQ(error.key(), "");
}

TEST(duplicate_key_error_line) {
  const sourcemeta::core::YAMLDuplicateKeyError error{"foo", 4, 7};
  EXPECT_EQ(error.line(), 4);
}

TEST(duplicate_key_error_column) {
  const sourcemeta::core::YAMLDuplicateKeyError error{"foo", 4, 7};
  EXPECT_EQ(error.column(), 7);
}

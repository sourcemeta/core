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

// YAML 1.2.2 production 183 writes a block sequence as entries each opening a
// line of its own, so one that opens on the line a node property sits on
// belongs to no sequence the property could decorate
TEST(block_sequence_on_the_line_of_a_node_property) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"&x - 1\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(),
                 "Block sequence after node property must start on a new line");
  }
}

// YAML 1.2.2 Section 8.2.1 reaches the block sequence of an entry through
// `s-l-comments`, which ends on a line break, so a sequence opens on a line of
// its own rather than on the one its key sits on
TEST(block_sequence_entry_on_the_line_of_its_key) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: - 1\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(),
                 "Block sequence entry on same line as mapping key");
  }
}

// Section 8.2.2 reaches a nested block mapping the same way, so an explicit
// key of one opens on a line of its own as well
TEST(explicit_key_on_the_line_of_its_parent_key) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: ? b\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(
        error.what(),
        "Explicit mapping key in block value on same line as parent key");
  }
}

// A mapping that opens on the line starting the document is the whole of what
// that document carries, so a key below it continues nothing
TEST(block_mapping_continued_below_the_document_start_line) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"--- a: 1\nb: 2\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(),
                 "Block mapping continuation after document start line");
  }
}

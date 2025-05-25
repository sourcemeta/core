#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <string>
#include <string_view>
#include <unordered_set>

TEST(JSON_string, foo_value_string) {
  const std::string value{"foo"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.to_string(), "foo");
}

TEST(JSON_string, foo_value_string_view) {
  const std::string_view value{"foo"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.to_string(), "foo");
}

TEST(JSON_string, type) {
  const sourcemeta::core::JSON document{"foo"};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::String);
}

TEST(JSON_string, to_stringstream) {
  const std::string value{"foo bar"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_string());
  auto stream{document.to_stringstream()};
  EXPECT_EQ(stream.get(), 'f');
  EXPECT_EQ(stream.get(), 'o');
  EXPECT_EQ(stream.get(), 'o');
  EXPECT_EQ(stream.get(), ' ');
  EXPECT_EQ(stream.get(), 'b');
  EXPECT_EQ(stream.get(), 'a');
  EXPECT_EQ(stream.get(), 'r');
  EXPECT_EQ(stream.get(), -1);
  EXPECT_TRUE(stream.eof());
}

TEST(JSON_string, estimated_byte_size_foo) {
  const sourcemeta::core::JSON document{"foo"};
  EXPECT_EQ(document.estimated_byte_size(), 3);
}

TEST(JSON_string, estimated_byte_size_foo_bar_baz) {
  const sourcemeta::core::JSON document{"foo bar baz"};
  EXPECT_EQ(document.estimated_byte_size(), 11);
}

TEST(JSON_string, estimated_byte_size_empty) {
  const sourcemeta::core::JSON document{""};
  EXPECT_EQ(document.estimated_byte_size(), 0);
}

TEST(JSON_string, fast_hash_foo) {
  const sourcemeta::core::JSON document{"foo"};
  EXPECT_EQ(document.fast_hash(), 6);
}

TEST(JSON_string, fast_hash_foo_bar_baz) {
  const sourcemeta::core::JSON document{"foo bar baz"};
  EXPECT_EQ(document.fast_hash(), 14);
}

TEST(JSON_string, fast_hash_empty) {
  const sourcemeta::core::JSON document{""};
  EXPECT_EQ(document.fast_hash(), 3);
}

TEST(JSON_string, unicode_length_1) {
  // This unicode string corresponds to 简律纯
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "name": "\u7b80\u5f8b\u7eaf"
  })JSON");

  EXPECT_TRUE(document.is_object());
  EXPECT_TRUE(document.defines("name"));
  EXPECT_TRUE(document.at("name").is_string());
  EXPECT_EQ(document.at("name").size(), 3);
  EXPECT_EQ(document.at("name").string_size(), 3);

  // https://unicodeplus.com/U+7B80 (UTF-8: 0xE7 0xAE 0x80)
  // https://unicodeplus.com/U+5F8B (UTF-8: 0xE5 0xBE 0x8B)
  // https://unicodeplus.com/U+7EAF (UTF-8: 0xE7 0xBA 0xAF)
  EXPECT_EQ(document.at("name").byte_size(), 9);
}

TEST(JSON_string, contains_true) {
  const sourcemeta::core::JSON document{"foo bar baz"};
  EXPECT_TRUE(document.contains("foo"));
}

TEST(JSON_string, contains_false) {
  const sourcemeta::core::JSON document{"foo bar baz"};
  EXPECT_FALSE(document.contains("fooo"));
}

TEST(JSON_string, contains_character_true) {
  const sourcemeta::core::JSON document{"foo"};
  EXPECT_TRUE(document.contains('f'));
}

TEST(JSON_string, contains_character_false) {
  const sourcemeta::core::JSON document{"foo"};
  EXPECT_FALSE(document.contains('b'));
}

TEST(JSON_string, trim_const) {
  const sourcemeta::core::JSON document{" \r\t  foo   bar\n\v   \f"};
  EXPECT_EQ(document.trim(), "foo   bar");
  EXPECT_EQ(document.to_string(), " \r\t  foo   bar\n\v   \f");
}

TEST(JSON_string, trim_in_place) {
  sourcemeta::core::JSON document{" \r\t  foo   bar\n\v   \f"};
  const auto &result{document.trim()};
  EXPECT_EQ(result, "foo   bar");
  EXPECT_EQ(document.to_string(), "foo   bar");
}

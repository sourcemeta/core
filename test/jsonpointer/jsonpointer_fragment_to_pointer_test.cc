#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/uri.h>

TEST(JSONPointer_fragment_to_pointer, uri_with_base_and_fragment) {
  const sourcemeta::core::URI uri{"https://example.com#/foo/bar"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 2);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.value().at(1).is_property());
  EXPECT_EQ(pointer.value().at(1).to_property(), "bar");
}

TEST(JSONPointer_fragment_to_pointer, uri_without_fragment) {
  const sourcemeta::core::URI uri{"https://example.com"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_FALSE(pointer.has_value());
}

TEST(JSONPointer_fragment_to_pointer, uri_with_empty_fragment) {
  const auto uri{sourcemeta::core::URI::from_fragment("")};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_TRUE(pointer.value().empty());
}

TEST(JSONPointer_fragment_to_pointer, uri_with_plain_name_fragment) {
  const sourcemeta::core::URI uri{"#foo"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_FALSE(pointer.has_value());
}

TEST(JSONPointer_fragment_to_pointer, uri_with_index_token) {
  const sourcemeta::core::URI uri{"#/foo/0"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 2);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.value().at(1).is_index());
  EXPECT_EQ(pointer.value().at(1).to_index(), 0);
}

TEST(JSONPointer_fragment_to_pointer, uri_with_empty_tokens) {
  const sourcemeta::core::URI uri{"#//"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 2);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "");
  EXPECT_TRUE(pointer.value().at(1).is_property());
  EXPECT_EQ(pointer.value().at(1).to_property(), "");
}

TEST(JSONPointer_fragment_to_pointer, uri_with_percent_encoded_colon) {
  const sourcemeta::core::URI uri{
      "#/$defs/https%3A~1~1example.com~1schema~1type"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 2);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "$defs");
  EXPECT_TRUE(pointer.value().at(1).is_property());
  EXPECT_EQ(pointer.value().at(1).to_property(),
            "https://example.com/schema/type");
}

TEST(JSONPointer_fragment_to_pointer, uri_with_percent_encoded_percent) {
  const sourcemeta::core::URI uri{"#/c%25d"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "c%d");
}

TEST(JSONPointer_fragment_to_pointer, uri_with_percent_encoded_caret) {
  const sourcemeta::core::URI uri{"#/e%5Ef"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "e^f");
}

TEST(JSONPointer_fragment_to_pointer, uri_with_percent_encoded_slash) {
  const sourcemeta::core::URI uri{"#/foo%2Fbar"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 2);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.value().at(1).is_property());
  EXPECT_EQ(pointer.value().at(1).to_property(), "bar");
}

TEST(JSONPointer_fragment_to_pointer, uri_with_percent_encoded_leading_slash) {
  const sourcemeta::core::URI uri{"#%2Ffoo"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "foo");
}

TEST(JSONPointer_fragment_to_pointer, uri_with_lowercase_percent_encoding) {
  const sourcemeta::core::URI uri{"#/a%3ab"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "a:b");
}

TEST(JSONPointer_fragment_to_pointer, uri_with_lone_percent) {
  const auto uri{sourcemeta::core::URI::from_fragment("/100%")};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "100%");
}

TEST(JSONPointer_fragment_to_pointer, uri_with_invalid_tilde_escape) {
  const sourcemeta::core::URI uri{"#/a~b"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_FALSE(pointer.has_value());
}

TEST(JSONPointer_fragment_to_pointer,
     uri_with_percent_encoded_invalid_tilde_escape) {
  const sourcemeta::core::URI uri{"#/a%7Eb"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_FALSE(pointer.has_value());
}

TEST(JSONPointer_fragment_to_pointer, round_trip_tricky_tokens) {
  const sourcemeta::core::Pointer pointer{"a b",  "qu\"ote", "til~de", "sla/sh",
                                          "100%", "co:lon",  "café"};
  const auto result{
      sourcemeta::core::fragment_to_pointer(sourcemeta::core::to_uri(pointer))};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), pointer);
}

TEST(JSONPointer_fragment_to_pointer, round_trip_tricky_tokens_with_base) {
  const sourcemeta::core::Pointer pointer{"a b",  "qu\"ote", "til~de", "sla/sh",
                                          "100%", "co:lon",  "café"};
  const sourcemeta::core::URI base{"https://www.example.com"};
  const auto result{sourcemeta::core::fragment_to_pointer(
      sourcemeta::core::to_uri(pointer, base))};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), pointer);
}

TEST(JSONPointer_fragment_to_pointer, round_trip_percent_encoded_colon) {
  const sourcemeta::core::Pointer pointer{"$defs",
                                          "https://example.com/schema/type"};
  const auto result{
      sourcemeta::core::fragment_to_pointer(sourcemeta::core::to_uri(pointer))};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), pointer);
}

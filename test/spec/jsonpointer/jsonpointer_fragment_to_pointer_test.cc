#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(uri_with_base_and_fragment) {
  const sourcemeta::core::URI uri{"https://example.com#/foo/bar"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 2);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.value().at(1).is_property());
  EXPECT_EQ(pointer.value().at(1).to_property(), "bar");
}

TEST(uri_without_fragment) {
  const sourcemeta::core::URI uri{"https://example.com"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_FALSE(pointer.has_value());
}

TEST(uri_with_empty_fragment) {
  const auto uri{sourcemeta::core::URI::from_fragment("")};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_TRUE(pointer.value().empty());
}

TEST(uri_with_plain_name_fragment) {
  const sourcemeta::core::URI uri{"#foo"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_FALSE(pointer.has_value());
}

TEST(uri_with_index_token) {
  const sourcemeta::core::URI uri{"#/foo/0"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 2);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.value().at(1).is_index());
  EXPECT_EQ(pointer.value().at(1).to_index(), 0);
}

TEST(uri_with_empty_tokens) {
  const sourcemeta::core::URI uri{"#//"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 2);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "");
  EXPECT_TRUE(pointer.value().at(1).is_property());
  EXPECT_EQ(pointer.value().at(1).to_property(), "");
}

TEST(uri_with_percent_encoded_colon) {
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

TEST(uri_with_percent_encoded_percent) {
  const sourcemeta::core::URI uri{"#/c%25d"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "c%d");
}

TEST(uri_with_percent_encoded_caret) {
  const sourcemeta::core::URI uri{"#/e%5Ef"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "e^f");
}

// RFC 3986 Section 2.4 separates the subcomponents of a URI before decoding
// the percent-encoded octets within them, and RFC 3986 Section 2.2 keeps a
// reserved character from being equivalent to the octet that stands for it, so
// the slash that RFC 6901 Section 3 puts in front of every reference token is
// never one that was percent-encoded. RFC 6901 Section 3 gives the slash that
// belongs to a token the one spelling `~1`, so a decoded one leaves the
// fragment conforming to nothing
TEST(uri_with_percent_encoded_slash) {
  const sourcemeta::core::URI uri{"#/foo%2Fbar"};
  EXPECT_FALSE(sourcemeta::core::fragment_to_pointer(uri).has_value());
}

TEST(uri_with_lowercase_percent_encoded_slash) {
  const sourcemeta::core::URI uri{"#/foo%2fbar"};
  EXPECT_FALSE(sourcemeta::core::fragment_to_pointer(uri).has_value());
}

TEST(uri_with_percent_encoded_slash_and_an_unencoded_one) {
  const sourcemeta::core::URI uri{"#/foo%2Fbar/baz"};
  EXPECT_FALSE(sourcemeta::core::fragment_to_pointer(uri).has_value());
}

TEST(uri_with_percent_encoded_slash_as_a_whole_token) {
  const sourcemeta::core::URI uri{"#/%2F"};
  EXPECT_FALSE(sourcemeta::core::fragment_to_pointer(uri).has_value());
}

TEST(uri_with_percent_encoded_slash_ending_a_token) {
  const sourcemeta::core::URI uri{"#/foo%2F"};
  EXPECT_FALSE(sourcemeta::core::fragment_to_pointer(uri).has_value());
}

TEST(uri_with_percent_encoded_slash_after_an_empty_token) {
  const sourcemeta::core::URI uri{"#//%2F"};
  EXPECT_FALSE(sourcemeta::core::fragment_to_pointer(uri).has_value());
}

TEST(uri_with_percent_encoded_slash_before_another_token) {
  const sourcemeta::core::URI uri{"#/%2F/x"};
  EXPECT_FALSE(sourcemeta::core::fragment_to_pointer(uri).has_value());
}

TEST(uri_with_a_lone_separator) {
  const sourcemeta::core::URI uri{"#/"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "");
}

// RFC 6901 Section 3 wants a slash in front of every reference token, and a
// percent-encoded one does not stand in for it, so nothing opens the first
// token here
TEST(uri_with_percent_encoded_leading_slash) {
  const sourcemeta::core::URI uri{"#%2Ffoo"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_FALSE(pointer.has_value());
}

// RFC 3986 Section 2.4 lets an octet that stands for an unreserved character
// be decoded at any time, and the tilde is one, so it still forms the escape
// that RFC 6901 Section 3 reads it as
TEST(uri_with_percent_encoded_tilde_forming_an_escape) {
  const sourcemeta::core::URI uri{"#/a%7E1b"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "a/b");
}

TEST(uri_with_percent_encoded_slash_next_to_a_tilde) {
  const sourcemeta::core::URI uri{"#/a~%2Fb"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_FALSE(pointer.has_value());
}

TEST(round_trip_token_with_a_slash) {
  const sourcemeta::core::Pointer pointer{"foo/bar"};
  const auto result{
      sourcemeta::core::fragment_to_pointer(sourcemeta::core::to_uri(pointer))};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), pointer);
}

TEST(uri_with_lowercase_percent_encoding) {
  const sourcemeta::core::URI uri{"#/a%3ab"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "a:b");
}

TEST(uri_with_lone_percent) {
  const auto uri{sourcemeta::core::URI::from_fragment("/100%")};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "100%");
}

TEST(uri_with_invalid_tilde_escape) {
  const sourcemeta::core::URI uri{"#/a~b"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_FALSE(pointer.has_value());
}

TEST(uri_with_percent_encoded_invalid_tilde_escape) {
  const sourcemeta::core::URI uri{"#/a%7Eb"};
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_FALSE(pointer.has_value());
}

TEST(round_trip_tricky_tokens) {
  const sourcemeta::core::Pointer pointer{"a b",  "qu\"ote", "til~de", "sla/sh",
                                          "100%", "co:lon",  "café"};
  const auto result{
      sourcemeta::core::fragment_to_pointer(sourcemeta::core::to_uri(pointer))};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), pointer);
}

TEST(round_trip_tricky_tokens_with_base) {
  const sourcemeta::core::Pointer pointer{"a b",  "qu\"ote", "til~de", "sla/sh",
                                          "100%", "co:lon",  "café"};
  const sourcemeta::core::URI base{"https://www.example.com"};
  const auto result{sourcemeta::core::fragment_to_pointer(
      sourcemeta::core::to_uri(pointer, base))};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), pointer);
}

TEST(round_trip_token_with_percent_encoded_sequence) {
  const sourcemeta::core::Pointer pointer{"a%20b"};
  const auto result{
      sourcemeta::core::fragment_to_pointer(sourcemeta::core::to_uri(pointer))};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), pointer);
}

TEST(round_trip_token_with_percent_encoded_sequence_with_base) {
  const sourcemeta::core::Pointer pointer{"a%20b"};
  const sourcemeta::core::URI base{"https://www.example.com"};
  const auto result{sourcemeta::core::fragment_to_pointer(
      sourcemeta::core::to_uri(pointer, base))};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), pointer);
}

TEST(round_trip_percent_encoded_colon) {
  const sourcemeta::core::Pointer pointer{"$defs",
                                          "https://example.com/schema/type"};
  const auto result{
      sourcemeta::core::fragment_to_pointer(sourcemeta::core::to_uri(pointer))};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), pointer);
}

// RFC 3986 Section 2.1 wants two hex digits after the percent, and anything
// else is no escape at all, so the percent stands for itself
TEST(uri_with_a_percent_followed_by_non_hex_digits) {
  sourcemeta::core::URI uri{"#/placeholder"};
  uri.fragment("/a%zzb");
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "a%zzb");
}

TEST(uri_with_a_percent_whose_second_digit_is_not_hex) {
  sourcemeta::core::URI uri{"#/placeholder"};
  uri.fragment("/a%4zb");
  const auto pointer{sourcemeta::core::fragment_to_pointer(uri)};
  EXPECT_TRUE(pointer.has_value());
  EXPECT_EQ(pointer.value().size(), 1);
  EXPECT_TRUE(pointer.value().at(0).is_property());
  EXPECT_EQ(pointer.value().at(0).to_property(), "a%4zb");
}

#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/uri.h>

TEST(JSONPointer_to_uri, empty) {
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#");
}

TEST(JSONPointer_to_uri, empty_fragment) {
  const sourcemeta::core::Pointer pointer{""};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/");
}

TEST(JSONPointer_to_uri, with_property_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/bar/baz");
}

TEST(JSONPointer_to_uri, with_index_tokens) {
  const sourcemeta::core::Pointer pointer{0, 1, 2};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/0/1/2");
}

TEST(JSONPointer_to_uri, escape_slash) {
  const sourcemeta::core::Pointer pointer{"foo", "bar/baz"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/bar~1baz");
}

TEST(JSONPointer_to_uri, escape_tilde) {
  const sourcemeta::core::Pointer pointer{"foo", "bar~baz"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/bar~0baz");
}

TEST(JSONPointer_to_uri, escape_uri_percentage) {
  const sourcemeta::core::Pointer pointer{"foo", "percent%field"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/percent%25field");
}

TEST(JSONPointer_to_uri, escape_uri_quote) {
  const sourcemeta::core::Pointer pointer{"foo", "quote\"field"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/quote%22field");
}

TEST(JSONPointer_to_uri, escape_uri_backslash) {
  const sourcemeta::core::Pointer pointer{"foo", "bar\\baz"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/bar%5Cbaz");
}

TEST(JSONPointer_to_uri, escape_uri_dollar_sign) {
  const sourcemeta::core::Pointer pointer{"foo", "$id"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/$id");
}

TEST(JSONPointer_to_uri, escape_00) {
  using namespace std::string_literals;
  const sourcemeta::core::Pointer pointer{"foo\0bar"s};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0000bar");
}

TEST(JSONPointer_to_uri, escape_01) {
  const sourcemeta::core::Pointer pointer{"foo\u0001bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0001bar");
}

TEST(JSONPointer_to_uri, escape_02) {
  const sourcemeta::core::Pointer pointer{"foo\u0002bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0002bar");
}

TEST(JSONPointer_to_uri, escape_03) {
  const sourcemeta::core::Pointer pointer{"foo\u0003bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0003bar");
}

TEST(JSONPointer_to_uri, escape_04) {
  const sourcemeta::core::Pointer pointer{"foo\u0004bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0004bar");
}

TEST(JSONPointer_to_uri, escape_05) {
  const sourcemeta::core::Pointer pointer{"foo\u0005bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0005bar");
}

TEST(JSONPointer_to_uri, escape_06) {
  const sourcemeta::core::Pointer pointer{"foo\u0006bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0006bar");
}

TEST(JSONPointer_to_uri, escape_07) {
  const sourcemeta::core::Pointer pointer{"foo\u0007bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0007bar");
}

TEST(JSONPointer_to_uri, escape_08) {
  const sourcemeta::core::Pointer pointer{"foo\u0008bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cbbar");
}

TEST(JSONPointer_to_uri, escape_09) {
  const sourcemeta::core::Pointer pointer{"foo\u0009bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Ctbar");
}

TEST(JSONPointer_to_uri, escape_0A) {
  const sourcemeta::core::Pointer pointer{"foo\u000abar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cnbar");
}

TEST(JSONPointer_to_uri, escape_0B) {
  const sourcemeta::core::Pointer pointer{"foo\u000bbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu000Bbar");
}

TEST(JSONPointer_to_uri, escape_0C) {
  const sourcemeta::core::Pointer pointer{"foo\u000cbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cfbar");
}

TEST(JSONPointer_to_uri, escape_0D) {
  const sourcemeta::core::Pointer pointer{"foo\u000dbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Crbar");
}

TEST(JSONPointer_to_uri, escape_0E) {
  const sourcemeta::core::Pointer pointer{"foo\u000ebar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu000Ebar");
}

TEST(JSONPointer_to_uri, escape_0F) {
  const sourcemeta::core::Pointer pointer{"foo\u000fbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu000Fbar");
}

TEST(JSONPointer_to_uri, escape_10) {
  const sourcemeta::core::Pointer pointer{"foo\u0010bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0010bar");
}

TEST(JSONPointer_to_uri, escape_11) {
  const sourcemeta::core::Pointer pointer{"foo\u0011bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0011bar");
}

TEST(JSONPointer_to_uri, escape_12) {
  const sourcemeta::core::Pointer pointer{"foo\u0012bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0012bar");
}

TEST(JSONPointer_to_uri, escape_13) {
  const sourcemeta::core::Pointer pointer{"foo\u0013bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0013bar");
}

TEST(JSONPointer_to_uri, escape_14) {
  const sourcemeta::core::Pointer pointer{"foo\u0014bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0014bar");
}

TEST(JSONPointer_to_uri, escape_15) {
  const sourcemeta::core::Pointer pointer{"foo\u0015bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0015bar");
}

TEST(JSONPointer_to_uri, escape_16) {
  const sourcemeta::core::Pointer pointer{"foo\u0016bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0016bar");
}

TEST(JSONPointer_to_uri, escape_17) {
  const sourcemeta::core::Pointer pointer{"foo\u0017bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0017bar");
}

TEST(JSONPointer_to_uri, escape_18) {
  const sourcemeta::core::Pointer pointer{"foo\u0018bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0018bar");
}

TEST(JSONPointer_to_uri, escape_19) {
  const sourcemeta::core::Pointer pointer{"foo\u0019bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu0019bar");
}

TEST(JSONPointer_to_uri, escape_1A) {
  const sourcemeta::core::Pointer pointer{"foo\u001abar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu001Abar");
}

TEST(JSONPointer_to_uri, escape_1B) {
  const sourcemeta::core::Pointer pointer{"foo\u001bbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu001Bbar");
}

TEST(JSONPointer_to_uri, escape_1C) {
  const sourcemeta::core::Pointer pointer{"foo\u001cbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu001Cbar");
}

TEST(JSONPointer_to_uri, escape_1D) {
  const sourcemeta::core::Pointer pointer{"foo\u001dbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu001Dbar");
}

TEST(JSONPointer_to_uri, escape_1E) {
  const sourcemeta::core::Pointer pointer{"foo\u001ebar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu001Ebar");
}

TEST(JSONPointer_to_uri, escape_1F) {
  const sourcemeta::core::Pointer pointer{"foo\u001fbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%5Cu001Fbar");
}

TEST(JSONPointer_to_uri, with_absolute_base) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::URI base{"https://www.example.com"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer, base)};
  EXPECT_EQ(fragment.recompose(), "https://www.example.com#/foo/bar");
}

TEST(JSONPointer_to_uri, with_relative_base) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::URI base{"../baz"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer, base)};
  EXPECT_EQ(fragment.recompose(), "../baz#/foo/bar");
}

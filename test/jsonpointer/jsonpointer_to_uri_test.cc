#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(empty) {
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#");
}

TEST(empty_fragment) {
  const sourcemeta::core::Pointer pointer{""};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/");
}

TEST(with_property_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/bar/baz");
}

TEST(with_index_tokens) {
  const sourcemeta::core::Pointer pointer{0, 1, 2};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/0/1/2");
}

TEST(escape_slash) {
  const sourcemeta::core::Pointer pointer{"foo", "bar/baz"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/bar~1baz");
}

TEST(escape_tilde) {
  const sourcemeta::core::Pointer pointer{"foo", "bar~baz"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/bar~0baz");
}

TEST(escape_uri_percentage) {
  const sourcemeta::core::Pointer pointer{"foo", "percent%field"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/percent%25field");
}

TEST(escape_uri_quote) {
  const sourcemeta::core::Pointer pointer{"foo", "quote\"field"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/quote%22field");
}

TEST(escape_uri_backslash) {
  const sourcemeta::core::Pointer pointer{"foo", "bar\\baz"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/bar%5Cbaz");
}

TEST(escape_uri_dollar_sign) {
  const sourcemeta::core::Pointer pointer{"foo", "$id"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo/$id");
}

TEST(escape_00) {
  using namespace std::string_literals;
  const sourcemeta::core::Pointer pointer{"foo\0bar"s};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%00bar");
}

TEST(escape_01) {
  const sourcemeta::core::Pointer pointer{"foo\u0001bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%01bar");
}

TEST(escape_02) {
  const sourcemeta::core::Pointer pointer{"foo\u0002bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%02bar");
}

TEST(escape_03) {
  const sourcemeta::core::Pointer pointer{"foo\u0003bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%03bar");
}

TEST(escape_04) {
  const sourcemeta::core::Pointer pointer{"foo\u0004bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%04bar");
}

TEST(escape_05) {
  const sourcemeta::core::Pointer pointer{"foo\u0005bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%05bar");
}

TEST(escape_06) {
  const sourcemeta::core::Pointer pointer{"foo\u0006bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%06bar");
}

TEST(escape_07) {
  const sourcemeta::core::Pointer pointer{"foo\u0007bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%07bar");
}

TEST(escape_08) {
  const sourcemeta::core::Pointer pointer{"foo\u0008bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%08bar");
}

TEST(escape_09) {
  const sourcemeta::core::Pointer pointer{"foo\u0009bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%09bar");
}

TEST(escape_0A) {
  const sourcemeta::core::Pointer pointer{"foo\u000abar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%0Abar");
}

TEST(escape_0B) {
  const sourcemeta::core::Pointer pointer{"foo\u000bbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%0Bbar");
}

TEST(escape_0C) {
  const sourcemeta::core::Pointer pointer{"foo\u000cbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%0Cbar");
}

TEST(escape_0D) {
  const sourcemeta::core::Pointer pointer{"foo\u000dbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%0Dbar");
}

TEST(escape_0E) {
  const sourcemeta::core::Pointer pointer{"foo\u000ebar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%0Ebar");
}

TEST(escape_0F) {
  const sourcemeta::core::Pointer pointer{"foo\u000fbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%0Fbar");
}

TEST(escape_10) {
  const sourcemeta::core::Pointer pointer{"foo\u0010bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%10bar");
}

TEST(escape_11) {
  const sourcemeta::core::Pointer pointer{"foo\u0011bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%11bar");
}

TEST(escape_12) {
  const sourcemeta::core::Pointer pointer{"foo\u0012bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%12bar");
}

TEST(escape_13) {
  const sourcemeta::core::Pointer pointer{"foo\u0013bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%13bar");
}

TEST(escape_14) {
  const sourcemeta::core::Pointer pointer{"foo\u0014bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%14bar");
}

TEST(escape_15) {
  const sourcemeta::core::Pointer pointer{"foo\u0015bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%15bar");
}

TEST(escape_16) {
  const sourcemeta::core::Pointer pointer{"foo\u0016bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%16bar");
}

TEST(escape_17) {
  const sourcemeta::core::Pointer pointer{"foo\u0017bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%17bar");
}

TEST(escape_18) {
  const sourcemeta::core::Pointer pointer{"foo\u0018bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%18bar");
}

TEST(escape_19) {
  const sourcemeta::core::Pointer pointer{"foo\u0019bar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%19bar");
}

TEST(escape_1A) {
  const sourcemeta::core::Pointer pointer{"foo\u001abar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%1Abar");
}

TEST(escape_1B) {
  const sourcemeta::core::Pointer pointer{"foo\u001bbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%1Bbar");
}

TEST(escape_1C) {
  const sourcemeta::core::Pointer pointer{"foo\u001cbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%1Cbar");
}

TEST(escape_1D) {
  const sourcemeta::core::Pointer pointer{"foo\u001dbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%1Dbar");
}

TEST(escape_1E) {
  const sourcemeta::core::Pointer pointer{"foo\u001ebar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%1Ebar");
}

TEST(escape_1F) {
  const sourcemeta::core::Pointer pointer{"foo\u001fbar"};
  std::ostringstream stream;
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/foo%1Fbar");
}

TEST(with_absolute_base) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::URI base{"https://www.example.com"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer, base)};
  EXPECT_EQ(fragment.recompose(), "https://www.example.com#/foo/bar");
}

TEST(with_absolute_base_percentage) {
  const sourcemeta::core::Pointer pointer{"foo%bar"};
  const sourcemeta::core::URI base{"https://www.example.com"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer, base)};
  // The %ba in foo%bar happens to be a valid percent-encoded sequence.
  // Canonicalize uppercases the hex digits.
  EXPECT_EQ(fragment.recompose(), "https://www.example.com#/foo%BAr");
}

TEST(with_relative_base) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::URI base{"../baz"};
  const sourcemeta::core::URI fragment{sourcemeta::core::to_uri(pointer, base)};
  EXPECT_EQ(fragment.recompose(), "../baz#/foo/bar");
}

TEST(with_at_sign) {
  const sourcemeta::core::Pointer pointer{"@foo", "bar"};
  const auto fragment{sourcemeta::core::to_uri(pointer)};
  EXPECT_EQ(fragment.recompose(), "#/@foo/bar");
}

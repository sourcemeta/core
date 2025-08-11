#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <sstream>

TEST(JSONPointer_stringify, empty_pointer) {
  const sourcemeta::core::Pointer pointer;
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "");
}

TEST(JSONPointer_stringify, empty_string) {
  const sourcemeta::core::Pointer pointer{""};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/");
}

TEST(JSONPointer_stringify, string_space) {
  const sourcemeta::core::Pointer pointer{" "};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/ ");
}

TEST(JSONPointer_stringify, foo) {
  const sourcemeta::core::Pointer pointer{"foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo");
}

TEST(JSONPointer_stringify, hyphen_single) {
  const sourcemeta::core::Pointer pointer{"-"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/-");
}

TEST(JSONPointer_stringify, hyphen_last) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "-"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo/bar/-");
}

TEST(JSONPointer_stringify, hyphen_multi) {
  const sourcemeta::core::Pointer pointer{"-", "-", "-"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/-/-/-");
}

TEST(JSONPointer_stringify, foo_bar_baz) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo/bar/baz");
}

TEST(JSONPointer_stringify, foo_empty_bar) {
  const sourcemeta::core::Pointer pointer{"foo", "", "baz"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo//baz");
}

TEST(JSONPointer_stringify, 0) {
  const sourcemeta::core::Pointer pointer{0};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/0");
}

TEST(JSONPointer_stringify, 0_1_2) {
  const sourcemeta::core::Pointer pointer{0, 1, 2};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/0/1/2");
}

TEST(JSONPointer_stringify, multi_digit) {
  const sourcemeta::core::Pointer pointer{1234};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/1234");
}

TEST(JSONPointer_stringify, escape_slash) {
  const sourcemeta::core::Pointer pointer{"foo/bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo~1bar");
}

TEST(JSONPointer_stringify, escape_tilde) {
  const sourcemeta::core::Pointer pointer{"foo~bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo~0bar");
}

TEST(JSONPointer_stringify, escape_multiple) {
  const sourcemeta::core::Pointer pointer{"a/b", "m~n"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/a~1b/m~0n");
}

TEST(JSONPointer_stringify, tilde_0) {
  const sourcemeta::core::Pointer pointer{"foo~0bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo~00bar");
}

TEST(JSONPointer_stringify, tilde_1) {
  const sourcemeta::core::Pointer pointer{"foo~1bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo~01bar");
}

TEST(JSONPointer_stringify, backslash) {
  const sourcemeta::core::Pointer pointer{"foo\\bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\bar");
}

TEST(JSONPointer_stringify, double_quote) {
  const sourcemeta::core::Pointer pointer{"foo\"bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\"bar");
}

TEST(JSONPointer_stringify, single_quote) {
  const sourcemeta::core::Pointer pointer{"foo'bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo'bar");
}

TEST(JSONPointer_stringify, escape_00) {
  using namespace std::string_literals;
  const sourcemeta::core::Pointer pointer{"foo\0bar"s};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0000bar");
}

TEST(JSONPointer_stringify, escape_01) {
  const sourcemeta::core::Pointer pointer{"foo\u0001bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0001bar");
}

TEST(JSONPointer_stringify, escape_02) {
  const sourcemeta::core::Pointer pointer{"foo\u0002bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0002bar");
}

TEST(JSONPointer_stringify, escape_03) {
  const sourcemeta::core::Pointer pointer{"foo\u0003bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0003bar");
}

TEST(JSONPointer_stringify, escape_04) {
  const sourcemeta::core::Pointer pointer{"foo\u0004bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0004bar");
}

TEST(JSONPointer_stringify, escape_05) {
  const sourcemeta::core::Pointer pointer{"foo\u0005bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0005bar");
}

TEST(JSONPointer_stringify, escape_06) {
  const sourcemeta::core::Pointer pointer{"foo\u0006bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0006bar");
}

TEST(JSONPointer_stringify, escape_07) {
  const sourcemeta::core::Pointer pointer{"foo\u0007bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0007bar");
}

TEST(JSONPointer_stringify, escape_08) {
  const sourcemeta::core::Pointer pointer{"foo\u0008bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\bbar");
}

TEST(JSONPointer_stringify, escape_09) {
  const sourcemeta::core::Pointer pointer{"foo\u0009bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\tbar");
}

TEST(JSONPointer_stringify, escape_0A) {
  const sourcemeta::core::Pointer pointer{"foo\u000abar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\nbar");
}

TEST(JSONPointer_stringify, escape_0B) {
  const sourcemeta::core::Pointer pointer{"foo\u000bbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u000Bbar");
}

TEST(JSONPointer_stringify, escape_0C) {
  const sourcemeta::core::Pointer pointer{"foo\u000cbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\fbar");
}

TEST(JSONPointer_stringify, escape_0D) {
  const sourcemeta::core::Pointer pointer{"foo\u000dbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\rbar");
}

TEST(JSONPointer_stringify, escape_0E) {
  const sourcemeta::core::Pointer pointer{"foo\u000ebar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u000Ebar");
}

TEST(JSONPointer_stringify, escape_0F) {
  const sourcemeta::core::Pointer pointer{"foo\u000fbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u000Fbar");
}

TEST(JSONPointer_stringify, escape_10) {
  const sourcemeta::core::Pointer pointer{"foo\u0010bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0010bar");
}

TEST(JSONPointer_stringify, escape_11) {
  const sourcemeta::core::Pointer pointer{"foo\u0011bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0011bar");
}

TEST(JSONPointer_stringify, escape_12) {
  const sourcemeta::core::Pointer pointer{"foo\u0012bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0012bar");
}

TEST(JSONPointer_stringify, escape_13) {
  const sourcemeta::core::Pointer pointer{"foo\u0013bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0013bar");
}

TEST(JSONPointer_stringify, escape_14) {
  const sourcemeta::core::Pointer pointer{"foo\u0014bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0014bar");
}

TEST(JSONPointer_stringify, escape_15) {
  const sourcemeta::core::Pointer pointer{"foo\u0015bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0015bar");
}

TEST(JSONPointer_stringify, escape_16) {
  const sourcemeta::core::Pointer pointer{"foo\u0016bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0016bar");
}

TEST(JSONPointer_stringify, escape_17) {
  const sourcemeta::core::Pointer pointer{"foo\u0017bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0017bar");
}

TEST(JSONPointer_stringify, escape_18) {
  const sourcemeta::core::Pointer pointer{"foo\u0018bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0018bar");
}

TEST(JSONPointer_stringify, escape_19) {
  const sourcemeta::core::Pointer pointer{"foo\u0019bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u0019bar");
}

TEST(JSONPointer_stringify, escape_1A) {
  const sourcemeta::core::Pointer pointer{"foo\u001abar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u001Abar");
}

TEST(JSONPointer_stringify, escape_1B) {
  const sourcemeta::core::Pointer pointer{"foo\u001bbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u001Bbar");
}

TEST(JSONPointer_stringify, escape_1C) {
  const sourcemeta::core::Pointer pointer{"foo\u001cbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u001Cbar");
}

TEST(JSONPointer_stringify, escape_1D) {
  const sourcemeta::core::Pointer pointer{"foo\u001dbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u001Dbar");
}

TEST(JSONPointer_stringify, escape_1E) {
  const sourcemeta::core::Pointer pointer{"foo\u001ebar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u001Ebar");
}

TEST(JSONPointer_stringify, escape_1F) {
  const sourcemeta::core::Pointer pointer{"foo\u001fbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\u001Fbar");
}

TEST(JSONPointer_stringify, no_uri_escape) {
  const sourcemeta::core::Pointer pointer{"foo", "percent%field"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo/percent%field");
}

TEST(JSONWeakPointer, empty_string) {
  const std::string empty_string = "";
  const sourcemeta::core::WeakPointer pointer{std::cref(empty_string)};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/");
}

TEST(JSONWeakPointer, string) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo");
}

TEST(JSONWeakPointer, index) {
  const sourcemeta::core::WeakPointer pointer{2};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/2");
}

TEST(JSONWeakPointer, foo_bar_baz) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo/bar/baz");
}

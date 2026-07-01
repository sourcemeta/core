#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <sstream>

TEST(empty_pointer) {
  const sourcemeta::core::Pointer pointer;
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "");
}

TEST(empty_string) {
  const sourcemeta::core::Pointer pointer{""};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/");
}

TEST(string_space) {
  const sourcemeta::core::Pointer pointer{" "};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/ ");
}

TEST(foo) {
  const sourcemeta::core::Pointer pointer{"foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo");
}

TEST(hyphen_single) {
  const sourcemeta::core::Pointer pointer{"-"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/-");
}

TEST(hyphen_last) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "-"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo/bar/-");
}

TEST(hyphen_multi) {
  const sourcemeta::core::Pointer pointer{"-", "-", "-"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/-/-/-");
}

TEST(foo_bar_baz) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo/bar/baz");
}

TEST(foo_empty_bar) {
  const sourcemeta::core::Pointer pointer{"foo", "", "baz"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo//baz");
}

TEST(0) {
  const sourcemeta::core::Pointer pointer{0};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/0");
}

TEST(0_1_2) {
  const sourcemeta::core::Pointer pointer{0, 1, 2};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/0/1/2");
}

TEST(multi_digit) {
  const sourcemeta::core::Pointer pointer{1234};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/1234");
}

TEST(escape_slash) {
  const sourcemeta::core::Pointer pointer{"foo/bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo~1bar");
}

TEST(escape_tilde) {
  const sourcemeta::core::Pointer pointer{"foo~bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo~0bar");
}

TEST(escape_multiple) {
  const sourcemeta::core::Pointer pointer{"a/b", "m~n"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/a~1b/m~0n");
}

TEST(tilde_0) {
  const sourcemeta::core::Pointer pointer{"foo~0bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo~00bar");
}

TEST(tilde_1) {
  const sourcemeta::core::Pointer pointer{"foo~1bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo~01bar");
}

TEST(backslash) {
  const sourcemeta::core::Pointer pointer{"foo\\bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\\bar");
}

TEST(double_quote) {
  const sourcemeta::core::Pointer pointer{"foo\"bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\"bar");
}

TEST(single_quote) {
  const sourcemeta::core::Pointer pointer{"foo'bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo'bar");
}

TEST(escape_00) {
  using namespace std::string_literals;
  const sourcemeta::core::Pointer pointer{"foo\0bar"s};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\0bar"s);
}

TEST(escape_01) {
  const sourcemeta::core::Pointer pointer{"foo\u0001bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0001bar");
}

TEST(escape_02) {
  const sourcemeta::core::Pointer pointer{"foo\u0002bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0002bar");
}

TEST(escape_03) {
  const sourcemeta::core::Pointer pointer{"foo\u0003bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0003bar");
}

TEST(escape_04) {
  const sourcemeta::core::Pointer pointer{"foo\u0004bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0004bar");
}

TEST(escape_05) {
  const sourcemeta::core::Pointer pointer{"foo\u0005bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0005bar");
}

TEST(escape_06) {
  const sourcemeta::core::Pointer pointer{"foo\u0006bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0006bar");
}

TEST(escape_07) {
  const sourcemeta::core::Pointer pointer{"foo\u0007bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0007bar");
}

TEST(escape_08) {
  const sourcemeta::core::Pointer pointer{"foo\u0008bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0008bar");
}

TEST(escape_09) {
  const sourcemeta::core::Pointer pointer{"foo\u0009bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0009bar");
}

TEST(escape_0A) {
  const sourcemeta::core::Pointer pointer{"foo\u000abar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u000Abar");
}

TEST(escape_0B) {
  const sourcemeta::core::Pointer pointer{"foo\u000bbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u000Bbar");
}

TEST(escape_0C) {
  const sourcemeta::core::Pointer pointer{"foo\u000cbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u000Cbar");
}

TEST(escape_0D) {
  const sourcemeta::core::Pointer pointer{"foo\u000dbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u000Dbar");
}

TEST(escape_0E) {
  const sourcemeta::core::Pointer pointer{"foo\u000ebar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u000Ebar");
}

TEST(escape_0F) {
  const sourcemeta::core::Pointer pointer{"foo\u000fbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u000Fbar");
}

TEST(escape_10) {
  const sourcemeta::core::Pointer pointer{"foo\u0010bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0010bar");
}

TEST(escape_11) {
  const sourcemeta::core::Pointer pointer{"foo\u0011bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0011bar");
}

TEST(escape_12) {
  const sourcemeta::core::Pointer pointer{"foo\u0012bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0012bar");
}

TEST(escape_13) {
  const sourcemeta::core::Pointer pointer{"foo\u0013bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0013bar");
}

TEST(escape_14) {
  const sourcemeta::core::Pointer pointer{"foo\u0014bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0014bar");
}

TEST(escape_15) {
  const sourcemeta::core::Pointer pointer{"foo\u0015bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0015bar");
}

TEST(escape_16) {
  const sourcemeta::core::Pointer pointer{"foo\u0016bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0016bar");
}

TEST(escape_17) {
  const sourcemeta::core::Pointer pointer{"foo\u0017bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0017bar");
}

TEST(escape_18) {
  const sourcemeta::core::Pointer pointer{"foo\u0018bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0018bar");
}

TEST(escape_19) {
  const sourcemeta::core::Pointer pointer{"foo\u0019bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u0019bar");
}

TEST(escape_1A) {
  const sourcemeta::core::Pointer pointer{"foo\u001abar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u001Abar");
}

TEST(escape_1B) {
  const sourcemeta::core::Pointer pointer{"foo\u001bbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u001Bbar");
}

TEST(escape_1C) {
  const sourcemeta::core::Pointer pointer{"foo\u001cbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u001Cbar");
}

TEST(escape_1D) {
  const sourcemeta::core::Pointer pointer{"foo\u001dbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u001Dbar");
}

TEST(escape_1E) {
  const sourcemeta::core::Pointer pointer{"foo\u001ebar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u001Ebar");
}

TEST(escape_1F) {
  const sourcemeta::core::Pointer pointer{"foo\u001fbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo\u001Fbar");
}

TEST(no_uri_escape) {
  const sourcemeta::core::Pointer pointer{"foo", "percent%field"};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo/percent%field");
}

TEST(weak_empty_string) {
  const std::string empty_string = "";
  const sourcemeta::core::WeakPointer pointer{std::cref(empty_string)};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/");
}

TEST(string) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo");
}

TEST(index) {
  const sourcemeta::core::WeakPointer pointer{2};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/2");
}

TEST(weak_foo_bar_baz) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);
  EXPECT_EQ(stream.str(), "/foo/bar/baz");
}

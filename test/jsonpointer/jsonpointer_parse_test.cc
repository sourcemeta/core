#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <string>
#include <string_view>

TEST(empty_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer(""));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("");
  EXPECT_EQ(pointer.size(), 0);
}

TEST(slash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "");
}

TEST(slash_f) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/f"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/f");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "f");
}

TEST(slash_foo) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
}

TEST(slash_foo_bar_baz) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/bar/baz"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo/bar/baz");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "baz");
}

TEST(slash_0) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/0"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/0");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 0);
}

TEST(slash_1) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 1);
}

TEST(slash_1234) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1234"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/1234");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 1234);
}

TEST(slash_0_slash_1_slash_2) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/0/1/2"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/0/1/2");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 0);
  EXPECT_TRUE(pointer.at(1).is_index());
  EXPECT_EQ(pointer.at(1).to_index(), 1);
  EXPECT_TRUE(pointer.at(2).is_index());
  EXPECT_EQ(pointer.at(2).to_index(), 2);
}

TEST(slash_0_slash_foo_slash_2) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/0/foo/2"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/0/foo/2");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 0);
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "foo");
  EXPECT_TRUE(pointer.at(2).is_index());
  EXPECT_EQ(pointer.at(2).to_index(), 2);
}

TEST(slash_0_slash_1234) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/0/1234"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/0/1234");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 0);
  EXPECT_TRUE(pointer.at(1).is_index());
  EXPECT_EQ(pointer.at(1).to_index(), 1234);
}

TEST(slash_1000) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1000"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/1000");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 1000);
}

TEST(negative_index_as_property) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/-1"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/-1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "-1");
}

TEST(slash_space_integer) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/ 12"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/ 12");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), " 12");
}

TEST(slash_foo_slash_space_integer) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/ 12"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo/ 12");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), " 12");
}

TEST(slash_foo_slash_space_bar_space) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/ bar "));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo/ bar ");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), " bar ");
}

TEST(slash_f_slash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/f/"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/f/");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "f");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "");
}

TEST(slash_slash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("//"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("//");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "");
}

TEST(slash_slash_slash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("///"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("///");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "");
}

TEST(slash_slash_slash_foo) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("///foo"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("///foo");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "foo");
}

TEST(positive_hex_index) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/0x23"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/0x23");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "0x23");
}

TEST(slash_00) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/00"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/00");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "00");
}

TEST(slash_01) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/01"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/01");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "01");
}

TEST(slash_001) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/001"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/001");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "001");
}

TEST(slash_0100) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/0100"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/0100");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "0100");
}

TEST(positive_real_number_index) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1.23"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/1.23");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "1.23");
}

TEST(index_in_scientific_notation) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1e1"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/1e1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "1e1");
}

TEST(integer_with_space) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1 1"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/1 1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "1 1");
}

TEST(percentage) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/c%d"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/c%d");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "c%d");
}

TEST(caret) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/e^f"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/e^f");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "e^f");
}

TEST(pipe) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/g|h"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/g|h");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "g|h");
}

TEST(backslash_after_letter) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/i\\j"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/i\\j");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "i\\j");
}

TEST(backslash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/\\");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\\");
}

TEST(double_quote_after_letter) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/k\"l"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/k\"l");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "k\"l");
}

TEST(double_quote) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\""));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/\"");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\"");
}

TEST(space) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/ "));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/ ");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), " ");
}

TEST(escaped_slash_after_letter) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a~1b"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/a~1b");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "a/b");
}

TEST(escaped_slash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~1"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/~1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "/");
}

TEST(escaped_slash_with_backslash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\/"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/\\/");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\\");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "");
}

TEST(escaped_slash_with_backslash_after_letter) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/x\\/"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/x\\/");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "x\\");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "");
}

TEST(escaped_tilde_after_letter) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/m~0n"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/m~0n");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "m~n");
}

TEST(escaped_tilde) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/~0");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "~");
}

TEST(tilde_zero_one) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~01"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/~01");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "~1");
}

TEST(tilde_zero_one_after_letter) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/x~01"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/x~01");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "x~1");
}

TEST(hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/-"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/-");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_hyphen());
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "-");
}

TEST(hyphen_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/--"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/--");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_FALSE(pointer.at(0).is_hyphen());
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "--");
}

TEST(hyphen_slash_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/-/-"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/-/-");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_hyphen());
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "-");
  EXPECT_TRUE(pointer.at(1).is_hyphen());
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "-");
}

TEST(quote_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\"bar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\"bar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\"bar");
}

TEST(quote) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\""));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/\"");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\"");
}

TEST(backslash_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\\bar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\\bar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\\bar");
}

TEST(backslash_only) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/\\");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\\");
}

TEST(control_backspace) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\b"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/\b");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\b");
}

TEST(control_backspace_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\bbar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\bbar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\bbar");
}

TEST(control_formfeed) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\f"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/\f");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\f");
}

TEST(control_formfeed_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\fbar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\fbar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\fbar");
}

TEST(control_newline) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\n"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/\n");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\n");
}

TEST(control_newline_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\nbar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\nbar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\nbar");
}

TEST(control_carriage) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\r"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/\r");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\r");
}

TEST(control_carriage_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\rbar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\rbar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\rbar");
}

TEST(control_tab) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\t"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/\t");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\t");
}

TEST(control_tab_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\tbar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\tbar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\tbar");
}

TEST(property_with_asterisk) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo*bar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo*bar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo*bar");
}

TEST(two_tokens) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/bar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo/bar");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
}

TEST(regex_caret) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/patternProperties/^v/foo"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/patternProperties/^v/foo");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "patternProperties");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "^v");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "foo");
}

TEST(regex_backslash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/[\\-]"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/[\\-]");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "[\\-]");
}

TEST(json_escape_backslash_n_treated_literally) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\\nbar"));
  const auto pointer = sourcemeta::core::to_pointer("/foo\\nbar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\\nbar");
}

TEST(json_escape_unicode_treated_literally) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\\u002Abar"));
  const auto pointer = sourcemeta::core::to_pointer("/foo\\u002Abar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\\u002Abar");
}

TEST(unicode_0001) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\u0001"));
  const auto pointer = sourcemeta::core::to_pointer("/\u0001");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\u0001");
}

TEST(unicode_0002) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\u0002"));
  const auto pointer = sourcemeta::core::to_pointer("/\u0002");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\u0002");
}

TEST(unicode_000B) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\u000B"));
  const auto pointer = sourcemeta::core::to_pointer("/\u000B");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\u000B");
}

TEST(unicode_000E) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\u000E"));
  const auto pointer = sourcemeta::core::to_pointer("/\u000E");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\u000E");
}

TEST(unicode_001F) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\u001F"));
  const auto pointer = sourcemeta::core::to_pointer("/\u001F");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\u001F");
}

TEST(unicode_0001_within_word) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\u0001bar"));
  const auto pointer = sourcemeta::core::to_pointer("/foo\u0001bar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\u0001bar");
}

TEST(unicode_000E_within_word) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\u000Ebar"));
  const auto pointer = sourcemeta::core::to_pointer("/foo\u000Ebar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\u000Ebar");
}

TEST(unicode_001F_within_word) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\u001Fbar"));
  const auto pointer = sourcemeta::core::to_pointer("/foo\u001Fbar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\u001Fbar");
}

TEST(to_pointer_with_string_view) {
  const std::string_view input{"/foo/bar"};
  const auto pointer = sourcemeta::core::to_pointer(input);
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
}

TEST(to_pointer_with_string_view_subview) {
  const std::string buffer{"prefix/foo/bar/0suffix"};
  const std::string_view view{buffer.data() + 6, 10};
  const auto pointer = sourcemeta::core::to_pointer(view);
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
  EXPECT_TRUE(pointer.at(2).is_index());
  EXPECT_EQ(pointer.at(2).to_index(), 0);
}

TEST(to_pointer_default_string_view) {
  const std::string_view input{};
  EXPECT_EQ(input.data(), nullptr);
  const auto pointer = sourcemeta::core::to_pointer(input);
  EXPECT_EQ(pointer.size(), 0);
}

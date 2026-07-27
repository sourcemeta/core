#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

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

TEST(four_empty_tokens) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("////"));
  const auto pointer = sourcemeta::core::to_pointer("////");
  EXPECT_EQ(pointer.size(), 4);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "");
  EXPECT_TRUE(pointer.at(3).is_property());
  EXPECT_EQ(pointer.at(3).to_property(), "");
}

TEST(empty_interior_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo//bar"));
  const auto pointer = sourcemeta::core::to_pointer("/foo//bar");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "bar");
}

TEST(trailing_empty_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/bar/"));
  const auto pointer = sourcemeta::core::to_pointer("/foo/bar/");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "");
}

TEST(leading_then_empty) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("//foo"));
  const auto pointer = sourcemeta::core::to_pointer("//foo");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "foo");
}

TEST(single_char_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a"));
  const auto pointer = sourcemeta::core::to_pointer("/a");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "a");
}

TEST(deep_six_tokens) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a/b/c/d/e/f"));
  const auto pointer = sourcemeta::core::to_pointer("/a/b/c/d/e/f");
  EXPECT_EQ(pointer.size(), 6);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "a");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "b");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "c");
  EXPECT_TRUE(pointer.at(3).is_property());
  EXPECT_EQ(pointer.at(3).to_property(), "d");
  EXPECT_TRUE(pointer.at(4).is_property());
  EXPECT_EQ(pointer.at(4).to_property(), "e");
  EXPECT_TRUE(pointer.at(5).is_property());
  EXPECT_EQ(pointer.at(5).to_property(), "f");
}

TEST(deep_twelve_tokens) {
  EXPECT_TRUE(
      sourcemeta::core::is_pointer("/t0/t1/t2/t3/t4/t5/t6/t7/t8/t9/t10/t11"));
  const auto pointer =
      sourcemeta::core::to_pointer("/t0/t1/t2/t3/t4/t5/t6/t7/t8/t9/t10/t11");
  EXPECT_EQ(pointer.size(), 12);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "t0");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "t1");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "t2");
  EXPECT_TRUE(pointer.at(3).is_property());
  EXPECT_EQ(pointer.at(3).to_property(), "t3");
  EXPECT_TRUE(pointer.at(4).is_property());
  EXPECT_EQ(pointer.at(4).to_property(), "t4");
  EXPECT_TRUE(pointer.at(5).is_property());
  EXPECT_EQ(pointer.at(5).to_property(), "t5");
  EXPECT_TRUE(pointer.at(6).is_property());
  EXPECT_EQ(pointer.at(6).to_property(), "t6");
  EXPECT_TRUE(pointer.at(7).is_property());
  EXPECT_EQ(pointer.at(7).to_property(), "t7");
  EXPECT_TRUE(pointer.at(8).is_property());
  EXPECT_EQ(pointer.at(8).to_property(), "t8");
  EXPECT_TRUE(pointer.at(9).is_property());
  EXPECT_EQ(pointer.at(9).to_property(), "t9");
  EXPECT_TRUE(pointer.at(10).is_property());
  EXPECT_EQ(pointer.at(10).to_property(), "t10");
  EXPECT_TRUE(pointer.at(11).is_property());
  EXPECT_EQ(pointer.at(11).to_property(), "t11");
}

TEST(long_single_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer(
      "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  const auto pointer = sourcemeta::core::to_pointer(
      "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(),
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
}

TEST(escape_tilde0_tilde1) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0~1"));
  const auto pointer = sourcemeta::core::to_pointer("/~0~1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "~/");
}

TEST(escape_tilde1_tilde0) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~1~0"));
  const auto pointer = sourcemeta::core::to_pointer("/~1~0");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "/~");
}

TEST(escape_tilde0_tilde0) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0~0"));
  const auto pointer = sourcemeta::core::to_pointer("/~0~0");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "~~");
}

TEST(escape_tilde1_tilde1) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~1~1"));
  const auto pointer = sourcemeta::core::to_pointer("/~1~1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "//");
}

TEST(escape_tilde00) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~00"));
  const auto pointer = sourcemeta::core::to_pointer("/~00");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "~0");
}

TEST(escape_tilde11) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~11"));
  const auto pointer = sourcemeta::core::to_pointer("/~11");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "/1");
}

TEST(escape_tilde10) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~10"));
  const auto pointer = sourcemeta::core::to_pointer("/~10");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "/0");
}

TEST(escape_run) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~1~0~0~1~1"));
  const auto pointer = sourcemeta::core::to_pointer("/~1~0~0~1~1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "/~~//");
}

TEST(escape_then_fraction_1) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~1.1"));
  const auto pointer = sourcemeta::core::to_pointer("/~1.1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "/.1");
}

TEST(escape_then_fraction_0) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0.1"));
  const auto pointer = sourcemeta::core::to_pointer("/~0.1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "~.1");
}

TEST(escape_only_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0~1~0~1"));
  const auto pointer = sourcemeta::core::to_pointer("/~0~1~0~1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "~/~/");
}

TEST(escape_each_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0/~1/~0"));
  const auto pointer = sourcemeta::core::to_pointer("/~0/~1/~0");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "~");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "/");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "~");
}

TEST(unescaped_nul) {
  EXPECT_TRUE(sourcemeta::core::is_pointer(std::string_view{"/\000", 2}));
  const auto pointer =
      sourcemeta::core::to_pointer(std::string_view{"/\000", 2});
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), (std::string_view{"\000", 1}));
}

TEST(unescaped_dot_upper) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/."));
  const auto pointer = sourcemeta::core::to_pointer("/.");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), ".");
}

TEST(unescaped_brace_upper) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/}"));
  const auto pointer = sourcemeta::core::to_pointer("/}");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "}");
}

TEST(unescaped_del) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\177"));
  const auto pointer = sourcemeta::core::to_pointer("/\177");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\177");
}

TEST(unescaped_high_0x80) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\302\200"));
  const auto pointer = sourcemeta::core::to_pointer("/\302\200");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\302\200");
}

TEST(unescaped_latin1) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\303\251"));
  const auto pointer = sourcemeta::core::to_pointer("/\303\251");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\303\251");
}

TEST(unescaped_astral_face) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/bar/\360\237\230\216"));
  const auto pointer =
      sourcemeta::core::to_pointer("/foo/bar/\360\237\230\216");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "\360\237\230\216");
}

TEST(unescaped_astral_gclef) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\360\235\204\236"));
  const auto pointer = sourcemeta::core::to_pointer("/\360\235\204\236");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\360\235\204\236");
}

TEST(unescaped_control_run) {
  EXPECT_TRUE(sourcemeta::core::is_pointer(
      std::string_view{"/foo\000bar\012\011baz", 13}));
  const auto pointer = sourcemeta::core::to_pointer(
      std::string_view{"/foo\000bar\012\011baz", 13});
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(),
            (std::string_view{"foo\000bar\012\011baz", 12}));
}

TEST(literal_pct_slash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a%2Fb"));
  const auto pointer = sourcemeta::core::to_pointer("/a%2Fb");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "a%2Fb");
}

TEST(suite_mixed_valid) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/bar~0/baz~1/%a"));
  const auto pointer = sourcemeta::core::to_pointer("/foo/bar~0/baz~1/%a");
  EXPECT_EQ(pointer.size(), 4);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar~");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "baz/");
  EXPECT_TRUE(pointer.at(3).is_property());
  EXPECT_EQ(pointer.at(3).to_property(), "%a");
}

TEST(index_zero_zero_seven) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/007"));
  const auto pointer = sourcemeta::core::to_pointer("/007");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "007");
}

TEST(index_leading_zero_num) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/0123"));
  const auto pointer = sourcemeta::core::to_pointer("/0123");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "0123");
}

TEST(index_ten) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/10"));
  const auto pointer = sourcemeta::core::to_pointer("/10");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 10);
}

TEST(index_dash_after) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/-"));
  const auto pointer = sourcemeta::core::to_pointer("/foo/-");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "-");
}

TEST(index_dash_member) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/-/bar"));
  const auto pointer = sourcemeta::core::to_pointer("/foo/-/bar");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "-");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "bar");
}

TEST(index_exponent_like) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1e0"));
  const auto pointer = sourcemeta::core::to_pointer("/1e0");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "1e0");
}

TEST(index_long_numeric) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1234567890"));
  const auto pointer = sourcemeta::core::to_pointer("/1234567890");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 1234567890);
}

TEST(realworld_openapi_ref) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/paths/~1p~1{}/get"));
  const auto pointer = sourcemeta::core::to_pointer("/paths/~1p~1{}/get");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "paths");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "/p/{}");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "get");
}

TEST(realworld_components) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/components/schemas/Foo"));
  const auto pointer = sourcemeta::core::to_pointer("/components/schemas/Foo");
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "components");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "schemas");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "Foo");
}

TEST(realworld_escaped_slash_key) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/definitions/~1etc~1passwd"));
  const auto pointer =
      sourcemeta::core::to_pointer("/definitions/~1etc~1passwd");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "definitions");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "/etc/passwd");
}

TEST(trailing_lf) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\012"));
  const auto pointer = sourcemeta::core::to_pointer("/foo\012");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\012");
}

TEST(trailing_two_lf) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\012\012"));
  const auto pointer = sourcemeta::core::to_pointer("/foo\012\012");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\012\012");
}

TEST(trailing_cr) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\015"));
  const auto pointer = sourcemeta::core::to_pointer("/foo\015");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\015");
}

TEST(trailing_crlf) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\015\012"));
  const auto pointer = sourcemeta::core::to_pointer("/foo\015\012");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\015\012");
}

TEST(space_in_token) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo bar"));
  const auto pointer = sourcemeta::core::to_pointer("/foo bar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo bar");
}

TEST(escape_then_trailing_lf) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0\012"));
  const auto pointer = sourcemeta::core::to_pointer("/~0\012");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "~\012");
}

TEST(nul_interior_valid) {
  EXPECT_TRUE(sourcemeta::core::is_pointer(std::string_view{"/foo\000bar", 8}));
  const auto pointer =
      sourcemeta::core::to_pointer(std::string_view{"/foo\000bar", 8});
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), (std::string_view{"foo\000bar", 7}));
}

TEST(nul_trailing_valid) {
  EXPECT_TRUE(sourcemeta::core::is_pointer(std::string_view{"/foo\000", 5}));
  const auto pointer =
      sourcemeta::core::to_pointer(std::string_view{"/foo\000", 5});
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), (std::string_view{"foo\000", 4}));
}

TEST(boundary_brace_interior) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a}b"));
  const auto pointer = sourcemeta::core::to_pointer("/a}b");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "a}b");
}

TEST(fullwidth_tilde_alone) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\357\275\236"));
  const auto pointer = sourcemeta::core::to_pointer("/\357\275\236");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\357\275\236");
}

TEST(fullwidth_tilde_interior) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a\357\275\236b"));
  const auto pointer = sourcemeta::core::to_pointer("/a\357\275\236b");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "a\357\275\236b");
}

TEST(fullwidth_tilde_then_2) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\357\275\236"
                                           "2"));
  const auto pointer = sourcemeta::core::to_pointer("/\357\275\236"
                                                    "2");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\357\275\236"
                                         "2");
}

TEST(fullwidth_solidus_interior) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a\357\274\217b"));
  const auto pointer = sourcemeta::core::to_pointer("/a\357\274\217b");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "a\357\274\217b");
}

TEST(percent_7e) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/%7E"));
  const auto pointer = sourcemeta::core::to_pointer("/%7E");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "%7E");
}

TEST(percent_7e_foo) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/%7Efoo"));
  const auto pointer = sourcemeta::core::to_pointer("/%7Efoo");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "%7Efoo");
}

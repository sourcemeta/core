#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <sstream>
#include <string>

TEST(JSONPointer_parse, empty_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer(""));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("");
  EXPECT_EQ(pointer.size(), 0);
}

TEST(JSONPointer_parse, slash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "");
}

TEST(JSONPointer_parse, slash_f) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/f"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/f");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "f");
}

TEST(JSONPointer_parse, slash_foo) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
}

TEST(JSONPointer_parse, slash_foo_bar_baz) {
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

TEST(JSONPointer_parse, slash_0) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/0"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/0");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 0);
}

TEST(JSONPointer_parse, slash_1) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 1);
}

TEST(JSONPointer_parse, slash_1234) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1234"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/1234");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 1234);
}

TEST(JSONPointer_parse, slash_0_slash_1_slash_2) {
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

TEST(JSONPointer_parse, slash_0_slash_foo_slash_2) {
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

TEST(JSONPointer_parse, slash_0_slash_1234) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/0/1234"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/0/1234");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 0);
  EXPECT_TRUE(pointer.at(1).is_index());
  EXPECT_EQ(pointer.at(1).to_index(), 1234);
}

TEST(JSONPointer_parse, slash_1000) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1000"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/1000");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 1000);
}

TEST(JSONPointer_parse, negative_index_as_property) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/-1"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/-1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "-1");
}

TEST(JSONPointer_parse, slash_space_integer) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/ 12"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/ 12");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), " 12");
}

TEST(JSONPointer_parse, slash_foo_slash_space_integer) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/ 12"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo/ 12");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), " 12");
}

TEST(JSONPointer_parse, slash_foo_slash_space_bar_space) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo/ bar "));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo/ bar ");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), " bar ");
}

TEST(JSONPointer_parse, slash_f_slash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/f/"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/f/");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "f");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "");
}

TEST(JSONPointer_parse, slash_slash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("//"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("//");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "");
}

TEST(JSONPointer_parse, slash_slash_slash) {
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

TEST(JSONPointer_parse, slash_slash_slash_foo) {
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

TEST(JSONPointer_parse, positive_hex_index) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/0x23"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/0x23");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "0x23");
}

TEST(JSONPointer_parse, slash_00) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/00"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/00");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "00");
}

TEST(JSONPointer_parse, slash_01) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/01"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/01");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "01");
}

TEST(JSONPointer_parse, slash_001) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/001"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/001");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "001");
}

TEST(JSONPointer_parse, slash_0100) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/0100"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/0100");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "0100");
}

TEST(JSONPointer_parse, positive_real_number_index) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1.23"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/1.23");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "1.23");
}

TEST(JSONPointer_parse, index_in_scientific_notation) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1e1"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/1e1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "1e1");
}

TEST(JSONPointer_parse, integer_with_space) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/1 1"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/1 1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "1 1");
}

TEST(JSONPointer_parse, percentage) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/c%d"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/c%d");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "c%d");
}

TEST(JSONPointer_parse, caret) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/e^f"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/e^f");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "e^f");
}

TEST(JSONPointer_parse, pipe) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/g|h"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/g|h");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "g|h");
}

TEST(JSONPointer_parse, backslash_after_letter) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/i\\\\j"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/i\\\\j");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "i\\j");
}

TEST(JSONPointer_parse, backslash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\\\"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/\\\\");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\\");
}

TEST(JSONPointer_parse, double_quote_after_letter) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/k\\\"l"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/k\\\"l");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "k\"l");
}

TEST(JSONPointer_parse, double_quote) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\\""));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/\\\"");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\"");
}

TEST(JSONPointer_parse, space) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/ "));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/ ");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), " ");
}

TEST(JSONPointer_parse, escaped_slash_after_letter) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/a~1b"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/a~1b");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "a/b");
}

TEST(JSONPointer_parse, escaped_slash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~1"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/~1");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "/");
}

TEST(JSONPointer_parse, escaped_slash_with_backslash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\\\/"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/\\\\/");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\\");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "");
}

TEST(JSONPointer_parse, escaped_slash_with_backslash_after_letter) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/x\\\\/"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/x\\\\/");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "x\\");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "");
}

TEST(JSONPointer_parse, escaped_tilde_after_letter) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/m~0n"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/m~0n");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "m~n");
}

TEST(JSONPointer_parse, escaped_tilde) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~0"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/~0");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "~");
}

TEST(JSONPointer_parse, tilde_zero_one) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/~01"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/~01");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "~1");
}

TEST(JSONPointer_parse, tilde_zero_one_after_letter) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/x~01"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/x~01");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "x~1");
}

TEST(JSONPointer_parse, hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/-"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/-");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_hyphen());
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "-");
}

TEST(JSONPointer_parse, hyphen_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/--"));
  const sourcemeta::core::Pointer pointer = sourcemeta::core::to_pointer("/--");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_FALSE(pointer.at(0).is_hyphen());
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "--");
}

TEST(JSONPointer_parse, hyphen_slash_hyphen) {
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

TEST(JSONPointer_parse, escaped_quote_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\\\"bar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\\\"bar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\"bar");
}

TEST(JSONPointer_parse, escaped_quote) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\\""));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/\\\"");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\"");
}

TEST(JSONPointer_parse, escaped_backslash_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\\\\bar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\\\\bar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\\bar");
}

TEST(JSONPointer_parse, escaped_backslash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\\\"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/\\\\");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\\");
}

TEST(JSONPointer_parse, control_backslash_backspace) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\b"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/\\b");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\b");
}

TEST(JSONPointer_parse, control_backslash_backspace_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\\bbar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\\bbar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\bbar");
}

TEST(JSONPointer_parse, control_backslash_formfeed) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\f"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/\\f");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\f");
}

TEST(JSONPointer_parse, control_backslash_formfeed_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\\fbar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\\fbar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\fbar");
}

TEST(JSONPointer_parse, control_backslash_newline) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\n"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/\\n");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\n");
}

TEST(JSONPointer_parse, control_backslash_newline_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\\nbar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\\nbar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\nbar");
}

TEST(JSONPointer_parse, control_backslash_carriage) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\r"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/\\r");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\r");
}

TEST(JSONPointer_parse, control_backslash_carriage_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\\rbar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\\rbar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\rbar");
}

TEST(JSONPointer_parse, control_backslash_tab) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/\\t"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/\\t");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "\t");
}

TEST(JSONPointer_parse, control_backslash_tab_within_string) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\\tbar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\\tbar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\tbar");
}

TEST(JSONPointer_parse, property_with_unicode_code_point) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/foo\\u002Abar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("/foo\\u002Abar");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo\u002Abar");
}

TEST(JSONPointer_parse, unicode_slashes) {
  // The raw string starts with '\', not '/' - invalid per RFC 6901.
  // to_pointer() succeeds because the JSON parser interprets \u002F as '/'
  EXPECT_FALSE(sourcemeta::core::is_pointer("\\u002Ffoo\\u002Fbar"));
  const sourcemeta::core::Pointer pointer =
      sourcemeta::core::to_pointer("\\u002Ffoo\\u002Fbar");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
}

TEST(JSONPointer_parse, regex_caret) {
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

TEST(JSONPointer_parse, regex_backslash) {
  EXPECT_TRUE(sourcemeta::core::is_pointer("/[\\\\-]"));
  const sourcemeta::core::Pointer pointer =
      // Needs escaping because here we are interpreting as a C string and not
      // as a JSON string
      sourcemeta::core::to_pointer("/[\\\\-]");
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "[\\-]");
}

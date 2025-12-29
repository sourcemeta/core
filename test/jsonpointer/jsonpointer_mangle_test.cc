#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

TEST(JSONPointer_mangle, empty_pointer_lowercase_prefix) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema");
}

TEST(JSONPointer_mangle, empty_pointer_uppercase_prefix) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "Schema")};
  EXPECT_EQ(result, "Schema");
}

TEST(JSONPointer_mangle, empty_pointer_custom_prefix) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "myNamespace")};
  EXPECT_EQ(result, "MyNamespace");
}

TEST(JSONPointer_mangle, single_lowercase_token) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_Foo");
}

TEST(JSONPointer_mangle, single_lowercase_token_long) {
  const sourcemeta::core::Pointer pointer{"foobar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_Foobar");
}

TEST(JSONPointer_mangle, single_uppercase_token) {
  const sourcemeta::core::Pointer pointer{"Foo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_UFoo");
}

TEST(JSONPointer_mangle, single_all_uppercase_token) {
  const sourcemeta::core::Pointer pointer{"FOO"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_UFOO");
}

TEST(JSONPointer_mangle, lowercase_x_at_start) {
  const sourcemeta::core::Pointer pointer{"xoo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X78oo");
}

TEST(JSONPointer_mangle, uppercase_X_at_start) {
  const sourcemeta::core::Pointer pointer{"Xoo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X58oo");
}

TEST(JSONPointer_mangle, lowercase_u_at_start) {
  const sourcemeta::core::Pointer pointer{"uoo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X75oo");
}

TEST(JSONPointer_mangle, uppercase_U_at_start) {
  const sourcemeta::core::Pointer pointer{"Uoo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X55oo");
}

TEST(JSONPointer_mangle, uppercase_X_in_middle) {
  const sourcemeta::core::Pointer pointer{"foXbar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_FoX58bar");
}

TEST(JSONPointer_mangle, uppercase_U_in_middle) {
  const sourcemeta::core::Pointer pointer{"foUbar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_FoX55bar");
}

TEST(JSONPointer_mangle, lowercase_x_in_middle) {
  const sourcemeta::core::Pointer pointer{"foxbar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_Foxbar");
}

TEST(JSONPointer_mangle, lowercase_u_in_middle) {
  const sourcemeta::core::Pointer pointer{"foubar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_Foubar");
}

TEST(JSONPointer_mangle, hyphen_in_middle) {
  const sourcemeta::core::Pointer pointer{"foo-bar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_FooX2DBar");
}

TEST(JSONPointer_mangle, underscore_in_token) {
  const sourcemeta::core::Pointer pointer{"foo_bar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_FooX5FBar");
}

TEST(JSONPointer_mangle, space_in_token) {
  const sourcemeta::core::Pointer pointer{"foo bar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_FooX20Bar");
}

TEST(JSONPointer_mangle, at_sign_in_token) {
  const sourcemeta::core::Pointer pointer{"foo@bar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_FooX40Bar");
}

TEST(JSONPointer_mangle, special_char_at_start) {
  const sourcemeta::core::Pointer pointer{"@foo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X40Foo");
}

TEST(JSONPointer_mangle, only_special_characters) {
  const sourcemeta::core::Pointer pointer{"@#$%"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X40X23X24X25");
}

TEST(JSONPointer_mangle, multiple_hyphens) {
  const sourcemeta::core::Pointer pointer{"foo--bar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_FooX2DX2DBar");
}

TEST(JSONPointer_mangle, digit_at_start) {
  const sourcemeta::core::Pointer pointer{"123foo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_123foo");
}

TEST(JSONPointer_mangle, only_digits) {
  const sourcemeta::core::Pointer pointer{"123"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_123");
}

TEST(JSONPointer_mangle, digits_in_middle) {
  const sourcemeta::core::Pointer pointer{"foo123bar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_Foo123bar");
}

TEST(JSONPointer_mangle, digits_at_end) {
  const sourcemeta::core::Pointer pointer{"foo123"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_Foo123");
}

TEST(JSONPointer_mangle, two_lowercase_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_Foo_Bar");
}

TEST(JSONPointer_mangle, three_lowercase_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_Foo_Bar_Baz");
}

TEST(JSONPointer_mangle, camelcase_token) {
  const sourcemeta::core::Pointer pointer{"fooBar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_FooBar");
}

TEST(JSONPointer_mangle, mixed_case_multi_token) {
  const sourcemeta::core::Pointer pointer{"foo", "Bar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_Foo_UBar");
}

TEST(JSONPointer_mangle, lowercase_x_followed_by_digits) {
  const sourcemeta::core::Pointer pointer{"x40"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X7840");
}

TEST(JSONPointer_mangle, uppercase_X_followed_by_digits) {
  const sourcemeta::core::Pointer pointer{"X40"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X5840");
}

TEST(JSONPointer_mangle, lowercase_u_followed_by_uppercase) {
  const sourcemeta::core::Pointer pointer{"uFoo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X75Foo");
}

TEST(JSONPointer_mangle, uppercase_U_followed_by_uppercase) {
  const sourcemeta::core::Pointer pointer{"UFoo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X55Foo");
}

TEST(JSONPointer_mangle, empty_token) {
  const sourcemeta::core::Pointer pointer{""};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_ZEmpty");
}

TEST(JSONPointer_mangle, empty_token_then_property) {
  const sourcemeta::core::Pointer pointer{"", "foo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_ZEmpty_Foo");
}

TEST(JSONPointer_mangle, property_then_empty_token) {
  const sourcemeta::core::Pointer pointer{"foo", ""};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_Foo_ZEmpty");
}

TEST(JSONPointer_mangle, property_ZEmpty_escaped) {
  const sourcemeta::core::Pointer pointer{"ZEmpty"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X5AEmpty");
}

TEST(JSONPointer_mangle, deeply_nested_path) {
  const sourcemeta::core::Pointer pointer{"a", "b", "c", "d", "e"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_A_B_C_D_E");
}

TEST(JSONPointer_mangle, preserves_case_after_first_letter) {
  const sourcemeta::core::Pointer pointer{"fooBAR"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_FooBAR");
}

TEST(JSONPointer_mangle, lowercase_z_at_start) {
  const sourcemeta::core::Pointer pointer{"zoo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X7Aoo");
}

TEST(JSONPointer_mangle, uppercase_Z_at_start) {
  const sourcemeta::core::Pointer pointer{"Zoo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X5Aoo");
}

TEST(JSONPointer_mangle, property_ZIndex_escaped) {
  const sourcemeta::core::Pointer pointer{"ZIndex"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X5AIndex");
}

TEST(JSONPointer_mangle, unicode_single_char) {
  const sourcemeta::core::Pointer pointer{"föo"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_FXC3XB6o");
}

TEST(JSONPointer_mangle, unicode_start) {
  const sourcemeta::core::Pointer pointer{"über"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_XC3XBCber");
}

TEST(JSONPointer_mangle, unicode_emoji) {
  const sourcemeta::core::Pointer pointer{"foo🎉bar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_FooXF0X9FX8EX89bar");
}

TEST(JSONPointer_mangle, unicode_chinese) {
  const sourcemeta::core::Pointer pointer{"名前"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_XE5X90X8DXE5X89X8D");
}

TEST(JSONPointer_mangle, prefix_with_spaces) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "my schema")};
  EXPECT_EQ(result, "MyX20Schema");
}

TEST(JSONPointer_mangle, prefix_only_spaces) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "   ")};
  EXPECT_EQ(result, "X20X20X20");
}

TEST(JSONPointer_mangle, prefix_unicode) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "schéma")};
  EXPECT_EQ(result, "SchXC3XA9Ma");
}

TEST(JSONPointer_mangle, prefix_unicode_start) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "über")};
  EXPECT_EQ(result, "XC3XBCBer");
}

TEST(JSONPointer_mangle, prefix_emoji) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "schema🎉")};
  EXPECT_EQ(result, "SchemaXF0X9FX8EX89");
}

TEST(JSONPointer_mangle, prefix_uppercase) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "Schema")};
  EXPECT_EQ(result, "Schema");
}

TEST(JSONPointer_mangle, prefix_all_uppercase) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "SCHEMA")};
  EXPECT_EQ(result, "SCHEMA");
}

TEST(JSONPointer_mangle, prefix_with_digits) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "schema123")};
  EXPECT_EQ(result, "Schema123");
}

TEST(JSONPointer_mangle, prefix_starts_with_digit) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "123schema")};
  EXPECT_EQ(result, "_123schema");
}

TEST(JSONPointer_mangle, prefix_with_underscore) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "my_schema")};
  EXPECT_EQ(result, "MySchema");
}

TEST(JSONPointer_mangle, prefix_with_hyphen) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "my-schema")};
  EXPECT_EQ(result, "MySchema");
}

TEST(JSONPointer_mangle, prefix_special_chars) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "foo@bar")};
  EXPECT_EQ(result, "FooX40Bar");
}

TEST(JSONPointer_mangle, prefix_with_tab) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "foo\tbar")};
  EXPECT_EQ(result, "FooX09Bar");
}

TEST(JSONPointer_mangle, prefix_with_newline) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::mangle(pointer, "foo\nbar")};
  EXPECT_EQ(result, "FooX0ABar");
}

TEST(JSONPointer_mangle, prefix_with_null_byte) {
  const sourcemeta::core::Pointer pointer;
  const std::string prefix{"foo\0bar", 7};
  const auto result{sourcemeta::core::mangle(pointer, prefix)};
  EXPECT_EQ(result, "FooX00Bar");
}

TEST(JSONPointer_mangle, utf8_does_not_start_segment) {
  const sourcemeta::core::Pointer pointer{"ébar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_XC3XA9bar");
}

TEST(JSONPointer_mangle, utf8_multibyte_preserves_continuity) {
  const sourcemeta::core::Pointer pointer{"日test"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_XE6X97XA5test");
}

TEST(JSONPointer_mangle, ascii_special_starts_new_segment) {
  const sourcemeta::core::Pointer pointer{"foo bar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_FooX20Bar");
}

TEST(JSONPointer_mangle, mixed_utf8_and_ascii_special) {
  const sourcemeta::core::Pointer pointer{"é bar"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_XC3XA9X20Bar");
}

TEST(JSONPointer_mangle, high_byte_always_escaped) {
  sourcemeta::core::Pointer pointer;
  pointer.emplace_back("\x80test");
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_X80test");
}

TEST(JSONPointer_mangle, latin1_supplement_escaped) {
  sourcemeta::core::Pointer pointer;
  pointer.emplace_back("\xA9test");
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_XA9test");
}

TEST(JSONPointer_mangle, consecutive_utf8_chars) {
  const sourcemeta::core::Pointer pointer{"日本"};
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_XE6X97XA5XE6X9CXAC");
}

TEST(JSONPointer_mangle, single_index_zero) {
  sourcemeta::core::Pointer pointer;
  pointer.emplace_back(0);
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_ZIndex0");
}

TEST(JSONPointer_mangle, single_index_nonzero) {
  sourcemeta::core::Pointer pointer;
  pointer.emplace_back(5);
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_ZIndex5");
}

TEST(JSONPointer_mangle, single_index_large) {
  sourcemeta::core::Pointer pointer;
  pointer.emplace_back(12345);
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_ZIndex12345");
}

TEST(JSONPointer_mangle, property_then_index) {
  sourcemeta::core::Pointer pointer;
  pointer.emplace_back("foo");
  pointer.emplace_back(0);
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_Foo_ZIndex0");
}

TEST(JSONPointer_mangle, index_then_property) {
  sourcemeta::core::Pointer pointer;
  pointer.emplace_back(0);
  pointer.emplace_back("foo");
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_ZIndex0_Foo");
}

TEST(JSONPointer_mangle, multiple_indexes) {
  sourcemeta::core::Pointer pointer;
  pointer.emplace_back(0);
  pointer.emplace_back(1);
  pointer.emplace_back(2);
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_ZIndex0_ZIndex1_ZIndex2");
}

TEST(JSONPointer_mangle, mixed_property_and_index) {
  sourcemeta::core::Pointer pointer;
  pointer.emplace_back("items");
  pointer.emplace_back(0);
  pointer.emplace_back("properties");
  pointer.emplace_back("name");
  const auto result{sourcemeta::core::mangle(pointer, "schema")};
  EXPECT_EQ(result, "Schema_Items_ZIndex0_Properties_Name");
}

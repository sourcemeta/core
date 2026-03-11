#include <gtest/gtest.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/yaml.h>

#include <sstream> // std::ostringstream

TEST(YAML_stringify, null) {
  const sourcemeta::core::JSON document{nullptr};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "null\n");
}

TEST(YAML_stringify, true_value) {
  const sourcemeta::core::JSON document{true};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "true\n");
}

TEST(YAML_stringify, false_value) {
  const sourcemeta::core::JSON document{false};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "false\n");
}

TEST(YAML_stringify, integer_positive) {
  const sourcemeta::core::JSON document{42};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "42\n");
}

TEST(YAML_stringify, integer_negative) {
  const sourcemeta::core::JSON document{-7};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "-7\n");
}

TEST(YAML_stringify, integer_zero) {
  const sourcemeta::core::JSON document{0};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "0\n");
}

TEST(YAML_stringify, integer_large) {
  const sourcemeta::core::JSON document{9999999};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "9999999\n");
}

TEST(YAML_stringify, integer_large_negative) {
  const sourcemeta::core::JSON document{-9999999};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "-9999999\n");
}

TEST(YAML_stringify, real_zero) {
  const sourcemeta::core::JSON document{0.0};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "0.0\n");
}

TEST(YAML_stringify, real_positive_fractional) {
  const sourcemeta::core::JSON document{3.14};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "3.14\n");
}

TEST(YAML_stringify, real_negative_fractional) {
  const sourcemeta::core::JSON document{-2.5};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "-2.5\n");
}

TEST(YAML_stringify, real_positive_whole) {
  const sourcemeta::core::JSON document{5.0};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "5.0\n");
}

TEST(YAML_stringify, real_negative_whole) {
  const sourcemeta::core::JSON document{-10.0};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "-10.0\n");
}

TEST(YAML_stringify, real_large_whole) {
  const sourcemeta::core::JSON document{1000000.0};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "1000000.0\n");
}

TEST(YAML_stringify, real_small_fractional) {
  const sourcemeta::core::JSON document{0.001};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "0.001\n");
}

TEST(YAML_stringify, real_one) {
  const sourcemeta::core::JSON document{1.0};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "1.0\n");
}

TEST(YAML_stringify, real_negative_one) {
  const sourcemeta::core::JSON document{-1.0};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "-1.0\n");
}

TEST(YAML_stringify, real_large_fractional) {
  const sourcemeta::core::JSON document{1234.56};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "1234.56\n");
}

TEST(YAML_stringify, real_half) {
  const sourcemeta::core::JSON document{0.5};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "0.5\n");
}

TEST(YAML_stringify, decimal_large_integer) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{static_cast<std::int64_t>(1000000)}};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "1.000000e+6\n");
}

TEST(YAML_stringify, decimal_high_precision_real) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"3.14159265358979"}};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "3.14159265358979e+0\n");
}

TEST(YAML_stringify, decimal_exponential_notation) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"0.001"}};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "1e-3\n");
}

TEST(YAML_stringify, decimal_in_object) {
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("value",
                  sourcemeta::core::JSON{sourcemeta::core::Decimal{42}});
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "value: 4.2e+1\n");
}

TEST(YAML_stringify, decimal_in_array) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(sourcemeta::core::JSON{sourcemeta::core::Decimal{42}});
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "- 4.2e+1\n");
}

TEST(YAML_stringify, decimal_negative) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{static_cast<std::int64_t>(-42)}};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "-4.2e+1\n");
}

TEST(YAML_stringify, real_in_object) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "pi": 3.14 })JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "pi: 3.14\n");
}

TEST(YAML_stringify, real_in_array) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1.5, 2.5 ])JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "- 1.5\n- 2.5\n");
}

TEST(YAML_stringify, sequence_mixed_with_real) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ 1, 2.5, "three" ])JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "- 1\n- 2.5\n- three\n");
}

TEST(YAML_stringify, plain_string) {
  const sourcemeta::core::JSON document{"hello"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "hello\n");
}

TEST(YAML_stringify, plain_string_with_dash_in_middle) {
  const sourcemeta::core::JSON document{"foo-bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "foo-bar\n");
}

TEST(YAML_stringify, plain_string_with_colon_no_space) {
  const sourcemeta::core::JSON document{"http://example.com"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "http://example.com\n");
}

TEST(YAML_stringify, plain_string_with_dot) {
  const sourcemeta::core::JSON document{"file.txt"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "file.txt\n");
}

TEST(YAML_stringify, plain_string_with_underscore) {
  const sourcemeta::core::JSON document{"my_var"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "my_var\n");
}

TEST(YAML_stringify, plain_string_with_slash) {
  const sourcemeta::core::JSON document{"path/to/file"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "path/to/file\n");
}

TEST(YAML_stringify, plain_string_with_quotes_inside) {
  const sourcemeta::core::JSON document{"say \"hello\""};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "say \"hello\"\n");
}

TEST(YAML_stringify, plain_string_with_backslash) {
  const sourcemeta::core::JSON document{"back\\slash"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "back\\slash\n");
}

TEST(YAML_stringify, plain_string_with_parentheses) {
  const sourcemeta::core::JSON document{"foo(bar)"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "foo(bar)\n");
}

TEST(YAML_stringify, plain_string_with_equals) {
  const sourcemeta::core::JSON document{"a=b"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "a=b\n");
}

TEST(YAML_stringify, plain_string_single_char) {
  const sourcemeta::core::JSON document{"x"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "x\n");
}

TEST(YAML_stringify, string_quoting_true_lowercase) {
  const sourcemeta::core::JSON document{"true"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"true\"\n");
}

TEST(YAML_stringify, string_quoting_True) {
  const sourcemeta::core::JSON document{"True"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"True\"\n");
}

TEST(YAML_stringify, string_quoting_TRUE) {
  const sourcemeta::core::JSON document{"TRUE"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"TRUE\"\n");
}

TEST(YAML_stringify, string_quoting_false_lowercase) {
  const sourcemeta::core::JSON document{"false"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"false\"\n");
}

TEST(YAML_stringify, string_quoting_False) {
  const sourcemeta::core::JSON document{"False"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"False\"\n");
}

TEST(YAML_stringify, string_quoting_FALSE) {
  const sourcemeta::core::JSON document{"FALSE"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"FALSE\"\n");
}

TEST(YAML_stringify, string_quoting_null_lowercase) {
  const sourcemeta::core::JSON document{"null"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"null\"\n");
}

TEST(YAML_stringify, string_quoting_Null) {
  const sourcemeta::core::JSON document{"Null"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"Null\"\n");
}

TEST(YAML_stringify, string_quoting_NULL) {
  const sourcemeta::core::JSON document{"NULL"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"NULL\"\n");
}

TEST(YAML_stringify, string_quoting_tilde) {
  const sourcemeta::core::JSON document{"~"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"~\"\n");
}

TEST(YAML_stringify, string_quoting_integer) {
  const sourcemeta::core::JSON document{"42"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"42\"\n");
}

TEST(YAML_stringify, string_quoting_negative_integer) {
  const sourcemeta::core::JSON document{"-7"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"-7\"\n");
}

TEST(YAML_stringify, string_quoting_float) {
  const sourcemeta::core::JSON document{"3.14"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"3.14\"\n");
}

TEST(YAML_stringify, string_quoting_scientific) {
  const sourcemeta::core::JSON document{"1e10"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"1e10\"\n");
}

TEST(YAML_stringify, string_quoting_hex) {
  const sourcemeta::core::JSON document{"0xFF"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"0xFF\"\n");
}

TEST(YAML_stringify, string_quoting_octal) {
  const sourcemeta::core::JSON document{"0o77"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"0o77\"\n");
}

TEST(YAML_stringify, string_quoting_positive_sign) {
  const sourcemeta::core::JSON document{"+5"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"+5\"\n");
}

TEST(YAML_stringify, string_quoting_zero) {
  const sourcemeta::core::JSON document{"0"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"0\"\n");
}

TEST(YAML_stringify, string_quoting_inf) {
  const sourcemeta::core::JSON document{".inf"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\".inf\"\n");
}

TEST(YAML_stringify, string_quoting_Inf) {
  const sourcemeta::core::JSON document{".Inf"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\".Inf\"\n");
}

TEST(YAML_stringify, string_quoting_negative_inf) {
  const sourcemeta::core::JSON document{"-.inf"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"-.inf\"\n");
}

TEST(YAML_stringify, string_quoting_nan) {
  const sourcemeta::core::JSON document{".nan"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\".nan\"\n");
}

TEST(YAML_stringify, string_quoting_NaN) {
  const sourcemeta::core::JSON document{".NaN"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\".NaN\"\n");
}

TEST(YAML_stringify, string_quoting_document_start) {
  const sourcemeta::core::JSON document{"---"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"---\"\n");
}

TEST(YAML_stringify, string_quoting_document_end) {
  const sourcemeta::core::JSON document{"..."};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"...\"\n");
}

TEST(YAML_stringify, string_quoting_document_start_with_space) {
  const sourcemeta::core::JSON document{"--- foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"--- foo\"\n");
}

TEST(YAML_stringify, string_quoting_document_end_with_space) {
  const sourcemeta::core::JSON document{"... foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"... foo\"\n");
}

TEST(YAML_stringify, string_four_dashes_plain) {
  const sourcemeta::core::JSON document{"----"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "----\n");
}

TEST(YAML_stringify, string_four_dots_plain) {
  const sourcemeta::core::JSON document{"...."};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "....\n");
}

TEST(YAML_stringify, string_quoting_trailing_colon) {
  const sourcemeta::core::JSON document{"foo:"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"foo:\"\n");
}

TEST(YAML_stringify, string_quoting_just_colon) {
  const sourcemeta::core::JSON document{":"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\":\"\n");
}

TEST(YAML_stringify, string_quoting_colon_space) {
  const sourcemeta::core::JSON document{"foo: bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"foo: bar\"\n");
}

TEST(YAML_stringify, string_plain_colon_no_space) {
  const sourcemeta::core::JSON document{"a:b:c"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "a:b:c\n");
}

TEST(YAML_stringify, string_quoting_starts_with_ampersand) {
  const sourcemeta::core::JSON document{"&foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"&foo\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_asterisk) {
  const sourcemeta::core::JSON document{"*foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"*foo\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_open_bracket) {
  const sourcemeta::core::JSON document{"[foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"[foo\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_close_bracket) {
  const sourcemeta::core::JSON document{"]foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"]foo\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_open_brace) {
  const sourcemeta::core::JSON document{"{foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"{foo\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_close_brace) {
  const sourcemeta::core::JSON document{"}foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"}foo\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_hash) {
  const sourcemeta::core::JSON document{"#comment"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"#comment\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_comma) {
  const sourcemeta::core::JSON document{",foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\",foo\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_exclamation) {
  const sourcemeta::core::JSON document{"!tag"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"!tag\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_pipe) {
  const sourcemeta::core::JSON document{"|literal"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"|literal\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_greater) {
  const sourcemeta::core::JSON document{">folded"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\">folded\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_single_quote) {
  const sourcemeta::core::JSON document{"'hello'"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"'hello'\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_double_quote) {
  const sourcemeta::core::JSON document{"\"hello\""};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"\\\"hello\\\"\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_percent) {
  const sourcemeta::core::JSON document{"%TAG"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"%TAG\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_at) {
  const sourcemeta::core::JSON document{"@foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"@foo\"\n");
}

TEST(YAML_stringify, string_quoting_starts_with_backtick) {
  const sourcemeta::core::JSON document{"`foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"`foo\"\n");
}

TEST(YAML_stringify, string_quoting_dash_space) {
  const sourcemeta::core::JSON document{"- item"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"- item\"\n");
}

TEST(YAML_stringify, string_quoting_question_space) {
  const sourcemeta::core::JSON document{"? key"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"? key\"\n");
}

TEST(YAML_stringify, string_quoting_colon_space_start) {
  const sourcemeta::core::JSON document{": value"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\": value\"\n");
}

TEST(YAML_stringify, string_quoting_lone_dash) {
  const sourcemeta::core::JSON document{"-"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"-\"\n");
}

TEST(YAML_stringify, string_quoting_lone_question) {
  const sourcemeta::core::JSON document{"?"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"?\"\n");
}

TEST(YAML_stringify, string_empty) {
  const sourcemeta::core::JSON document{""};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"\"\n");
}

TEST(YAML_stringify, string_quoting_leading_space) {
  const sourcemeta::core::JSON document{" hello"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\" hello\"\n");
}

TEST(YAML_stringify, string_quoting_trailing_space) {
  const sourcemeta::core::JSON document{"hello "};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"hello \"\n");
}

TEST(YAML_stringify, string_quoting_all_spaces) {
  const sourcemeta::core::JSON document{"   "};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"   \"\n");
}

TEST(YAML_stringify, string_quoting_newline) {
  const sourcemeta::core::JSON document{"line1\nline2"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"line1\\nline2\"\n");
}

TEST(YAML_stringify, string_quoting_tab) {
  const sourcemeta::core::JSON document{"col1\tcol2"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"col1\\tcol2\"\n");
}

TEST(YAML_stringify, string_quoting_carriage_return) {
  const sourcemeta::core::JSON document{"line1\rline2"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"line1\\rline2\"\n");
}

TEST(YAML_stringify, string_quoting_space_hash) {
  const sourcemeta::core::JSON document{"foo #bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"foo #bar\"\n");
}

TEST(YAML_stringify, empty_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({})JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "{}\n");
}

TEST(YAML_stringify, empty_array) {
  const auto document{sourcemeta::core::parse_json(R"JSON([])JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "[]\n");
}

TEST(YAML_stringify, mapping_string_value_needs_quoting) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "key": "true" })JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "key: \"true\"\n");
}

TEST(YAML_stringify, mapping_key_needs_quoting) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "null": "value" })JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"null\": value\n");
}

TEST(YAML_stringify, mapping_with_empty_nested_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "foo": {} })JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "foo: {}\n");
}

TEST(YAML_stringify, mapping_with_empty_nested_array) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "foo": [] })JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "foo: []\n");
}

TEST(YAML_stringify, mapping_with_null_value) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "key": null })JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "key: null\n");
}

TEST(YAML_stringify, mapping_with_boolean_value) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "enabled": true })JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "enabled: true\n");
}

TEST(YAML_stringify, mapping_with_integer_value) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "count": 42 })JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "count: 42\n");
}

TEST(YAML_stringify, mapping_single_key) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "only": "one" })JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "only: one\n");
}

TEST(YAML_stringify, mapping_key_with_trailing_colon) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "foo:": "bar" })JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"foo:\": bar\n");
}

TEST(YAML_stringify, mapping_key_with_space) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "my key": "value" })JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "my key: value\n");
}

TEST(YAML_stringify, mapping_key_empty_string) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "": "value" })JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "\"\": value\n");
}

TEST(YAML_stringify, sequence_with_empty_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ {} ])JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "- {}\n");
}

TEST(YAML_stringify, sequence_with_empty_array) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ [] ])JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "- []\n");
}

TEST(YAML_stringify, sequence_single_null) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ null ])JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "- null\n");
}

TEST(YAML_stringify, sequence_single_boolean) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ true ])JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "- true\n");
}

TEST(YAML_stringify, sequence_single_integer) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 99 ])JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "- 99\n");
}

TEST(YAML_stringify, sequence_single_string) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ "hello" ])JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "- hello\n");
}

TEST(YAML_stringify, sequence_mixed_types) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ 1, "two", true, null ])JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "- 1\n- two\n- true\n- null\n");
}

TEST(YAML_stringify, sequence_with_quoted_strings) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ "true", "42", "null" ])JSON")};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream);
  EXPECT_EQ(stream.str(), "- \"true\"\n- \"42\"\n- \"null\"\n");
}

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <sstream>

TEST(boolean_false) {
  const sourcemeta::core::JSON document{false};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "false");
}

TEST(boolean_true) {
  const sourcemeta::core::JSON document{true};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "true");
}

TEST(null) {
  const sourcemeta::core::JSON document{nullptr};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "null");
}

TEST(integer_positive) {
  const sourcemeta::core::JSON document{1234};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "1234");
}

TEST(integer_negative) {
  const sourcemeta::core::JSON document{-1234};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "-1234");
}

TEST(integer_zero) {
  const sourcemeta::core::JSON document{0};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "0");
}

TEST(integer_minus_zero) {
  const sourcemeta::core::JSON document{-0};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "0");
}

TEST(real_positive) {
  const sourcemeta::core::JSON document{12.34};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "12.34");
}

TEST(real_negative) {
  const sourcemeta::core::JSON document{-12.34};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "-12.34");
}

TEST(real_zero) {
  const sourcemeta::core::JSON document{0.0};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "0.0");
}

TEST(real_minus_zero) {
  const sourcemeta::core::JSON document{-0.0};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "0.0");
}

TEST(integer_real_positive) {
  const sourcemeta::core::JSON document{2.0};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "2.0");
}

TEST(integer_real_negative) {
  const sourcemeta::core::JSON document{-2.0};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "-2.0");
}

TEST(very_large_real_number_with_decimal) {
  const sourcemeta::core::JSON document{9.9999999999999997e+34};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "99999999999999996863366107917975552.0");
}

TEST(empty_string) {
  const sourcemeta::core::JSON document{""};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"\"");
}

TEST(small_string) {
  const sourcemeta::core::JSON document{"foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\"");
}

TEST(array_integers) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::JSON{1}, sourcemeta::core::JSON{2},
      sourcemeta::core::JSON{3}, sourcemeta::core::JSON{4}};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "[1,2,3,4]");
}

TEST(array_nested) {
  sourcemeta::core::JSON array{
      sourcemeta::core::JSON{1}, sourcemeta::core::JSON{2},
      sourcemeta::core::JSON{3}, sourcemeta::core::JSON{4}};
  sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};
  document.push_back(std::move(array));
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "[[1,2,3,4]]");
}

TEST(array_empty) {
  const sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "[]");
}

TEST(object_integers) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                        {"bar", sourcemeta::core::JSON{2}}};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  // Because order is irrelevant
  const bool matches = stream.str() == "{\"foo\":1,\"bar\":2}" ||
                       stream.str() == "{\"bar\":2,\"foo\":1}";
  EXPECT_TRUE(matches);
}

TEST(object_integers_order_1) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                  {"bar", sourcemeta::core::JSON{2}},
                                  {"baz", sourcemeta::core::JSON{3}}};
  document.reorder(
      [](const auto &left, const auto &right) { return left < right; });
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "{\"bar\":2,\"baz\":3,\"foo\":1}");
}

TEST(object_integers_order_2) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                  {"bar", sourcemeta::core::JSON{2}},
                                  {"baz", sourcemeta::core::JSON{3}}};
  document.reorder(
      [](const auto &left, const auto &right) { return left > right; });
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "{\"foo\":1,\"baz\":3,\"bar\":2}");
}

TEST(object_empty) {
  const sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "{}");
}

TEST(object_with_array) {
  sourcemeta::core::JSON array{sourcemeta::core::JSON::Array{}};
  array.push_back(sourcemeta::core::JSON{1});
  array.push_back(sourcemeta::core::JSON{2});
  const sourcemeta::core::JSON document{{"foo", std::move(array)}};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "{\"foo\":[1,2]}");
}

TEST(stringify_quote) {
  const sourcemeta::core::JSON document{"\""};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"\\\"\"");
}

TEST(stringify_escape) {
  const sourcemeta::core::JSON document{"\\"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"\\\\\"");
}

TEST(stringify_solidus) {
  const sourcemeta::core::JSON document{"/"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"/\"");
}

TEST(stringify_backspace) {
  const sourcemeta::core::JSON document{"foo\bbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\bbar\"");
}

TEST(stringify_formfeed) {
  const sourcemeta::core::JSON document{"foo\fbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\fbar\"");
}

TEST(stringify_newline) {
  const sourcemeta::core::JSON document{"foo\nbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\nbar\"");
}

TEST(stringify_carriage_return) {
  const sourcemeta::core::JSON document{"foo\rbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\rbar\"");
}

TEST(stringify_tabulation) {
  const sourcemeta::core::JSON document{"foo\tbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\tbar\"");
}

TEST(stringify_scientific_real) {
  const sourcemeta::core::JSON document{4.321768E3};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "4321.768");
}

TEST(escape_00) {
  using namespace std::string_literals;
  const sourcemeta::core::JSON document{"foo\0bar"s};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0000bar\"");
}

TEST(escape_01) {
  const sourcemeta::core::JSON document{"foo\u0001bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0001bar\"");
}

TEST(escape_02) {
  const sourcemeta::core::JSON document{"foo\u0002bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0002bar\"");
}

TEST(escape_03) {
  const sourcemeta::core::JSON document{"foo\u0003bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0003bar\"");
}

TEST(escape_04) {
  const sourcemeta::core::JSON document{"foo\u0004bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0004bar\"");
}

TEST(escape_05) {
  const sourcemeta::core::JSON document{"foo\u0005bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0005bar\"");
}

TEST(escape_06) {
  const sourcemeta::core::JSON document{"foo\u0006bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0006bar\"");
}

TEST(escape_07) {
  const sourcemeta::core::JSON document{"foo\u0007bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0007bar\"");
}

TEST(escape_08) {
  const sourcemeta::core::JSON document{"foo\u0008bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\bbar\"");
}

TEST(escape_09) {
  const sourcemeta::core::JSON document{"foo\u0009bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\tbar\"");
}

TEST(escape_0A) {
  const sourcemeta::core::JSON document{"foo\u000abar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\nbar\"");
}

TEST(escape_0B) {
  const sourcemeta::core::JSON document{"foo\u000bbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u000Bbar\"");
}

TEST(escape_0C) {
  const sourcemeta::core::JSON document{"foo\u000cbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\fbar\"");
}

TEST(escape_0D) {
  const sourcemeta::core::JSON document{"foo\u000dbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\rbar\"");
}

TEST(escape_0E) {
  const sourcemeta::core::JSON document{"foo\u000ebar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u000Ebar\"");
}

TEST(escape_0F) {
  const sourcemeta::core::JSON document{"foo\u000fbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u000Fbar\"");
}

TEST(escape_10) {
  const sourcemeta::core::JSON document{"foo\u0010bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0010bar\"");
}

TEST(escape_11) {
  const sourcemeta::core::JSON document{"foo\u0011bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0011bar\"");
}

TEST(escape_12) {
  const sourcemeta::core::JSON document{"foo\u0012bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0012bar\"");
}

TEST(escape_13) {
  const sourcemeta::core::JSON document{"foo\u0013bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0013bar\"");
}

TEST(escape_14) {
  const sourcemeta::core::JSON document{"foo\u0014bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0014bar\"");
}

TEST(escape_15) {
  const sourcemeta::core::JSON document{"foo\u0015bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0015bar\"");
}

TEST(escape_16) {
  const sourcemeta::core::JSON document{"foo\u0016bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0016bar\"");
}

TEST(escape_17) {
  const sourcemeta::core::JSON document{"foo\u0017bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0017bar\"");
}

TEST(escape_18) {
  const sourcemeta::core::JSON document{"foo\u0018bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0018bar\"");
}

TEST(escape_19) {
  const sourcemeta::core::JSON document{"foo\u0019bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0019bar\"");
}

TEST(escape_1A) {
  const sourcemeta::core::JSON document{"foo\u001abar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u001Abar\"");
}

TEST(escape_1B) {
  const sourcemeta::core::JSON document{"foo\u001bbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u001Bbar\"");
}

TEST(escape_1C) {
  const sourcemeta::core::JSON document{"foo\u001cbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u001Cbar\"");
}

TEST(escape_1D) {
  const sourcemeta::core::JSON document{"foo\u001dbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u001Dbar\"");
}

TEST(escape_1E) {
  const sourcemeta::core::JSON document{"foo\u001ebar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u001Ebar\"");
}

TEST(escape_1F) {
  const sourcemeta::core::JSON document{"foo\u001fbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u001Fbar\"");
}

TEST(decimal_positive_integer) {
  const sourcemeta::core::Decimal value{12345};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "1.2345e+4");
}

TEST(decimal_negative_integer) {
  const sourcemeta::core::Decimal value{-67890};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "-6.7890e+4");
}

TEST(decimal_zero) {
  const sourcemeta::core::Decimal value{0};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "0e+0");
}

TEST(decimal_fractional) {
  const sourcemeta::core::Decimal value{"3.14159"};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "3.14159e+0");
}

TEST(decimal_negative_fractional) {
  const sourcemeta::core::Decimal value{"-2.71828"};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "-2.71828e+0");
}

TEST(decimal_large_integer) {
  const sourcemeta::core::Decimal value{"123456789012345678901234567890"};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "1.23456789012345678901234567890e+29");
}

TEST(decimal_large_negative_integer) {
  const sourcemeta::core::Decimal value{"-987654321098765432109876543210"};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "-9.87654321098765432109876543210e+29");
}

TEST(decimal_high_precision_fractional) {
  const sourcemeta::core::Decimal value{
      "3.141592653589793238462643383279502884197"};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "3.141592653589793238462643383279502884197e+0");
}

TEST(decimal_very_small_fractional) {
  const sourcemeta::core::Decimal value{"0.000000000000000000000000000001"};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "1e-30");
}

TEST(decimal_scientific_notation) {
  const sourcemeta::core::Decimal value{"1.23e10"};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "1.23e+10");
}

TEST(scientific_constant_planck) {
  const sourcemeta::core::Decimal value{"6.62607E-34"};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "6.62607e-34");
}

TEST(scientific_constant_elementary_charge) {
  const sourcemeta::core::Decimal value{"1.60218E-19"};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "1.60218e-19");
}

TEST(scientific_constant_boltzmann) {
  const sourcemeta::core::Decimal value{"1.38065E-23"};
  const sourcemeta::core::JSON document{value};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "1.38065e-23");
}

TEST(decimal_in_array) {
  const sourcemeta::core::Decimal value1{100};
  const sourcemeta::core::Decimal value2{"999.999"};
  const sourcemeta::core::Decimal value3{"-42"};
  const sourcemeta::core::JSON document{sourcemeta::core::JSON{value1},
                                        sourcemeta::core::JSON{value2},
                                        sourcemeta::core::JSON{value3}};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "[1.00e+2,9.99999e+2,-4.2e+1]");
}

TEST(decimal_large_numbers_in_array) {
  const sourcemeta::core::Decimal value1{"123456789012345678901234567890"};
  const sourcemeta::core::Decimal value2{
      "98765432109876543210.98765432109876543210"};
  const sourcemeta::core::JSON document{sourcemeta::core::JSON{value1},
                                        sourcemeta::core::JSON{value2}};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "[1.23456789012345678901234567890e+29,9."
                          "876543210987654321098765432109876543210e+19]");
}

TEST(decimal_nested_in_array) {
  const sourcemeta::core::Decimal value1{"111.111"};
  const sourcemeta::core::Decimal value2{"222.222"};
  sourcemeta::core::JSON inner_array{sourcemeta::core::JSON{value1},
                                     sourcemeta::core::JSON{value2}};
  sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};
  document.push_back(std::move(inner_array));
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "[[1.11111e+2,2.22222e+2]]");
}

TEST(decimal_in_object) {
  const sourcemeta::core::Decimal value1{"12345"};
  const sourcemeta::core::Decimal value2{"-67.89"};
  const sourcemeta::core::JSON document{
      {"integer", sourcemeta::core::JSON{value1}},
      {"fractional", sourcemeta::core::JSON{value2}}};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  const bool matches =
      stream.str() == "{\"integer\":1.2345e+4,\"fractional\":-6.789e+1}" ||
      stream.str() == "{\"fractional\":-6.789e+1,\"integer\":1.2345e+4}";
  EXPECT_TRUE(matches);
}

TEST(decimal_large_numbers_in_object) {
  const sourcemeta::core::Decimal big_int{"999999999999999999999999999999"};
  const sourcemeta::core::Decimal big_real{
      "123456789.123456789123456789123456789"};
  sourcemeta::core::JSON document{
      {"bigInt", sourcemeta::core::JSON{big_int}},
      {"bigReal", sourcemeta::core::JSON{big_real}}};
  document.reorder(
      [](const auto &left, const auto &right) { return left < right; });
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(),
            "{\"bigInt\":9.99999999999999999999999999999e+29,\"bigReal\":1."
            "23456789123456789123456789123456789e+8}");
}

TEST(decimal_mixed_with_other_types_in_object) {
  const sourcemeta::core::Decimal decimal_value{"3.14159"};
  sourcemeta::core::JSON document{
      {"string", sourcemeta::core::JSON{"hello"}},
      {"integer", sourcemeta::core::JSON{42}},
      {"decimal", sourcemeta::core::JSON{decimal_value}},
      {"boolean", sourcemeta::core::JSON{true}}};
  document.reorder(
      [](const auto &left, const auto &right) { return left < right; });
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(
      stream.str(),
      "{\"boolean\":true,\"decimal\":3.14159e+0,\"integer\":42,\"string\":"
      "\"hello\"}");
}

TEST(decimal_nested_in_object_with_array) {
  const sourcemeta::core::Decimal value1{"111"};
  const sourcemeta::core::Decimal value2{"222.222"};
  sourcemeta::core::JSON array{sourcemeta::core::JSON::Array{}};
  array.push_back(sourcemeta::core::JSON{value1});
  array.push_back(sourcemeta::core::JSON{value2});
  const sourcemeta::core::JSON document{{"decimals", std::move(array)}};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "{\"decimals\":[1.11e+2,2.22222e+2]}");
}

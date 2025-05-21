#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <sstream>

TEST(JSON_stringify, boolean_false) {
  const sourcemeta::core::JSON document{false};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "false");
}

TEST(JSON_stringify, boolean_true) {
  const sourcemeta::core::JSON document{true};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "true");
}

TEST(JSON_stringify, null) {
  const sourcemeta::core::JSON document{nullptr};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "null");
}

TEST(JSON_stringify, integer_positive) {
  const sourcemeta::core::JSON document{1234};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "1234");
}

TEST(JSON_stringify, integer_negative) {
  const sourcemeta::core::JSON document{-1234};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "-1234");
}

TEST(JSON_stringify, integer_zero) {
  const sourcemeta::core::JSON document{0};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "0");
}

TEST(JSON_stringify, integer_minus_zero) {
  const sourcemeta::core::JSON document{-0};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "0");
}

TEST(JSON_stringify, real_positive) {
  const sourcemeta::core::JSON document{12.34};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "12.34");
}

TEST(JSON_stringify, real_negative) {
  const sourcemeta::core::JSON document{-12.34};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "-12.34");
}

TEST(JSON_stringify, real_zero) {
  const sourcemeta::core::JSON document{0.0};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "0.0");
}

TEST(JSON_stringify, real_minus_zero) {
  const sourcemeta::core::JSON document{-0.0};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "0.0");
}

TEST(JSON_stringify, integer_real_positive) {
  const sourcemeta::core::JSON document{2.0};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "2.0");
}

TEST(JSON_stringify, integer_real_negative) {
  const sourcemeta::core::JSON document{-2.0};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "-2.0");
}

TEST(JSON_stringify, empty_string) {
  const sourcemeta::core::JSON document{""};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"\"");
}

TEST(JSON_stringify, small_string) {
  const sourcemeta::core::JSON document{"foo"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\"");
}

TEST(JSON_stringify, array_integers) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::JSON{1}, sourcemeta::core::JSON{2},
      sourcemeta::core::JSON{3}, sourcemeta::core::JSON{4}};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "[1,2,3,4]");
}

TEST(JSON_stringify, array_nested) {
  sourcemeta::core::JSON array{
      sourcemeta::core::JSON{1}, sourcemeta::core::JSON{2},
      sourcemeta::core::JSON{3}, sourcemeta::core::JSON{4}};
  sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};
  document.push_back(std::move(array));
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "[[1,2,3,4]]");
}

TEST(JSON_stringify, array_empty) {
  const sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "[]");
}

TEST(JSON_stringify, object_integers) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                        {"bar", sourcemeta::core::JSON{2}}};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  // Because order is irrelevant
  const bool matches = stream.str() == "{\"foo\":1,\"bar\":2}" ||
                       stream.str() == "{\"bar\":2,\"foo\":1}";
  EXPECT_TRUE(matches);
}

TEST(JSON_stringify, object_integers_order_1) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                        {"bar", sourcemeta::core::JSON{2}},
                                        {"baz", sourcemeta::core::JSON{3}}};
  std::ostringstream stream;
  sourcemeta::core::stringify(
      document, stream,
      [](const auto &left, const auto &right) { return left < right; });
  EXPECT_EQ(stream.str(), "{\"bar\":2,\"baz\":3,\"foo\":1}");
}

TEST(JSON_stringify, object_integers_order_2) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                        {"bar", sourcemeta::core::JSON{2}},
                                        {"baz", sourcemeta::core::JSON{3}}};
  std::ostringstream stream;
  sourcemeta::core::stringify(
      document, stream,
      [](const auto &left, const auto &right) { return left > right; });
  EXPECT_EQ(stream.str(), "{\"foo\":1,\"baz\":3,\"bar\":2}");
}

TEST(JSON_stringify, object_empty) {
  const sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "{}");
}

TEST(JSON_stringify, object_with_array) {
  sourcemeta::core::JSON array{sourcemeta::core::JSON::Array{}};
  array.push_back(sourcemeta::core::JSON{1});
  array.push_back(sourcemeta::core::JSON{2});
  const sourcemeta::core::JSON document{{"foo", std::move(array)}};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "{\"foo\":[1,2]}");
}

TEST(JSON_stringify, stringify_quote) {
  const sourcemeta::core::JSON document{"\""};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"\\\"\"");
}

TEST(JSON_stringify, stringify_escape) {
  const sourcemeta::core::JSON document{"\\"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"\\\\\"");
}

TEST(JSON_stringify, stringify_solidus) {
  const sourcemeta::core::JSON document{"/"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"/\"");
}

TEST(JSON_stringify, stringify_backspace) {
  const sourcemeta::core::JSON document{"foo\bbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\bbar\"");
}

TEST(JSON_stringify, stringify_formfeed) {
  const sourcemeta::core::JSON document{"foo\fbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\fbar\"");
}

TEST(JSON_stringify, stringify_newline) {
  const sourcemeta::core::JSON document{"foo\nbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\nbar\"");
}

TEST(JSON_stringify, stringify_carriage_return) {
  const sourcemeta::core::JSON document{"foo\rbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\rbar\"");
}

TEST(JSON_stringify, stringify_tabulation) {
  const sourcemeta::core::JSON document{"foo\tbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\tbar\"");
}

TEST(JSON_stringify, stringify_scientific_real) {
  const sourcemeta::core::JSON document{4.321768E3};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "4321.77");
}

TEST(JSON_stringify, escape_00) {
  using namespace std::string_literals;
  const sourcemeta::core::JSON document{"foo\0bar"s};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0000bar\"");
}

TEST(JSON_stringify, escape_01) {
  const sourcemeta::core::JSON document{"foo\u0001bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0001bar\"");
}

TEST(JSON_stringify, escape_02) {
  const sourcemeta::core::JSON document{"foo\u0002bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0002bar\"");
}

TEST(JSON_stringify, escape_03) {
  const sourcemeta::core::JSON document{"foo\u0003bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0003bar\"");
}

TEST(JSON_stringify, escape_04) {
  const sourcemeta::core::JSON document{"foo\u0004bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0004bar\"");
}

TEST(JSON_stringify, escape_05) {
  const sourcemeta::core::JSON document{"foo\u0005bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0005bar\"");
}

TEST(JSON_stringify, escape_06) {
  const sourcemeta::core::JSON document{"foo\u0006bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0006bar\"");
}

TEST(JSON_stringify, escape_07) {
  const sourcemeta::core::JSON document{"foo\u0007bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0007bar\"");
}

TEST(JSON_stringify, escape_08) {
  const sourcemeta::core::JSON document{"foo\u0008bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\bbar\"");
}

TEST(JSON_stringify, escape_09) {
  const sourcemeta::core::JSON document{"foo\u0009bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\tbar\"");
}

TEST(JSON_stringify, escape_0A) {
  const sourcemeta::core::JSON document{"foo\u000abar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\nbar\"");
}

TEST(JSON_stringify, escape_0B) {
  const sourcemeta::core::JSON document{"foo\u000bbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u000Bbar\"");
}

TEST(JSON_stringify, escape_0C) {
  const sourcemeta::core::JSON document{"foo\u000cbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\fbar\"");
}

TEST(JSON_stringify, escape_0D) {
  const sourcemeta::core::JSON document{"foo\u000dbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\rbar\"");
}

TEST(JSON_stringify, escape_0E) {
  const sourcemeta::core::JSON document{"foo\u000ebar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u000Ebar\"");
}

TEST(JSON_stringify, escape_0F) {
  const sourcemeta::core::JSON document{"foo\u000fbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u000Fbar\"");
}

TEST(JSON_stringify, escape_10) {
  const sourcemeta::core::JSON document{"foo\u0010bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0010bar\"");
}

TEST(JSON_stringify, escape_11) {
  const sourcemeta::core::JSON document{"foo\u0011bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0011bar\"");
}

TEST(JSON_stringify, escape_12) {
  const sourcemeta::core::JSON document{"foo\u0012bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0012bar\"");
}

TEST(JSON_stringify, escape_13) {
  const sourcemeta::core::JSON document{"foo\u0013bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0013bar\"");
}

TEST(JSON_stringify, escape_14) {
  const sourcemeta::core::JSON document{"foo\u0014bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0014bar\"");
}

TEST(JSON_stringify, escape_15) {
  const sourcemeta::core::JSON document{"foo\u0015bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0015bar\"");
}

TEST(JSON_stringify, escape_16) {
  const sourcemeta::core::JSON document{"foo\u0016bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0016bar\"");
}

TEST(JSON_stringify, escape_17) {
  const sourcemeta::core::JSON document{"foo\u0017bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0017bar\"");
}

TEST(JSON_stringify, escape_18) {
  const sourcemeta::core::JSON document{"foo\u0018bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0018bar\"");
}

TEST(JSON_stringify, escape_19) {
  const sourcemeta::core::JSON document{"foo\u0019bar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u0019bar\"");
}

TEST(JSON_stringify, escape_1A) {
  const sourcemeta::core::JSON document{"foo\u001abar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u001Abar\"");
}

TEST(JSON_stringify, escape_1B) {
  const sourcemeta::core::JSON document{"foo\u001bbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u001Bbar\"");
}

TEST(JSON_stringify, escape_1C) {
  const sourcemeta::core::JSON document{"foo\u001cbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u001Cbar\"");
}

TEST(JSON_stringify, escape_1D) {
  const sourcemeta::core::JSON document{"foo\u001dbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u001Dbar\"");
}

TEST(JSON_stringify, escape_1E) {
  const sourcemeta::core::JSON document{"foo\u001ebar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u001Ebar\"");
}

TEST(JSON_stringify, escape_1F) {
  const sourcemeta::core::JSON document{"foo\u001fbar"};
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\u001Fbar\"");
}

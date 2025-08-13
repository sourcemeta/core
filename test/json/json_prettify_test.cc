#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <sstream>

TEST(JSON_prettify, boolean_false) {
  const sourcemeta::core::JSON document{false};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "false");
}

TEST(JSON_prettify, boolean_true) {
  const sourcemeta::core::JSON document{true};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "true");
}

TEST(JSON_prettify, null) {
  const sourcemeta::core::JSON document{nullptr};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "null");
}

TEST(JSON_prettify, integer_positive) {
  const sourcemeta::core::JSON document{1234};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "1234");
}

TEST(JSON_prettify, integer_negative) {
  const sourcemeta::core::JSON document{-1234};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "-1234");
}

TEST(JSON_prettify, integer_zero) {
  const sourcemeta::core::JSON document{0};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "0");
}

TEST(JSON_prettify, integer_minus_zero) {
  const sourcemeta::core::JSON document{-0};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "0");
}

TEST(JSON_prettify, real_positive) {
  const sourcemeta::core::JSON document{12.34};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "12.34");
}

TEST(JSON_prettify, real_negative) {
  const sourcemeta::core::JSON document{-12.34};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "-12.34");
}

TEST(JSON_prettify, real_zero) {
  const sourcemeta::core::JSON document{0.0};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "0.0");
}

TEST(JSON_prettify, real_minus_zero) {
  const sourcemeta::core::JSON document{-0.0};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "0.0");
}

TEST(JSON_prettify, empty_string) {
  const sourcemeta::core::JSON document{""};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "\"\"");
}

TEST(JSON_prettify, small_string) {
  const sourcemeta::core::JSON document{"foo"};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\"");
}

TEST(JSON_prettify, array_integers) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::JSON{1}, sourcemeta::core::JSON{2},
      sourcemeta::core::JSON{3}, sourcemeta::core::JSON{4}};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "[ 1, 2, 3, 4 ]");
}

TEST(JSON_prettify, array_strings_almost_long) {
  const sourcemeta::core::JSON document{sourcemeta::core::JSON{"aaaaaaaaaa"},
                                        sourcemeta::core::JSON{"bbbbbbbbbb"},
                                        sourcemeta::core::JSON{"cccccccccc"},
                                        sourcemeta::core::JSON{"dddddddddd"},
                                        sourcemeta::core::JSON{"eeeeeeeeee"}};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "[ \"aaaaaaaaaa\", \"bbbbbbbbbb\", \"cccccccccc\", "
                          "\"dddddddddd\", \"eeeeeeeeee\" ]");
}

TEST(JSON_prettify, array_strings_almost_long_with_indentation) {
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("1", sourcemeta::core::JSON::make_object());
  document.at("1").assign("2", sourcemeta::core::JSON::make_object());
  document.at("1").at("2").assign("3", sourcemeta::core::JSON::make_object());
  document.at("1").at("2").at("3").assign(
      "4", sourcemeta::core::JSON::make_object());
  document.at("1").at("2").at("3").at("4").assign(
      "5", sourcemeta::core::JSON::make_array());

  document.at("1").at("2").at("3").at("4").at("5").push_back(
      sourcemeta::core::JSON{"aaaaaaaaaa"});
  document.at("1").at("2").at("3").at("4").at("5").push_back(
      sourcemeta::core::JSON{"bbbbbbbbbb"});
  document.at("1").at("2").at("3").at("4").at("5").push_back(
      sourcemeta::core::JSON{"cccccccccc"});
  document.at("1").at("2").at("3").at("4").at("5").push_back(
      sourcemeta::core::JSON{"dddddddddd"});
  document.at("1").at("2").at("3").at("4").at("5").push_back(
      sourcemeta::core::JSON{"eeeeeeeeee"});

  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);

  const auto expected = R"JSON({
  "1": {
    "2": {
      "3": {
        "4": {
          "5": [
            "aaaaaaaaaa",
            "bbbbbbbbbb",
            "cccccccccc",
            "dddddddddd",
            "eeeeeeeeee"
          ]
        }
      }
    }
  }
})JSON";

  EXPECT_EQ(stream.str(), expected);
}

TEST(JSON_prettify, array_strings_almost_long_with_long_property) {
  auto document{sourcemeta::core::JSON::make_object()};
  // Key has 40 characters (plus 2 for quotes + 1 for semicolon + 1 for space) =
  // 44
  document.assign("longlonglonglonglonglonglonglonglonglong",
                  sourcemeta::core::JSON::make_array());

  document.at("longlonglonglonglonglonglonglonglonglong")
      .push_back(sourcemeta::core::JSON{"aaaaaaaaaa"});
  document.at("longlonglonglonglonglonglonglonglonglong")
      .push_back(sourcemeta::core::JSON{"bbbbbbbbbb"});
  document.at("longlonglonglonglonglonglonglonglonglong")
      .push_back(sourcemeta::core::JSON{"cccccccccc"});
  document.at("longlonglonglonglonglonglonglonglonglong")
      .push_back(sourcemeta::core::JSON{"dddddddddd"});

  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);

  const auto expected = R"JSON({
  "longlonglonglonglonglonglonglonglonglong": [
    "aaaaaaaaaa",
    "bbbbbbbbbb",
    "cccccccccc",
    "dddddddddd"
  ]
})JSON";

  EXPECT_EQ(stream.str(), expected);
}

TEST(JSON_prettify, array_strings_long) {
  const sourcemeta::core::JSON document{sourcemeta::core::JSON{"aaaaaaaaaa"},
                                        sourcemeta::core::JSON{"bbbbbbbbbb"},
                                        sourcemeta::core::JSON{"cccccccccc"},
                                        sourcemeta::core::JSON{"dddddddddd"},
                                        sourcemeta::core::JSON{"eeeeeeeeee"},
                                        sourcemeta::core::JSON{"ffffffffff"},
                                        sourcemeta::core::JSON{"gggggggggg"},
                                        sourcemeta::core::JSON{"hhhhhhhhhh"}};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(),
            "[\n  \"aaaaaaaaaa\",\n  \"bbbbbbbbbb\",\n  \"cccccccccc\",\n  "
            "\"dddddddddd\",\n  \"eeeeeeeeee\",\n  \"ffffffffff\",\n  "
            "\"gggggggggg\",\n  \"hhhhhhhhhh\"\n]");
}

TEST(JSON_prettify, array_nested_1) {
  sourcemeta::core::JSON array{
      sourcemeta::core::JSON{1}, sourcemeta::core::JSON{2},
      sourcemeta::core::JSON{3}, sourcemeta::core::JSON{4}};
  sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};
  document.push_back(std::move(array));
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "[\n  [ 1, 2, 3, 4 ]\n]");
}

TEST(JSON_prettify, array_nested_2) {
  sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};
  document.push_back(sourcemeta::core::JSON{1});
  sourcemeta::core::JSON nested{sourcemeta::core::JSON{2},
                                sourcemeta::core::JSON{3}};
  document.push_back(std::move(nested));
  document.push_back(sourcemeta::core::JSON{4});
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "[\n  1,\n  [ 2, 3 ],\n  4\n]");
}

TEST(JSON_prettify, array_empty) {
  const sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "[]");
}

TEST(JSON_prettify, object_integers) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                        {"bar", sourcemeta::core::JSON{2}}};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  // Because order is irrelevant
  const bool matches = stream.str() == "{\n  \"foo\": 1,\n  \"bar\": 2\n}" ||
                       stream.str() == "{\n  \"bar\": 2,\n  \"foo\": 1\n}";
  EXPECT_TRUE(matches);
}

TEST(JSON_prettify, object_integers_order_1) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                        {"bar", sourcemeta::core::JSON{2}},
                                        {"baz", sourcemeta::core::JSON{3}}};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream,
                             [](const auto &, const auto &left,
                                const auto &right) { return left < right; });
  EXPECT_EQ(stream.str(), "{\n  \"bar\": 2,\n  \"baz\": 3,\n  \"foo\": 1\n}");
}

TEST(JSON_prettify, object_integers_order_2) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                        {"bar", sourcemeta::core::JSON{2}},
                                        {"baz", sourcemeta::core::JSON{3}}};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream,
                             [](const auto &, const auto &left,
                                const auto &right) { return left > right; });
  EXPECT_EQ(stream.str(), "{\n  \"foo\": 1,\n  \"baz\": 3,\n  \"bar\": 2\n}");
}

TEST(JSON_prettify, object_empty) {
  const sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "{}");
}

TEST(JSON_prettify, object_with_array) {
  sourcemeta::core::JSON array{sourcemeta::core::JSON::Array{}};
  array.push_back(sourcemeta::core::JSON{1});
  array.push_back(sourcemeta::core::JSON{2});
  const sourcemeta::core::JSON document{{"foo", std::move(array)}};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "{\n  \"foo\": [ 1, 2 ]\n}");
}

TEST(JSON_prettify, object_nested) {
  sourcemeta::core::JSON object = sourcemeta::core::JSON::make_object();
  object.assign("bar", sourcemeta::core::JSON{1});
  const sourcemeta::core::JSON document{{"foo", std::move(object)}};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "{\n  \"foo\": {\n    \"bar\": 1\n  }\n}");
}

TEST(JSON_prettify, array_with_object) {
  sourcemeta::core::JSON object = sourcemeta::core::JSON::make_object();
  object.assign("foo", sourcemeta::core::JSON{1});
  sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};
  document.push_back(std::move(object));
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "[\n  {\n    \"foo\": 1\n  }\n]");
}

TEST(JSON_prettify, simple_object) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}}};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "{\n  \"foo\": 1\n}");
}

TEST(JSON_prettify, prettify_quote) {
  const sourcemeta::core::JSON document{"\""};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "\"\\\"\"");
}

TEST(JSON_prettify, prettify_escape) {
  const sourcemeta::core::JSON document{"\\"};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "\"\\\\\"");
}

TEST(JSON_prettify, prettify_solidus) {
  const sourcemeta::core::JSON document{"/"};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "\"/\"");
}

TEST(JSON_prettify, prettify_backspace) {
  const sourcemeta::core::JSON document{"foo\bbar"};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\bbar\"");
}

TEST(JSON_prettify, prettify_formfeed) {
  const sourcemeta::core::JSON document{"foo\fbar"};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\fbar\"");
}

TEST(JSON_prettify, prettify_newline) {
  const sourcemeta::core::JSON document{"foo\nbar"};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\nbar\"");
}

TEST(JSON_prettify, prettify_carriage_return) {
  const sourcemeta::core::JSON document{"foo\rbar"};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\rbar\"");
}

TEST(JSON_prettify, prettify_tabulation) {
  const sourcemeta::core::JSON document{"foo\tbar"};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "\"foo\\tbar\"");
}

TEST(JSON_prettify, prettify_scientific_real) {
  const sourcemeta::core::JSON document{4.321768E3};
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "4321.77");
}

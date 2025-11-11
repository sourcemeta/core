#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>

#include <sstream>

TEST(JSON_parse, true) {
  std::istringstream input{"true"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_boolean());
  EXPECT_TRUE(document.to_boolean());
}

TEST(JSON_parse, false) {
  std::istringstream input{"false"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_boolean());
  EXPECT_FALSE(document.to_boolean());
}

TEST(JSON_parse, true_equality) {
  std::istringstream input_1{"true"};
  std::istringstream input_2{"   true    "};
  std::istringstream input_3{"false"};
  const sourcemeta::core::JSON document_1 =
      sourcemeta::core::parse_json(input_1);
  const sourcemeta::core::JSON document_2 =
      sourcemeta::core::parse_json(input_2);
  const sourcemeta::core::JSON document_3 =
      sourcemeta::core::parse_json(input_3);
  EXPECT_EQ(document_1, document_2);
  EXPECT_FALSE(document_1 == document_3);
  EXPECT_FALSE(document_2 == document_3);
}

TEST(JSON_parse, false_equality) {
  std::istringstream input_1{"false"};
  std::istringstream input_2{"   false    "};
  std::istringstream input_3{"true"};
  const sourcemeta::core::JSON document_1 =
      sourcemeta::core::parse_json(input_1);
  const sourcemeta::core::JSON document_2 =
      sourcemeta::core::parse_json(input_2);
  const sourcemeta::core::JSON document_3 =
      sourcemeta::core::parse_json(input_3);
  EXPECT_EQ(document_1, document_2);
  EXPECT_FALSE(document_1 == document_3);
  EXPECT_FALSE(document_2 == document_3);
}

TEST(JSON_parse, true_with_spacing) {
  std::istringstream input{"   true   "};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_boolean());
  EXPECT_TRUE(document.to_boolean());
}

TEST(JSON_parse, false_with_spacing) {
  std::istringstream input{"   false   "};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_boolean());
  EXPECT_FALSE(document.to_boolean());
}

TEST(JSON_parse, null) {
  std::istringstream input{"null"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_null());
}

TEST(JSON_parse, null_with_spacing) {
  std::istringstream input{"   null   "};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_null());
}

TEST(JSON_parse, null_equality) {
  std::istringstream input_1{"null"};
  std::istringstream input_2{"   null    "};
  std::istringstream input_3{"true"};
  const sourcemeta::core::JSON document_1 =
      sourcemeta::core::parse_json(input_1);
  const sourcemeta::core::JSON document_2 =
      sourcemeta::core::parse_json(input_2);
  const sourcemeta::core::JSON document_3 =
      sourcemeta::core::parse_json(input_3);
  EXPECT_EQ(document_1, document_2);
  EXPECT_FALSE(document_1 == document_3);
  EXPECT_FALSE(document_2 == document_3);
}

TEST(JSON_parse, string_empty) {
  std::istringstream input{"\"\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(sourcemeta::core::JSON::size(document.to_string()), 0);
  EXPECT_EQ(document.to_string(), "");
}

TEST(JSON_parse, string_with_null) {
  std::istringstream input{"\"foo \\u0000 bar\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 9);
  EXPECT_EQ(sourcemeta::core::JSON::size(document.to_string()), 9);

  // See https://stackoverflow.com/a/164274
  using namespace std::string_literals;
  EXPECT_EQ(document.to_string(), "foo \0 bar"s);
}

TEST(JSON_parse, string_foo) {
  std::istringstream input{"\"foo\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(sourcemeta::core::JSON::size(document.to_string()), 3);
  EXPECT_EQ(document.to_string(), "foo");
}

TEST(JSON_parse, string_foo_with_spacing) {
  std::istringstream input{"      \"foo\"     "};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(sourcemeta::core::JSON::size(document.to_string()), 3);
  EXPECT_EQ(document.to_string(), "foo");
}

TEST(JSON_parse, string_foo_padded) {
  std::istringstream input{"\"   foo   \""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 9);
  EXPECT_EQ(sourcemeta::core::JSON::size(document.to_string()), 9);
  EXPECT_EQ(document.to_string(), "   foo   ");
}

TEST(JSON_parse, string_escape_quote) {
  std::istringstream input{"\"foo\\\"bar\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 7);
  EXPECT_EQ(sourcemeta::core::JSON::size(document.to_string()), 7);
  EXPECT_EQ(document.to_string(), "foo\"bar");
}

TEST(JSON_parse, empty_array) {
  std::istringstream input{"[]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 0);
}

TEST(JSON_parse, empty_array_with_inner_space) {
  std::istringstream input{"[            ]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 0);
}

TEST(JSON_parse, array_one_null_item) {
  std::istringstream input{"[null]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_null());
}

TEST(JSON_parse, array_one_boolean_item) {
  std::istringstream input{"[true]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_boolean());
  EXPECT_TRUE(document.at(0).to_boolean());
}

TEST(JSON_parse, array_one_boolean_item_with_inner_space) {
  std::istringstream input{"[   true     ]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_boolean());
  EXPECT_TRUE(document.at(0).to_boolean());
}

TEST(JSON_parse, array_one_boolean_item_with_padding) {
  std::istringstream input{"   [ true  ]   "};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_boolean());
  EXPECT_TRUE(document.at(0).to_boolean());
}

TEST(JSON_parse, array_one_nested_null_item) {
  std::istringstream input{"[[null]]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_array());
  EXPECT_EQ(document.at(0).size(), 1);
  EXPECT_TRUE(document.at(0).at(0).is_null());
}

TEST(JSON_parse, array_one_nested_nested_null_item) {
  std::istringstream input{"[[[null]]]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_array());
  EXPECT_EQ(document.at(0).size(), 1);
  EXPECT_TRUE(document.at(0).at(0).is_array());
  EXPECT_EQ(document.at(0).at(0).size(), 1);
  EXPECT_TRUE(document.at(0).at(0).at(0).is_null());
}

TEST(JSON_parse, array_nested_nested_string) {
  std::istringstream input{"[[[\"nested\"]]]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_array());
  EXPECT_EQ(document.at(0).size(), 1);
  EXPECT_TRUE(document.at(0).at(0).is_array());
  EXPECT_EQ(document.at(0).at(0).size(), 1);
  EXPECT_TRUE(document.at(0).at(0).at(0).is_string());
  EXPECT_EQ(document.at(0).at(0).at(0).to_string(), "nested");
}

TEST(JSON_parse, array_false_true_false) {
  std::istringstream input{"[false,true,false]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_TRUE(document.at(0).is_boolean());
  EXPECT_TRUE(document.at(1).is_boolean());
  EXPECT_TRUE(document.at(2).is_boolean());
  EXPECT_FALSE(document.at(0).to_boolean());
  EXPECT_TRUE(document.at(1).to_boolean());
  EXPECT_FALSE(document.at(2).to_boolean());
}

TEST(JSON_parse, array_foo_bar_baz) {
  std::istringstream input{"[\"foo\",\"bar\",\"baz\"]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_TRUE(document.at(0).is_string());
  EXPECT_TRUE(document.at(1).is_string());
  EXPECT_TRUE(document.at(2).is_string());
  EXPECT_EQ(document.at(0).to_string(), "foo");
  EXPECT_EQ(document.at(1).to_string(), "bar");
  EXPECT_EQ(document.at(2).to_string(), "baz");
}

TEST(JSON_parse, array_mixed_boolean_nested) {
  std::istringstream input{"[false,[true],false]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_TRUE(document.at(0).is_boolean());
  EXPECT_TRUE(document.at(1).is_array());
  EXPECT_TRUE(document.at(2).is_boolean());
  EXPECT_FALSE(document.at(0).to_boolean());
  EXPECT_EQ(document.at(1).size(), 1);
  EXPECT_TRUE(document.at(1).at(0).is_boolean());
  EXPECT_TRUE(document.at(1).at(0).to_boolean());
  EXPECT_FALSE(document.at(2).to_boolean());
}

TEST(JSON_parse, array_nested_double) {
  std::istringstream input{"[true,[false,[true]]]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 2);
  EXPECT_TRUE(document.at(0).is_boolean());
  EXPECT_TRUE(document.at(0).to_boolean());
  EXPECT_TRUE(document.at(1).is_array());
  EXPECT_EQ(document.at(1).size(), 2);
  EXPECT_TRUE(document.at(1).at(0).is_boolean());
  EXPECT_FALSE(document.at(1).at(0).to_boolean());
  EXPECT_TRUE(document.at(1).at(1).is_array());
  EXPECT_EQ(document.at(1).at(1).size(), 1);
  EXPECT_TRUE(document.at(1).at(1).at(0).is_boolean());
  EXPECT_TRUE(document.at(1).at(1).at(0).to_boolean());
}

TEST(JSON_parse, array_nested_double_with_spacing) {
  std::istringstream input{"   [   true  ,  [ false  ,  [ true] ] ]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 2);
  EXPECT_TRUE(document.at(0).is_boolean());
  EXPECT_TRUE(document.at(0).to_boolean());
  EXPECT_TRUE(document.at(1).is_array());
  EXPECT_EQ(document.at(1).size(), 2);
  EXPECT_TRUE(document.at(1).at(0).is_boolean());
  EXPECT_FALSE(document.at(1).at(0).to_boolean());
  EXPECT_TRUE(document.at(1).at(1).is_array());
  EXPECT_EQ(document.at(1).at(1).size(), 1);
  EXPECT_TRUE(document.at(1).at(1).at(0).is_boolean());
  EXPECT_TRUE(document.at(1).at(1).at(0).to_boolean());
}

TEST(JSON_parse, array_mixed_boolean_nested_with_spaces) {
  std::istringstream input{"  [   false  ,  [ true]    ,   false ]   "};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_TRUE(document.at(0).is_boolean());
  EXPECT_TRUE(document.at(1).is_array());
  EXPECT_TRUE(document.at(2).is_boolean());
  EXPECT_FALSE(document.at(0).to_boolean());
  EXPECT_EQ(document.at(1).size(), 1);
  EXPECT_TRUE(document.at(1).at(0).is_boolean());
  EXPECT_TRUE(document.at(1).at(0).to_boolean());
  EXPECT_FALSE(document.at(2).to_boolean());
}

TEST(JSON_parse, array_one_string_item) {
  std::istringstream input{"[\"foo\"]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_string());
  EXPECT_EQ(document.at(0).to_string(), "foo");
}

TEST(JSON_parse, array_one_positive_integer_item) {
  std::istringstream input{"[5]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_integer());
  EXPECT_EQ(document.at(0).to_integer(), 5);
}

TEST(JSON_parse, array_one_negative_integer_item) {
  std::istringstream input{"[-5]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_integer());
  EXPECT_EQ(document.at(0).to_integer(), -5);
}

TEST(JSON_parse, array_one_positive_real_item) {
  std::istringstream input{"[5.5]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_real());
  EXPECT_EQ(document.at(0).to_real(), 5.5);
}

TEST(JSON_parse, array_one_negative_real_item) {
  std::istringstream input{"[-5.5]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_real());
  EXPECT_EQ(document.at(0).to_real(), -5.5);
}

TEST(JSON_parse, array_comma_within_string) {
  std::istringstream input{"[\"foo,bar\"]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_string());
  EXPECT_EQ(document.at(0).to_string(), "foo,bar");
}

TEST(JSON_parse, array_with_stringified_array) {
  std::istringstream input{"[\"[false,true]\"]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_string());
  EXPECT_EQ(document.at(0).to_string(), "[false,true]");
}

TEST(JSON_parse, array_equality) {
  std::istringstream input_1{"[ 1, 2, 3 ]"};
  std::istringstream input_2{"    [   1, 2    ,3]    "};
  std::istringstream input_3{"   [1,2,2]"};
  const sourcemeta::core::JSON document_1 =
      sourcemeta::core::parse_json(input_1);
  const sourcemeta::core::JSON document_2 =
      sourcemeta::core::parse_json(input_2);
  const sourcemeta::core::JSON document_3 =
      sourcemeta::core::parse_json(input_3);
  EXPECT_EQ(document_1, document_2);
  EXPECT_FALSE(document_1 == document_3);
  EXPECT_FALSE(document_2 == document_3);
}

TEST(JSON_parse, single_exponential_number_element) {
  std::istringstream input{"[3e2]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_real());
  EXPECT_EQ(document.at(0).to_real(), 300.0);
}

TEST(JSON_parse, object_equality) {
  std::istringstream input_1{"{\"foo\":1}"};
  std::istringstream input_2{"   {   \"foo\"   :  1  }    "};
  std::istringstream input_3{"{\"foo\":2}"};
  const sourcemeta::core::JSON document_1 =
      sourcemeta::core::parse_json(input_1);
  const sourcemeta::core::JSON document_2 =
      sourcemeta::core::parse_json(input_2);
  const sourcemeta::core::JSON document_3 =
      sourcemeta::core::parse_json(input_3);
  EXPECT_EQ(document_1, document_2);
  EXPECT_FALSE(document_1 == document_3);
  EXPECT_FALSE(document_2 == document_3);
}

TEST(JSON_parse, object_single_null) {
  std::istringstream input{"{\"foo\":null}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_null());
}

TEST(JSON_parse, object_single_null_with_spaces) {
  std::istringstream input{"   {   \"foo\"   :   null  }  "};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_null());
}

TEST(JSON_parse, object_multiple_null) {
  std::istringstream input{"{\"foo\":null,\"bar\":null,\"baz\":null}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.defines("baz"));
  EXPECT_TRUE(document.at("foo").is_null());
  EXPECT_TRUE(document.at("bar").is_null());
  EXPECT_TRUE(document.at("baz").is_null());
}

TEST(JSON_parse, array_with_object_size_1) {
  std::istringstream input{"[ { \"foo\": null } ]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_object());
  EXPECT_TRUE(document.at(0).defines("foo"));
  EXPECT_TRUE(document.at(0).at("foo").is_null());
}

TEST(JSON_parse, array_with_object_size_2) {
  std::istringstream input{"[ { \"foo\": false, \"bar\": true } ]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_object());
  EXPECT_EQ(document.at(0).size(), 2);
  EXPECT_TRUE(document.at(0).defines("foo"));
  EXPECT_TRUE(document.at(0).defines("bar"));
  EXPECT_TRUE(document.at(0).at("foo").is_boolean());
  EXPECT_TRUE(document.at(0).at("bar").is_boolean());
  EXPECT_FALSE(document.at(0).at("foo").to_boolean());
  EXPECT_TRUE(document.at(0).at("bar").to_boolean());
}

TEST(JSON_parse, array_with_empty_object) {
  std::istringstream input{"[{}]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_object());
  EXPECT_EQ(document.at(0).size(), 0);
}

TEST(JSON_parse, array_escaped_quote_within_string_element) {
  std::istringstream input{"[\"foo\\\"bar\"]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at(0).is_string());
  EXPECT_EQ(document.at(0).to_string(), "foo\"bar");
}

TEST(JSON_parse, object_with_array) {
  std::istringstream input{"{ \"foo\": [null] }"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_array());
  EXPECT_EQ(document.at("foo").size(), 1);
  EXPECT_TRUE(document.at("foo").at(0).is_null());
}

TEST(JSON_parse, object_with_array_two_elements) {
  std::istringstream input{"{ \"foo\": [false, true] }"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_array());
  EXPECT_EQ(document.at("foo").size(), 2);
  EXPECT_TRUE(document.at("foo").at(0).is_boolean());
  EXPECT_TRUE(document.at("foo").at(1).is_boolean());
  EXPECT_FALSE(document.at("foo").at(0).to_boolean());
  EXPECT_TRUE(document.at("foo").at(1).to_boolean());
}

TEST(JSON_parse, object_with_object) {
  std::istringstream input{"{ \"foo\": { \"bar\": null } }"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_object());
  EXPECT_EQ(document.at("foo").size(), 1);
  EXPECT_TRUE(document.at("foo").defines("bar"));
  EXPECT_TRUE(document.at("foo").at("bar").is_null());
}

TEST(JSON_parse, object_with_positive_integer) {
  std::istringstream input{"{ \"foo\": 5 }"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_integer());
  EXPECT_EQ(document.at("foo").to_integer(), 5);
}

TEST(JSON_parse, empty_object) {
  std::istringstream input{"{}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
}

TEST(JSON_parse, empty_object_with_spacing) {
  std::istringstream input{"{      }"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
}

TEST(JSON_parse, object_string_value_with_comma) {
  std::istringstream input{"{\"foo\":\"bar,baz\"}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_string());
  EXPECT_EQ(document.at("foo").to_string(), "bar,baz");
}

TEST(JSON_parse, object_string_value_with_escaped_quote) {
  std::istringstream input{"{\"foo\":\"bar\\\"baz\"}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_string());
  EXPECT_EQ(document.at("foo").to_string(), "bar\"baz");
}

TEST(JSON_parse, object_empty_string_key) {
  std::istringstream input{"{\"\":\"foo\"}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines(""));
  EXPECT_TRUE(document.at("").is_string());
  EXPECT_EQ(document.at("").to_string(), "foo");
}

TEST(JSON_parse, object_string_key_with_comma) {
  std::istringstream input{"{\"foo,bar\":\"baz\"}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo,bar"));
  EXPECT_TRUE(document.at("foo,bar").is_string());
  EXPECT_EQ(document.at("foo,bar").to_string(), "baz");
}

TEST(JSON_parse, object_string_key_with_space) {
  std::istringstream input{"{\"foo bar\":\"baz\"}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo bar"));
  EXPECT_TRUE(document.at("foo bar").is_string());
  EXPECT_EQ(document.at("foo bar").to_string(), "baz");
}

TEST(JSON_parse, object_string_value_with_stringified) {
  std::istringstream input{"{\"foo\":\"{\\\"x\\\":1}\"}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_string());
  EXPECT_EQ(document.at("foo").to_string(), "{\"x\":1}");
}

TEST(JSON_parse, object_one_true_boolean_element_with_space) {
  std::istringstream input{"    {   \"foo\"   :   true  }    "};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_boolean());
  EXPECT_TRUE(document.at("foo").to_boolean());
}

TEST(JSON_parse, object_two_boolean_values_with_space) {
  std::istringstream input{
      "{   \"foo\"  :   true    ,     \"bar\"   :   false}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.at("foo").is_boolean());
  EXPECT_TRUE(document.at("foo").to_boolean());
  EXPECT_TRUE(document.at("bar").is_boolean());
  EXPECT_FALSE(document.at("bar").to_boolean());
}

TEST(JSON_parse, object_one_array_element_with_space) {
  std::istringstream input{"{   \"foo\"   :  [  true  ,  false   ]   }"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_array());
  EXPECT_EQ(document.at("foo").size(), 2);
  EXPECT_TRUE(document.at("foo").at(0).is_boolean());
  EXPECT_TRUE(document.at("foo").at(1).is_boolean());
  EXPECT_TRUE(document.at("foo").at(0).to_boolean());
  EXPECT_FALSE(document.at("foo").at(1).to_boolean());
}

TEST(JSON_parse, object_minified_nested) {
  std::istringstream input{"{\"foo\":{\"bar\":true}}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_object());
  EXPECT_EQ(document.at("foo").size(), 1);
  EXPECT_TRUE(document.at("foo").defines("bar"));
}

TEST(JSON_parse, object_empty_nested_with_new_line_before_end) {
  std::istringstream input("{\"x\":{}\n}");
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("x"));
  EXPECT_TRUE(document.at("x").is_object());
  EXPECT_EQ(document.at("x").size(), 0);
}

TEST(JSON_parse, zero_integer) {
  std::istringstream input{"0"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 0);
}

TEST(JSON_parse, minus_zero_integer) {
  std::istringstream input{"-0"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 0);
}

TEST(JSON_parse, positive_single_digit_positive_integer) {
  std::istringstream input{"1"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 1);
}

TEST(JSON_parse, positive_multi_digit_positive_integer) {
  std::istringstream input{"1234"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 1234);
}

TEST(JSON_parse, positive_single_digit_negative_integer) {
  std::istringstream input{"-1"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), -1);
}

TEST(JSON_parse, positive_multi_digit_negative_integer) {
  std::istringstream input{"-1234"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), -1234);
}

TEST(JSON_parse, positive_large_integer) {
  std::istringstream input{"12391239123"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 12391239123);
}

TEST(JSON_parse, positive_real_trailing_zeroes) {
  std::istringstream input{"1.50000"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 1.5);
}

TEST(JSON_parse, negative_real_trailing_zeroes) {
  std::istringstream input{"-1.50000"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -1.5);
}

TEST(JSON_parse, positive_real) {
  std::istringstream input{"1.5"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 1.5);
}

TEST(JSON_parse, negative_real) {
  std::istringstream input{"-1.5"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -1.5);
}

TEST(JSON_parse, real_leading_decimal_zero) {
  std::istringstream input{"1.125"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 1.125);
}

TEST(JSON_parse, real_multi_left_digit_positive_real) {
  std::istringstream input{"1234.5"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 1234.5);
}

TEST(JSON_parse, real_multi_left_digit_negative_real) {
  std::istringstream input{"-1234.5"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -1234.5);
}

TEST(JSON_parse, real_long_positive_real) {
  std::istringstream input{"1234.56789"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 1234.56789);
}

TEST(JSON_parse, real_long_negative_real) {
  std::istringstream input{"-1234.56789"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -1234.56789);
}

TEST(JSON_parse, single_digit_positive_real_integer) {
  std::istringstream input{"1.0"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 1.0);
}

TEST(JSON_parse, single_digit_positive_real_integer_trailing_zero) {
  std::istringstream input{"1.0000000"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 1.0);
}

TEST(JSON_parse, single_digit_negative_real_integer) {
  std::istringstream input{"-1.0"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -1.0);
}

TEST(JSON_parse, single_digit_negative_real_integer_trailing_zero) {
  std::istringstream input{"-1.0000000"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -1.0);
}

TEST(JSON_parse, leading_zero_real_number) {
  std::istringstream input{"-0.125"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -0.125);
}

TEST(JSON_parse, zero_integer_with_exponent) {
  std::istringstream input{"0e2"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 0.0);
}

TEST(JSON_parse, zero_real_with_exponent) {
  std::istringstream input{"0.0e2"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 0.0);
}

TEST(JSON_parse, large_negative_exponential_number) {
  std::istringstream input{"-1.0e10"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -1e10);
}

TEST(JSON_parse, large_positive_exponential_number_with_plus_exponent) {
  std::istringstream input{"1.0e+10"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 1e10);
}

TEST(JSON_parse, large_negative_exponential_number_with_plus_exponent) {
  std::istringstream input{"-1.0e+10"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -1e10);
}

TEST(JSON_parse, number_exponential_notation_plus_after_e) {
  std::istringstream input{"3E+2"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 300.0);
}

TEST(JSON_parse, exponential_notation_integer_1_upper) {
  std::istringstream input{"2E0"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 2.0);
}

TEST(JSON_parse, exponential_notation_integer_2_upper) {
  std::istringstream input{"3E2"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 300.0);
}

TEST(JSON_parse, exponential_notation_integer_3_upper) {
  std::istringstream input{"4.321768E3"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 4321.768);
}

TEST(JSON_parse, exponential_notation_integer_4_upper) {
  std::istringstream input{"-5.3E4"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -53000);
}

TEST(JSON_parse, exponential_notation_integer_5_upper) {
  std::istringstream input{"6.72E9"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 6720000000);
}

TEST(JSON_parse, exponential_notation_integer_6_upper) {
  std::istringstream input{"5E-1"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 0.5);
}

TEST(JSON_parse, exponential_notation_integer_7_upper) {
  std::istringstream input{"9.5E2"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 950);
}

TEST(JSON_parse, exponential_notation_integer_8_upper) {
  std::istringstream input{"5E-1"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 0.5);
}

TEST(JSON_parse, exponential_notation_integer_1_lower) {
  std::istringstream input{"2e0"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 2.0);
}

TEST(JSON_parse, exponential_notation_integer_2_lower) {
  std::istringstream input{"3e2"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 300.0);
}

TEST(JSON_parse, exponential_notation_integer_3_lower) {
  std::istringstream input{"4.321768e3"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 4321.768);
}

TEST(JSON_parse, exponential_notation_integer_4_lower) {
  std::istringstream input{"-5.3e4"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -53000);
}

TEST(JSON_parse, exponential_notation_integer_5_lower) {
  std::istringstream input{"6.72e9"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 6720000000);
}

TEST(JSON_parse, exponential_notation_integer_6_lower) {
  std::istringstream input{"5e-1"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 0.5);
}

TEST(JSON_parse, exponential_notation_integer_7_lower) {
  std::istringstream input{"9.5e2"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 950);
}

TEST(JSON_parse, exponential_notation_integer_8_lower) {
  std::istringstream input{"5e-1"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 0.5);
}

TEST(JSON_parse, exponential_notation_integer_1_real) {
  std::istringstream input{"2.0e0"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 2.0);
}

TEST(JSON_parse, exponential_notation_integer_2_real) {
  std::istringstream input{"3.0e2"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 300.0);
}

TEST(JSON_parse, exponential_notation_integer_3_real) {
  std::istringstream input{"5.0e-1"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 0.5);
}

TEST(JSON_parse, integer_equality) {
  std::istringstream input_1{"5"};
  std::istringstream input_2{"   5    "};
  std::istringstream input_3{" 5.1 "};
  const sourcemeta::core::JSON document_1 =
      sourcemeta::core::parse_json(input_1);
  const sourcemeta::core::JSON document_2 =
      sourcemeta::core::parse_json(input_2);
  const sourcemeta::core::JSON document_3 =
      sourcemeta::core::parse_json(input_3);
  EXPECT_EQ(document_1, document_2);
  EXPECT_FALSE(document_1 == document_3);
  EXPECT_FALSE(document_2 == document_3);
}

TEST(JSON_parse, real_equality) {
  std::istringstream input_1{"5.1"};
  std::istringstream input_2{"   5.1    "};
  std::istringstream input_3{" 5 "};
  const sourcemeta::core::JSON document_1 =
      sourcemeta::core::parse_json(input_1);
  const sourcemeta::core::JSON document_2 =
      sourcemeta::core::parse_json(input_2);
  const sourcemeta::core::JSON document_3 =
      sourcemeta::core::parse_json(input_3);
  EXPECT_EQ(document_1, document_2);
  EXPECT_FALSE(document_1 == document_3);
  EXPECT_FALSE(document_2 == document_3);
}

TEST(JSON_parse, integer_real_equal) {
  std::istringstream left{"1"};
  std::istringstream right{"1.0"};
  const sourcemeta::core::JSON document_1 = sourcemeta::core::parse_json(left);
  const sourcemeta::core::JSON document_2 = sourcemeta::core::parse_json(right);
  EXPECT_TRUE(document_1 == document_2);
  EXPECT_TRUE(document_2 == document_1);
  EXPECT_FALSE(document_1 != document_2);
  EXPECT_FALSE(document_2 != document_1);
}

TEST(JSON_parse, string_escaped_quotes) {
  std::istringstream input{"\"\\\"foo\\\"\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 5);
  EXPECT_EQ(document.to_string(), "\"foo\"");
}

TEST(JSON_parse, string_escaped_reverse_solidus) {
  std::istringstream input{"\"foo\\\\bar\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 7);
  EXPECT_EQ(document.to_string(), "foo\\bar");
}

TEST(JSON_parse, string_escaped_solidus) {
  std::istringstream input{"\"foo\\/bar\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 7);
  EXPECT_EQ(document.to_string(), "foo/bar");
}

TEST(JSON_parse, string_escaped_backspace) {
  std::istringstream input{"\"foo\\bbar\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 7);
  EXPECT_EQ(document.to_string(), "foo\bbar");
}

TEST(JSON_parse, string_escaped_form_feed) {
  std::istringstream input{"\"foo\\fbar\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 7);
  EXPECT_EQ(document.to_string(), "foo\fbar");
}

TEST(JSON_parse, string_escaped_line_feed) {
  std::istringstream input{"\"foo\\nbar\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 7);
  EXPECT_EQ(document.to_string(), "foo\nbar");
}

TEST(JSON_parse, string_escaped_carriage_return) {
  std::istringstream input{"\"foo\\rbar\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 7);
  EXPECT_EQ(document.to_string(), "foo\rbar");
}

TEST(JSON_parse, string_escaped_tab) {
  std::istringstream input{"\"foo\\tbar\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 7);
  EXPECT_EQ(document.to_string(), "foo\tbar");
}

TEST(JSON_parse, string_unicode_code_points) {
  std::istringstream input{"\"\\u002F\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.to_string(), "\u002F");
}

TEST(JSON_parse, string_unicode_length_surrogates) {
  // See https://en.wikipedia.org/wiki/UTF-8#Surrogates
  // https://unicodeplus.com/U+D83D
  // https://unicodeplus.com/U+DCA9
  std::istringstream input{"\"\\uD83D\\uDCA9\""};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.byte_size(), 3);
}

TEST(JSON_parse, string_unicode_code_point_equality) {
  std::istringstream input_1{"\"\\u002F\""};
  std::istringstream input_2{"\"\\u002f\""};
  std::istringstream input_3{"\"\\/\""};
  std::istringstream input_4{"\"/\""};

  const sourcemeta::core::JSON document_1 =
      sourcemeta::core::parse_json(input_1);
  const sourcemeta::core::JSON document_2 =
      sourcemeta::core::parse_json(input_2);
  const sourcemeta::core::JSON document_3 =
      sourcemeta::core::parse_json(input_3);
  const sourcemeta::core::JSON document_4 =
      sourcemeta::core::parse_json(input_4);

  EXPECT_EQ(document_1.to_string(), document_2.to_string());
  EXPECT_EQ(document_2.to_string(), document_3.to_string());
  EXPECT_EQ(document_3.to_string(), document_4.to_string());
}

TEST(JSON_parse, string_equality) {
  std::istringstream input_1{"\"foo\""};
  std::istringstream input_2{"   \"foo\"    "};
  std::istringstream input_3{"\"fo\""};
  const sourcemeta::core::JSON document_1 =
      sourcemeta::core::parse_json(input_1);
  const sourcemeta::core::JSON document_2 =
      sourcemeta::core::parse_json(input_2);
  const sourcemeta::core::JSON document_3 =
      sourcemeta::core::parse_json(input_3);
  EXPECT_EQ(document_1, document_2);
  EXPECT_FALSE(document_1 == document_3);
  EXPECT_FALSE(document_2 == document_3);
}

// https://datatracker.ietf.org/doc/html/rfc8259#section-13
TEST(JSON_parse, rfc8259_example_1) {
  std::istringstream input{
      "{\n"
      "\"Image\": {\n"
      "\"Width\":  800,\n"
      "\"Height\": 600,\n"
      "\"Title\":  \"View from 15th Floor\",\n"
      "\"Thumbnail\": {\n"
      "\"Url\":    \"http://www.example.com/image/481989943\",\n"
      "\"Height\": 125,\n"
      "\"Width\":  100\n"
      "},\n"
      "\"Animated\" : false,\n"
      "\"IDs\": [116, 943, 234, 38793]\n"
      "}\n"
      "}"};

  const sourcemeta::core::JSON value = sourcemeta::core::parse_json(input);

  // Top level object
  EXPECT_TRUE(value.is_object());
  EXPECT_EQ(value.size(), 1);
  EXPECT_TRUE(value.defines("Image"));

  // Image object
  EXPECT_TRUE(value.at("Image").is_object());
  EXPECT_EQ(value.at("Image").size(), 6);
  EXPECT_TRUE(value.at("Image").defines("Width"));
  EXPECT_TRUE(value.at("Image").defines("Height"));
  EXPECT_TRUE(value.at("Image").defines("Title"));
  EXPECT_TRUE(value.at("Image").defines("Thumbnail"));
  EXPECT_TRUE(value.at("Image").defines("Animated"));
  EXPECT_TRUE(value.at("Image").defines("IDs"));
  EXPECT_TRUE(value.at("Image").at("Width").is_integer());
  EXPECT_TRUE(value.at("Image").at("Height").is_integer());
  EXPECT_TRUE(value.at("Image").at("Title").is_string());
  EXPECT_TRUE(value.at("Image").at("Thumbnail").is_object());
  EXPECT_TRUE(value.at("Image").at("Animated").is_boolean());
  EXPECT_TRUE(value.at("Image").at("IDs").is_array());
  EXPECT_EQ(value.at("Image").at("Width").to_integer(), 800);
  EXPECT_EQ(value.at("Image").at("Height").to_integer(), 600);
  EXPECT_EQ(value.at("Image").at("Title").to_string(), "View from 15th Floor");
  EXPECT_FALSE(value.at("Image").at("Animated").to_boolean());

  // Image.Thumbnail object
  EXPECT_EQ(value.at("Image").at("Thumbnail").size(), 3);
  EXPECT_TRUE(value.at("Image").at("Thumbnail").defines("Url"));
  EXPECT_TRUE(value.at("Image").at("Thumbnail").defines("Height"));
  EXPECT_TRUE(value.at("Image").at("Thumbnail").defines("Width"));
  EXPECT_TRUE(value.at("Image").at("Thumbnail").at("Url").is_string());
  EXPECT_TRUE(value.at("Image").at("Thumbnail").at("Height").is_integer());
  EXPECT_TRUE(value.at("Image").at("Thumbnail").at("Width").is_integer());
  EXPECT_EQ(value.at("Image").at("Thumbnail").at("Url").to_string(),
            "http://www.example.com/image/481989943");
  EXPECT_EQ(value.at("Image").at("Thumbnail").at("Height").to_integer(), 125);
  EXPECT_EQ(value.at("Image").at("Thumbnail").at("Width").to_integer(), 100);

  // Image.IDs array
  EXPECT_EQ(value.at("Image").at("IDs").size(), 4);
  EXPECT_TRUE(value.at("Image").at("IDs").at(0).is_integer());
  EXPECT_TRUE(value.at("Image").at("IDs").at(1).is_integer());
  EXPECT_TRUE(value.at("Image").at("IDs").at(2).is_integer());
  EXPECT_TRUE(value.at("Image").at("IDs").at(3).is_integer());
  EXPECT_EQ(value.at("Image").at("IDs").at(0).to_integer(), 116);
  EXPECT_EQ(value.at("Image").at("IDs").at(1).to_integer(), 943);
  EXPECT_EQ(value.at("Image").at("IDs").at(2).to_integer(), 234);
  EXPECT_EQ(value.at("Image").at("IDs").at(3).to_integer(), 38793);
}

// https://datatracker.ietf.org/doc/html/rfc8259#section-13
TEST(JSON_parse, rfc8259_example_2) {
  std::istringstream input{"[\n"
                           "{\n"
                           "\"precision\": \"zip\",\n"
                           "\"Latitude\":  37.7668,\n"
                           "\"Longitude\": -122.3959,\n"
                           "\"Address\":   \"\",\n"
                           "\"City\":      \"SAN FRANCISCO\",\n"
                           "\"State\":     \"CA\",\n"
                           "\"Zip\":       \"94107\",\n"
                           "\"Country\":   \"US\"\n"
                           "},\n"
                           "{\n"
                           "\"precision\": \"zip\",\n"
                           "\"Latitude\":  37.371991,\n"
                           "\"Longitude\": -122.026020,\n"
                           "\"Address\":   \"\",\n"
                           "\"City\":      \"SUNNYVALE\",\n"
                           "\"State\":     \"CA\",\n"
                           "\"Zip\":       \"94085\",\n"
                           "\"Country\":   \"US\"\n"
                           "}\n"
                           "]"};

  const sourcemeta::core::JSON value = sourcemeta::core::parse_json(input);

  // Type and size
  EXPECT_TRUE(value.is_array());
  EXPECT_EQ(value.size(), 2);
  EXPECT_EQ(value.size(), 2);

  // Type and size
  EXPECT_TRUE(value.at(0).is_object());
  EXPECT_EQ(value.at(0).size(), 8);
  EXPECT_EQ(value.at(0).size(), 8);

  // Member keys
  EXPECT_TRUE(value.at(0).defines("precision"));
  EXPECT_TRUE(value.at(0).defines("Latitude"));
  EXPECT_TRUE(value.at(0).defines("Longitude"));
  EXPECT_TRUE(value.at(0).defines("Address"));
  EXPECT_TRUE(value.at(0).defines("City"));
  EXPECT_TRUE(value.at(0).defines("State"));
  EXPECT_TRUE(value.at(0).defines("Zip"));
  EXPECT_TRUE(value.at(0).defines("Country"));

  // Member types
  EXPECT_TRUE(value.at(0).at("precision").is_string());
  EXPECT_TRUE(value.at(0).at("Latitude").is_real());
  EXPECT_TRUE(value.at(0).at("Longitude").is_real());
  EXPECT_TRUE(value.at(0).at("Address").is_string());
  EXPECT_TRUE(value.at(0).at("City").is_string());
  EXPECT_TRUE(value.at(0).at("State").is_string());
  EXPECT_TRUE(value.at(0).at("Zip").is_string());
  EXPECT_TRUE(value.at(0).at("Country").is_string());

  // Member values
  EXPECT_EQ(value.at(0).at("precision").to_string(), "zip");
  EXPECT_EQ(value.at(0).at("Latitude").to_real(), 37.7668);
  EXPECT_EQ(value.at(0).at("Longitude").to_real(), -122.3959);
  EXPECT_EQ(value.at(0).at("Address").to_string(), "");
  EXPECT_EQ(value.at(0).at("City").to_string(), "SAN FRANCISCO");
  EXPECT_EQ(value.at(0).at("State").to_string(), "CA");
  EXPECT_EQ(value.at(0).at("Zip").to_string(), "94107");
  EXPECT_EQ(value.at(0).at("Country").to_string(), "US");

  // Type and size
  EXPECT_TRUE(value.at(1).is_object());
  EXPECT_EQ(value.at(1).size(), 8);

  // Member keys
  EXPECT_TRUE(value.at(1).defines("precision"));
  EXPECT_TRUE(value.at(1).defines("Latitude"));
  EXPECT_TRUE(value.at(1).defines("Longitude"));
  EXPECT_TRUE(value.at(1).defines("Address"));
  EXPECT_TRUE(value.at(1).defines("City"));
  EXPECT_TRUE(value.at(1).defines("State"));
  EXPECT_TRUE(value.at(1).defines("Zip"));
  EXPECT_TRUE(value.at(1).defines("Country"));

  // Member types
  EXPECT_TRUE(value.at(1).at("precision").is_string());
  EXPECT_TRUE(value.at(1).at("Latitude").is_real());
  EXPECT_TRUE(value.at(1).at("Longitude").is_real());
  EXPECT_TRUE(value.at(1).at("Address").is_string());
  EXPECT_TRUE(value.at(1).at("City").is_string());
  EXPECT_TRUE(value.at(1).at("State").is_string());
  EXPECT_TRUE(value.at(1).at("Zip").is_string());
  EXPECT_TRUE(value.at(1).at("Country").is_string());

  // Member values
  EXPECT_EQ(value.at(1).at("precision").to_string(), "zip");
  EXPECT_EQ(value.at(1).at("Latitude").to_real(), 37.371991);
  EXPECT_EQ(value.at(1).at("Longitude").to_real(), -122.026020);
  EXPECT_EQ(value.at(1).at("Address").to_string(), "");
  EXPECT_EQ(value.at(1).at("City").to_string(), "SUNNYVALE");
  EXPECT_EQ(value.at(1).at("State").to_string(), "CA");
  EXPECT_EQ(value.at(1).at("Zip").to_string(), "94085");
  EXPECT_EQ(value.at(1).at("Country").to_string(), "US");
}

TEST(JSON_parse, custom_line_column_from_string_stream) {
  std::istringstream input{"[\n  false,\n  true\n]"};
  std::uint64_t line{5};
  std::uint64_t column{2};
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(input, line, column);

  // Line & column get mutated
  EXPECT_EQ(line, 8);
  EXPECT_EQ(column, 1);

  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 2);
  EXPECT_TRUE(document.at(0).is_boolean());
  EXPECT_FALSE(document.at(0).to_boolean());
  EXPECT_TRUE(document.at(1).is_boolean());
  EXPECT_TRUE(document.at(1).to_boolean());
}

TEST(JSON_parse, read_json) {
  const sourcemeta::core::JSON document{sourcemeta::core::read_json(
      std::filesystem::path{TEST_DIRECTORY} / "stub_valid.json")};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_integer());
  EXPECT_EQ(document.at("foo").to_integer(), 1);
}

TEST(JSON_parse, read_file) {
  auto stream{sourcemeta::core::read_file(
      std::filesystem::path{TEST_DIRECTORY} / "stub_valid.json")};
  const auto document{sourcemeta::core::parse_json(stream)};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_integer());
  EXPECT_EQ(document.at("foo").to_integer(), 1);
}

TEST(JSON_parse, big_integer_beyond_64_bit) {
  std::istringstream input{"9223372036854776000"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "9223372036854776000");
}

TEST(JSON_parse, big_negative_integer_beyond_64_bit) {
  std::istringstream input{"-9223372036854776000"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "-9223372036854776000");
}

TEST(JSON_parse, very_large_integer) {
  std::istringstream input{"123456789012345678901234567890"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(),
            "123456789012345678901234567890");
}

TEST(JSON_parse, big_integer_in_array) {
  std::istringstream input{"[1, 9223372036854776000, 3]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_TRUE(document.at(0).is_integer());
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_TRUE(document.at(1).is_decimal());
  EXPECT_EQ(document.at(1).to_decimal().to_string(), "9223372036854776000");
  EXPECT_TRUE(document.at(2).is_integer());
  EXPECT_EQ(document.at(2).to_integer(), 3);
}

TEST(JSON_parse, big_integer_in_object) {
  std::istringstream input{"{\"big\":9223372036854776000,\"small\":42}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_TRUE(document.at("big").is_decimal());
  EXPECT_EQ(document.at("big").to_decimal().to_string(), "9223372036854776000");
  EXPECT_TRUE(document.at("small").is_integer());
  EXPECT_EQ(document.at("small").to_integer(), 42);
}

TEST(JSON_parse, big_real_number) {
  std::istringstream input{"1.234567890123456789012345678901234567890"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 1.234567890123456789012345678901234567890);
}

TEST(JSON_parse, very_small_real_number) {
  std::istringstream input{"0.000000000000000000000000000001"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 0.000000000000000000000000000001);
}

TEST(JSON_parse, big_real_in_array) {
  std::istringstream input{
      "[1.5, 1.234567890123456789012345678901234567890, 2.5]"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_TRUE(document.at(0).is_real());
  EXPECT_EQ(document.at(0).to_real(), 1.5);
  EXPECT_TRUE(document.at(1).is_real());
  EXPECT_EQ(document.at(1).to_real(),
            1.234567890123456789012345678901234567890);
  EXPECT_TRUE(document.at(2).is_real());
  EXPECT_EQ(document.at(2).to_real(), 2.5);
}

TEST(JSON_parse, big_real_in_object) {
  std::istringstream input{
      "{\"pi\":3.14159265358979323846264338327950288419716939937510}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.at("pi").is_real());
  EXPECT_EQ(document.at("pi").to_real(),
            3.14159265358979323846264338327950288419716939937510);
}

TEST(JSON_parse, big_number_with_exponent) {
  std::istringstream input{"1.5e10"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_DOUBLE_EQ(document.to_real(), 1.5e10);
}

TEST(JSON_parse, big_number_with_negative_exponent) {
  std::istringstream input{"1.5e-10"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_real());
  EXPECT_DOUBLE_EQ(document.to_real(), 1.5e-10);
}

TEST(JSON_parse, read_json_stub_bigint) {
  const sourcemeta::core::JSON document{sourcemeta::core::read_json(
      std::filesystem::path{TEST_DIRECTORY} / "stub_bigint.json")};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "9223372036854776000");
}

TEST(JSON_parse, nested_big_integers_and_reals) {
  std::istringstream input{
      "{\"data\":[{\"big_int\":9223372036854776000,"
      "\"big_real\":3.14159265358979323846264338327950288419716939937510}]}"};
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  EXPECT_TRUE(document.is_object());
  EXPECT_TRUE(document.at("data").is_array());
  EXPECT_EQ(document.at("data").size(), 1);
  EXPECT_TRUE(document.at("data").at(0).is_object());
  EXPECT_EQ(document.at("data").at(0).size(), 2);
  EXPECT_TRUE(document.at("data").at(0).at("big_int").is_decimal());
  EXPECT_EQ(document.at("data").at(0).at("big_int").to_decimal().to_string(),
            "9223372036854776000");
  EXPECT_TRUE(document.at("data").at(0).at("big_real").is_real());
  EXPECT_EQ(document.at("data").at(0).at("big_real").to_real(),
            3.14159265358979323846264338327950288419716939937510);
}

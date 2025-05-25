#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <type_traits>

TEST(JSON_object, general_traits) {
  EXPECT_TRUE(
      std::is_default_constructible<sourcemeta::core::JSON::Object>::value);
  EXPECT_FALSE(std::is_nothrow_default_constructible<
               sourcemeta::core::JSON::Object>::value);
  EXPECT_TRUE(std::is_destructible<sourcemeta::core::JSON::Object>::value);
  EXPECT_TRUE(
      std::is_nothrow_destructible<sourcemeta::core::JSON::Object>::value);
}

TEST(JSON_object, copy_traits) {
  EXPECT_TRUE(std::is_copy_assignable<sourcemeta::core::JSON::Object>::value);
  EXPECT_TRUE(
      std::is_copy_constructible<sourcemeta::core::JSON::Object>::value);
  EXPECT_FALSE(
      std::is_nothrow_copy_assignable<sourcemeta::core::JSON::Object>::value);
  EXPECT_FALSE(std::is_nothrow_copy_constructible<
               sourcemeta::core::JSON::Object>::value);
}

TEST(JSON_object, move_traits) {
  EXPECT_TRUE(std::is_move_assignable<sourcemeta::core::JSON::Object>::value);
  EXPECT_TRUE(
      std::is_move_constructible<sourcemeta::core::JSON::Object>::value);
  EXPECT_TRUE(
      std::is_nothrow_move_assignable<sourcemeta::core::JSON::Object>::value);
}

TEST(JSON_object, type) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                        {"bar", sourcemeta::core::JSON{true}}};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Object);
}

TEST(JSON_object, initializer_list_2_booleans) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                        {"bar", sourcemeta::core::JSON{true}}};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.at("foo").is_boolean());
  EXPECT_TRUE(document.at("bar").is_boolean());
  EXPECT_FALSE(document.at("foo").to_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());
}

TEST(JSON_object, empty_with_copy_constructor) {
  const sourcemeta::core::JSON document{sourcemeta::core::JSON::Object{}};

  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.object_size(), 0);
}

TEST(JSON_object, empty_with_make_object) {
  const sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.object_size(), 0);
}

TEST(JSON_object, assign_booleans) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  EXPECT_TRUE(document.is_object());
  document.assign("foo", sourcemeta::core::JSON{false});
  document.assign("bar", sourcemeta::core::JSON{true});
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.at("foo").is_boolean());
  EXPECT_TRUE(document.at("bar").is_boolean());
  EXPECT_FALSE(document.at("foo").to_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());
}

TEST(JSON_object, must_delete_one_existent_key) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                  {"bar", sourcemeta::core::JSON{true}}};

  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));

  document.erase("foo");

  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_FALSE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
}

TEST(JSON_object, must_delete_one_non_existent_key) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                  {"bar", sourcemeta::core::JSON{true}}};

  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));

  document.erase("xxx");

  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
}

TEST(JSON_object, modify_after_copy) {
  // Original document
  sourcemeta::core::JSON document{{"x", sourcemeta::core::JSON{1}},
                                  {"y", sourcemeta::core::JSON{2}}};
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("x"));
  EXPECT_TRUE(document.defines("y"));
  EXPECT_EQ(document.at("x").to_integer(), 1);
  EXPECT_EQ(document.at("y").to_integer(), 2);

  // Make copy
  sourcemeta::core::JSON copy = document;
  EXPECT_EQ(copy.size(), 2);
  EXPECT_EQ(copy.object_size(), 2);
  EXPECT_TRUE(copy.defines("x"));
  EXPECT_TRUE(copy.defines("y"));
  EXPECT_EQ(copy.at("x").to_integer(), 1);
  EXPECT_EQ(copy.at("y").to_integer(), 2);

  // Modify copy
  copy.erase("x");
  EXPECT_EQ(copy.size(), 1);
  EXPECT_EQ(copy.object_size(), 1);
  EXPECT_TRUE(copy.defines("y"));
  EXPECT_EQ(copy.at("y").to_integer(), 2);

  // Original document must remain intact
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("x"));
  EXPECT_TRUE(document.defines("y"));
  EXPECT_EQ(document.at("x").to_integer(), 1);
  EXPECT_EQ(document.at("y").to_integer(), 2);
}

TEST(JSON_object, nested_object_clear) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}}};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  document.clear();
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.object_size(), 0);
}

TEST(JSON_object, const_all_of_false) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                        {"bar", sourcemeta::core::JSON{"2"}}};
  const bool result =
      std::all_of(document.as_object().cbegin(), document.as_object().cend(),
                  [](const auto &pair) { return pair.second.is_integer(); });
  EXPECT_FALSE(result);
}

TEST(JSON_object, const_all_of_true) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                        {"bar", sourcemeta::core::JSON{2}}};
  const bool result =
      std::all_of(document.as_object().cbegin(), document.as_object().cend(),
                  [](const auto &pair) { return pair.second.is_integer(); });
  EXPECT_TRUE(result);
}

TEST(JSON_object, assign_literal_lvalue_string) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}}};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("foo"));

  const sourcemeta::core::JSON value{"baz"};
  document.assign("bar", value);
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));

  EXPECT_EQ(document.at("foo").to_integer(), 1);
  EXPECT_EQ(document.at("bar").to_string(), "baz");
}

TEST(JSON_object, assign_literal_rvalue_string) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}}};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("foo"));

  document.assign("bar", sourcemeta::core::JSON{"baz"});
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));

  EXPECT_EQ(document.at("foo").to_integer(), 1);
  EXPECT_EQ(document.at("bar").to_string(), "baz");
}

TEST(JSON_object, key_copy_assignment_same_type) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{"bar"}}};
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_string());
  EXPECT_EQ(document.at("foo").to_string(), "bar");

  const sourcemeta::core::JSON value{"baz"};
  document.assign("foo", value);

  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_string());
  EXPECT_EQ(document.at("foo").to_string(), "baz");
}

TEST(JSON_object, key_move_assignment_same_type) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{"bar"}}};
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_string());
  EXPECT_EQ(document.at("foo").to_string(), "bar");

  sourcemeta::core::JSON value{"baz"};
  document.assign("foo", std::move(value));

  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_string());
  EXPECT_EQ(document.at("foo").to_string(), "baz");
}

TEST(JSON_object, key_copy_assignment_different_type) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{"bar"}}};
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_string());
  EXPECT_EQ(document.at("foo").to_string(), "bar");

  const sourcemeta::core::JSON value{1};
  document.assign("foo", value);

  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_integer());
  EXPECT_EQ(document.at("foo").to_integer(), 1);
}

TEST(JSON_object, key_move_assignment_different_type) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{"bar"}}};
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_string());
  EXPECT_EQ(document.at("foo").to_string(), "bar");

  sourcemeta::core::JSON value{1};
  document.assign("foo", std::move(value));

  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_integer());
  EXPECT_EQ(document.at("foo").to_integer(), 1);
}

TEST(JSON_object, all_of_true) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1, \"bar\": 2 }");
  const bool result =
      std::all_of(document.as_object().begin(), document.as_object().end(),
                  [](auto &pair) { return pair.second.is_integer(); });
  EXPECT_TRUE(result);
}

TEST(JSON_object, all_of_with_key_true) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1, \"bar\": 2 }");
  const bool result =
      std::all_of(document.as_object().begin(), document.as_object().end(),
                  [](auto &pair) { return pair.first.size() > 2; });
  EXPECT_TRUE(result);
}

TEST(JSON_object, all_of_with_key_false) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"f\": 1, \"bar\": 2 }");
  const bool result =
      std::all_of(document.as_object().begin(), document.as_object().end(),
                  [](auto &pair) { return pair.first.size() > 2; });
  EXPECT_FALSE(result);
}

TEST(JSON, const_object_iterator) {
  std::map<std::string, std::int64_t> result;
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1, \"bar\": 2 }");
  for (const auto &pair : document.as_object()) {
    result.insert({pair.first, pair.second.to_integer()});
  }

  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("foo"), 1);
  EXPECT_EQ(result.at("bar"), 2);
}

TEST(JSON_object, iterator) {
  std::map<std::string, std::int64_t> result;
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1, \"bar\": 2 }");
  for (auto &pair : document.as_object()) {
    result.insert({pair.first, pair.second.to_integer()});
  }

  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("foo"), 1);
  EXPECT_EQ(result.at("bar"), 2);
}

TEST(JSON_object, into_object) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into_object();
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.object_size(), 0);
}

TEST(JSON_object, erase_with_initializer_list) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":true,\"bar\":false,\"baz\":true}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.object_size(), 3);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.defines("baz"));
  document.erase_keys({"foo", "baz"});
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("bar"));
}

TEST(JSON_object, erase_with_vector_iterators) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":true,\"bar\":false,\"baz\":true}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.object_size(), 3);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.defines("baz"));
  const std::vector<std::string> keys{"foo", "baz"};
  document.erase_keys(keys.cbegin(), keys.cend());
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("bar"));
}

TEST(JSON_object, erase_non_existent) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":true,\"bar\":false,\"baz\":true}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.object_size(), 3);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.defines("baz"));
  document.erase_keys({"foo", "qux"});
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_FALSE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.defines("baz"));
}

TEST(JSON_object, assign_move_empty_object) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  EXPECT_TRUE(document.empty());
  document.assign("foo", sourcemeta::core::JSON{true});
  EXPECT_TRUE(document.defines("foo"));
}

TEST(JSON_object, long_key_assign) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.object_size(), 0);
  const std::string key(30, 'x');
  EXPECT_EQ(key.size(), 30);
  document.assign(key, sourcemeta::core::JSON{true});
  EXPECT_TRUE(document.defines(key));
  const auto &value{document.at(key)};
  EXPECT_TRUE(value.is_boolean());
  EXPECT_TRUE(value.to_boolean());
}

TEST(JSON_object, clear_except_one_key_initializer_list) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":true,\"bar\":false}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  document.clear_except({"bar"});
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_FALSE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
}

TEST(JSON_object, clear_except_extra_key_initializer_list) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":true,\"bar\":false}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  document.clear_except({"qux"});
  EXPECT_TRUE(document.empty());
}

TEST(JSON_object, clear_except_multiple_intersection_initializer_list) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2,\"baz\":3,\"qux\":4}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 4);
  EXPECT_EQ(document.object_size(), 4);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.defines("baz"));
  EXPECT_TRUE(document.defines("qux"));
  document.clear_except({"foo", "baz", "xxx", "yyy"});
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_FALSE(document.defines("bar"));
  EXPECT_TRUE(document.defines("baz"));
  EXPECT_FALSE(document.defines("qux"));
}

TEST(JSON_object, at_index_using_integer) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"0\":1,\"1\":2,\"2\":3}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.object_size(), 3);
  EXPECT_TRUE(document.defines("0"));
  EXPECT_TRUE(document.defines("1"));
  EXPECT_TRUE(document.defines("2"));
  EXPECT_TRUE(document.at("0").is_integer());
  EXPECT_TRUE(document.at("1").is_integer());
  EXPECT_TRUE(document.at("2").is_integer());
  EXPECT_EQ(document.at("0").to_integer(), 1);
  EXPECT_EQ(document.at("1").to_integer(), 2);
  EXPECT_EQ(document.at("2").to_integer(), 3);
}

TEST(JSON_object, at_index_using_integer_non_const) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"0\":1,\"1\":2,\"2\":3}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.object_size(), 3);
  EXPECT_TRUE(document.defines("0"));
  EXPECT_TRUE(document.defines("1"));
  EXPECT_TRUE(document.defines("2"));
  EXPECT_TRUE(document.at("0").is_integer());
  EXPECT_TRUE(document.at("1").is_integer());
  EXPECT_TRUE(document.at("2").is_integer());
  EXPECT_EQ(document.at("0").to_integer(), 1);
  EXPECT_EQ(document.at("1").to_integer(), 2);
  EXPECT_EQ(document.at("2").to_integer(), 3);
}

TEST(JSON_object, defines_property_using_integer) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"0\":1,\"1\":2,\"2\":3}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.object_size(), 3);
  EXPECT_TRUE(document.defines(0));
  EXPECT_TRUE(document.defines(1));
  EXPECT_TRUE(document.defines(2));
}

TEST(JSON_object, assign_if_missing_not_missing_lvalue) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                  {"bar", sourcemeta::core::JSON{true}}};

  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_FALSE(document.at("foo").to_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());

  const sourcemeta::core::JSON new_value{1};
  document.assign_if_missing("foo", new_value);

  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_FALSE(document.at("foo").to_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());
}

TEST(JSON_object, assign_if_missing_missing_lvalue) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                  {"bar", sourcemeta::core::JSON{true}}};

  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_FALSE(document.at("foo").to_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());

  const sourcemeta::core::JSON new_value{1};
  document.assign_if_missing("baz", new_value);

  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.object_size(), 3);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.defines("baz"));
  EXPECT_FALSE(document.at("foo").to_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());
  EXPECT_EQ(document.at("baz").to_integer(), 1);
}

TEST(JSON_object, assign_if_missing_not_missing_rvalue) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                  {"bar", sourcemeta::core::JSON{true}}};

  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_FALSE(document.at("foo").to_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());

  document.assign_if_missing("foo", sourcemeta::core::JSON{1});

  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_FALSE(document.at("foo").to_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());
}

TEST(JSON_object, assign_if_missing_missing_rvalue) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                  {"bar", sourcemeta::core::JSON{true}}};

  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_FALSE(document.at("foo").to_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());

  document.assign_if_missing("baz", sourcemeta::core::JSON{1});

  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.object_size(), 3);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.defines("baz"));
  EXPECT_FALSE(document.at("foo").to_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());
  EXPECT_EQ(document.at("baz").to_integer(), 1);
}

TEST(JSON_object, estimated_byte_size_integers) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1, \"bar\": 2 }");
  EXPECT_EQ(document.estimated_byte_size(), 22);
}

TEST(JSON_object, estimated_byte_size_nested) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(
      "{ \"foo\": 1, \"bar\": { \"bar\": true } }");
  EXPECT_EQ(document.estimated_byte_size(), 18);
}

TEST(JSON_object, fast_hash_integers) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1, \"bar\": 2 }");
  EXPECT_EQ(document.fast_hash(), 26);
}

TEST(JSON_object, fast_hash_nested) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(
      "{ \"foo\": 1, \"bar\": { \"bar\": true } }");
  EXPECT_EQ(document.fast_hash(), 32);
}

TEST(JSON_object, find) {
  const auto document{sourcemeta::core::parse_json("{\"foo\":5}")};
  EXPECT_NE(document.as_object().find("foo"), document.as_object().cend());
  EXPECT_EQ(document.as_object().find("foo")->first, "foo");
  EXPECT_TRUE(document.as_object().find("foo")->second.is_integer());
  EXPECT_EQ(document.as_object().find("foo")->second.to_integer(), 5);
  EXPECT_EQ(document.as_object().find("bar"), document.as_object().cend());
}

TEST(JSON_object, at_hash_const) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                        {"bar", sourcemeta::core::JSON{true}}};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);

  const auto hash_foo{document.as_object().hash("foo")};
  const auto hash_bar{document.as_object().hash("bar")};

  EXPECT_TRUE(document.at("foo", hash_foo).is_boolean());
  EXPECT_TRUE(document.at("bar", hash_bar).is_boolean());
  EXPECT_FALSE(document.at("foo", hash_foo).to_boolean());
  EXPECT_TRUE(document.at("bar", hash_bar).to_boolean());
}

TEST(JSON_object, at_hash_non_const) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                  {"bar", sourcemeta::core::JSON{true}}};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);

  const auto hash_foo{document.as_object().hash("foo")};
  const auto hash_bar{document.as_object().hash("bar")};

  EXPECT_TRUE(document.at("foo", hash_foo).is_boolean());
  EXPECT_TRUE(document.at("bar", hash_bar).is_boolean());
  EXPECT_FALSE(document.at("foo", hash_foo).to_boolean());
  EXPECT_TRUE(document.at("bar", hash_bar).to_boolean());
}

TEST(JSON_object, defines_hash_const) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                        {"bar", sourcemeta::core::JSON{true}}};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);

  EXPECT_TRUE(document.defines("foo", document.as_object().hash("foo")));
  EXPECT_TRUE(document.defines("bar", document.as_object().hash("bar")));
  EXPECT_FALSE(document.defines("baz", document.as_object().hash("baz")));
}

TEST(JSON_object, iterator_hash) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                        {"bar", sourcemeta::core::JSON{true}}};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);

  for (const auto &entry : document.as_object()) {
    EXPECT_EQ(document.at(entry.first, entry.hash), entry.second);
  }
}

TEST(JSON_object, as_object_defines) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{"bar"}}};
  for (const auto &entry : document.as_object()) {
    EXPECT_TRUE(document.as_object().defines(entry.first, entry.hash));
  }
}

TEST(JSON_object, as_object_size) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{"bar"}}};
  EXPECT_EQ(document.as_object().size(), 1);
}

TEST(JSON_object, at_index) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                        {"bar", sourcemeta::core::JSON{true}}};

  const auto &entry_1{document.as_object().at(0)};
  const auto &entry_2{document.as_object().at(1)};

  EXPECT_EQ(entry_1.first, "foo");
  EXPECT_FALSE(entry_1.second.to_boolean());
  EXPECT_EQ(entry_1.hash, document.as_object().hash("foo"));

  EXPECT_EQ(entry_2.first, "bar");
  EXPECT_TRUE(entry_2.second.to_boolean());
  EXPECT_EQ(entry_2.hash, document.as_object().hash("bar"));
}

TEST(JSON_object, rename_match) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}}};
  document.rename("foo", "bar");

  EXPECT_EQ(document.size(), 1);
  EXPECT_FALSE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.at("bar").is_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());
}

TEST(JSON_object, rename_no_match) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}}};
  document.rename("xxx", "bar");

  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_boolean());
  EXPECT_TRUE(document.at("foo").to_boolean());
}

TEST(JSON_object, rename_match_destination_exists) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}},
                                  {"bar", sourcemeta::core::JSON{1}}};
  document.rename("foo", "bar");

  EXPECT_EQ(document.size(), 1);
  EXPECT_FALSE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.at("bar").is_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());
}

TEST(JSON_object, merge_with_overlap) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}},
                                  {"bar", sourcemeta::core::JSON{1}}};

  const sourcemeta::core::JSON other{{"bar", sourcemeta::core::JSON{false}},
                                     {"baz", sourcemeta::core::JSON{9}}};

  document.merge(other.as_object());

  EXPECT_EQ(document.size(), 3);

  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.defines("baz"));

  EXPECT_TRUE(document.at("foo").is_boolean());
  EXPECT_TRUE(document.at("bar").is_boolean());
  EXPECT_TRUE(document.at("baz").is_integer());

  EXPECT_TRUE(document.at("foo").to_boolean());
  EXPECT_FALSE(document.at("bar").to_boolean());
  EXPECT_EQ(document.at("baz").to_integer(), 9);
}

TEST(JSON_object, at_or_defined) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}},
                                        {"bar", sourcemeta::core::JSON{1}}};
  const sourcemeta::core::JSON default_value{99};

  const auto &result{document.at_or("foo", default_value)};
  EXPECT_TRUE(result.is_boolean());
  EXPECT_TRUE(result.to_boolean());
}

TEST(JSON_object, at_or_defined_with_hash) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}},
                                        {"bar", sourcemeta::core::JSON{1}}};
  const sourcemeta::core::JSON default_value{99};

  const auto hash{document.as_object().hash("foo")};
  const auto &result{document.at_or("foo", hash, default_value)};
  EXPECT_TRUE(result.is_boolean());
  EXPECT_TRUE(result.to_boolean());
}

TEST(JSON_object, at_or_not_defined) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}},
                                        {"bar", sourcemeta::core::JSON{1}}};
  const sourcemeta::core::JSON default_value{99};

  const auto &result{document.at_or("baz", default_value)};
  EXPECT_TRUE(result.is_integer());
  EXPECT_EQ(result.to_integer(), 99);
}

TEST(JSON_object, at_or_not_defined_with_hash) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}},
                                        {"bar", sourcemeta::core::JSON{1}}};
  const sourcemeta::core::JSON default_value{99};

  const auto hash{document.as_object().hash("baz")};
  const auto &result{document.at_or("baz", hash, default_value)};
  EXPECT_TRUE(result.is_integer());
  EXPECT_EQ(result.to_integer(), 99);
}

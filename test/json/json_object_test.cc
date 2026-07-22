#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <algorithm>
#include <array>
#include <map>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

TEST(general_traits) {
  EXPECT_TRUE(
      std::is_default_constructible<sourcemeta::core::JSON::Object>::value);
  EXPECT_TRUE(std::is_nothrow_default_constructible<
              sourcemeta::core::JSON::Object>::value);
  EXPECT_TRUE(std::is_destructible<sourcemeta::core::JSON::Object>::value);
  EXPECT_TRUE(
      std::is_nothrow_destructible<sourcemeta::core::JSON::Object>::value);
}

TEST(copy_traits) {
  EXPECT_TRUE(std::is_copy_assignable<sourcemeta::core::JSON::Object>::value);
  EXPECT_TRUE(
      std::is_copy_constructible<sourcemeta::core::JSON::Object>::value);
  EXPECT_FALSE(
      std::is_nothrow_copy_assignable<sourcemeta::core::JSON::Object>::value);
  EXPECT_FALSE(std::is_nothrow_copy_constructible<
               sourcemeta::core::JSON::Object>::value);
}

TEST(move_traits) {
  EXPECT_TRUE(std::is_move_assignable<sourcemeta::core::JSON::Object>::value);
  EXPECT_TRUE(
      std::is_move_constructible<sourcemeta::core::JSON::Object>::value);
  EXPECT_TRUE(
      std::is_nothrow_move_assignable<sourcemeta::core::JSON::Object>::value);
}

TEST(type) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                        {"bar", sourcemeta::core::JSON{true}}};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Object);
}

TEST(initializer_list_2_booleans) {
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

TEST(empty_with_copy_constructor) {
  const sourcemeta::core::JSON document{sourcemeta::core::JSON::Object{}};

  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.object_size(), 0);
}

TEST(empty_with_make_object) {
  const sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.object_size(), 0);
}

TEST(assign_booleans) {
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

TEST(must_delete_one_existent_key) {
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

TEST(must_delete_one_non_existent_key) {
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

TEST(modify_after_copy) {
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

TEST(nested_object_clear) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}}};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  document.clear();
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.object_size(), 0);
}

TEST(const_all_of_false) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                        {"bar", sourcemeta::core::JSON{"2"}}};
  const bool result =
      std::all_of(document.as_object().cbegin(), document.as_object().cend(),
                  [](const auto &pair) { return pair.second.is_integer(); });
  EXPECT_FALSE(result);
}

TEST(const_all_of_true) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                        {"bar", sourcemeta::core::JSON{2}}};
  const bool result =
      std::all_of(document.as_object().cbegin(), document.as_object().cend(),
                  [](const auto &pair) { return pair.second.is_integer(); });
  EXPECT_TRUE(result);
}

TEST(assign_literal_lvalue_string) {
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

TEST(assign_literal_rvalue_string) {
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

TEST(key_copy_assignment_same_type) {
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

TEST(key_move_assignment_same_type) {
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

TEST(key_copy_assignment_different_type) {
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

TEST(key_move_assignment_different_type) {
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

TEST(all_of_true) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1, \"bar\": 2 }");
  const bool result =
      std::all_of(document.as_object().begin(), document.as_object().end(),
                  [](auto &pair) { return pair.second.is_integer(); });
  EXPECT_TRUE(result);
}

TEST(all_of_with_key_true) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1, \"bar\": 2 }");
  const bool result =
      std::all_of(document.as_object().begin(), document.as_object().end(),
                  [](auto &pair) { return pair.first.size() > 2; });
  EXPECT_TRUE(result);
}

TEST(all_of_with_key_false) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"f\": 1, \"bar\": 2 }");
  const bool result =
      std::all_of(document.as_object().begin(), document.as_object().end(),
                  [](auto &pair) { return pair.first.size() > 2; });
  EXPECT_FALSE(result);
}

TEST(const_object_iterator) {
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

TEST(iterator) {
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

TEST(into_object) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into_object();
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.object_size(), 0);
}

TEST(erase_with_initializer_list) {
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

TEST(erase_with_vector_iterators) {
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

TEST(erase_non_existent) {
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

TEST(assign_move_empty_object) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  EXPECT_TRUE(document.empty());
  document.assign("foo", sourcemeta::core::JSON{true});
  EXPECT_TRUE(document.defines("foo"));
}

TEST(long_key_assign) {
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

TEST(long_key_assign_rvalue) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.object_size(), 0);
  const std::string key(32, 'x');
  document.assign(key, sourcemeta::core::JSON{"value"});
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines(key));
  EXPECT_TRUE(document.at(key).is_string());
  EXPECT_EQ(document.at(key).to_string(), "value");
}

TEST(long_key_move_assignment_same_type) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  const std::string key(32, 'x');
  document.assign(key, sourcemeta::core::JSON{"before"});
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines(key));
  EXPECT_TRUE(document.at(key).is_string());
  EXPECT_EQ(document.at(key).to_string(), "before");

  sourcemeta::core::JSON value{"after"};
  document.assign(key, std::move(value));

  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines(key));
  EXPECT_TRUE(document.at(key).is_string());
  EXPECT_EQ(document.at(key).to_string(), "after");
}

TEST(long_key_move_assignment_hash_collision) {
  const std::string prefix(31, 'x');
  const std::string first_key{prefix + "ay"};
  const std::string second_key{prefix + "by"};
  EXPECT_EQ(sourcemeta::core::JSON::Object::hash(first_key),
            sourcemeta::core::JSON::Object::hash(second_key));

  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  document.assign(first_key, sourcemeta::core::JSON{1});
  document.assign(second_key, sourcemeta::core::JSON{2});

  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines(first_key));
  EXPECT_TRUE(document.defines(second_key));
  EXPECT_EQ(document.at(first_key).to_integer(), 1);
  EXPECT_EQ(document.at(second_key).to_integer(), 2);

  document.assign(second_key, sourcemeta::core::JSON{3});

  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_EQ(document.at(first_key).to_integer(), 1);
  EXPECT_EQ(document.at(second_key).to_integer(), 3);
}

TEST(clear_except_one_key_initializer_list) {
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

TEST(clear_except_extra_key_initializer_list) {
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

TEST(clear_except_multiple_intersection_initializer_list) {
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

TEST(at_index_using_integer) {
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

TEST(at_index_using_integer_non_const) {
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

TEST(defines_property_using_integer) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"0\":1,\"1\":2,\"2\":3}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.object_size(), 3);
  EXPECT_TRUE(document.defines(0));
  EXPECT_TRUE(document.defines(1));
  EXPECT_TRUE(document.defines(2));
}

TEST(assign_if_missing_not_missing_lvalue) {
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

TEST(assign_if_missing_missing_lvalue) {
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

TEST(assign_if_missing_not_missing_rvalue) {
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

TEST(assign_if_missing_missing_rvalue) {
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

TEST(assign_assume_new_rvalue) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();

  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);

  document.assign_assume_new("foo", sourcemeta::core::JSON{false});
  document.assign_assume_new("bar", sourcemeta::core::JSON{42});

  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_FALSE(document.at("foo").to_boolean());
  EXPECT_EQ(document.at("bar").to_integer(), 42);
}

TEST(assign_assume_new_rvalue_key) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();

  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);

  std::string key1{"foo"};
  std::string key2{"bar"};
  document.assign_assume_new(std::move(key1), sourcemeta::core::JSON{false});
  document.assign_assume_new(std::move(key2), sourcemeta::core::JSON{42});

  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_FALSE(document.at("foo").to_boolean());
  EXPECT_EQ(document.at("bar").to_integer(), 42);
}

TEST(assign_assume_new_multiple_types) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();

  document.assign_assume_new("string", sourcemeta::core::JSON{"hello"});
  document.assign_assume_new("integer", sourcemeta::core::JSON{123});
  document.assign_assume_new("boolean", sourcemeta::core::JSON{true});
  document.assign_assume_new("null", sourcemeta::core::JSON{nullptr});

  EXPECT_EQ(document.size(), 4);
  EXPECT_EQ(document.at("string").to_string(), "hello");
  EXPECT_EQ(document.at("integer").to_integer(), 123);
  EXPECT_TRUE(document.at("boolean").to_boolean());
  EXPECT_TRUE(document.at("null").is_null());
}

TEST(assign_assume_new_rvalue_key_with_hash) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();

  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);

  std::string key{"foo"};
  const auto key_hash{sourcemeta::core::JSON::Object::hash(key)};
  auto &inserted{document.assign_assume_new(
      std::move(key), sourcemeta::core::JSON::make_object(), key_hash)};

  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.object_size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(inserted.is_object());
  EXPECT_EQ(&inserted, &document.at("foo"));

  inserted.assign("bar", sourcemeta::core::JSON{42});

  EXPECT_EQ(document.at("foo").object_size(), 1);
  EXPECT_TRUE(document.at("foo").defines("bar"));
  EXPECT_EQ(document.at("foo").at("bar").to_integer(), 42);
}

TEST(estimated_byte_size_integers) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1, \"bar\": 2 }");
  EXPECT_EQ(document.estimated_byte_size(), 22);
}

TEST(estimated_byte_size_nested) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(
      "{ \"foo\": 1, \"bar\": { \"bar\": true } }");
  EXPECT_EQ(document.estimated_byte_size(), 18);
}

TEST(fast_hash_integers) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1, \"bar\": 2 }");
  EXPECT_EQ(document.fast_hash(), 26);
}

TEST(fast_hash_nested) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(
      "{ \"foo\": 1, \"bar\": { \"bar\": true } }");
  EXPECT_EQ(document.fast_hash(), 32);
}

TEST(find) {
  const auto document{sourcemeta::core::parse_json("{\"foo\":5}")};
  EXPECT_NE(document.as_object().find("foo"), document.as_object().cend());
  EXPECT_EQ(document.as_object().find("foo")->first, "foo");
  EXPECT_TRUE(document.as_object().find("foo")->second.is_integer());
  EXPECT_EQ(document.as_object().find("foo")->second.to_integer(), 5);
  EXPECT_EQ(document.as_object().find("bar"), document.as_object().cend());
}

TEST(at_hash_const) {
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

TEST(at_hash_non_const) {
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

TEST(defines_hash_const) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                        {"bar", sourcemeta::core::JSON{true}}};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);

  EXPECT_TRUE(document.defines("foo", document.as_object().hash("foo")));
  EXPECT_TRUE(document.defines("bar", document.as_object().hash("bar")));
  EXPECT_FALSE(document.defines("baz", document.as_object().hash("baz")));
}

TEST(iterator_hash) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{false}},
                                        {"bar", sourcemeta::core::JSON{true}}};
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.object_size(), 2);

  for (const auto &entry : document.as_object()) {
    EXPECT_EQ(document.at(entry.first, entry.hash), entry.second);
  }
}

TEST(as_object_defines) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{"bar"}}};
  for (const auto &entry : document.as_object()) {
    EXPECT_TRUE(document.as_object().defines(entry.first, entry.hash));
  }
}

TEST(as_object_size) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{"bar"}}};
  EXPECT_EQ(document.as_object().size(), 1);
}

TEST(at_index) {
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

TEST(rename_match) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}}};
  document.rename("foo", "bar");

  EXPECT_EQ(document.size(), 1);
  EXPECT_FALSE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.at("bar").is_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());
}

TEST(rename_no_match) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}}};
  document.rename("xxx", "bar");

  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_boolean());
  EXPECT_TRUE(document.at("foo").to_boolean());
}

TEST(rename_match_destination_exists) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}},
                                  {"bar", sourcemeta::core::JSON{1}}};
  document.rename("foo", "bar");

  EXPECT_EQ(document.size(), 1);
  EXPECT_FALSE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.at("bar").is_boolean());
  EXPECT_TRUE(document.at("bar").to_boolean());
}

TEST(merge_with_overlap) {
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

TEST(merge_deep_object) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": {
      "bar": {
        "baz": 1,
        "array": [ { "x": 1 } ]
      }
    },
    "x": true
  })JSON")};

  const auto other{sourcemeta::core::parse_json(R"JSON({
    "foo": {
      "bar": {
        "qux": 2,
        "array": [ { "y": 2 }, 2, 3 ]
      }
    },
    "x": {
      "y": 1
    }
  })JSON")};

  document.merge(other.as_object());

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "foo": {
      "bar": {
        "baz": 1,
        "qux": 2,
        "array": [ { "y": 2 }, 2, 3 ]
      }
    },
    "x": {
      "y": 1
    }
  })JSON")};

  EXPECT_EQ(document, expected);
}

TEST(merge_with_own_object) {
  auto document{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1, "bar": 2 })JSON")};
  document.merge(document.as_object());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.at("foo").to_integer(), 1);
  EXPECT_EQ(document.at("bar").to_integer(), 2);
}

TEST(copy_assignment_from_own_member) {
  auto document{
      sourcemeta::core::parse_json(R"JSON({ "foo": { "bar": 42 } })JSON")};
  document = document.at("foo");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_EQ(document.at("bar").to_integer(), 42);
}

TEST(ordering_is_asymmetric) {
  const auto left{
      sourcemeta::core::parse_json(R"JSON({ "a": 1, "b": 9 })JSON")};
  const auto right{
      sourcemeta::core::parse_json(R"JSON({ "a": 9, "b": 1 })JSON")};
  EXPECT_NE(left, right);
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
  EXPECT_NE(left < right, right < left);
}

TEST(ordering_with_distinct_keys) {
  const auto left{
      sourcemeta::core::parse_json(R"JSON({ "a": 1, "b": 1 })JSON")};
  const auto right{
      sourcemeta::core::parse_json(R"JSON({ "a": 1, "c": 1 })JSON")};
  EXPECT_NE(left, right);
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(ordering_key_difference_outranks_value_difference) {
  const auto left{
      sourcemeta::core::parse_json(R"JSON({ "a": 9, "b": 1 })JSON")};
  const auto right{
      sourcemeta::core::parse_json(R"JSON({ "a": 1, "c": 1 })JSON")};
  EXPECT_NE(left, right);
  EXPECT_FALSE(left < right);
  EXPECT_TRUE(right < left);
}

TEST(at_or_defined) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}},
                                        {"bar", sourcemeta::core::JSON{1}}};
  const sourcemeta::core::JSON default_value{99};

  const auto &result{document.at_or("foo", default_value)};
  EXPECT_TRUE(result.is_boolean());
  EXPECT_TRUE(result.to_boolean());
}

TEST(at_or_defined_with_hash) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}},
                                        {"bar", sourcemeta::core::JSON{1}}};
  const sourcemeta::core::JSON default_value{99};

  const auto hash{document.as_object().hash("foo")};
  const auto &result{document.at_or("foo", hash, default_value)};
  EXPECT_TRUE(result.is_boolean());
  EXPECT_TRUE(result.to_boolean());
}

TEST(at_or_not_defined) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}},
                                        {"bar", sourcemeta::core::JSON{1}}};
  const sourcemeta::core::JSON default_value{99};

  const auto &result{document.at_or("baz", default_value)};
  EXPECT_TRUE(result.is_integer());
  EXPECT_EQ(result.to_integer(), 99);
}

TEST(at_or_not_defined_with_hash) {
  const sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{true}},
                                        {"bar", sourcemeta::core::JSON{1}}};
  const sourcemeta::core::JSON default_value{99};

  const auto hash{document.as_object().hash("baz")};
  const auto &result{document.at_or("baz", hash, default_value)};
  EXPECT_TRUE(result.is_integer());
  EXPECT_EQ(result.to_integer(), 99);
}

TEST(try_assign_before_exists) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                  {"bar", sourcemeta::core::JSON{2}}};

  document.try_assign_before("baz", sourcemeta::core::JSON{99}, "bar");

  std::vector<sourcemeta::core::JSON::String> properties;
  for (const auto &entry : document.as_object()) {
    properties.emplace_back(entry.first);
  }

  EXPECT_EQ(properties.size(), 3);
  EXPECT_EQ(properties.at(0), "foo");
  EXPECT_EQ(properties.at(1), "baz");
  EXPECT_EQ(properties.at(2), "bar");

  EXPECT_EQ(document.at("foo").to_integer(), 1);
  EXPECT_EQ(document.at("bar").to_integer(), 2);
  EXPECT_EQ(document.at("baz").to_integer(), 99);
}

TEST(try_assign_before_not_exists) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                  {"bar", sourcemeta::core::JSON{2}}};

  document.try_assign_before("baz", sourcemeta::core::JSON{99}, "qux");

  std::vector<sourcemeta::core::JSON::String> properties;
  for (const auto &entry : document.as_object()) {
    properties.emplace_back(entry.first);
  }

  EXPECT_EQ(properties.size(), 3);
  EXPECT_EQ(properties.at(0), "foo");
  EXPECT_EQ(properties.at(1), "bar");
  EXPECT_EQ(properties.at(2), "baz");

  EXPECT_EQ(document.at("foo").to_integer(), 1);
  EXPECT_EQ(document.at("bar").to_integer(), 2);
  EXPECT_EQ(document.at("baz").to_integer(), 99);
}

TEST(try_assign_before_exists_front) {
  sourcemeta::core::JSON document{{"foo", sourcemeta::core::JSON{1}},
                                  {"bar", sourcemeta::core::JSON{2}}};

  document.try_assign_before("baz", sourcemeta::core::JSON{99}, "foo");

  std::vector<sourcemeta::core::JSON::String> properties;
  for (const auto &entry : document.as_object()) {
    properties.emplace_back(entry.first);
  }

  EXPECT_EQ(properties.size(), 3);
  EXPECT_EQ(properties.at(0), "baz");
  EXPECT_EQ(properties.at(1), "foo");
  EXPECT_EQ(properties.at(2), "bar");

  EXPECT_EQ(document.at("foo").to_integer(), 1);
  EXPECT_EQ(document.at("bar").to_integer(), 2);
  EXPECT_EQ(document.at("baz").to_integer(), 99);
}

TEST(try_assign_before_empty) {
  sourcemeta::core::JSON document{sourcemeta::core::JSON::make_object()};

  document.try_assign_before("baz", sourcemeta::core::JSON{99}, "foo");

  std::vector<sourcemeta::core::JSON::String> properties;
  for (const auto &entry : document.as_object()) {
    properties.emplace_back(entry.first);
  }

  EXPECT_EQ(properties.size(), 1);
  EXPECT_EQ(properties.at(0), "baz");
  EXPECT_EQ(document.at("baz").to_integer(), 99);
}

TEST(erase_must_not_affect_ordering) {
  sourcemeta::core::JSON document{{"x", sourcemeta::core::JSON{1}},
                                  {"y", sourcemeta::core::JSON{2}},
                                  {"z", sourcemeta::core::JSON{2}}};

  auto before_iterator = document.as_object().begin();
  EXPECT_EQ(before_iterator->first, "x");
  std::advance(before_iterator, 1);
  EXPECT_EQ(before_iterator->first, "y");
  std::advance(before_iterator, 1);
  EXPECT_EQ(before_iterator->first, "z");

  document.erase("x");

  auto after_iterator = document.as_object().begin();
  EXPECT_EQ(after_iterator->first, "y");
  std::advance(after_iterator, 1);
  EXPECT_EQ(after_iterator->first, "z");
}

TEST(reorder_empty) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  document.reorder(
      [](const auto &left, const auto &right) { return left < right; });
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 0);
}

TEST(reorder_single_element) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  document.assign("apple", sourcemeta::core::JSON{1});
  document.reorder(
      [](const auto &left, const auto &right) { return left < right; });
  EXPECT_EQ(document.size(), 1);
  auto iterator = document.as_object().cbegin();
  EXPECT_EQ(iterator->first, "apple");
  EXPECT_EQ(iterator->second.to_integer(), 1);
}

TEST(reorder_ascending) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  document.assign("zebra", sourcemeta::core::JSON{1});
  document.assign("apple", sourcemeta::core::JSON{2});
  document.assign("banana", sourcemeta::core::JSON{3});

  document.reorder(
      [](const auto &left, const auto &right) { return left < right; });

  EXPECT_EQ(document.size(), 3);
  auto iterator = document.as_object().cbegin();
  EXPECT_EQ(iterator->first, "apple");
  EXPECT_EQ(iterator->second.to_integer(), 2);
  std::advance(iterator, 1);
  EXPECT_EQ(iterator->first, "banana");
  EXPECT_EQ(iterator->second.to_integer(), 3);
  std::advance(iterator, 1);
  EXPECT_EQ(iterator->first, "zebra");
  EXPECT_EQ(iterator->second.to_integer(), 1);
}

TEST(reorder_descending) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  document.assign("apple", sourcemeta::core::JSON{1});
  document.assign("zebra", sourcemeta::core::JSON{2});
  document.assign("banana", sourcemeta::core::JSON{3});

  document.reorder(
      [](const auto &left, const auto &right) { return left > right; });

  EXPECT_EQ(document.size(), 3);
  auto iterator = document.as_object().cbegin();
  EXPECT_EQ(iterator->first, "zebra");
  EXPECT_EQ(iterator->second.to_integer(), 2);
  std::advance(iterator, 1);
  EXPECT_EQ(iterator->first, "banana");
  EXPECT_EQ(iterator->second.to_integer(), 3);
  std::advance(iterator, 1);
  EXPECT_EQ(iterator->first, "apple");
  EXPECT_EQ(iterator->second.to_integer(), 1);
}

TEST(reorder_already_ordered) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  document.assign("apple", sourcemeta::core::JSON{1});
  document.assign("banana", sourcemeta::core::JSON{2});
  document.assign("cherry", sourcemeta::core::JSON{3});

  document.reorder(
      [](const auto &left, const auto &right) { return left < right; });

  EXPECT_EQ(document.size(), 3);
  auto iterator = document.as_object().cbegin();
  EXPECT_EQ(iterator->first, "apple");
  EXPECT_EQ(iterator->second.to_integer(), 1);
  std::advance(iterator, 1);
  EXPECT_EQ(iterator->first, "banana");
  EXPECT_EQ(iterator->second.to_integer(), 2);
  std::advance(iterator, 1);
  EXPECT_EQ(iterator->first, "cherry");
  EXPECT_EQ(iterator->second.to_integer(), 3);
}

TEST(try_at_start_hit) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2,\"baz\":3}");
  const auto &object{document.as_object()};
  sourcemeta::core::JSON::Object::size_type start{0};

  const auto hash_foo{object.hash("foo")};
  const auto *result_foo{object.try_at("foo", hash_foo, start)};
  EXPECT_TRUE(result_foo);
  EXPECT_EQ(result_foo->to_integer(), 1);
  EXPECT_EQ(start, 1);

  const auto hash_bar{object.hash("bar")};
  const auto *result_bar{object.try_at("bar", hash_bar, start)};
  EXPECT_TRUE(result_bar);
  EXPECT_EQ(result_bar->to_integer(), 2);
  EXPECT_EQ(start, 2);

  const auto hash_baz{object.hash("baz")};
  const auto *result_baz{object.try_at("baz", hash_baz, start)};
  EXPECT_TRUE(result_baz);
  EXPECT_EQ(result_baz->to_integer(), 3);
  EXPECT_EQ(start, 3);
}

TEST(try_at_start_miss) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2}");
  const auto &object{document.as_object()};
  sourcemeta::core::JSON::Object::size_type start{0};

  const auto hash_missing{object.hash("missing")};
  const auto *result{object.try_at("missing", hash_missing, start)};
  EXPECT_FALSE(result);
  EXPECT_EQ(start, 0);
}

TEST(try_at_start_reverse_order) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2,\"baz\":3}");
  const auto &object{document.as_object()};
  sourcemeta::core::JSON::Object::size_type start{0};

  const auto hash_baz{object.hash("baz")};
  const auto *result_baz{object.try_at("baz", hash_baz, start)};
  EXPECT_TRUE(result_baz);
  EXPECT_EQ(result_baz->to_integer(), 3);
  EXPECT_EQ(start, 3);

  const auto hash_bar{object.hash("bar")};
  const auto *result_bar{object.try_at("bar", hash_bar, start)};
  EXPECT_TRUE(result_bar);
  EXPECT_EQ(result_bar->to_integer(), 2);
  EXPECT_EQ(start, 2);

  const auto hash_foo{object.hash("foo")};
  const auto *result_foo{object.try_at("foo", hash_foo, start)};
  EXPECT_TRUE(result_foo);
  EXPECT_EQ(result_foo->to_integer(), 1);
  EXPECT_EQ(start, 1);
}

TEST(try_at_start_single_property) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"only\":42}");
  const auto &object{document.as_object()};
  sourcemeta::core::JSON::Object::size_type start{0};

  const auto hash_only{object.hash("only")};
  const auto *result{object.try_at("only", hash_only, start)};
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 42);
  EXPECT_EQ(start, 1);
}

TEST(try_at_start_wraparound) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2,\"baz\":3}");
  const auto &object{document.as_object()};
  sourcemeta::core::JSON::Object::size_type start{object.size()};

  const auto hash_foo{object.hash("foo")};
  const auto *result{object.try_at("foo", hash_foo, start)};
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 1);
  EXPECT_EQ(start, 1);
}

TEST(try_at_start_sequential_advances) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"a\":1,\"b\":2,\"c\":3,\"d\":4}");
  const auto &object{document.as_object()};
  sourcemeta::core::JSON::Object::size_type start{0};

  const auto hash_a{object.hash("a")};
  const auto *result_a{object.try_at("a", hash_a, start)};
  EXPECT_TRUE(result_a);
  EXPECT_EQ(result_a->to_integer(), 1);

  const auto hash_b{object.hash("b")};
  const auto *result_b{object.try_at("b", hash_b, start)};
  EXPECT_TRUE(result_b);
  EXPECT_EQ(result_b->to_integer(), 2);

  const auto hash_c{object.hash("c")};
  const auto *result_c{object.try_at("c", hash_c, start)};
  EXPECT_TRUE(result_c);
  EXPECT_EQ(result_c->to_integer(), 3);

  const auto hash_d{object.hash("d")};
  const auto *result_d{object.try_at("d", hash_d, start)};
  EXPECT_TRUE(result_d);
  EXPECT_EQ(result_d->to_integer(), 4);

  EXPECT_EQ(start, object.size());
}

TEST(defines_with_string_view) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2}");
  const std::string_view foo{"foo"};
  const std::string_view bar{"bar"};
  const std::string_view qux{"qux"};
  EXPECT_TRUE(document.defines(foo));
  EXPECT_TRUE(document.defines(bar));
  EXPECT_FALSE(document.defines(qux));
}

TEST(defines_with_string_view_subview) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2}");
  const std::string buffer{"foobar"};
  const std::string_view foo_view{buffer.data(), 3};
  const std::string_view bar_view{buffer.data() + 3, 3};
  EXPECT_TRUE(document.defines(foo_view));
  EXPECT_TRUE(document.defines(bar_view));
}

TEST(defines_with_string_view_and_hash) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2}");
  const auto &object{document.as_object()};
  const std::string_view foo{"foo"};
  const auto foo_hash{object.hash(foo)};
  EXPECT_TRUE(document.defines(foo, foo_hash));
}

TEST(at_const_with_string_view) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2}");
  const std::string_view foo{"foo"};
  const std::string_view bar{"bar"};
  EXPECT_EQ(document.at(foo).to_integer(), 1);
  EXPECT_EQ(document.at(bar).to_integer(), 2);
}

TEST(at_const_with_string_view_subview) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2}");
  const std::string buffer{"foobar"};
  const std::string_view foo_view{buffer.data(), 3};
  const std::string_view bar_view{buffer.data() + 3, 3};
  EXPECT_EQ(document.at(foo_view).to_integer(), 1);
  EXPECT_EQ(document.at(bar_view).to_integer(), 2);
}

TEST(at_mutable_with_string_view) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2}");
  const std::string_view foo{"foo"};
  document.at(foo) = sourcemeta::core::JSON{42};
  EXPECT_EQ(document.at("foo").to_integer(), 42);
}

TEST(at_const_with_string_view_and_hash) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2}");
  const auto &object{document.as_object()};
  const std::string_view foo{"foo"};
  const auto foo_hash{object.hash(foo)};
  EXPECT_EQ(document.at(foo, foo_hash).to_integer(), 1);
}

TEST(defines_with_string_view_long_key) {
  const std::string_view long_key{
      "this_is_a_long_key_more_than_thirty_one_characters"};
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  document.assign(long_key, sourcemeta::core::JSON{1});
  EXPECT_TRUE(document.defines(long_key));
  EXPECT_EQ(document.at(long_key).to_integer(), 1);
}

TEST(assign_lvalue_with_string_view_key) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  const std::string_view key{"foo"};
  const sourcemeta::core::JSON value{42};
  document.assign(key, value);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_EQ(document.at("foo").to_integer(), 42);
}

TEST(assign_rvalue_with_string_view_key) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  const std::string_view key{"foo"};
  document.assign(key, sourcemeta::core::JSON{42});
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_EQ(document.at("foo").to_integer(), 42);
}

TEST(assign_with_string_view_subview_key) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  const std::string buffer{"foobar"};
  const std::string_view foo_view{buffer.data(), 3};
  const std::string_view bar_view{buffer.data() + 3, 3};
  document.assign(foo_view, sourcemeta::core::JSON{1});
  document.assign(bar_view, sourcemeta::core::JSON{2});
  EXPECT_EQ(document.at("foo").to_integer(), 1);
  EXPECT_EQ(document.at("bar").to_integer(), 2);
}

TEST(erase_with_string_view) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2}");
  const std::string_view foo{"foo"};
  document.erase(foo);
  EXPECT_FALSE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
}

TEST(erase_with_string_view_subview) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":1,\"bar\":2}");
  const std::string buffer{"foobar"};
  const std::string_view foo_view{buffer.data(), 3};
  document.erase(foo_view);
  EXPECT_FALSE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
}

TEST(assign_if_missing_with_string_view) {
  sourcemeta::core::JSON document = sourcemeta::core::JSON::make_object();
  document.assign("foo", sourcemeta::core::JSON{1});
  const std::string_view foo{"foo"};
  const std::string_view bar{"bar"};
  document.assign_if_missing(foo, sourcemeta::core::JSON{99});
  document.assign_if_missing(bar, sourcemeta::core::JSON{2});
  EXPECT_EQ(document.at("foo").to_integer(), 1);
  EXPECT_EQ(document.at("bar").to_integer(), 2);
}

TEST(entry_key_equals_perfect_match) {
  const auto document{sourcemeta::core::parse_json(R"({ "foo": 1 })")};
  const auto &entry{*document.as_object().cbegin()};
  EXPECT_TRUE(
      entry.key_equals("foo", sourcemeta::core::JSON::Object::hash("foo")));
}

TEST(entry_key_equals_perfect_mismatch) {
  const auto document{sourcemeta::core::parse_json(R"({ "foo": 1 })")};
  const auto &entry{*document.as_object().cbegin()};
  EXPECT_FALSE(
      entry.key_equals("bar", sourcemeta::core::JSON::Object::hash("bar")));
}

TEST(entry_key_equals_imperfect_match) {
  const std::string key(40, 'a');
  sourcemeta::core::JSON document{sourcemeta::core::JSON::make_object()};
  document.assign(key, sourcemeta::core::JSON{1});
  const auto &entry{*document.as_object().cbegin()};
  EXPECT_FALSE(document.as_object().hash(key) ==
               sourcemeta::core::JSON::Object::hash("a"));
  EXPECT_TRUE(entry.key_equals(key, sourcemeta::core::JSON::Object::hash(key)));
}

TEST(entry_key_equals_imperfect_collision_is_rejected) {
  const std::string stored{std::string(31, 'a') + "11111110" + "z"};
  const std::string other{std::string(31, 'a') + "22222220" + "z"};
  sourcemeta::core::JSON document{sourcemeta::core::JSON::make_object()};
  document.assign(stored, sourcemeta::core::JSON{1});
  const auto &entry{*document.as_object().cbegin()};
  EXPECT_EQ(sourcemeta::core::JSON::Object::hash(stored),
            sourcemeta::core::JSON::Object::hash(other));
  EXPECT_FALSE(
      entry.key_equals(other, sourcemeta::core::JSON::Object::hash(other)));
}

TEST(key_with_trailing_null_not_matched_as_prefix) {
  auto document{sourcemeta::core::JSON::make_object()};
  sourcemeta::core::JSON::String key_with_null{"a"};
  key_with_null.push_back('\x00');
  document.assign(key_with_null, sourcemeta::core::JSON{1});
  EXPECT_FALSE(document.defines("a"));
  EXPECT_TRUE(document.defines(key_with_null));
}

TEST(keys_differing_by_trailing_null_are_distinct) {
  auto document{sourcemeta::core::JSON::make_object()};
  sourcemeta::core::JSON::String key_with_null{"a"};
  key_with_null.push_back('\x00');
  document.assign("a", sourcemeta::core::JSON{1});
  document.assign(key_with_null, sourcemeta::core::JSON{2});
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.at("a").to_integer(), 1);
  EXPECT_EQ(document.at(key_with_null).to_integer(), 2);
}

TEST(array_member_contains_present) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "tags": [ "a", "b" ] })JSON")};
  EXPECT_TRUE(document.array_member_contains(
      "tags", document.as_object().hash("tags"), "b"));
}

TEST(array_member_contains_absent_value) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "tags": [ "a", "b" ] })JSON")};
  EXPECT_FALSE(document.array_member_contains(
      "tags", document.as_object().hash("tags"), "c"));
}

TEST(array_member_contains_missing_member) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "other": 1 })JSON")};
  EXPECT_FALSE(document.array_member_contains(
      "tags", document.as_object().hash("tags"), "a"));
}

TEST(array_member_contains_wrong_member_type) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "tags": "a" })JSON")};
  EXPECT_FALSE(document.array_member_contains(
      "tags", document.as_object().hash("tags"), "a"));
}

TEST(array_member_contains_computes_hash) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "tags": [ "a", "b" ] })JSON")};
  EXPECT_TRUE(document.array_member_contains("tags", "a"));
  EXPECT_FALSE(document.array_member_contains("tags", "z"));
}

TEST(assign_if_nonempty_scalar_assigns) {
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign_if_nonempty("foo", document.as_object().hash("foo"), "bar");
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_EQ(document.at("foo").to_string(), "bar");
}

TEST(assign_if_nonempty_scalar_skips_empty) {
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign_if_nonempty("foo", document.as_object().hash("foo"), "");
  EXPECT_FALSE(document.defines("foo"));
}

TEST(assign_if_nonempty_scalar_computes_hash) {
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign_if_nonempty("foo", "bar");
  EXPECT_EQ(document.at("foo").to_string(), "bar");
}

TEST(assign_if_nonempty_array_assigns) {
  auto document{sourcemeta::core::JSON::make_object()};
  const std::array<std::string_view, 2> values{{"a", "b"}};
  document.assign_if_nonempty("foo", document.as_object().hash("foo"), values);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_array());
  EXPECT_EQ(document.at("foo").size(), 2);
  EXPECT_EQ(document.at("foo").at(0).to_string(), "a");
}

TEST(assign_if_nonempty_array_skips_empty) {
  auto document{sourcemeta::core::JSON::make_object()};
  const std::span<const std::string_view> values{};
  document.assign_if_nonempty("foo", document.as_object().hash("foo"), values);
  EXPECT_FALSE(document.defines("foo"));
}

TEST(assign_if_nonempty_array_computes_hash) {
  auto document{sourcemeta::core::JSON::make_object()};
  const std::array<std::string_view, 1> values{{"a"}};
  document.assign_if_nonempty("foo", values);
  EXPECT_EQ(document.at("foo").size(), 1);
}

TEST(hash_is_a_constant_expression_matching_runtime) {
  static constexpr auto compile_time{
      sourcemeta::core::JSON::Object::hash(std::string_view{"foobar"})};
  const auto run_time{
      sourcemeta::core::JSON::Object::hash(std::string_view{"foobar"})};
  EXPECT_TRUE(compile_time == run_time);
}

TEST(constexpr_hash_finds_an_object_member) {
  static constexpr auto key_hash{
      sourcemeta::core::JSON::Object::hash(std::string_view{"foo"})};
  const auto document{sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  const auto *value{document.try_at(std::string_view{"foo"}, key_hash)};
  EXPECT_TRUE(value != nullptr);
  EXPECT_EQ(value->to_integer(), 1);
}

TEST(constexpr_hash_matches_runtime_for_a_long_key) {
  static constexpr std::string_view key{
      "a_property_name_that_is_definitely_longer_than_thirty_one_bytes"};
  static constexpr auto compile_time{sourcemeta::core::JSON::Object::hash(key)};
  const auto run_time{sourcemeta::core::JSON::Object::hash(key)};
  EXPECT_TRUE(compile_time == run_time);
}

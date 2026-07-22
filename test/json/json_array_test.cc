#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <algorithm>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

TEST(general_traits) {
  EXPECT_TRUE(
      std::is_default_constructible<sourcemeta::core::JSON::Array>::value);
  EXPECT_FALSE(std::is_nothrow_default_constructible<
               sourcemeta::core::JSON::Array>::value);
  EXPECT_TRUE(std::is_destructible<sourcemeta::core::JSON::Array>::value);
  EXPECT_TRUE(
      std::is_nothrow_destructible<sourcemeta::core::JSON::Array>::value);
}

TEST(copy_traits) {
  EXPECT_TRUE(std::is_copy_assignable<sourcemeta::core::JSON::Array>::value);
  EXPECT_TRUE(std::is_copy_constructible<sourcemeta::core::JSON::Array>::value);
  EXPECT_FALSE(
      std::is_nothrow_copy_assignable<sourcemeta::core::JSON::Array>::value);
  EXPECT_FALSE(
      std::is_nothrow_copy_constructible<sourcemeta::core::JSON::Array>::value);
}

TEST(move_traits) {
  EXPECT_TRUE(std::is_move_assignable<sourcemeta::core::JSON::Array>::value);
  EXPECT_TRUE(std::is_move_constructible<sourcemeta::core::JSON::Array>::value);
  EXPECT_TRUE(
      std::is_nothrow_move_assignable<sourcemeta::core::JSON::Array>::value);
  EXPECT_TRUE(
      std::is_nothrow_move_constructible<sourcemeta::core::JSON::Array>::value);
}

TEST(initializer_list_2_booleans) {
  const sourcemeta::core::JSON document{sourcemeta::core::JSON{false},
                                        sourcemeta::core::JSON{true}};

  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.array_size(), 2);
  EXPECT_TRUE(document.at(0).is_boolean());
  EXPECT_TRUE(document.at(1).is_boolean());
  EXPECT_FALSE(document.at(0).to_boolean());
  EXPECT_TRUE(document.at(1).to_boolean());
}

TEST(type) {
  const sourcemeta::core::JSON document{sourcemeta::core::JSON{false},
                                        sourcemeta::core::JSON{true}};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Array);
}

TEST(empty_with_copy_constructor) {
  const sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};

  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.array_size(), 0);
}

TEST(empty_with_make_array) {
  const sourcemeta::core::JSON document = sourcemeta::core::JSON::make_array();
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.array_size(), 0);
}

TEST(push_back_booleans) {
  sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};
  EXPECT_TRUE(document.is_array());
  document.push_back(sourcemeta::core::JSON{false});
  document.push_back(sourcemeta::core::JSON{true});
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.array_size(), 2);
  EXPECT_TRUE(document.at(0).is_boolean());
  EXPECT_TRUE(document.at(1).is_boolean());
  EXPECT_FALSE(document.at(0).to_boolean());
  EXPECT_TRUE(document.at(1).to_boolean());
}

TEST(boolean_iterator) {
  const sourcemeta::core::JSON document{sourcemeta::core::JSON{true},
                                        sourcemeta::core::JSON{false},
                                        sourcemeta::core::JSON{false}};

  std::vector<bool> result;
  for (const auto &element : document.as_array()) {
    result.push_back(element.to_boolean());
  }

  EXPECT_EQ(result.size(), 3);
  EXPECT_TRUE(result.at(0));
  EXPECT_FALSE(result.at(1));
  EXPECT_FALSE(result.at(2));
}

TEST(reverse_boolean_iterator) {
  const sourcemeta::core::JSON document{sourcemeta::core::JSON{true},
                                        sourcemeta::core::JSON{false},
                                        sourcemeta::core::JSON{false}};

  std::vector<bool> result;
  for (auto iterator = document.as_array().crbegin();
       iterator != document.as_array().crend(); iterator++) {
    result.push_back(iterator->to_boolean());
  }

  EXPECT_EQ(result.size(), 3);
  EXPECT_FALSE(result.at(0));
  EXPECT_FALSE(result.at(1));
  EXPECT_TRUE(result.at(2));
}

TEST(push_back_json_copy) {
  sourcemeta::core::JSON document{sourcemeta::core::JSON{1},
                                  sourcemeta::core::JSON{2}};

  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.array_size(), 2);
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);

  const sourcemeta::core::JSON element{3};
  document.push_back(element);

  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.array_size(), 3);
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 3);
}

TEST(push_back_json_move) {
  sourcemeta::core::JSON document{sourcemeta::core::JSON{1},
                                  sourcemeta::core::JSON{2}};

  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 2);
  EXPECT_EQ(document.array_size(), 2);
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);

  sourcemeta::core::JSON element{3};
  document.push_back(std::move(element));

  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.array_size(), 3);
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 3);
}

TEST(modify_array_after_copy) {
  // Original document
  sourcemeta::core::JSON document{sourcemeta::core::JSON{1},
                                  sourcemeta::core::JSON{2},
                                  sourcemeta::core::JSON{3}};
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.array_size(), 3);
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 3);

  // Make copy
  sourcemeta::core::JSON copy = document;
  EXPECT_EQ(copy.size(), 3);
  EXPECT_EQ(copy.array_size(), 3);
  EXPECT_EQ(copy.at(0).to_integer(), 1);
  EXPECT_EQ(copy.at(1).to_integer(), 2);
  EXPECT_EQ(copy.at(2).to_integer(), 3);

  // Modify copy
  copy.push_back(sourcemeta::core::JSON{5});

  // Original document must remain intact
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.array_size(), 3);
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 3);
}

TEST(const_iterator_for_each) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ 1, 2, 3 ]");
  std::vector<std::int64_t> result;
  std::for_each(document.as_array().cbegin(), document.as_array().cend(),
                [&result](const auto &element) {
                  result.push_back(element.to_integer());
                });
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at(0), 1);
  EXPECT_EQ(result.at(1), 2);
  EXPECT_EQ(result.at(2), 3);
}

TEST(iterator_for_each) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[ 1, 2, 3 ]");
  std::vector<std::int64_t> result;
  std::for_each(
      document.as_array().begin(), document.as_array().end(),
      [&result](auto &element) { result.push_back(element.to_integer()); });
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at(0), 1);
  EXPECT_EQ(result.at(1), 2);
  EXPECT_EQ(result.at(2), 3);
}

TEST(reverse_const_iterator_for_each) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ 1, 2, 3 ]");
  std::vector<std::int64_t> result;
  std::for_each(document.as_array().crbegin(), document.as_array().crend(),
                [&result](const auto &element) {
                  result.push_back(element.to_integer());
                });
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at(0), 3);
  EXPECT_EQ(result.at(1), 2);
  EXPECT_EQ(result.at(2), 1);
}

TEST(reverse_iterator_for_each) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[ 1, 2, 3 ]");
  std::vector<std::int64_t> result;
  std::for_each(
      document.as_array().rbegin(), document.as_array().rend(),
      [&result](auto &element) { result.push_back(element.to_integer()); });
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at(0), 3);
  EXPECT_EQ(result.at(1), 2);
  EXPECT_EQ(result.at(2), 1);
}

TEST(const_iterator) {
  std::vector<std::int64_t> result;
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ 1, 2, 3 ]");
  for (const auto &element : document.as_array()) {
    result.push_back(element.to_integer());
  }

  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at(0), 1);
  EXPECT_EQ(result.at(1), 2);
  EXPECT_EQ(result.at(2), 3);
}

TEST(simple_iterator) {
  std::vector<std::int64_t> result;
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[ 1, 2, 3 ]");
  for (auto &element : document.as_array()) {
    result.push_back(element.to_integer());
  }

  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at(0), 1);
  EXPECT_EQ(result.at(1), 2);
  EXPECT_EQ(result.at(2), 3);
}

TEST(int_standard_sort) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[3,2,1]");
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.array_size(), 3);
  EXPECT_EQ(document.at(0).to_integer(), 3);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 1);
  std::sort(document.as_array().begin(), document.as_array().end());
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 3);
}

TEST(object_standard_sort) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(
      R"JSON([{"a":9,"b":1},{"a":1,"b":9},{"a":5,"b":5},{"a":1,"b":1}])JSON");
  std::sort(document.as_array().begin(), document.as_array().end());
  EXPECT_EQ(document.size(), 4);
  EXPECT_EQ(document.at(0).at("a").to_integer(), 1);
  EXPECT_EQ(document.at(0).at("b").to_integer(), 1);
  EXPECT_EQ(document.at(1).at("a").to_integer(), 1);
  EXPECT_EQ(document.at(1).at("b").to_integer(), 9);
  EXPECT_EQ(document.at(2).at("a").to_integer(), 5);
  EXPECT_EQ(document.at(2).at("b").to_integer(), 5);
  EXPECT_EQ(document.at(3).at("a").to_integer(), 9);
  EXPECT_EQ(document.at(3).at("b").to_integer(), 1);
}

TEST(move_assignment_from_own_element) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[[1,2,3]]");
  document = std::move(document.at(0));
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 3);
}

TEST(erase_many_full) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.array_size(), 3);
  document.erase(document.as_array().begin(), document.as_array().end());
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.array_size(), 0);
}

TEST(erase_many_partial) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.array_size(), 3);
  document.erase(std::next(document.as_array().begin()),
                 document.as_array().end());
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 1);
  EXPECT_EQ(document.array_size(), 1);
  EXPECT_EQ(document.at(0).to_integer(), 1);
}

TEST(erase_many_full_const) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.array_size(), 3);
  document.erase(document.as_array().cbegin(), document.as_array().cend());
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.array_size(), 0);
}

TEST(contains_string_key_true) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ \"foo\", \"bar\" ]");
  EXPECT_TRUE(document.contains(sourcemeta::core::JSON{"bar"}));
}

TEST(contains_string_key_false) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ \"foo\", \"bar\" ]");
  EXPECT_FALSE(document.contains(sourcemeta::core::JSON{"baz"}));
}

TEST(contains_string_literal_true) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON([ "foo", "bar", "baz" ])JSON");
  EXPECT_TRUE(document.contains("bar"));
}

TEST(contains_string_literal_false) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON([ "foo", "bar", "baz" ])JSON");
  EXPECT_FALSE(document.contains("qux"));
}

TEST(contains_string_view_true) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON([ "foo", "bar", "baz" ])JSON");
  const sourcemeta::core::JSON::StringView element{"bar"};
  EXPECT_TRUE(document.contains(element));
}

TEST(contains_string_view_false) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON([ "foo", "bar", "baz" ])JSON");
  const sourcemeta::core::JSON::StringView element{"qux"};
  EXPECT_FALSE(document.contains(element));
}

TEST(contains_string_from_std_string) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON([ "foo", "bar", "baz" ])JSON");
  const std::string element{"bar"};
  EXPECT_TRUE(document.contains(element));
}

TEST(contains_string_in_mixed_array) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON([ 1, "bar", true, null ])JSON");
  EXPECT_TRUE(document.contains("bar"));
  EXPECT_FALSE(document.contains("foo"));
}

TEST(defines_any_with_iterators_has_one) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":true,\"bar\":false,\"baz\":true}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.defines("bar"));
  EXPECT_TRUE(document.defines("baz"));
  const std::vector<std::string> keys{"foo", "qux"};
  EXPECT_TRUE(document.defines_any(keys.cbegin(), keys.cend()));
}

TEST(defines_any_with_iterators_has_two) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":true,\"bar\":false,\"baz\":true}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  const std::vector<std::string> keys{"foo", "baz"};
  EXPECT_TRUE(document.defines_any(keys.cbegin(), keys.cend()));
}

TEST(defines_any_with_iterators_has_none) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":true,\"bar\":false,\"baz\":true}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  const std::vector<std::string> keys{"qux", "test"};
  EXPECT_FALSE(document.defines_any(keys.cbegin(), keys.cend()));
}

TEST(defines_any_with_initializer_list_has_one) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":true,\"bar\":false,\"baz\":true}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  EXPECT_TRUE(document.defines_any({"foo", "qux"}));
}

TEST(defines_any_with_initializer_list_has_two) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":true,\"bar\":false,\"baz\":true}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  EXPECT_TRUE(document.defines_any({"foo", "baz"}));
}

TEST(defines_any_with_initializer_list_has_none) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":true,\"bar\":false,\"baz\":true}");
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 3);
  EXPECT_FALSE(document.defines_any({"qux", "test"}));
}

TEST(into_array) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into_array();
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.array_size(), 0);
}

TEST(clear) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.array_size(), 3);
  document.clear();
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 0);
  EXPECT_EQ(document.array_size(), 0);
}

TEST(estimated_byte_size_integers) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_EQ(document.estimated_byte_size(), 24);
}

TEST(estimated_byte_size_nested) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[1,[\"foo\"],[[true]]]");
  EXPECT_EQ(document.estimated_byte_size(), 12);
}

TEST(fast_hash_integers) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_EQ(document.fast_hash(), 27);
}

TEST(fast_hash_nested) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[1,[\"foo\"],[[true]]]");
  EXPECT_EQ(document.fast_hash(), 42);
}

TEST(push_back_if_unique_copy_exists) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[1,2,3]");

  const sourcemeta::core::JSON element{2};
  const auto result{document.push_back_if_unique(element)};

  EXPECT_TRUE(result.first.get().is_integer());
  EXPECT_EQ(result.first.get().to_integer(), 2);
  EXPECT_FALSE(result.second);

  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.array_size(), 3);
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 3);
}

TEST(push_back_if_unique_copy_not_exist) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[1,2,3]");

  const sourcemeta::core::JSON element{4};
  const auto result{document.push_back_if_unique(element)};

  EXPECT_TRUE(result.first.get().is_integer());
  EXPECT_EQ(result.first.get().to_integer(), 4);
  EXPECT_TRUE(result.second);

  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 4);
  EXPECT_EQ(document.array_size(), 4);
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 3);
  EXPECT_EQ(document.at(3).to_integer(), 4);
}

TEST(push_back_if_unique_move_exists) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[1,2,3]");

  sourcemeta::core::JSON element{2};
  const auto result{document.push_back_if_unique(std::move(element))};

  EXPECT_TRUE(result.first.get().is_integer());
  EXPECT_EQ(result.first.get().to_integer(), 2);
  EXPECT_FALSE(result.second);

  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 3);
  EXPECT_EQ(document.array_size(), 3);
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 3);
}

TEST(push_back_if_unique_move_not_exist) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[1,2,3]");

  sourcemeta::core::JSON element{4};
  const auto result{document.push_back_if_unique(std::move(element))};

  EXPECT_TRUE(result.first.get().is_integer());
  EXPECT_EQ(result.first.get().to_integer(), 4);
  EXPECT_TRUE(result.second);

  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 4);
  EXPECT_EQ(document.array_size(), 4);
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 3);
  EXPECT_EQ(document.at(3).to_integer(), 4);
}

TEST(unique_empty) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("[]");
  EXPECT_TRUE(document.unique());
}

TEST(unique_true) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ 1, 2, {} ]");
  EXPECT_TRUE(document.unique());
}

TEST(unique_false) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ [2], 1, [2] ]");
  EXPECT_FALSE(document.unique());
}

// The same numeric value in different representations is a duplicate, even
// when the values are large enough to fall in distinct hash buckets
TEST(unique_false_integer_and_real_same_value) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ 1000, 1000.0 ]");
  EXPECT_FALSE(document.unique());
}

TEST(unique_false_integer_and_decimal_same_value) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ 1000, 1000e0 ]");
  EXPECT_FALSE(document.unique());
}

TEST(unique_true_distinct_numeric_values) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ 1000, 1001.0 ]");
  EXPECT_TRUE(document.unique());
}

TEST(unique_false_integer_real_and_decimal_same_value) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ 1000, 1000.0, 1000e0 ]");
  EXPECT_FALSE(document.unique());
}

TEST(unique_false_negative_number_same_value) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ -1000, -1000.0 ]");
  EXPECT_FALSE(document.unique());
}

TEST(unique_false_fractional_real_and_decimal_same_value) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ 0.25, 2.5e-1 ]");
  EXPECT_FALSE(document.unique());
}

TEST(unique_true_distinct_fractional_values) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ 0.25, 0.5 ]");
  EXPECT_TRUE(document.unique());
}

TEST(sort_object_items) {
  auto document = sourcemeta::core::parse_json(R"JSON([
    { "type": "string" },
    { "type": "integer" },
    { "type": "string" }
  ])JSON");

  std::sort(document.as_array().begin(), document.as_array().end());

  const auto expected = sourcemeta::core::parse_json(R"JSON([
    { "type": "integer" },
    { "type": "string" },
    { "type": "string" }
  ])JSON");

  EXPECT_EQ(document, expected);
}

TEST(erase_if_some) {
  auto document = sourcemeta::core::parse_json(R"JSON([
    1, 2, 3, 4, 5
  ])JSON");

  document.erase_if([](const auto &item) {
    return item.is_integer() && item.to_integer() % 2 == 0;
  });

  const auto expected = sourcemeta::core::parse_json(R"JSON([
    1, 3, 5
  ])JSON");

  EXPECT_EQ(document, expected);
}

TEST(erase_if_none) {
  auto document = sourcemeta::core::parse_json(R"JSON([
    1, 2, 3, 4, 5
  ])JSON");

  document.erase_if([](const auto &item) { return item.is_boolean(); });

  const auto expected = sourcemeta::core::parse_json(R"JSON([
    1, 2, 3, 4, 5
  ])JSON");

  EXPECT_EQ(document, expected);
}

TEST(erase_if_all) {
  auto document = sourcemeta::core::parse_json(R"JSON([
    1, 2, 3, 4, 5
  ])JSON");

  document.erase_if([](const auto &item) { return item.is_integer(); });

  const auto expected = sourcemeta::core::parse_json(R"JSON([])JSON");

  EXPECT_EQ(document, expected);
}

TEST(is_array_of_strings_true) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ "a", "b" ])JSON")};
  EXPECT_TRUE(document.is_array_of_strings());
}

TEST(is_array_of_strings_empty_is_vacuously_true) {
  const auto document{sourcemeta::core::parse_json(R"JSON([])JSON")};
  EXPECT_TRUE(document.is_array_of_strings());
}

TEST(is_array_of_strings_with_a_non_string_element) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ "a", 1 ])JSON")};
  EXPECT_FALSE(document.is_array_of_strings());
}

TEST(is_array_of_strings_non_array) {
  const sourcemeta::core::JSON document{"a"};
  EXPECT_FALSE(document.is_array_of_strings());
}

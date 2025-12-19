#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_template, equality_without_wildcard_true) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::PointerTemplate left{pointer};
  const sourcemeta::core::PointerTemplate right{pointer};

  EXPECT_EQ(left, right);
}

TEST(JSONPointer_template, equality_without_wildcard_false) {
  const sourcemeta::core::Pointer pointer_left{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_right{"foo", "baz"};
  const sourcemeta::core::PointerTemplate left{pointer_left};
  const sourcemeta::core::PointerTemplate right{pointer_right};

  EXPECT_NE(left, right);
}

TEST(JSONPointer_template, equality_with_property_wildcard_true) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate left{prefix};
  left.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Property);
  left.push_back(suffix);

  sourcemeta::core::PointerTemplate right{prefix};
  right.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Property);
  right.push_back(suffix);

  EXPECT_EQ(left, right);
}

TEST(JSONPointer_template, equality_with_property_wildcard_false) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate left{prefix};
  left.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Property);
  left.push_back(suffix);

  sourcemeta::core::PointerTemplate right{prefix};
  right.push_back(suffix);
  right.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Property);

  EXPECT_NE(left, right);
}

TEST(JSONPointer_template, equality_with_regex_wildcard_true) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate left{prefix};
  left.emplace_back(sourcemeta::core::PointerTemplate::Regex{"^@"});
  left.push_back(suffix);

  sourcemeta::core::PointerTemplate right{prefix};
  right.emplace_back(sourcemeta::core::PointerTemplate::Regex{"^@"});
  right.push_back(suffix);

  EXPECT_EQ(left, right);
}

TEST(JSONPointer_template, equality_with_regex_wildcard_false) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate left{prefix};
  left.emplace_back(sourcemeta::core::PointerTemplate::Regex{"^@"});
  left.push_back(suffix);

  sourcemeta::core::PointerTemplate right{prefix};
  right.push_back(suffix);
  right.emplace_back(sourcemeta::core::PointerTemplate::Regex{"^@"});

  EXPECT_NE(left, right);
}

TEST(JSONPointer_template, equality_with_item_wildcard_true) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate left{prefix};
  left.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Item);
  left.push_back(suffix);

  sourcemeta::core::PointerTemplate right{prefix};
  right.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Item);
  right.push_back(suffix);

  EXPECT_EQ(left, right);
}

TEST(JSONPointer_template, equality_with_item_wildcard_false) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate left{prefix};
  left.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Item);
  left.push_back(suffix);

  sourcemeta::core::PointerTemplate right{prefix};
  right.push_back(suffix);
  right.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Item);

  EXPECT_NE(left, right);
}

TEST(JSONPointer_template, equality_with_key_wildcard_true) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};

  sourcemeta::core::PointerTemplate left{prefix};
  left.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Key);

  sourcemeta::core::PointerTemplate right{prefix};
  right.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Key);

  EXPECT_EQ(left, right);
}

TEST(JSONPointer_template, equality_with_key_wildcard_false) {
  const sourcemeta::core::Pointer prefix_1{"foo"};
  const sourcemeta::core::Pointer prefix_2{"bar"};

  sourcemeta::core::PointerTemplate left{prefix_1};
  left.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Key);

  sourcemeta::core::PointerTemplate right{prefix_2};
  right.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Key);

  EXPECT_NE(left, right);
}

TEST(JSONPointer_template, equality_with_condition_suffix_true) {
  sourcemeta::core::PointerTemplate left;
  left.emplace_back(sourcemeta::core::PointerTemplate::Condition{"foo"});

  sourcemeta::core::PointerTemplate right;
  right.emplace_back(sourcemeta::core::PointerTemplate::Condition{"foo"});

  EXPECT_EQ(left, right);
}

TEST(JSONPointer_template, equality_with_condition_suffix_false) {
  sourcemeta::core::PointerTemplate left;
  left.emplace_back(sourcemeta::core::PointerTemplate::Condition{"foo"});

  sourcemeta::core::PointerTemplate right;
  right.emplace_back(sourcemeta::core::PointerTemplate::Condition{"bar"});

  EXPECT_NE(left, right);
}

TEST(JSONPointer_template, equality_with_condition_wildcard_true) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate left{prefix};
  left.emplace_back(sourcemeta::core::PointerTemplate::Condition{});
  left.push_back(suffix);

  sourcemeta::core::PointerTemplate right{prefix};
  right.emplace_back(sourcemeta::core::PointerTemplate::Condition{});
  right.push_back(suffix);

  EXPECT_EQ(left, right);
}

TEST(JSONPointer_template, equality_with_condition_wildcard_false) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate left{prefix};
  left.emplace_back(sourcemeta::core::PointerTemplate::Condition{});
  left.push_back(suffix);

  sourcemeta::core::PointerTemplate right{prefix};
  right.push_back(suffix);
  right.emplace_back(sourcemeta::core::PointerTemplate::Condition{});

  EXPECT_NE(left, right);
}

TEST(JSONPointer_template, equality_with_negation_wildcard_true) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate left{prefix};
  left.emplace_back(sourcemeta::core::PointerTemplate::Negation{});
  left.push_back(suffix);

  sourcemeta::core::PointerTemplate right{prefix};
  right.emplace_back(sourcemeta::core::PointerTemplate::Negation{});
  right.push_back(suffix);

  EXPECT_EQ(left, right);
}

TEST(JSONPointer_template, equality_with_negation_wildcard_false) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate left{prefix};
  left.emplace_back(sourcemeta::core::PointerTemplate::Negation{});
  left.push_back(suffix);

  sourcemeta::core::PointerTemplate right{prefix};
  right.push_back(suffix);
  right.emplace_back(sourcemeta::core::PointerTemplate::Negation{});

  EXPECT_NE(left, right);
}

TEST(JSONPointer_template, trivial_empty) {
  // const sourcemeta::core::PointerTemplate left{
  // sourcemeta::core::PointerTemplate::Negation{}};
  const sourcemeta::core::PointerTemplate pointer;
  EXPECT_TRUE(pointer.trivial());
}

TEST(JSONPointer_template, trivial_wildcard_property) {
  const sourcemeta::core::PointerTemplate pointer{
      sourcemeta::core::PointerTemplate::Wildcard::Property};
  EXPECT_FALSE(pointer.trivial());
}

TEST(JSONPointer_template, trivial_wildcard_item) {
  const sourcemeta::core::PointerTemplate pointer{
      sourcemeta::core::PointerTemplate::Wildcard::Item};
  EXPECT_FALSE(pointer.trivial());
}

TEST(JSONPointer_template, trivial_wildcard_key) {
  const sourcemeta::core::PointerTemplate pointer{
      sourcemeta::core::PointerTemplate::Wildcard::Key};
  EXPECT_FALSE(pointer.trivial());
}

TEST(JSONPointer_template, trivial_condition) {
  const sourcemeta::core::PointerTemplate pointer{
      sourcemeta::core::PointerTemplate::Condition{}};
  EXPECT_FALSE(pointer.trivial());
}

TEST(JSONPointer_template, trivial_negation) {
  const sourcemeta::core::PointerTemplate pointer{
      sourcemeta::core::PointerTemplate::Negation{}};
  EXPECT_FALSE(pointer.trivial());
}

TEST(JSONPointer_template, trivial_regex) {
  const sourcemeta::core::PointerTemplate pointer{
      sourcemeta::core::PointerTemplate::Regex{"^f"}};
  EXPECT_FALSE(pointer.trivial());
}

TEST(JSONPointer_template, trivial_token_index) {
  const sourcemeta::core::PointerTemplate pointer{
      sourcemeta::core::Pointer::Token{5}};
  EXPECT_TRUE(pointer.trivial());
}

TEST(JSONPointer_template, trivial_token_property) {
  const sourcemeta::core::PointerTemplate pointer{
      sourcemeta::core::Pointer::Token{"foo"}};
  EXPECT_TRUE(pointer.trivial());
}

TEST(JSONPointer_template, trivial_mixed) {
  const sourcemeta::core::PointerTemplate pointer{
      sourcemeta::core::Pointer::Token{"foo"},
      sourcemeta::core::PointerTemplate::Condition{}};
  EXPECT_FALSE(pointer.trivial());
}

TEST(JSONPointer_template, pop_back) {
  const sourcemeta::core::Pointer base{"foo", "bar"};
  sourcemeta::core::PointerTemplate pointer{base};
  pointer.pop_back();

  const sourcemeta::core::Pointer expected_base{"foo"};
  const sourcemeta::core::PointerTemplate expected_pointer{expected_base};

  EXPECT_EQ(pointer, expected_pointer);
}

TEST(JSONPointer_template, stringify_multiple_property_wildcards) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate pointer{prefix};
  pointer.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Property);
  pointer.push_back(suffix);
  pointer.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Property);

  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);

  EXPECT_EQ(stream.str(), "/foo/bar/~P~/baz/~P~");
}

TEST(JSONPointer_template, stringify_multiple_regex_wildcards) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate pointer{prefix};
  pointer.emplace_back(sourcemeta::core::PointerTemplate::Regex{"^@"});
  pointer.push_back(suffix);
  pointer.emplace_back(sourcemeta::core::PointerTemplate::Regex{"[0-9]+"});

  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);

  EXPECT_EQ(stream.str(), "/foo/bar/~R^@~/baz/~R[0-9]+~");
}

TEST(JSONPointer_template, stringify_multiple_item_wildcards) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer suffix{"baz"};

  sourcemeta::core::PointerTemplate pointer{prefix};
  pointer.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Item);
  pointer.push_back(suffix);
  pointer.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Item);

  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);

  EXPECT_EQ(stream.str(), "/foo/bar/~I~/baz/~I~");
}

TEST(JSONPointer_template, stringify_key_wildcard) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};

  sourcemeta::core::PointerTemplate pointer{prefix};
  pointer.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Key);

  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);

  EXPECT_EQ(stream.str(), "/foo/bar/~K~");
}

TEST(JSONPointer_template, stringify_condition) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};

  sourcemeta::core::PointerTemplate pointer{prefix};
  pointer.emplace_back(sourcemeta::core::PointerTemplate::Condition{});

  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);

  EXPECT_EQ(stream.str(), "/foo/bar/~?~");
}

TEST(JSONPointer_template, stringify_condition_with_suffix) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};

  sourcemeta::core::PointerTemplate pointer{prefix};
  pointer.emplace_back(sourcemeta::core::PointerTemplate::Condition{"hello"});

  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);

  EXPECT_EQ(stream.str(), "/foo/bar/~?hello~");
}

TEST(JSONPointer_template, stringify_negation) {
  const sourcemeta::core::Pointer prefix{"foo", "bar"};

  sourcemeta::core::PointerTemplate pointer{prefix};
  pointer.emplace_back(sourcemeta::core::PointerTemplate::Negation{});

  std::ostringstream stream;
  sourcemeta::core::stringify(pointer, stream);

  EXPECT_EQ(stream.str(), "/foo/bar/~!~");
}

TEST(JSONPointer_template, concat_move) {
  const sourcemeta::core::Pointer pointer_left{"foo"};
  const sourcemeta::core::Pointer pointer_right{"bar", "baz"};
  const sourcemeta::core::Pointer pointer_expected{"foo", "bar", "baz"};

  const sourcemeta::core::PointerTemplate left{pointer_left};
  sourcemeta::core::PointerTemplate right{pointer_right};
  const sourcemeta::core::PointerTemplate expected{pointer_expected};

  const auto result{left.concat(std::move(right))};

  EXPECT_EQ(result, expected);
}

TEST(JSONPointer_template, empty_true) {
  const sourcemeta::core::PointerTemplate pointer;
  EXPECT_TRUE(pointer.empty());
}

TEST(JSONPointer_template, empty_false) {
  const sourcemeta::core::Pointer base{"foo"};
  const sourcemeta::core::PointerTemplate pointer{base};
  EXPECT_FALSE(pointer.empty());
}

TEST(JSONPointer_template, size_empty) {
  const sourcemeta::core::PointerTemplate pointer;
  EXPECT_EQ(pointer.size(), 0);
}

TEST(JSONPointer_template, size_one) {
  const sourcemeta::core::Pointer base{"foo"};
  const sourcemeta::core::PointerTemplate pointer{base};
  EXPECT_EQ(pointer.size(), 1);
}

TEST(JSONPointer_template, size_multiple) {
  const sourcemeta::core::Pointer base{"foo", "bar"};
  const sourcemeta::core::PointerTemplate pointer{base};
  EXPECT_EQ(pointer.size(), 2);
}

TEST(JSONPointer_template, size_with_wildcard) {
  sourcemeta::core::PointerTemplate pointer;
  pointer.emplace_back(sourcemeta::core::Pointer::Token{"foo"});
  pointer.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Property);
  pointer.emplace_back(sourcemeta::core::Pointer::Token{"bar"});
  EXPECT_EQ(pointer.size(), 3);
}

TEST(JSONPointer_template, hash_unordered_set) {
  std::unordered_set<sourcemeta::core::PointerTemplate> set;

  sourcemeta::core::PointerTemplate pointer_1;
  pointer_1.emplace_back(sourcemeta::core::Pointer::Token{"foo"});
  pointer_1.emplace_back(sourcemeta::core::Pointer::Token{"bar"});

  sourcemeta::core::PointerTemplate pointer_2;
  pointer_2.emplace_back(sourcemeta::core::Pointer::Token{"baz"});

  set.insert(pointer_1);
  set.insert(pointer_2);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(pointer_1));
  EXPECT_TRUE(set.contains(pointer_2));
}

TEST(JSONPointer_template, hash_unordered_map) {
  std::unordered_map<sourcemeta::core::PointerTemplate, int> map;

  sourcemeta::core::PointerTemplate pointer_1;
  pointer_1.emplace_back(sourcemeta::core::Pointer::Token{"foo"});

  sourcemeta::core::PointerTemplate pointer_2;
  pointer_2.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Property);

  map[pointer_1] = 1;
  map[pointer_2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(pointer_1), 1);
  EXPECT_EQ(map.at(pointer_2), 2);
}

TEST(JSONPointer_template, hash_consistency) {
  sourcemeta::core::PointerTemplate pointer;
  pointer.emplace_back(sourcemeta::core::Pointer::Token{"foo"});
  pointer.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Item);
  pointer.emplace_back(sourcemeta::core::Pointer::Token{0});

  const std::hash<sourcemeta::core::PointerTemplate> hasher;
  const auto hash_1{hasher(pointer)};
  const auto hash_2{hasher(pointer)};

  EXPECT_EQ(hash_1, hash_2);
}

TEST(JSONPointer_template, matches_empty) {
  const sourcemeta::core::PointerTemplate left;
  const sourcemeta::core::PointerTemplate right;
  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_equal_no_conditional_one) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{"foo"}};
  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::Pointer::Token{"foo"}};
  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_equal_no_conditional_many) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{"foo"},
      sourcemeta::core::Pointer::Token{0},
      sourcemeta::core::Pointer::Token{"bar"}};
  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::Pointer::Token{"foo"},
      sourcemeta::core::Pointer::Token{0},
      sourcemeta::core::Pointer::Token{"bar"}};

  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_not_equal_no_conditional_one) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{"foo"}};
  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::Pointer::Token{"fo"}};
  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_not_equal_no_conditional_many) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{"foo"},
      sourcemeta::core::Pointer::Token{0},
      sourcemeta::core::Pointer::Token{"bar"}};
  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::Pointer::Token{"foo"},
      sourcemeta::core::Pointer::Token{0},
      sourcemeta::core::Pointer::Token{"ba"}};

  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_equal_conditional_one) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Condition{}};
  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Condition{}};
  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_equal_conditional_many) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{}};
  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{}};

  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_equal_conditional_different) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{}};
  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{}};

  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_equal_mix_1) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Wildcard{},
      sourcemeta::core::PointerTemplate::Negation{},
      sourcemeta::core::Pointer::Token{"foo"},
      sourcemeta::core::Pointer::Token{0},
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard{},
      sourcemeta::core::PointerTemplate::Negation{},
      sourcemeta::core::Pointer::Token{"foo"},
      sourcemeta::core::Pointer::Token{0},
      sourcemeta::core::PointerTemplate::Condition{}};

  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_equal_mix_2) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{0},
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Wildcard{},
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Negation{},
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::Pointer::Token{0},
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Wildcard{},
      sourcemeta::core::PointerTemplate::Negation{},
      sourcemeta::core::PointerTemplate::Condition{}};

  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_not_equal_mix_1) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Wildcard{},
      sourcemeta::core::PointerTemplate::Negation{},
      sourcemeta::core::Pointer::Token{0},
      sourcemeta::core::Pointer::Token{"foo"},
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Condition{}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard{},
      sourcemeta::core::PointerTemplate::Negation{},
      sourcemeta::core::Pointer::Token{"foo"},
      sourcemeta::core::Pointer::Token{0},
      sourcemeta::core::PointerTemplate::Condition{}};

  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_regex_true) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{"foo"}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Regex{"^f"}};

  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_regex_false) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{"foo"}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Regex{"^b"}};

  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_regex_true_conditional) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::Pointer::Token{"foo"},
      sourcemeta::core::PointerTemplate::Condition{}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Regex{"^f"},
      sourcemeta::core::PointerTemplate::Condition{}};

  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_property_wildcard_property) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{"foo"}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Property};

  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_item_wildcard_property) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{"foo"}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Item};

  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_key_wildcard_property) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{"foo"}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Key};

  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_item_wildcard_index) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{0}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Item};

  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_property_wildcard_index) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{0}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Property};

  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_key_wildcard_index) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{0}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Key};

  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_property_wildcard_multi_token) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::Pointer::Token{"foo"},
      sourcemeta::core::Pointer::Token{"bar"}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Property};

  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_property_wildcard_property_conditional) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::Pointer::Token{"foo"}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Property};

  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_property_wildcard_regex) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Regex{"^f"}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Property};

  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

TEST(JSONPointer_template, matches_item_wildcard_regex) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Regex{"^f"}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Item};

  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_key_wildcard_regex) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Regex{"^f"}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Key};

  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_property_wildcard_negation) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Negation{}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Property};

  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_item_wildcard_negation) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Negation{}};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::PointerTemplate::Wildcard::Item};

  EXPECT_FALSE(left.matches(right));
  EXPECT_FALSE(right.matches(left));
}

TEST(JSONPointer_template, matches_conditional_property_wildcard) {
  const sourcemeta::core::PointerTemplate left{
      sourcemeta::core::PointerTemplate::Condition{},
      sourcemeta::core::PointerTemplate::Wildcard::Property};

  const sourcemeta::core::PointerTemplate right{
      sourcemeta::core::Pointer::Token{"foo"}};

  EXPECT_TRUE(left.matches(right));
  EXPECT_TRUE(right.matches(left));
}

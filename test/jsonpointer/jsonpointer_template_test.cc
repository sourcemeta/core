#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

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

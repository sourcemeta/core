#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

TEST(JSONPointer_json_auto, foo_bar_baz) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::to_json(pointer)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 3);
  EXPECT_TRUE(result.at(0).is_string());
  EXPECT_EQ(result.at(0).to_string(), "foo");
  EXPECT_TRUE(result.at(1).is_string());
  EXPECT_EQ(result.at(1).to_string(), "bar");
  EXPECT_TRUE(result.at(2).is_string());
  EXPECT_EQ(result.at(2).to_string(), "baz");
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(pointer, back.value());
}

TEST(JSONPointer_json_auto, with_index) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  const auto result{sourcemeta::core::to_json(pointer)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 3);
  EXPECT_TRUE(result.at(0).is_string());
  EXPECT_EQ(result.at(0).to_string(), "foo");
  EXPECT_TRUE(result.at(1).is_integer());
  EXPECT_EQ(result.at(1).to_integer(), 1);
  EXPECT_TRUE(result.at(2).is_string());
  EXPECT_EQ(result.at(2).to_string(), "bar");
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(pointer, back.value());
}

TEST(JSONPointer_json_auto, empty_pointer) {
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::to_json(pointer)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 0);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(pointer, back.value());
}

TEST(JSONPointer_json_auto, from_json_invalid_string) {
  const sourcemeta::core::JSON input{"x"};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSONPointer_json_auto, from_json_invalid_type) {
  const sourcemeta::core::JSON input{1};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSONPointer_json_auto, from_json_invalid_element_type) {
  auto input{sourcemeta::core::JSON::make_array()};
  input.push_back(sourcemeta::core::JSON{true});
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSONWeakPointer_json_auto, to_json_foo_bar_baz) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};

  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};

  const auto result{sourcemeta::core::to_json(pointer)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 3);
  EXPECT_TRUE(result.at(0).is_string());
  EXPECT_EQ(result.at(0).to_string(), "foo");
  EXPECT_TRUE(result.at(1).is_string());
  EXPECT_EQ(result.at(1).to_string(), "bar");
  EXPECT_TRUE(result.at(2).is_string());
  EXPECT_EQ(result.at(2).to_string(), "baz");
}

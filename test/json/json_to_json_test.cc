#include <gtest/gtest.h>

#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <sourcemeta/core/json.h>

struct ClassWithCustomMethod {
  auto to_json() const -> sourcemeta::core::JSON {
    return sourcemeta::core::JSON{42};
  }
};

enum class SampleEnum { Foo = 0, Bar = 1, Baz };

TEST(JSON_to_json, class_with_custom_method) {
  const ClassWithCustomMethod value;
  const auto result{sourcemeta::core::to_json(value)};
  const sourcemeta::core::JSON expected{42};
  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, object_hash) {
  const auto value{
      sourcemeta::core::JSON::make_object().as_object().hash("foo")};
  const auto result{sourcemeta::core::to_json(value)};
  EXPECT_TRUE(result.is_array());
  EXPECT_FALSE(result.empty());
}

TEST(JSON_to_json, json) {
  const sourcemeta::core::JSON value{true};
  const auto result{sourcemeta::core::to_json(value)};
  const sourcemeta::core::JSON expected{true};
  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, from_constructor) {
  const auto value{"foo bar"};
  const auto result{sourcemeta::core::to_json(value)};
  const sourcemeta::core::JSON expected{"foo bar"};
  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, enum_class) {
  const SampleEnum value{SampleEnum::Bar};
  const auto result{sourcemeta::core::to_json(value)};
  const sourcemeta::core::JSON expected{1};
  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, optional_with_value) {
  const std::optional<std::string> value{"foo bar"};
  const auto result{sourcemeta::core::to_json(value)};
  const sourcemeta::core::JSON expected{"foo bar"};
  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, optional_without_value) {
  const std::optional<std::string> value;
  const auto result{sourcemeta::core::to_json(value)};
  const sourcemeta::core::JSON expected{nullptr};
  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, vector_with_iterators) {
  const std::vector<std::string> value{"foo", "bar", "baz"};
  const auto result{
      sourcemeta::core::to_json<decltype(value)>(value.cbegin(), value.cend())};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo", "bar", "baz"
  ])JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, vector_with_iterators_and_transform) {
  const std::vector<std::string> value{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::to_json<decltype(value)>(
      value.cbegin(), value.cend(), [](const auto &subvalue) {
        return sourcemeta::core::JSON{subvalue.substr(0, 1)};
      })};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "f", "b", "b"
  ])JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, vector_without_iterators) {
  const std::vector<std::string> value{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo", "bar", "baz"
  ])JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, vector_without_iterators_and_transform) {
  const std::vector<std::string> value{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::to_json(value, [](const auto &subvalue) {
    return sourcemeta::core::JSON{subvalue.substr(0, 1)};
  })};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "f", "b", "b"
  ])JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, map_with_iterators) {
  const std::map<std::string, std::size_t> value{
      {"foo", 1}, {"bar", 2}, {"baz", 3}};
  const auto result{
      sourcemeta::core::to_json<decltype(value)>(value.cbegin(), value.cend())};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "foo": 1,
    "bar": 2,
    "baz": 3
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, map_without_iterators) {
  const std::map<std::string, std::size_t> value{
      {"foo", 1}, {"bar", 2}, {"baz", 3}};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "foo": 1,
    "bar": 2,
    "baz": 3
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, map_without_iterators_and_transform) {
  const std::map<std::string, std::size_t> value{
      {"foo", 1}, {"bar", 2}, {"baz", 3}};
  const auto result{sourcemeta::core::to_json(value, [](const auto subvalue) {
    return sourcemeta::core::JSON{subvalue * 2};
  })};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "foo": 2,
    "bar": 4,
    "baz": 6
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, pair) {
  const std::pair<std::string, std::size_t> value{"foo", 1};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo", 1
  ])JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, pair_with_pair) {
  const std::pair<sourcemeta::core::JSON::Type,
                  std::pair<sourcemeta::core::JSON::Type, std::size_t>>
      value{sourcemeta::core::JSON::Type::String,
            {sourcemeta::core::JSON::Type::String, 5}};

  const auto result{sourcemeta::core::to_json(value)};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON([ 4, [ 4, 5 ] ])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, tuple_1) {
  const std::tuple<std::string> value{"foo"};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo"
  ])JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, tuple_2) {
  const std::tuple<std::string, std::size_t> value{"foo", 1};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo", 1
  ])JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSON_to_json, tuple_3) {
  const std::tuple<std::string, std::size_t, bool> value{"foo", 1, true};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo", 1, true
  ])JSON")};

  EXPECT_EQ(result, expected);
}

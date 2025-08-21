#include <gtest/gtest.h>

#include <filesystem>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#include <sourcemeta/core/json.h>

struct ClassWithCustomMethod {
  auto to_json() const -> sourcemeta::core::JSON {
    return sourcemeta::core::JSON{42};
  }

  static auto from_json(const sourcemeta::core::JSON &)
      -> std::optional<ClassWithCustomMethod> {
    return ClassWithCustomMethod{};
  }

  auto operator==(const ClassWithCustomMethod &) const noexcept
      -> bool = default;
};

enum class SampleEnum { Foo = 0, Bar = 1, Baz };

TEST(JSON_auto, class_with_custom_method) {
  const ClassWithCustomMethod value;
  const auto result{sourcemeta::core::to_json(value)};
  const sourcemeta::core::JSON expected{42};
  EXPECT_EQ(result, expected);
  const auto back{sourcemeta::core::from_json<ClassWithCustomMethod>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, object_hash) {
  const auto value{
      sourcemeta::core::JSON::make_object().as_object().hash("foo")};
  const auto result{sourcemeta::core::to_json(value)};
  EXPECT_TRUE(result.is_array());
  EXPECT_FALSE(result.empty());
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::JSON::Object::hash_type>(
          result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, json) {
  const sourcemeta::core::JSON value{true};
  const auto result{sourcemeta::core::to_json(value)};
  const sourcemeta::core::JSON expected{true};
  EXPECT_EQ(result, expected);
  const auto back{sourcemeta::core::from_json<sourcemeta::core::JSON>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, from_constructor) {
  const auto value{"foo bar"};
  const auto result{sourcemeta::core::to_json(value)};
  const sourcemeta::core::JSON expected{"foo bar"};
  EXPECT_EQ(result, expected);
  const auto back{sourcemeta::core::from_json<std::string>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, enum_class) {
  const SampleEnum value{SampleEnum::Bar};
  const auto result{sourcemeta::core::to_json(value)};
  const sourcemeta::core::JSON expected{1};
  EXPECT_EQ(result, expected);
  const auto back{sourcemeta::core::from_json<SampleEnum>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, optional_with_value) {
  const std::optional<std::string> value{"foo bar"};
  const auto result{sourcemeta::core::to_json(value)};
  const sourcemeta::core::JSON expected{"foo bar"};
  EXPECT_EQ(result, expected);
  const auto back{
      sourcemeta::core::from_json<std::optional<std::string>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, optional_without_value_1) {
  const std::optional<std::string> value;
  const auto result{sourcemeta::core::to_json(value)};
  const auto expected{sourcemeta::core::parse_json(R"JSON(null)JSON")};
  EXPECT_EQ(result, expected);
  const auto back{
      sourcemeta::core::from_json<std::optional<std::string>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, optional_without_value_2) {
  const std::optional<std::size_t> value;
  const auto result{sourcemeta::core::to_json(value)};
  const auto expected{sourcemeta::core::parse_json(R"JSON(null)JSON")};
  EXPECT_EQ(result, expected);
  const auto back{
      sourcemeta::core::from_json<std::optional<std::size_t>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, vector_strings_without_iterators) {
  const std::vector<std::string> value{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo", "bar", "baz"
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{
      sourcemeta::core::from_json<std::vector<std::string>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, vector_paths_without_iterators) {
  const std::vector<std::filesystem::path> value{TEST_DIRECTORY};
  const auto result{sourcemeta::core::to_json(value)};
  EXPECT_EQ(result.size(), 1);
  const auto back{
      sourcemeta::core::from_json<std::vector<std::filesystem::path>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, vector_with_iterators) {
  const std::vector<std::string> value{"foo", "bar", "baz"};
  const auto result{
      sourcemeta::core::to_json<decltype(value)>(value.cbegin(), value.cend())};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo", "bar", "baz"
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{
      sourcemeta::core::from_json<std::vector<std::string>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, vector_with_iterators_and_transform) {
  const std::vector<std::string> value{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::to_json<decltype(value)>(
      value.cbegin(), value.cend(), [](const auto &subvalue) {
        return sourcemeta::core::JSON{std::string{"$"} + subvalue};
      })};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "$foo", "$bar", "$baz"
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{sourcemeta::core::from_json<std::vector<std::string>>(
      result,
      [](const auto &subvalue) { return subvalue.to_string().substr(1); })};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, vector_without_iterators) {
  const std::vector<std::string> value{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo", "bar", "baz"
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{
      sourcemeta::core::from_json<std::vector<std::string>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, vector_without_iterators_and_transform) {
  const std::vector<std::string> value{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::to_json(value, [](const auto &subvalue) {
    return sourcemeta::core::JSON{std::string{"$"} + subvalue};
  })};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "$foo", "$bar", "$baz"
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{sourcemeta::core::from_json<std::vector<std::string>>(
      result,
      [](const auto &subvalue) { return subvalue.to_string().substr(1); })};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, vector_of_pairs) {
  const std::vector<std::pair<std::string, std::size_t>> value{
      {"foo", 1}, {"bar", 2}, {"baz", 3}};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    [ "foo", 1 ],
    [ "bar", 2 ],
    [ "baz", 3 ]
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{sourcemeta::core::from_json<
      std::vector<std::pair<std::string, std::size_t>>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, map_with_iterators) {
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
  const auto back{
      sourcemeta::core::from_json<std::map<std::string, std::size_t>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, map_without_iterators) {
  const std::map<std::string, std::size_t> value{
      {"foo", 1}, {"bar", 2}, {"baz", 3}};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "foo": 1,
    "bar": 2,
    "baz": 3
  })JSON")};

  EXPECT_EQ(result, expected);
  const auto back{
      sourcemeta::core::from_json<std::map<std::string, std::size_t>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, map_without_iterators_and_transform) {
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
  const auto back{
      sourcemeta::core::from_json<std::map<std::string, std::size_t>>(
          result,
          [](const auto &subvalue) { return subvalue.to_integer() / 2; })};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, set_without_iterators) {
  const std::set<std::string> value{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "bar", "baz", "foo"
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{sourcemeta::core::from_json<std::set<std::string>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, unordered_set_without_iterators) {
  const std::unordered_set<std::string> value{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "bar", "baz", "foo"
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{
      sourcemeta::core::from_json<std::unordered_set<std::string>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, unsigned_long_long) {
  const unsigned long long value{15};
  const auto result{sourcemeta::core::to_json(value)};
  const auto expected{sourcemeta::core::JSON{15}};
  EXPECT_EQ(result, expected);
  const auto back{sourcemeta::core::from_json<unsigned long long>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, pair) {
  const std::pair<std::string, std::size_t> value{"foo", 1};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo", 1
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{
      sourcemeta::core::from_json<std::pair<std::string, std::size_t>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, pair_with_pair) {
  const std::pair<sourcemeta::core::JSON::Type,
                  std::pair<sourcemeta::core::JSON::Type, std::size_t>>
      value{sourcemeta::core::JSON::Type::String,
            {sourcemeta::core::JSON::Type::String, 5}};

  const auto result{sourcemeta::core::to_json(value)};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON([ 4, [ 4, 5 ] ])JSON")};
  EXPECT_EQ(result, expected);

  const auto back{sourcemeta::core::from_json<
      std::pair<sourcemeta::core::JSON::Type,
                std::pair<sourcemeta::core::JSON::Type, std::size_t>>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, tuple_1) {
  const std::tuple<std::string> value{"foo"};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo"
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{sourcemeta::core::from_json<std::tuple<std::string>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, tuple_2) {
  const std::tuple<std::string, std::size_t> value{"foo", 1};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo", 1
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{
      sourcemeta::core::from_json<std::tuple<std::string, std::size_t>>(
          result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, tuple_3) {
  const std::tuple<std::string, std::size_t, bool> value{"foo", 1, true};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    "foo", 1, true
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{
      sourcemeta::core::from_json<std::tuple<std::string, std::size_t, bool>>(
          result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, tuple_4) {
  const std::tuple<std::size_t, std::optional<std::size_t>, bool> value{
      1, {}, false};
  const auto result{sourcemeta::core::to_json(value)};

  const auto expected{sourcemeta::core::parse_json(R"JSON([
    1, null, false
  ])JSON")};

  EXPECT_EQ(result, expected);
  const auto back{sourcemeta::core::from_json<
      std::tuple<std::size_t, std::optional<std::size_t>, bool>>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, file_time_1) {
  const std::filesystem::path file{std::filesystem::path{TEST_DIRECTORY} /
                                   "stub_valid.json"};
  EXPECT_TRUE(std::filesystem::exists(file));
  const auto value{std::filesystem::last_write_time(file)};
  const auto result{sourcemeta::core::to_json(value)};
  EXPECT_TRUE(result.is_integer());
  const auto back{
      sourcemeta::core::from_json<std::filesystem::file_time_type>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, file_time_2) {
  const std::filesystem::path file{std::filesystem::path{TEST_DIRECTORY} /
                                   "stub_invalid_1.json"};
  EXPECT_TRUE(std::filesystem::exists(file));
  const auto value{std::filesystem::last_write_time(file)};
  const auto result{sourcemeta::core::to_json(value)};
  EXPECT_TRUE(result.is_integer());
  const auto back{
      sourcemeta::core::from_json<std::filesystem::file_time_type>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, file_time_3) {
  const std::filesystem::path file{std::filesystem::path{TEST_DIRECTORY} /
                                   "stub_invalid_2.json"};
  EXPECT_TRUE(std::filesystem::exists(file));
  const auto value{std::filesystem::last_write_time(file)};
  const auto result{sourcemeta::core::to_json(value)};
  EXPECT_TRUE(result.is_integer());
  const auto back{
      sourcemeta::core::from_json<std::filesystem::file_time_type>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, file_time_4) {
  const std::filesystem::path file{std::filesystem::path{TEST_DIRECTORY} /
                                   "stub_bigint.json"};
  EXPECT_TRUE(std::filesystem::exists(file));
  const auto value{std::filesystem::last_write_time(file)};
  const auto result{sourcemeta::core::to_json(value)};
  EXPECT_TRUE(result.is_integer());
  const auto back{
      sourcemeta::core::from_json<std::filesystem::file_time_type>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, file_time_idempotency) {
  const std::filesystem::path file{std::filesystem::path{TEST_DIRECTORY} /
                                   "stub_valid.json"};
  EXPECT_TRUE(std::filesystem::exists(file));

  const auto value_1{std::filesystem::last_write_time(file)};
  const auto result_1{sourcemeta::core::to_json(value_1)};

  const auto value_2{std::filesystem::last_write_time(file)};
  const auto result_2{sourcemeta::core::to_json(value_2)};

  const auto value_3{std::filesystem::last_write_time(file)};
  const auto result_3{sourcemeta::core::to_json(value_3)};

  EXPECT_EQ(result_1, result_2);
  EXPECT_EQ(result_2, result_3);
  EXPECT_EQ(result_3, result_1);
}

TEST(JSON_auto, filesystem_path) {
  const std::filesystem::path value{TEST_DIRECTORY};
  const auto result{sourcemeta::core::to_json(value)};
  EXPECT_TRUE(result.is_string());
  const auto back{sourcemeta::core::from_json<std::filesystem::path>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(value, back.value());
}

TEST(JSON_auto, from_json_invalid_bool) {
  const sourcemeta::core::JSON document{42};
  const auto result{sourcemeta::core::from_json<bool>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_integer) {
  const sourcemeta::core::JSON document{true};
  const auto result{sourcemeta::core::from_json<int>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_hash_1) {
  const sourcemeta::core::JSON document{true};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::JSON::Object::hash_type>(
          document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_hash_2) {
  const auto document{sourcemeta::core::parse_json(R"JSON([
    "foo", "bar", "baz"
  ])JSON")};

  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::JSON::Object::hash_type>(
          document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_hash_3) {
  const auto document{sourcemeta::core::parse_json(R"JSON([
    "foo", "bar", "baz", "qux"
  ])JSON")};

  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::JSON::Object::hash_type>(
          document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_string) {
  const sourcemeta::core::JSON document{true};
  const auto result{sourcemeta::core::from_json<std::string>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_enum) {
  const sourcemeta::core::JSON document{true};
  const auto result{sourcemeta::core::from_json<SampleEnum>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_optional) {
  const sourcemeta::core::JSON document{true};
  const auto result{sourcemeta::core::from_json<std::optional<int>>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_vector) {
  const sourcemeta::core::JSON document{true};
  const auto result{sourcemeta::core::from_json<std::vector<bool>>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_vector_transform) {
  const auto document{sourcemeta::core::parse_json(R"JSON([
    "foo", "bar", "baz", true
  ])JSON")};

  const auto result{sourcemeta::core::from_json<std::vector<std::string>>(
      document, [](const auto &entry) {
        return sourcemeta::core::from_json<std::string>(entry);
      })};

  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_map) {
  const sourcemeta::core::JSON document{true};
  const auto result{
      sourcemeta::core::from_json<std::map<std::string, bool>>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_map_transform) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": 1,
    "bar": true
  })JSON")};

  const auto result{sourcemeta::core::from_json<std::map<std::string, int>>(
      document, [](const auto &entry) {
        return sourcemeta::core::from_json<int>(entry);
      })};

  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_pair_type) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "foo": "bar", "baz": 2 })JSON")};
  const auto result{
      sourcemeta::core::from_json<std::pair<std::string, int>>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_pair_size) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1 ])JSON")};
  const auto result{
      sourcemeta::core::from_json<std::pair<std::string, int>>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_pair_first) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2 ])JSON")};
  const auto result{
      sourcemeta::core::from_json<std::pair<std::string, int>>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_pair_second) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ "foo", "bar" ])JSON")};
  const auto result{
      sourcemeta::core::from_json<std::pair<std::string, int>>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_tuple_mono_type) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "foo": "bar" })JSON")};
  const auto result{
      sourcemeta::core::from_json<std::tuple<std::string>>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_tuple_mono_size) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ "foo", "bar" ])JSON")};
  const auto result{
      sourcemeta::core::from_json<std::tuple<std::string>>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_tuple_mono_element) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ "foo" ])JSON")};
  const auto result{sourcemeta::core::from_json<std::tuple<int>>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_tuple_poly_type) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON({ "foo": "bar", "baz": "qux" })JSON")};
  const auto result{
      sourcemeta::core::from_json<std::tuple<std::string, std::string>>(
          document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_tuple_poly_size) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ "foo" ])JSON")};
  const auto result{
      sourcemeta::core::from_json<std::tuple<std::string, std::string>>(
          document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSON_auto, from_json_invalid_tuple_poly_element) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ "foo", 1 ])JSON")};
  const auto result{
      sourcemeta::core::from_json<std::tuple<std::string, std::string>>(
          document)};
  EXPECT_FALSE(result.has_value());
}

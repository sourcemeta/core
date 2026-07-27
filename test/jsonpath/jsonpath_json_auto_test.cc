#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpath.h>
#include <sourcemeta/core/test.h>

TEST(jsonpath_to_json_single_name) {
  const sourcemeta::core::JSONPath path{"$.foo"};
  const auto result{sourcemeta::core::to_json(path)};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "$.foo");
}

TEST(jsonpath_to_json_filter_expression) {
  const sourcemeta::core::JSONPath path{
      "$..books[?@.price < 10 && match(@.category, 'fic.*')]"};
  const auto result{sourcemeta::core::to_json(path)};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(),
            "$..books[?@.price < 10 && match(@.category, 'fic.*')]");
}

TEST(jsonpath_from_json_valid) {
  const sourcemeta::core::JSON input{"$.foo[0]"};
  const auto path{
      sourcemeta::core::from_json<sourcemeta::core::JSONPath>(input)};
  EXPECT_TRUE(path.has_value());
  const auto result{sourcemeta::core::to_json(path.value())};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "$.foo[0]");
}

TEST(jsonpath_from_json_round_trip_evaluate) {
  const sourcemeta::core::JSONPath path{"$.foo[0]"};
  const auto back{sourcemeta::core::from_json<sourcemeta::core::JSONPath>(
      sourcemeta::core::to_json(path))};
  EXPECT_TRUE(back.has_value());
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "foo": [ 1, 2 ] })JSON")};
  std::size_t count{0};
  back.value().evaluate(
      document,
      [&count](const sourcemeta::core::JSON &value,
               const sourcemeta::core::WeakPointer &location) -> void {
        EXPECT_TRUE(value.is_integer());
        EXPECT_EQ(value.to_integer(), 1);
        EXPECT_EQ(location.size(), 2);
        count += 1;
      });
  EXPECT_EQ(count, 1);
}

TEST(jsonpath_from_json_invalid_type_integer) {
  const sourcemeta::core::JSON input{1};
  const auto path{
      sourcemeta::core::from_json<sourcemeta::core::JSONPath>(input)};
  EXPECT_FALSE(path.has_value());
}

TEST(jsonpath_from_json_invalid_type_array) {
  const auto input{sourcemeta::core::parse_json(R"JSON([ "$.foo" ])JSON")};
  const auto path{
      sourcemeta::core::from_json<sourcemeta::core::JSONPath>(input)};
  EXPECT_FALSE(path.has_value());
}

TEST(jsonpath_from_json_invalid_expression) {
  const sourcemeta::core::JSON input{"foo"};
  const auto path{
      sourcemeta::core::from_json<sourcemeta::core::JSONPath>(input)};
  EXPECT_FALSE(path.has_value());
}

TEST(jsonpath_from_json_empty_string) {
  const sourcemeta::core::JSON input{""};
  const auto path{
      sourcemeta::core::from_json<sourcemeta::core::JSONPath>(input)};
  EXPECT_FALSE(path.has_value());
}

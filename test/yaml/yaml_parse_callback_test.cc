#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/yaml.h>

#include <sstream>
#include <tuple>
#include <vector>

#define PARSE_YAML_WITH_TRACES(result, input, expected_size)                   \
  std::vector<std::tuple<sourcemeta::core::JSON::ParsePhase,                   \
                         sourcemeta::core::JSON::Type, std::uint64_t,          \
                         std::uint64_t, sourcemeta::core::JSON>>               \
      traces;                                                                  \
  const auto result{sourcemeta::core::parse_yaml(                              \
      input, [&traces](const sourcemeta::core::JSON::ParsePhase phase,         \
                       const sourcemeta::core::JSON::Type type,                \
                       const std::uint64_t line, const std::uint64_t column,   \
                       const sourcemeta::core::JSON &value) {                  \
        traces.emplace_back(phase, type, line, column, value);                 \
      })};                                                                     \
  EXPECT_EQ(traces.size(), expected_size)

#define READ_YAML_OR_JSON_WITH_TRACES(result, input, expected_size)            \
  std::vector<std::tuple<sourcemeta::core::JSON::ParsePhase,                   \
                         sourcemeta::core::JSON::Type, std::uint64_t,          \
                         std::uint64_t, sourcemeta::core::JSON>>               \
      traces;                                                                  \
  const auto result{sourcemeta::core::read_yaml_or_json(                       \
      input, [&traces](const sourcemeta::core::JSON::ParsePhase phase,         \
                       const sourcemeta::core::JSON::Type type,                \
                       const std::uint64_t line, const std::uint64_t column,   \
                       const sourcemeta::core::JSON &value) {                  \
        traces.emplace_back(phase, type, line, column, value);                 \
      })};                                                                     \
  EXPECT_EQ(traces.size(), expected_size)

#define EXPECT_TRACE(index, expected_phase, expected_type, expected_line,      \
                     expected_column, expected_value)                          \
  EXPECT_EQ(std::get<0>(traces.at(index)),                                     \
            sourcemeta::core::JSON::ParsePhase::expected_phase);               \
  EXPECT_EQ(std::get<1>(traces.at(index)),                                     \
            sourcemeta::core::JSON::Type::expected_type);                      \
  EXPECT_EQ(std::get<2>(traces.at(index)), expected_line);                     \
  EXPECT_EQ(std::get<3>(traces.at(index)), expected_column);                   \
  EXPECT_EQ(std::get<4>(traces.at(index)), expected_value);

TEST(YAML_parse_callback, yaml_true) {
  const auto input{"true"};
  PARSE_YAML_WITH_TRACES(document, input, 2);
  EXPECT_TRACE(0, Pre, Boolean, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Post, Boolean, 1, 4, sourcemeta::core::JSON{true});
}

TEST(YAML_parse_callback, yaml_false) {
  const auto input{"false"};
  PARSE_YAML_WITH_TRACES(document, input, 2);
  EXPECT_TRACE(0, Pre, Boolean, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Post, Boolean, 1, 5, sourcemeta::core::JSON{false});
}

TEST(YAML_parse_callback, yaml_null) {
  const auto input{"null"};
  PARSE_YAML_WITH_TRACES(document, input, 2);
  EXPECT_TRACE(0, Pre, Null, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Post, Null, 1, 4, sourcemeta::core::JSON{nullptr});
}

TEST(YAML_parse_callback, yaml_string) {
  const auto input{"\"foo\""};
  PARSE_YAML_WITH_TRACES(document, input, 2);
  EXPECT_TRACE(0, Pre, String, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Post, String, 1, 5, sourcemeta::core::JSON{"foo"});
}

TEST(YAML_parse_callback, yaml_integer) {
  const auto input{"1234"};
  PARSE_YAML_WITH_TRACES(document, input, 2);
  EXPECT_TRACE(0, Pre, Integer, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Post, Integer, 1, 4, sourcemeta::core::JSON{1234});
}

TEST(YAML_parse_callback, yaml_real) {
  const auto input{"3.14"};
  PARSE_YAML_WITH_TRACES(document, input, 2);
  EXPECT_TRACE(0, Pre, Real, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Post, Real, 1, 4, sourcemeta::core::JSON{3.14});
}

TEST(YAML_parse_callback, yaml_empty_array) {
  const auto input{"[]"};
  PARSE_YAML_WITH_TRACES(document, input, 2);
  EXPECT_TRACE(0, Pre, Array, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Post, Array, 1, 2, sourcemeta::core::JSON::make_array());
}

TEST(YAML_parse_callback, yaml_array_integers) {
  const auto input{"[1, 2, 3]"};
  PARSE_YAML_WITH_TRACES(document, input, 8);
  EXPECT_TRACE(0, Pre, Array, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Integer, 1, 2, sourcemeta::core::JSON{0});
  EXPECT_TRACE(2, Post, Integer, 1, 2, sourcemeta::core::JSON{1});
  EXPECT_TRACE(3, Pre, Integer, 1, 5, sourcemeta::core::JSON{1});
  EXPECT_TRACE(4, Post, Integer, 1, 5, sourcemeta::core::JSON{2});
  EXPECT_TRACE(5, Pre, Integer, 1, 8, sourcemeta::core::JSON{2});
  EXPECT_TRACE(6, Post, Integer, 1, 8, sourcemeta::core::JSON{3});
  EXPECT_TRACE(7, Post, Array, 1, 9, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_empty_object) {
  const auto input{"{}"};
  PARSE_YAML_WITH_TRACES(document, input, 2);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Post, Object, 1, 2, sourcemeta::core::JSON::make_object());
}

TEST(YAML_parse_callback, yaml_object_simple) {
  const auto input{"{\"foo\": \"bar\"}"};
  PARSE_YAML_WITH_TRACES(document, input, 4);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, String, 1, 2, sourcemeta::core::JSON{"foo"});
  EXPECT_TRACE(2, Post, String, 1, 13, sourcemeta::core::JSON{"bar"});
  EXPECT_TRACE(3, Post, Object, 1, 14, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_block_mapping_multiline) {
  const auto input{"foo: bar\nbaz: qux\ntest: 123"};
  PARSE_YAML_WITH_TRACES(document, input, 8);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, String, 1, 1, sourcemeta::core::JSON{"foo"});
  EXPECT_TRACE(2, Post, String, 1, 8, sourcemeta::core::JSON{"bar"});
  EXPECT_TRACE(3, Pre, String, 2, 1, sourcemeta::core::JSON{"baz"});
  EXPECT_TRACE(4, Post, String, 2, 8, sourcemeta::core::JSON{"qux"});
  EXPECT_TRACE(5, Pre, Integer, 3, 1, sourcemeta::core::JSON{"test"});
  EXPECT_TRACE(6, Post, Integer, 3, 9, sourcemeta::core::JSON{123});
  EXPECT_TRACE(7, Post, Object, 4, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_block_sequence_multiline) {
  const auto input{"- foo\n- bar\n- baz"};
  PARSE_YAML_WITH_TRACES(document, input, 8);
  EXPECT_TRACE(0, Pre, Array, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, String, 1, 3, sourcemeta::core::JSON{0});
  EXPECT_TRACE(2, Post, String, 1, 5, sourcemeta::core::JSON{"foo"});
  EXPECT_TRACE(3, Pre, String, 2, 3, sourcemeta::core::JSON{1});
  EXPECT_TRACE(4, Post, String, 2, 5, sourcemeta::core::JSON{"bar"});
  EXPECT_TRACE(5, Pre, String, 3, 3, sourcemeta::core::JSON{2});
  EXPECT_TRACE(6, Post, String, 3, 5, sourcemeta::core::JSON{"baz"});
  EXPECT_TRACE(7, Post, Array, 4, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_nested_arrays) {
  const auto input{"[[1, 2], [3, 4]]"};
  PARSE_YAML_WITH_TRACES(document, input, 14);
  EXPECT_TRACE(0, Pre, Array, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Array, 1, 2, sourcemeta::core::JSON{0});
  EXPECT_TRACE(2, Pre, Integer, 1, 3, sourcemeta::core::JSON{0});
  EXPECT_TRACE(3, Post, Integer, 1, 3, sourcemeta::core::JSON{1});
  EXPECT_TRACE(4, Pre, Integer, 1, 6, sourcemeta::core::JSON{1});
  EXPECT_TRACE(5, Post, Integer, 1, 6, sourcemeta::core::JSON{2});
  EXPECT_TRACE(6, Post, Array, 1, 7, sourcemeta::core::parse_yaml("[1, 2]"));
  EXPECT_TRACE(7, Pre, Array, 1, 10, sourcemeta::core::JSON{1});
  EXPECT_TRACE(8, Pre, Integer, 1, 11, sourcemeta::core::JSON{0});
  EXPECT_TRACE(9, Post, Integer, 1, 11, sourcemeta::core::JSON{3});
  EXPECT_TRACE(10, Pre, Integer, 1, 14, sourcemeta::core::JSON{1});
  EXPECT_TRACE(11, Post, Integer, 1, 14, sourcemeta::core::JSON{4});
  EXPECT_TRACE(12, Post, Array, 1, 15, sourcemeta::core::parse_yaml("[3, 4]"));
  EXPECT_TRACE(13, Post, Array, 1, 16, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_nested_objects) {
  const auto input{"{\"outer\": {\"inner\": \"value\"}}"};
  PARSE_YAML_WITH_TRACES(document, input, 6);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Object, 1, 2, sourcemeta::core::JSON{"outer"});
  EXPECT_TRACE(2, Pre, String, 1, 12, sourcemeta::core::JSON{"inner"});
  EXPECT_TRACE(3, Post, String, 1, 27, sourcemeta::core::JSON{"value"});
  EXPECT_TRACE(4, Post, Object, 1, 28,
               sourcemeta::core::parse_yaml("{\"inner\": \"value\"}"));
  EXPECT_TRACE(5, Post, Object, 1, 29, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_block_nested_objects) {
  const auto input{"outer:\n  inner:\n    deep: value"};
  PARSE_YAML_WITH_TRACES(document, input, 8);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Object, 1, 1, sourcemeta::core::JSON{"outer"});
  EXPECT_TRACE(2, Pre, Object, 2, 3, sourcemeta::core::JSON{"inner"});
  EXPECT_TRACE(3, Pre, String, 3, 5, sourcemeta::core::JSON{"deep"});
  EXPECT_TRACE(4, Post, String, 3, 15, sourcemeta::core::JSON{"value"});
  EXPECT_TRACE(5, Post, Object, 4, 0,
               sourcemeta::core::parse_yaml("deep: value"));
  EXPECT_TRACE(6, Post, Object, 4, 0,
               sourcemeta::core::parse_yaml("inner:\n  deep: value"));
  EXPECT_TRACE(7, Post, Object, 4, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_block_nested_arrays) {
  const auto input{"- - foo\n  - bar\n- - baz"};
  PARSE_YAML_WITH_TRACES(document, input, 12);
  EXPECT_TRACE(0, Pre, Array, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Array, 1, 3, sourcemeta::core::JSON{0});
  EXPECT_TRACE(2, Pre, String, 1, 5, sourcemeta::core::JSON{0});
  EXPECT_TRACE(3, Post, String, 1, 7, sourcemeta::core::JSON{"foo"});
  EXPECT_TRACE(4, Pre, String, 2, 5, sourcemeta::core::JSON{1});
  EXPECT_TRACE(5, Post, String, 2, 7, sourcemeta::core::JSON{"bar"});
  EXPECT_TRACE(6, Post, Array, 3, 0,
               sourcemeta::core::parse_yaml("- foo\n- bar"));
  EXPECT_TRACE(7, Pre, Array, 3, 3, sourcemeta::core::JSON{1});
  EXPECT_TRACE(8, Pre, String, 3, 5, sourcemeta::core::JSON{0});
  EXPECT_TRACE(9, Post, String, 3, 7, sourcemeta::core::JSON{"baz"});
  EXPECT_TRACE(10, Post, Array, 4, 0, sourcemeta::core::parse_yaml("- baz"));
  EXPECT_TRACE(11, Post, Array, 4, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_mixed_block_and_flow) {
  const auto input{"array:\n  - {key: value}\n  - [1, 2, 3]"};
  PARSE_YAML_WITH_TRACES(document, input, 16);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Array, 1, 1, sourcemeta::core::JSON{"array"});
  EXPECT_TRACE(2, Pre, Object, 2, 5, sourcemeta::core::JSON{0});
  EXPECT_TRACE(3, Pre, String, 2, 6, sourcemeta::core::JSON{"key"});
  EXPECT_TRACE(4, Post, String, 2, 15, sourcemeta::core::JSON{"value"});
  EXPECT_TRACE(5, Post, Object, 2, 16,
               sourcemeta::core::parse_yaml("{key: value}"));
  EXPECT_TRACE(6, Pre, Array, 3, 5, sourcemeta::core::JSON{1});
  EXPECT_TRACE(7, Pre, Integer, 3, 6, sourcemeta::core::JSON{0});
  EXPECT_TRACE(8, Post, Integer, 3, 6, sourcemeta::core::JSON{1});
  EXPECT_TRACE(9, Pre, Integer, 3, 9, sourcemeta::core::JSON{1});
  EXPECT_TRACE(10, Post, Integer, 3, 9, sourcemeta::core::JSON{2});
  EXPECT_TRACE(11, Pre, Integer, 3, 12, sourcemeta::core::JSON{2});
  EXPECT_TRACE(12, Post, Integer, 3, 12, sourcemeta::core::JSON{3});
  EXPECT_TRACE(13, Post, Array, 3, 13,
               sourcemeta::core::parse_yaml("[1, 2, 3]"));
  EXPECT_TRACE(14, Post, Array, 4, 0,
               sourcemeta::core::parse_yaml("- {key: value}\n- [1, 2, 3]"));
  EXPECT_TRACE(15, Post, Object, 4, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_quoted_strings) {
  const auto input{"single: 'foo bar'\ndouble: \"baz qux\""};
  PARSE_YAML_WITH_TRACES(document, input, 6);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, String, 1, 1, sourcemeta::core::JSON{"single"});
  EXPECT_TRACE(2, Post, String, 1, 17, sourcemeta::core::JSON{"foo bar"});
  EXPECT_TRACE(3, Pre, String, 2, 1, sourcemeta::core::JSON{"double"});
  EXPECT_TRACE(4, Post, String, 2, 17, sourcemeta::core::JSON{"baz qux"});
  EXPECT_TRACE(5, Post, Object, 3, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_unquoted_strings) {
  const auto input{"plain: hello world\nunquoted: test"};
  PARSE_YAML_WITH_TRACES(document, input, 6);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, String, 1, 1, sourcemeta::core::JSON{"plain"});
  EXPECT_TRACE(2, Post, String, 1, 18, sourcemeta::core::JSON{"hello world"});
  EXPECT_TRACE(3, Pre, String, 2, 1, sourcemeta::core::JSON{"unquoted"});
  EXPECT_TRACE(4, Post, String, 2, 14, sourcemeta::core::JSON{"test"});
  EXPECT_TRACE(5, Post, Object, 3, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_numbers_various_formats) {
  const auto input{
      "decimal: 42\nfloat: 3.14\nnegative: -10\nexponential: 1e10"};
  PARSE_YAML_WITH_TRACES(document, input, 10);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Integer, 1, 1, sourcemeta::core::JSON{"decimal"});
  EXPECT_TRACE(2, Post, Integer, 1, 11, sourcemeta::core::JSON{42});
  EXPECT_TRACE(3, Pre, Real, 2, 1, sourcemeta::core::JSON{"float"});
  EXPECT_TRACE(4, Post, Real, 2, 11, sourcemeta::core::JSON{3.14});
  EXPECT_TRACE(5, Pre, Integer, 3, 1, sourcemeta::core::JSON{"negative"});
  EXPECT_TRACE(6, Post, Integer, 3, 13, sourcemeta::core::JSON{-10});
  EXPECT_TRACE(7, Pre, Real, 4, 1, sourcemeta::core::JSON{"exponential"});
  EXPECT_TRACE(8, Post, Real, 4, 17, sourcemeta::core::JSON{1e10});
  EXPECT_TRACE(9, Post, Object, 5, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_booleans_all_forms) {
  const auto input{"bool_true: true\nbool_false: false"};
  PARSE_YAML_WITH_TRACES(document, input, 6);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Boolean, 1, 1, sourcemeta::core::JSON{"bool_true"});
  EXPECT_TRACE(2, Post, Boolean, 1, 15, sourcemeta::core::JSON{true});
  EXPECT_TRACE(3, Pre, Boolean, 2, 1, sourcemeta::core::JSON{"bool_false"});
  EXPECT_TRACE(4, Post, Boolean, 2, 17, sourcemeta::core::JSON{false});
  EXPECT_TRACE(5, Post, Object, 3, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_null_values) {
  const auto input{"explicit_null: null\n"};
  PARSE_YAML_WITH_TRACES(document, input, 4);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Null, 1, 1, sourcemeta::core::JSON{"explicit_null"});
  EXPECT_TRACE(2, Post, Null, 1, 19, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(3, Post, Object, 2, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_empty_values) {
  const auto input{"empty_key:"};
  PARSE_YAML_WITH_TRACES(document, input, 4);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Null, 1, 1, sourcemeta::core::JSON{"empty_key"});
  EXPECT_TRACE(2, Post, Null, 1, 10, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(3, Post, Object, 2, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_simple_multiline_object) {
  const auto input{"name: Alice\nage: 30"};
  PARSE_YAML_WITH_TRACES(document, input, 6);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, String, 1, 1, sourcemeta::core::JSON{"name"});
  EXPECT_TRACE(2, Post, String, 1, 11, sourcemeta::core::JSON{"Alice"});
  EXPECT_TRACE(3, Pre, Integer, 2, 1, sourcemeta::core::JSON{"age"});
  EXPECT_TRACE(4, Post, Integer, 2, 7, sourcemeta::core::JSON{30});
  EXPECT_TRACE(5, Post, Object, 3, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_array_of_objects) {
  const auto input{"[\n  {id: 1, name: foo},\n  {id: 2, name: bar}\n]"};
  PARSE_YAML_WITH_TRACES(document, input, 14);
  EXPECT_TRACE(0, Pre, Array, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Object, 2, 3, sourcemeta::core::JSON{0});
  EXPECT_TRACE(2, Pre, Integer, 2, 4, sourcemeta::core::JSON{"id"});
  EXPECT_TRACE(3, Post, Integer, 2, 8, sourcemeta::core::JSON{1});
  EXPECT_TRACE(4, Pre, String, 2, 11, sourcemeta::core::JSON{"name"});
  EXPECT_TRACE(5, Post, String, 2, 19, sourcemeta::core::JSON{"foo"});
  EXPECT_TRACE(6, Post, Object, 2, 20,
               sourcemeta::core::parse_yaml("{id: 1, name: foo}"));
  EXPECT_TRACE(7, Pre, Object, 3, 3, sourcemeta::core::JSON{1});
  EXPECT_TRACE(8, Pre, Integer, 3, 4, sourcemeta::core::JSON{"id"});
  EXPECT_TRACE(9, Post, Integer, 3, 8, sourcemeta::core::JSON{2});
  EXPECT_TRACE(10, Pre, String, 3, 11, sourcemeta::core::JSON{"name"});
  EXPECT_TRACE(11, Post, String, 3, 19, sourcemeta::core::JSON{"bar"});
  EXPECT_TRACE(12, Post, Object, 3, 20,
               sourcemeta::core::parse_yaml("{id: 2, name: bar}"));
  EXPECT_TRACE(13, Post, Array, 4, 1, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_deeply_nested) {
  const auto input{"{a: {b: {c: {d: {e: 42}}}}}"};
  PARSE_YAML_WITH_TRACES(document, input, 12);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Object, 1, 2, sourcemeta::core::JSON{"a"});
  EXPECT_TRACE(2, Pre, Object, 1, 6, sourcemeta::core::JSON{"b"});
  EXPECT_TRACE(3, Pre, Object, 1, 10, sourcemeta::core::JSON{"c"});
  EXPECT_TRACE(4, Pre, Object, 1, 14, sourcemeta::core::JSON{"d"});
  EXPECT_TRACE(5, Pre, Integer, 1, 18, sourcemeta::core::JSON{"e"});
  EXPECT_TRACE(6, Post, Integer, 1, 22, sourcemeta::core::JSON{42});
  EXPECT_TRACE(7, Post, Object, 1, 23, sourcemeta::core::parse_yaml("{e: 42}"));
  EXPECT_TRACE(8, Post, Object, 1, 24,
               sourcemeta::core::parse_yaml("{d: {e: 42}}"));
  EXPECT_TRACE(9, Post, Object, 1, 25,
               sourcemeta::core::parse_yaml("{c: {d: {e: 42}}}"));
  EXPECT_TRACE(10, Post, Object, 1, 26,
               sourcemeta::core::parse_yaml("{b: {c: {d: {e: 42}}}}"));
  EXPECT_TRACE(11, Post, Object, 1, 27, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_or_json_stub_test_1) {
  const auto input{std::filesystem::path{STUBS_PATH} / "test_1.yaml"};
  READ_YAML_OR_JSON_WITH_TRACES(document, input, 6);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, String, 1, 1, sourcemeta::core::JSON{"foo"});
  EXPECT_TRACE(2, Post, String, 1, 8, sourcemeta::core::JSON{"bar"});
  EXPECT_TRACE(3, Pre, Integer, 2, 1, sourcemeta::core::JSON{"baz"});
  EXPECT_TRACE(4, Post, Integer, 2, 6, sourcemeta::core::JSON{2});
  EXPECT_TRACE(5, Post, Object, 3, 0, sourcemeta::core::read_yaml(input));
}

TEST(YAML_parse_callback, yaml_or_json_stub_test_2) {
  const auto input{std::filesystem::path{STUBS_PATH} / "test_1.json"};
  READ_YAML_OR_JSON_WITH_TRACES(document, input, 6);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, String, 2, 3, sourcemeta::core::JSON{"foo"});
  EXPECT_TRACE(2, Post, String, 2, 14, sourcemeta::core::JSON{"bar"});
  EXPECT_TRACE(3, Pre, Integer, 3, 3, sourcemeta::core::JSON{"baz"});
  EXPECT_TRACE(4, Post, Integer, 3, 10, sourcemeta::core::JSON{2});
  EXPECT_TRACE(5, Post, Object, 4, 1, sourcemeta::core::read_json(input));
}

TEST(YAML_parse_callback, yaml_simple_anchor_and_alias) {
  const auto input{"anchor: &node foo\nalias: *node"};
  PARSE_YAML_WITH_TRACES(document, input, 5);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, String, 1, 1, sourcemeta::core::JSON{"anchor"});
  EXPECT_TRACE(2, Post, String, 1, 17, sourcemeta::core::JSON{"foo"});
  // Replayed Post callback for alias with original position
  EXPECT_TRACE(3, Post, String, 1, 17, sourcemeta::core::JSON{"foo"});
  EXPECT_TRACE(4, Post, Object, 3, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_anchor_object_with_alias) {
  const auto input{"original: &obj {x: 1, y: 2}\ncopy: *obj"};
  PARSE_YAML_WITH_TRACES(document, input, 12);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Object, 1, 1, sourcemeta::core::JSON{"original"});
  EXPECT_TRACE(2, Pre, Integer, 1, 17, sourcemeta::core::JSON{"x"});
  EXPECT_TRACE(3, Post, Integer, 1, 20, sourcemeta::core::JSON{1});
  EXPECT_TRACE(4, Pre, Integer, 1, 23, sourcemeta::core::JSON{"y"});
  EXPECT_TRACE(5, Post, Integer, 1, 26, sourcemeta::core::JSON{2});
  EXPECT_TRACE(6, Post, Object, 1, 27,
               sourcemeta::core::parse_yaml("{x: 1, y: 2}"));
  // Alias replay - internal structure only
  EXPECT_TRACE(7, Pre, Integer, 1, 17, sourcemeta::core::JSON{"x"});
  EXPECT_TRACE(8, Post, Integer, 1, 20, sourcemeta::core::JSON{1});
  EXPECT_TRACE(9, Pre, Integer, 1, 23, sourcemeta::core::JSON{"y"});
  EXPECT_TRACE(10, Post, Integer, 1, 26, sourcemeta::core::JSON{2});
  EXPECT_TRACE(11, Post, Object, 3, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_anchor_array_with_multiple_aliases) {
  const auto input{"items: &list [a, b, c]\nfirst: *list\nsecond: *list"};
  PARSE_YAML_WITH_TRACES(document, input, 22);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Array, 1, 1, sourcemeta::core::JSON{"items"});
  EXPECT_TRACE(2, Pre, String, 1, 15, sourcemeta::core::JSON{0});
  EXPECT_TRACE(3, Post, String, 1, 15, sourcemeta::core::JSON{"a"});
  EXPECT_TRACE(4, Pre, String, 1, 18, sourcemeta::core::JSON{1});
  EXPECT_TRACE(5, Post, String, 1, 18, sourcemeta::core::JSON{"b"});
  EXPECT_TRACE(6, Pre, String, 1, 21, sourcemeta::core::JSON{2});
  EXPECT_TRACE(7, Post, String, 1, 21, sourcemeta::core::JSON{"c"});
  EXPECT_TRACE(8, Post, Array, 1, 22,
               sourcemeta::core::parse_yaml("[a, b, c]"));
  // Replayed internal structure for "first"
  EXPECT_TRACE(9, Pre, String, 1, 15, sourcemeta::core::JSON{0});
  EXPECT_TRACE(10, Post, String, 1, 15, sourcemeta::core::JSON{"a"});
  EXPECT_TRACE(11, Pre, String, 1, 18, sourcemeta::core::JSON{1});
  EXPECT_TRACE(12, Post, String, 1, 18, sourcemeta::core::JSON{"b"});
  EXPECT_TRACE(13, Pre, String, 1, 21, sourcemeta::core::JSON{2});
  EXPECT_TRACE(14, Post, String, 1, 21, sourcemeta::core::JSON{"c"});
  // Replayed internal structure for "second"
  EXPECT_TRACE(15, Pre, String, 1, 15, sourcemeta::core::JSON{0});
  EXPECT_TRACE(16, Post, String, 1, 15, sourcemeta::core::JSON{"a"});
  EXPECT_TRACE(17, Pre, String, 1, 18, sourcemeta::core::JSON{1});
  EXPECT_TRACE(18, Post, String, 1, 18, sourcemeta::core::JSON{"b"});
  EXPECT_TRACE(19, Pre, String, 1, 21, sourcemeta::core::JSON{2});
  EXPECT_TRACE(20, Post, String, 1, 21, sourcemeta::core::JSON{"c"});
  EXPECT_TRACE(21, Post, Object, 4, 0, sourcemeta::core::parse_yaml(input));
}

TEST(YAML_parse_callback, yaml_nested_anchor_and_alias) {
  const auto input{"outer:\n  inner: &val 42\n  ref: *val"};
  PARSE_YAML_WITH_TRACES(document, input, 7);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, Object, 1, 1, sourcemeta::core::JSON{"outer"});
  EXPECT_TRACE(2, Pre, Integer, 2, 3, sourcemeta::core::JSON{"inner"});
  EXPECT_TRACE(3, Post, Integer, 2, 16, sourcemeta::core::JSON{42});
  // Replayed Post callback for alias with original position
  EXPECT_TRACE(4, Post, Integer, 2, 16, sourcemeta::core::JSON{42});
  EXPECT_TRACE(5, Post, Object, 4, 0,
               sourcemeta::core::parse_yaml("inner: &val 42\nref: *val"));
  EXPECT_TRACE(6, Post, Object, 4, 0, sourcemeta::core::parse_yaml(input));
}

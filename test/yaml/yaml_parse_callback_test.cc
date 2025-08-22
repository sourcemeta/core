#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/yaml.h>

#include <sstream>
#include <tuple>
#include <vector>

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

TEST(YAML_parse_callback, yaml_or_json_stub_test_1) {
  const auto input{std::filesystem::path{STUBS_PATH} / "test_1.yaml"};
  READ_YAML_OR_JSON_WITH_TRACES(document, input, 0);
}

TEST(YAML_parse_callback, yaml_or_json_stub_test_2) {
  const auto input{std::filesystem::path{STUBS_PATH} / "test_1.json"};
  READ_YAML_OR_JSON_WITH_TRACES(document, input, 6);
  EXPECT_TRACE(0, Pre, Object, 1, 1, sourcemeta::core::JSON{nullptr});
  EXPECT_TRACE(1, Pre, String, 2, 10, sourcemeta::core::JSON{"foo"});
  EXPECT_TRACE(2, Post, String, 2, 14, sourcemeta::core::JSON{"bar"});
  EXPECT_TRACE(3, Pre, Integer, 3, 10, sourcemeta::core::JSON{"baz"});
  EXPECT_TRACE(4, Post, Integer, 3, 10, sourcemeta::core::JSON{2});
  EXPECT_TRACE(5, Post, Object, 4, 1, sourcemeta::core::read_json(input));
}

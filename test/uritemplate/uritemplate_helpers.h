#ifndef SOURCEMETA_CORE_URITEMPLATE_TEST_HELPERS_H_
#define SOURCEMETA_CORE_URITEMPLATE_TEST_HELPERS_H_

#include <cstdint> // std::uint8_t
#include <string>  // std::string
#include <tuple>   // std::tuple
#include <vector>  // std::vector

#define EXPECT_URITEMPLATE_LITERAL(uri_template, index, expected_value)        \
  EXPECT_TRUE(                                                                 \
      std::holds_alternative<sourcemeta::core::URITemplateTokenLiteral>(       \
          (uri_template).at(index)));                                          \
  EXPECT_EQ(std::get<sourcemeta::core::URITemplateTokenLiteral>(               \
                (uri_template).at(index))                                      \
                .value,                                                        \
            expected_value)

#define EXPECT_URITEMPLATE_EXPANSION(uri_template, token_index, type,          \
                                     var_index, expected_name,                 \
                                     expected_length, expected_explode)        \
  EXPECT_TRUE(std::holds_alternative<sourcemeta::core::type>(                  \
      (uri_template).at(token_index)));                                        \
  EXPECT_EQ(std::get<sourcemeta::core::type>((uri_template).at(token_index))   \
                .variables.at(var_index)                                       \
                .name,                                                         \
            expected_name);                                                    \
  EXPECT_EQ(std::get<sourcemeta::core::type>((uri_template).at(token_index))   \
                .variables.at(var_index)                                       \
                .length,                                                       \
            expected_length);                                                  \
  EXPECT_EQ(std::get<sourcemeta::core::type>((uri_template).at(token_index))   \
                .variables.at(var_index)                                       \
                .explode,                                                      \
            expected_explode)

#define EXPECT_ROUTER_MATCH(router, path, expected_handler, captures_name)     \
  std::vector<std::tuple<std::uint8_t, std::string, std::string>>              \
      captures_name;                                                           \
  EXPECT_EQ((router).match((path),                                             \
                           [&captures_name](const std::uint8_t index,          \
                                            const std::string_view name,       \
                                            const std::string_view value) {    \
                             captures_name.emplace_back(index, name, value);   \
                           }),                                                 \
            expected_handler)

#define EXPECT_ROUTER_CAPTURE(captures, index, expected_name, expected_value)  \
  EXPECT_EQ(std::get<0>((captures).at(index)), (index));                       \
  EXPECT_EQ(std::get<1>((captures).at(index)), (expected_name));               \
  EXPECT_EQ(std::get<2>((captures).at(index)), (expected_value))

#endif

#ifndef SOURCEMETA_CORE_URITEMPLATE_TEST_HELPERS_H_
#define SOURCEMETA_CORE_URITEMPLATE_TEST_HELPERS_H_

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

#endif

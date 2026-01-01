#include <gtest/gtest.h>

#include <sourcemeta/core/uritemplate.h>

TEST(URITemplate_is_matchable, empty_template) {
  const sourcemeta::core::URITemplate uri_template{""};
  EXPECT_TRUE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, root_template) {
  const sourcemeta::core::URITemplate uri_template{"/"};
  EXPECT_TRUE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, literal_only) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com/path/to/resource"};
  EXPECT_TRUE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, single_variable) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  EXPECT_TRUE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, literal_then_variable) {
  const sourcemeta::core::URITemplate uri_template{"/users/{id}"};
  EXPECT_TRUE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, variable_then_literal) {
  const sourcemeta::core::URITemplate uri_template{"{proto}://example.com"};
  EXPECT_TRUE(uri_template.is_matchable(':'));
}

TEST(URITemplate_is_matchable, multiple_variables_with_literals) {
  const sourcemeta::core::URITemplate uri_template{
      "/users/{id}/posts/{post_id}"};
  EXPECT_TRUE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, reserved_expansion_only) {
  const sourcemeta::core::URITemplate uri_template{"{+path}"};
  EXPECT_TRUE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, reserved_expansion_at_end) {
  const sourcemeta::core::URITemplate uri_template{"/files/{+path}"};
  EXPECT_TRUE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, literal_and_reserved_at_end) {
  const sourcemeta::core::URITemplate uri_template{"/api{+rest}"};
  EXPECT_TRUE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, variable_literal_reserved_at_end) {
  const sourcemeta::core::URITemplate uri_template{"{version}/api{+rest}"};
  EXPECT_TRUE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, multi_variable_expression) {
  const sourcemeta::core::URITemplate uri_template{"{a,b}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, multi_variable_expression_three) {
  const sourcemeta::core::URITemplate uri_template{"{a,b,c}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, prefix_modifier) {
  const sourcemeta::core::URITemplate uri_template{"{var:5}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, explode_modifier) {
  const sourcemeta::core::URITemplate uri_template{"{var*}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, adjacent_variables) {
  const sourcemeta::core::URITemplate uri_template{"{a}{b}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, adjacent_variables_three) {
  const sourcemeta::core::URITemplate uri_template{"{a}{b}{c}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, reserved_expansion_not_at_end) {
  const sourcemeta::core::URITemplate uri_template{"{+path}/file"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, reserved_expansion_in_middle) {
  const sourcemeta::core::URITemplate uri_template{"/api/{+path}/end"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, adjacent_variable_and_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{a}{+b}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, adjacent_reserved_and_variable) {
  const sourcemeta::core::URITemplate uri_template{"{+a}{b}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, fragment_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{#var}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, label_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{.var}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, path_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{/var}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, path_parameter_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{;var}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, query_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{?var}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, query_continuation_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{&var}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, reserved_multi_variable) {
  const sourcemeta::core::URITemplate uri_template{"{+a,b}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, reserved_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{+path:10}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, reserved_with_explode) {
  const sourcemeta::core::URITemplate uri_template{"{+path*}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, valid_then_invalid_modifier) {
  const sourcemeta::core::URITemplate uri_template{"/users/{id}/{name:5}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, level3_in_otherwise_valid) {
  const sourcemeta::core::URITemplate uri_template{"/users/{id}{?query}"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, mixed_delimiters_slash) {
  const sourcemeta::core::URITemplate uri_template{"/{a}-{b}/x"};
  EXPECT_FALSE(uri_template.is_matchable('/'));
}

TEST(URITemplate_is_matchable, mixed_delimiters_dash) {
  const sourcemeta::core::URITemplate uri_template{"/{a}-{b}/x"};
  EXPECT_FALSE(uri_template.is_matchable('-'));
}

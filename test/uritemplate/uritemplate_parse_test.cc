#include <gtest/gtest.h>

#include <sourcemeta/core/uritemplate.h>

#include "uritemplate_helpers.h"

TEST(URITemplate_parse, empty_string) {
  const sourcemeta::core::URITemplate uri_template{""};
  EXPECT_TRUE(uri_template.empty());
  EXPECT_EQ(uri_template.size(), 0);
}

TEST(URITemplate_parse, literal_only) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com/path/to/resource"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_FALSE(uri_template.empty());
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0,
                             "http://example.com/path/to/resource");
}

TEST(URITemplate_parse, single_variable) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  EXPECT_EQ(uri_template.size(), 1);

  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "var", 0, false);
}

TEST(URITemplate_parse, variable_with_underscore) {
  const sourcemeta::core::URITemplate uri_template{"{my_var}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "my_var", 0, false);
}

TEST(URITemplate_parse, variable_with_digits) {
  const sourcemeta::core::URITemplate uri_template{"{var123}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "var123", 0, false);
}

TEST(URITemplate_parse, variable_with_dot) {
  const sourcemeta::core::URITemplate uri_template{"{foo.bar}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "foo.bar", 0, false);
}

TEST(URITemplate_parse, variable_with_multiple_dots) {
  const sourcemeta::core::URITemplate uri_template{"{foo.bar.baz}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "foo.bar.baz", 0, false);
}

TEST(URITemplate_parse, variable_with_percent_encoded) {
  const sourcemeta::core::URITemplate uri_template{"{%3Bvar}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "%3Bvar", 0, false);
}

TEST(URITemplate_parse, literal_then_variable) {
  const sourcemeta::core::URITemplate uri_template{"http://example.com/{id}"};
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "http://example.com/");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenVariable, 0,
                               "id", 0, false);
}

TEST(URITemplate_parse, variable_then_literal) {
  const sourcemeta::core::URITemplate uri_template{"{id}/resource"};
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "id", 0, false);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 1, "/resource");
}

TEST(URITemplate_parse, literal_variable_literal) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com/~{username}/"};
  EXPECT_EQ(uri_template.size(), 3);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "http://example.com/~");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenVariable, 0,
                               "username", 0, false);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 2, "/");
}

TEST(URITemplate_parse, multiple_variables) {
  const sourcemeta::core::URITemplate uri_template{
      "/users/{id}/posts/{postId}"};
  EXPECT_EQ(uri_template.size(), 4);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "/users/");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenVariable, 0,
                               "id", 0, false);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 2, "/posts/");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 3, URITemplateTokenVariable, 0,
                               "postId", 0, false);
}

TEST(URITemplate_parse, adjacent_variables) {
  const sourcemeta::core::URITemplate uri_template{"{x}{y}"};
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenVariable, 0,
                               "y", 0, false);
}

TEST(URITemplate_parse, reserved_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 0, "var", 0, false);
}

TEST(URITemplate_parse, reserved_expansion_with_dots) {
  const sourcemeta::core::URITemplate uri_template{"{+foo.bar}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenReservedExpansion, 0, "foo.bar",
                               0, false);
}

TEST(URITemplate_parse, reserved_expansion_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"http://example.com{+path}"};
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "http://example.com");
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 1, URITemplateTokenReservedExpansion, 0, "path", 0, false);
}

TEST(URITemplate_parse, fragment_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{#var}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 0, "var", 0, false);
}

TEST(URITemplate_parse, fragment_expansion_with_dots) {
  const sourcemeta::core::URITemplate uri_template{"{#foo.bar}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenFragmentExpansion, 0, "foo.bar",
                               0, false);
}

TEST(URITemplate_parse, fragment_expansion_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"http://example.com{#frag}"};
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "http://example.com");
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 1, URITemplateTokenFragmentExpansion, 0, "frag", 0, false);
}

TEST(URITemplate_parse, mixed_level1_and_level2) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com{+path}{#frag}"};
  EXPECT_EQ(uri_template.size(), 3);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "http://example.com");
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 1, URITemplateTokenReservedExpansion, 0, "path", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 2, URITemplateTokenFragmentExpansion, 0, "frag", 0, false);
}

TEST(URITemplate_parse, variable_list_two) {
  const sourcemeta::core::URITemplate uri_template{"{x,y}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 1,
                               "y", 0, false);
}

TEST(URITemplate_parse, variable_list_three) {
  const sourcemeta::core::URITemplate uri_template{"{x,hello,y}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 1,
                               "hello", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 2,
                               "y", 0, false);
}

TEST(URITemplate_parse, variable_list_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"map?{x,y}"};
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "map?");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenVariable, 0,
                               "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenVariable, 1,
                               "y", 0, false);
}

TEST(URITemplate_parse, reserved_expansion_variable_list) {
  const sourcemeta::core::URITemplate uri_template{"{+x,hello,y}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 0, "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 1, "hello", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 2, "y", 0, false);
}

TEST(URITemplate_parse, reserved_expansion_variable_list_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"{+path,x}/here"};
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 0, "path", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 1, "x", 0, false);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 1, "/here");
}

TEST(URITemplate_parse, fragment_expansion_variable_list) {
  const sourcemeta::core::URITemplate uri_template{"{#x,hello,y}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 0, "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 1, "hello", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 2, "y", 0, false);
}

TEST(URITemplate_parse, fragment_expansion_variable_list_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"{#path,x}/here"};
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 0, "path", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 1, "x", 0, false);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 1, "/here");
}

TEST(URITemplate_parse, label_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"{.var}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenLabelExpansion,
                               0, "var", 0, false);
}

TEST(URITemplate_parse, label_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{.x,y}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenLabelExpansion,
                               0, "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenLabelExpansion,
                               1, "y", 0, false);
}

TEST(URITemplate_parse, label_expansion_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"X{.var}"};
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "X");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenLabelExpansion,
                               0, "var", 0, false);
}

TEST(URITemplate_parse, path_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"{/var}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               0, "var", 0, false);
}

TEST(URITemplate_parse, path_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{/var,x}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               0, "var", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               1, "x", 0, false);
}

TEST(URITemplate_parse, path_expansion_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"{/var,x}/here"};
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               0, "var", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               1, "x", 0, false);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 1, "/here");
}

TEST(URITemplate_parse, path_parameter_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"{;x}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenPathParameterExpansion, 0, "x",
                               0, false);
}

TEST(URITemplate_parse, path_parameter_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{;x,y}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenPathParameterExpansion, 0, "x",
                               0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenPathParameterExpansion, 1, "y",
                               0, false);
}

TEST(URITemplate_parse, path_parameter_expansion_three) {
  const sourcemeta::core::URITemplate uri_template{"{;x,y,empty}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenPathParameterExpansion, 0, "x",
                               0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenPathParameterExpansion, 1, "y",
                               0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenPathParameterExpansion, 2,
                               "empty", 0, false);
}

TEST(URITemplate_parse, query_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"{?x}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               0, "x", 0, false);
}

TEST(URITemplate_parse, query_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{?x,y}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               0, "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               1, "y", 0, false);
}

TEST(URITemplate_parse, query_expansion_three) {
  const sourcemeta::core::URITemplate uri_template{"{?x,y,empty}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               0, "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               1, "y", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               2, "empty", 0, false);
}

TEST(URITemplate_parse, query_continuation_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"{&x}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenQueryContinuationExpansion, 0,
                               "x", 0, false);
}

TEST(URITemplate_parse, query_continuation_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{&x,y,empty}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenQueryContinuationExpansion, 0,
                               "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenQueryContinuationExpansion, 1,
                               "y", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenQueryContinuationExpansion, 2,
                               "empty", 0, false);
}

TEST(URITemplate_parse, query_continuation_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"?fixed=yes{&x}"};
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "?fixed=yes");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1,
                               URITemplateTokenQueryContinuationExpansion, 0,
                               "x", 0, false);
}

TEST(URITemplate_parse, prefix_modifier) {
  const sourcemeta::core::URITemplate uri_template{"{var:3}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "var", 3, false);
}

TEST(URITemplate_parse, prefix_modifier_max) {
  const sourcemeta::core::URITemplate uri_template{"{var:9999}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "var", 9999, false);
}

TEST(URITemplate_parse, explode_modifier) {
  const sourcemeta::core::URITemplate uri_template{"{list*}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "list", 0, true);
}

TEST(URITemplate_parse, reserved_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{+path:6}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 0, "path", 6, false);
}

TEST(URITemplate_parse, fragment_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{#path:6}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 0, "path", 6, false);
}

TEST(URITemplate_parse, label_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{.var:3}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenLabelExpansion,
                               0, "var", 3, false);
}

TEST(URITemplate_parse, path_with_explode) {
  const sourcemeta::core::URITemplate uri_template{"{/list*}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               0, "list", 0, true);
}

TEST(URITemplate_parse, query_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{?var:3}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               0, "var", 3, false);
}

TEST(URITemplate_parse, mixed_modifiers_in_list) {
  const sourcemeta::core::URITemplate uri_template{"{/var:1,var}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               0, "var", 1, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               1, "var", 0, false);
}

TEST(URITemplate_parse, variable_single_char) {
  const sourcemeta::core::URITemplate uri_template{"{x}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "x", 0, false);
}

TEST(URITemplate_parse, variable_starting_with_digit) {
  const sourcemeta::core::URITemplate uri_template{"{1var}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "1var", 0, false);
}

TEST(URITemplate_parse, variable_only_digit) {
  const sourcemeta::core::URITemplate uri_template{"{1}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "1", 0, false);
}

TEST(URITemplate_parse, variable_only_underscore) {
  const sourcemeta::core::URITemplate uri_template{"{_}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "_", 0, false);
}

TEST(URITemplate_parse, variable_starting_with_underscore) {
  const sourcemeta::core::URITemplate uri_template{"{_private}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "_private", 0, false);
}

TEST(URITemplate_parse, variable_percent_encoded_start) {
  const sourcemeta::core::URITemplate uri_template{"{%41bc}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "%41bc", 0, false);
}

TEST(URITemplate_parse, variable_multiple_percent_encoded) {
  const sourcemeta::core::URITemplate uri_template{"{%41%42%43}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "%41%42%43", 0, false);
}

TEST(URITemplate_parse, variable_with_dot_and_digits) {
  const sourcemeta::core::URITemplate uri_template{"{a.1.b.2}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "a.1.b.2", 0, false);
}

TEST(URITemplate_parse, prefix_single_digit) {
  const sourcemeta::core::URITemplate uri_template{"{var:1}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "var", 1, false);
}

TEST(URITemplate_parse, prefix_four_digits) {
  const sourcemeta::core::URITemplate uri_template{"{var:1234}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "var", 1234, false);
}

TEST(URITemplate_parse, explode_with_reserved_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{+list*}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 0, "list", 0, true);
}

TEST(URITemplate_parse, explode_with_fragment_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{#keys*}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 0, "keys", 0, true);
}

TEST(URITemplate_parse, explode_with_label_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{.list*}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenLabelExpansion,
                               0, "list", 0, true);
}

TEST(URITemplate_parse, explode_with_path_parameter) {
  const sourcemeta::core::URITemplate uri_template{"{;keys*}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenPathParameterExpansion, 0,
                               "keys", 0, true);
}

TEST(URITemplate_parse, explode_with_query_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{?list*}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               0, "list", 0, true);
}

TEST(URITemplate_parse, explode_with_query_continuation) {
  const sourcemeta::core::URITemplate uri_template{"{&keys*}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenQueryContinuationExpansion, 0,
                               "keys", 0, true);
}

TEST(URITemplate_parse, multiple_variables_with_explode) {
  const sourcemeta::core::URITemplate uri_template{"{?x,list*,y}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               0, "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               1, "list", 0, true);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               2, "y", 0, false);
}

TEST(URITemplate_parse, multiple_variables_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{x:3,y:5,z}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "x", 3, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 1,
                               "y", 5, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 2,
                               "z", 0, false);
}

TEST(URITemplate_parse, long_variable_name) {
  const sourcemeta::core::URITemplate uri_template{
      "{abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenVariable, 0,
      "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789", 0,
      false);
}

TEST(URITemplate_parse, variable_list_many) {
  const sourcemeta::core::URITemplate uri_template{"{a,b,c,d,e,f,g,h,i,j}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "a", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 1,
                               "b", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 2,
                               "c", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 3,
                               "d", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 4,
                               "e", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 5,
                               "f", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 6,
                               "g", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 7,
                               "h", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 8,
                               "i", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 9,
                               "j", 0, false);
}

TEST(URITemplate_parse, lowercase_percent_encoding) {
  const sourcemeta::core::URITemplate uri_template{"{%3avar}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "%3avar", 0, false);
}

TEST(URITemplate_parse, mixed_case_percent_encoding) {
  const sourcemeta::core::URITemplate uri_template{"{%3Avar}"};
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "%3Avar", 0, false);
}

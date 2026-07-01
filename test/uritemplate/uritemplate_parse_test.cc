#include <sourcemeta/core/test.h>

#include <sourcemeta/core/uritemplate.h>

#include "uritemplate_helpers.h"

TEST(empty_string) {
  const sourcemeta::core::URITemplate uri_template{""};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate(""));
  EXPECT_TRUE(uri_template.empty());
  EXPECT_EQ(uri_template.size(), 0);
}

TEST(literal_only) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com/path/to/resource"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate(
      "http://example.com/path/to/resource"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_FALSE(uri_template.empty());
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0,
                             "http://example.com/path/to/resource");
}

TEST(single_variable) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{var}"));
  EXPECT_EQ(uri_template.size(), 1);

  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "var", 0, false);
}

TEST(variable_with_underscore) {
  const sourcemeta::core::URITemplate uri_template{"{my_var}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{my_var}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "my_var", 0, false);
}

TEST(variable_with_digits) {
  const sourcemeta::core::URITemplate uri_template{"{var123}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{var123}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "var123", 0, false);
}

TEST(variable_with_dot) {
  const sourcemeta::core::URITemplate uri_template{"{foo.bar}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{foo.bar}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "foo.bar", 0, false);
}

TEST(variable_with_multiple_dots) {
  const sourcemeta::core::URITemplate uri_template{"{foo.bar.baz}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{foo.bar.baz}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "foo.bar.baz", 0, false);
}

TEST(variable_with_percent_encoded) {
  const sourcemeta::core::URITemplate uri_template{"{%3Bvar}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{%3Bvar}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "%3Bvar", 0, false);
}

TEST(literal_then_variable) {
  const sourcemeta::core::URITemplate uri_template{"http://example.com/{id}"};
  EXPECT_TRUE(
      sourcemeta::core::URITemplate::is_uritemplate("http://example.com/{id}"));
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "http://example.com/");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenVariable, 0,
                               "id", 0, false);
}

TEST(variable_then_literal) {
  const sourcemeta::core::URITemplate uri_template{"{id}/resource"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{id}/resource"));
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "id", 0, false);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 1, "/resource");
}

TEST(literal_variable_literal) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com/~{username}/"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate(
      "http://example.com/~{username}/"));
  EXPECT_EQ(uri_template.size(), 3);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "http://example.com/~");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenVariable, 0,
                               "username", 0, false);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 2, "/");
}

TEST(multiple_variables) {
  const sourcemeta::core::URITemplate uri_template{
      "/users/{id}/posts/{postId}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate(
      "/users/{id}/posts/{postId}"));
  EXPECT_EQ(uri_template.size(), 4);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "/users/");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenVariable, 0,
                               "id", 0, false);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 2, "/posts/");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 3, URITemplateTokenVariable, 0,
                               "postId", 0, false);
}

TEST(adjacent_variables) {
  const sourcemeta::core::URITemplate uri_template{"{x}{y}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{x}{y}"));
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenVariable, 0,
                               "y", 0, false);
}

TEST(reserved_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{+var}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 0, "var", 0, false);
}

TEST(reserved_expansion_with_dots) {
  const sourcemeta::core::URITemplate uri_template{"{+foo.bar}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{+foo.bar}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenReservedExpansion, 0, "foo.bar",
                               0, false);
}

TEST(reserved_expansion_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"http://example.com{+path}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate(
      "http://example.com{+path}"));
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "http://example.com");
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 1, URITemplateTokenReservedExpansion, 0, "path", 0, false);
}

TEST(fragment_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{#var}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{#var}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 0, "var", 0, false);
}

TEST(fragment_expansion_with_dots) {
  const sourcemeta::core::URITemplate uri_template{"{#foo.bar}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{#foo.bar}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenFragmentExpansion, 0, "foo.bar",
                               0, false);
}

TEST(fragment_expansion_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"http://example.com{#frag}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate(
      "http://example.com{#frag}"));
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "http://example.com");
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 1, URITemplateTokenFragmentExpansion, 0, "frag", 0, false);
}

TEST(mixed_level1_and_level2) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com{+path}{#frag}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate(
      "http://example.com{+path}{#frag}"));
  EXPECT_EQ(uri_template.size(), 3);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "http://example.com");
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 1, URITemplateTokenReservedExpansion, 0, "path", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 2, URITemplateTokenFragmentExpansion, 0, "frag", 0, false);
}

TEST(variable_list_two) {
  const sourcemeta::core::URITemplate uri_template{"{x,y}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{x,y}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 1,
                               "y", 0, false);
}

TEST(variable_list_three) {
  const sourcemeta::core::URITemplate uri_template{"{x,hello,y}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{x,hello,y}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 1,
                               "hello", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 2,
                               "y", 0, false);
}

TEST(variable_list_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"map?{x,y}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("map?{x,y}"));
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "map?");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenVariable, 0,
                               "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenVariable, 1,
                               "y", 0, false);
}

TEST(reserved_expansion_variable_list) {
  const sourcemeta::core::URITemplate uri_template{"{+x,hello,y}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{+x,hello,y}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 0, "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 1, "hello", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 2, "y", 0, false);
}

TEST(reserved_expansion_variable_list_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"{+path,x}/here"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{+path,x}/here"));
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 0, "path", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 1, "x", 0, false);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 1, "/here");
}

TEST(fragment_expansion_variable_list) {
  const sourcemeta::core::URITemplate uri_template{"{#x,hello,y}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{#x,hello,y}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 0, "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 1, "hello", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 2, "y", 0, false);
}

TEST(fragment_expansion_variable_list_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"{#path,x}/here"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{#path,x}/here"));
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 0, "path", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 1, "x", 0, false);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 1, "/here");
}

TEST(label_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"{.var}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{.var}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenLabelExpansion,
                               0, "var", 0, false);
}

TEST(label_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{.x,y}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{.x,y}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenLabelExpansion,
                               0, "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenLabelExpansion,
                               1, "y", 0, false);
}

TEST(label_expansion_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"X{.var}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("X{.var}"));
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "X");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1, URITemplateTokenLabelExpansion,
                               0, "var", 0, false);
}

TEST(path_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"{/var}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{/var}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               0, "var", 0, false);
}

TEST(path_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{/var,x}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{/var,x}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               0, "var", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               1, "x", 0, false);
}

TEST(path_expansion_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"{/var,x}/here"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{/var,x}/here"));
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               0, "var", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               1, "x", 0, false);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 1, "/here");
}

TEST(path_parameter_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"{;x}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{;x}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenPathParameterExpansion, 0, "x",
                               0, false);
}

TEST(path_parameter_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{;x,y}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{;x,y}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenPathParameterExpansion, 0, "x",
                               0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenPathParameterExpansion, 1, "y",
                               0, false);
}

TEST(path_parameter_expansion_three) {
  const sourcemeta::core::URITemplate uri_template{"{;x,y,empty}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{;x,y,empty}"));
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

TEST(query_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"{?x}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{?x}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               0, "x", 0, false);
}

TEST(query_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{?x,y}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{?x,y}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               0, "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               1, "y", 0, false);
}

TEST(query_expansion_three) {
  const sourcemeta::core::URITemplate uri_template{"{?x,y,empty}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{?x,y,empty}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               0, "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               1, "y", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               2, "empty", 0, false);
}

TEST(query_continuation_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"{&x}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{&x}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenQueryContinuationExpansion, 0,
                               "x", 0, false);
}

TEST(query_continuation_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{&x,y,empty}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{&x,y,empty}"));
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

TEST(query_continuation_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"?fixed=yes{&x}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("?fixed=yes{&x}"));
  EXPECT_EQ(uri_template.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(uri_template, 0, "?fixed=yes");
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 1,
                               URITemplateTokenQueryContinuationExpansion, 0,
                               "x", 0, false);
}

TEST(prefix_modifier) {
  const sourcemeta::core::URITemplate uri_template{"{var:3}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{var:3}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "var", 3, false);
}

TEST(prefix_modifier_max) {
  const sourcemeta::core::URITemplate uri_template{"{var:9999}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{var:9999}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "var", 9999, false);
}

TEST(explode_modifier) {
  const sourcemeta::core::URITemplate uri_template{"{list*}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{list*}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "list", 0, true);
}

TEST(reserved_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{+path:6}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{+path:6}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 0, "path", 6, false);
}

TEST(fragment_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{#path:6}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{#path:6}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 0, "path", 6, false);
}

TEST(label_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{.var:3}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{.var:3}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenLabelExpansion,
                               0, "var", 3, false);
}

TEST(path_with_explode) {
  const sourcemeta::core::URITemplate uri_template{"{/list*}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{/list*}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               0, "list", 0, true);
}

TEST(query_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{?var:3}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{?var:3}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               0, "var", 3, false);
}

TEST(mixed_modifiers_in_list) {
  const sourcemeta::core::URITemplate uri_template{"{/var:1,var}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{/var:1,var}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               0, "var", 1, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenPathExpansion,
                               1, "var", 0, false);
}

TEST(variable_single_char) {
  const sourcemeta::core::URITemplate uri_template{"{x}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{x}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "x", 0, false);
}

TEST(variable_starting_with_digit) {
  const sourcemeta::core::URITemplate uri_template{"{1var}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{1var}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "1var", 0, false);
}

TEST(variable_only_digit) {
  const sourcemeta::core::URITemplate uri_template{"{1}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{1}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "1", 0, false);
}

TEST(variable_only_underscore) {
  const sourcemeta::core::URITemplate uri_template{"{_}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{_}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "_", 0, false);
}

TEST(variable_starting_with_underscore) {
  const sourcemeta::core::URITemplate uri_template{"{_private}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{_private}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "_private", 0, false);
}

TEST(variable_percent_encoded_start) {
  const sourcemeta::core::URITemplate uri_template{"{%41bc}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{%41bc}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "%41bc", 0, false);
}

TEST(variable_multiple_percent_encoded) {
  const sourcemeta::core::URITemplate uri_template{"{%41%42%43}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{%41%42%43}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "%41%42%43", 0, false);
}

TEST(variable_with_dot_and_digits) {
  const sourcemeta::core::URITemplate uri_template{"{a.1.b.2}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{a.1.b.2}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "a.1.b.2", 0, false);
}

TEST(prefix_single_digit) {
  const sourcemeta::core::URITemplate uri_template{"{var:1}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{var:1}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "var", 1, false);
}

TEST(prefix_four_digits) {
  const sourcemeta::core::URITemplate uri_template{"{var:1234}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{var:1234}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "var", 1234, false);
}

TEST(explode_with_reserved_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{+list*}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{+list*}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenReservedExpansion, 0, "list", 0, true);
}

TEST(explode_with_fragment_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{#keys*}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{#keys*}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenFragmentExpansion, 0, "keys", 0, true);
}

TEST(explode_with_label_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{.list*}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{.list*}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenLabelExpansion,
                               0, "list", 0, true);
}

TEST(explode_with_path_parameter) {
  const sourcemeta::core::URITemplate uri_template{"{;keys*}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{;keys*}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenPathParameterExpansion, 0,
                               "keys", 0, true);
}

TEST(explode_with_query_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{?list*}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{?list*}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               0, "list", 0, true);
}

TEST(explode_with_query_continuation) {
  const sourcemeta::core::URITemplate uri_template{"{&keys*}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{&keys*}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0,
                               URITemplateTokenQueryContinuationExpansion, 0,
                               "keys", 0, true);
}

TEST(multiple_variables_with_explode) {
  const sourcemeta::core::URITemplate uri_template{"{?x,list*,y}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{?x,list*,y}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               0, "x", 0, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               1, "list", 0, true);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenQueryExpansion,
                               2, "y", 0, false);
}

TEST(multiple_variables_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{x:3,y:5,z}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{x:3,y:5,z}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "x", 3, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 1,
                               "y", 5, false);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 2,
                               "z", 0, false);
}

TEST(long_variable_name) {
  const sourcemeta::core::URITemplate uri_template{
      "{abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate(
      "{abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(
      uri_template, 0, URITemplateTokenVariable, 0,
      "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789", 0,
      false);
}

TEST(variable_list_many) {
  const sourcemeta::core::URITemplate uri_template{"{a,b,c,d,e,f,g,h,i,j}"};
  EXPECT_TRUE(
      sourcemeta::core::URITemplate::is_uritemplate("{a,b,c,d,e,f,g,h,i,j}"));
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

TEST(lowercase_percent_encoding) {
  const sourcemeta::core::URITemplate uri_template{"{%3avar}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{%3avar}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "%3avar", 0, false);
}

TEST(mixed_case_percent_encoding) {
  const sourcemeta::core::URITemplate uri_template{"{%3Avar}"};
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_uritemplate("{%3Avar}"));
  EXPECT_EQ(uri_template.size(), 1);
  EXPECT_URITEMPLATE_EXPANSION(uri_template, 0, URITemplateTokenVariable, 0,
                               "%3Avar", 0, false);
}

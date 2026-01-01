#include <gtest/gtest.h>

#include <sourcemeta/core/uritemplate.h>

#include "uritemplate_helpers.h"

#include <utility> // std::move

TEST(URITemplate, iterator) {
  const sourcemeta::core::URITemplate uri_template{"/{a}/{b}"};
  EXPECT_EQ(uri_template.size(), 4);

  auto iterator = uri_template.begin();
  EXPECT_NE(iterator, uri_template.end());
  EXPECT_TRUE(std::holds_alternative<sourcemeta::core::URITemplateTokenLiteral>(
      *iterator));
  EXPECT_EQ(
      std::get<sourcemeta::core::URITemplateTokenLiteral>(*iterator).value,
      "/");

  ++iterator;
  EXPECT_NE(iterator, uri_template.end());
  EXPECT_TRUE(
      std::holds_alternative<sourcemeta::core::URITemplateTokenVariable>(
          *iterator));
  const auto &variable_a =
      std::get<sourcemeta::core::URITemplateTokenVariable>(*iterator);
  EXPECT_EQ(variable_a.variables.size(), 1);
  EXPECT_EQ(variable_a.variables.at(0).name, "a");

  ++iterator;
  EXPECT_NE(iterator, uri_template.end());
  EXPECT_TRUE(std::holds_alternative<sourcemeta::core::URITemplateTokenLiteral>(
      *iterator));
  EXPECT_EQ(
      std::get<sourcemeta::core::URITemplateTokenLiteral>(*iterator).value,
      "/");

  ++iterator;
  EXPECT_NE(iterator, uri_template.end());
  EXPECT_TRUE(
      std::holds_alternative<sourcemeta::core::URITemplateTokenVariable>(
          *iterator));
  const auto &variable_b =
      std::get<sourcemeta::core::URITemplateTokenVariable>(*iterator);
  EXPECT_EQ(variable_b.variables.size(), 1);
  EXPECT_EQ(variable_b.variables.at(0).name, "b");

  ++iterator;
  EXPECT_EQ(iterator, uri_template.end());
}

TEST(URITemplate, copy_constructor) {
  const sourcemeta::core::URITemplate original{"http://example.com/{id}"};
  const sourcemeta::core::URITemplate copy{original};

  EXPECT_EQ(copy.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(copy, 0, "http://example.com/");
  EXPECT_URITEMPLATE_EXPANSION(copy, 1, URITemplateTokenVariable, 0, "id", 0,
                               false);
}

TEST(URITemplate, move_constructor) {
  sourcemeta::core::URITemplate original{"http://example.com/{id}"};
  const sourcemeta::core::URITemplate moved{std::move(original)};

  EXPECT_EQ(moved.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(moved, 0, "http://example.com/");
  EXPECT_URITEMPLATE_EXPANSION(moved, 1, URITemplateTokenVariable, 0, "id", 0,
                               false);
}

TEST(URITemplate, copy_assignment) {
  const sourcemeta::core::URITemplate original{"http://example.com/{id}"};
  sourcemeta::core::URITemplate copy{""};

  EXPECT_EQ(copy.size(), 0);
  copy = original;

  EXPECT_EQ(original.size(), 2);
  EXPECT_EQ(copy.size(), 2);

  EXPECT_URITEMPLATE_LITERAL(original, 0, "http://example.com/");
  EXPECT_URITEMPLATE_EXPANSION(original, 1, URITemplateTokenVariable, 0, "id",
                               0, false);
  EXPECT_URITEMPLATE_LITERAL(copy, 0, "http://example.com/");
  EXPECT_URITEMPLATE_EXPANSION(copy, 1, URITemplateTokenVariable, 0, "id", 0,
                               false);
}

TEST(URITemplate, move_assignment) {
  sourcemeta::core::URITemplate original{"http://example.com/{id}"};
  sourcemeta::core::URITemplate moved{""};

  EXPECT_EQ(moved.size(), 0);
  moved = std::move(original);

  EXPECT_EQ(moved.size(), 2);
  EXPECT_URITEMPLATE_LITERAL(moved, 0, "http://example.com/");
  EXPECT_URITEMPLATE_EXPANSION(moved, 1, URITemplateTokenVariable, 0, "id", 0,
                               false);
}

TEST(URITemplate, at_const_lvalue) {
  const sourcemeta::core::URITemplate uri_template{"/{id}"};
  const auto &token = uri_template.at(1);
  EXPECT_TRUE(
      std::holds_alternative<sourcemeta::core::URITemplateTokenVariable>(
          token));
  const auto &variable =
      std::get<sourcemeta::core::URITemplateTokenVariable>(token);
  EXPECT_EQ(variable.variables.size(), 1);
  EXPECT_EQ(variable.variables[0].name, "id");
}

TEST(URITemplate, at_rvalue_move) {
  sourcemeta::core::URITemplate uri_template{"/{id}"};
  auto token = std::move(uri_template).at(1);
  EXPECT_TRUE(
      std::holds_alternative<sourcemeta::core::URITemplateTokenVariable>(
          token));
  auto &variable = std::get<sourcemeta::core::URITemplateTokenVariable>(token);
  EXPECT_EQ(variable.variables.size(), 1);
  EXPECT_EQ(variable.variables[0].name, "id");
}

TEST(URITemplate, at_rvalue_move_multiple) {
  sourcemeta::core::URITemplate uri_template{"/users/{id}/posts/{post_id}"};
  EXPECT_EQ(uri_template.size(), 4);

  auto token0 = std::move(uri_template).at(0);
  EXPECT_TRUE(std::holds_alternative<sourcemeta::core::URITemplateTokenLiteral>(
      token0));
  EXPECT_EQ(std::get<sourcemeta::core::URITemplateTokenLiteral>(token0).value,
            "/users/");

  sourcemeta::core::URITemplate uri_template2{"/users/{id}/posts/{post_id}"};
  auto token1 = std::move(uri_template2).at(1);
  EXPECT_TRUE(
      std::holds_alternative<sourcemeta::core::URITemplateTokenVariable>(
          token1));
  auto &variable1 =
      std::get<sourcemeta::core::URITemplateTokenVariable>(token1);
  EXPECT_EQ(variable1.variables.size(), 1);
  EXPECT_EQ(variable1.variables[0].name, "id");
}

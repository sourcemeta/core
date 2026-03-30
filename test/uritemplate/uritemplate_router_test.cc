#include <gtest/gtest.h>

#include <sourcemeta/core/uritemplate.h>

#include "uritemplate_helpers.h"

#include <array>       // std::array
#include <climits>     // INT64_MIN, INT64_MAX
#include <cstdint>     // std::int64_t
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <variant>     // std::holds_alternative, std::get
#include <vector>      // std::vector

#define EXPECT_ROUTER_SEGMENT_ERROR(router, input, identifier, expected)       \
  try {                                                                        \
    router.add(input, identifier);                                             \
    FAIL() << "Expected error for: " << input;                                 \
  } catch (                                                                    \
      const sourcemeta::core::URITemplateRouterInvalidSegmentError &error) {   \
    EXPECT_EQ(error.segment(), expected);                                      \
  }

TEST(URITemplateRouter, single_literal_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", 1);
  EXPECT_ROUTER_MATCH(router, "/users", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, single_literal_route_no_match) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", 1);
  EXPECT_ROUTER_MATCH(router, "/posts", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, multi_segment_literal) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/list", 1);
  EXPECT_ROUTER_MATCH(router, "/users/list", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, single_variable) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}", 1);
  EXPECT_ROUTER_MATCH(router, "/users/123", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST(URITemplateRouter, multiple_variables) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}/posts/{post_id}", 1);
  EXPECT_ROUTER_MATCH(router, "/users/42/posts/99", 1, captures);
  EXPECT_EQ(captures.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
  EXPECT_ROUTER_CAPTURE(captures, 1, "post_id", "99");
}

TEST(URITemplateRouter, literal_before_variable_precedence) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/me", 1);
  router.add("/users/{id}", 2);
  EXPECT_ROUTER_MATCH(router, "/users/me", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, variable_fallback) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/me", 1);
  router.add("/users/{id}", 2);
  EXPECT_ROUTER_MATCH(router, "/users/123", 2, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST(URITemplateRouter, multiple_routes_match_users) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", 1);
  router.add("/users/{id}", 2);
  router.add("/posts", 3);
  router.add("/posts/{id}", 4);
  EXPECT_ROUTER_MATCH(router, "/users", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, multiple_routes_match_users_id) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", 1);
  router.add("/users/{id}", 2);
  router.add("/posts", 3);
  router.add("/posts/{id}", 4);
  EXPECT_ROUTER_MATCH(router, "/users/42", 2, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

TEST(URITemplateRouter, multiple_routes_match_posts) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", 1);
  router.add("/users/{id}", 2);
  router.add("/posts", 3);
  router.add("/posts/{id}", 4);
  EXPECT_ROUTER_MATCH(router, "/posts", 3, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, multiple_routes_match_posts_id) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", 1);
  router.add("/users/{id}", 2);
  router.add("/posts", 3);
  router.add("/posts/{id}", 4);
  EXPECT_ROUTER_MATCH(router, "/posts/99", 4, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "99");
}

TEST(URITemplateRouter, no_match_partial_path) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}/posts", 1);
  EXPECT_ROUTER_MATCH(router, "/users/123", 0, captures);
}

TEST(URITemplateRouter, no_match_extra_segments) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", 1);
  EXPECT_ROUTER_MATCH(router, "/users/123", 0, captures);
}

TEST(URITemplateRouter, empty_path) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", 1);
  EXPECT_ROUTER_MATCH(router, "", 0, captures);
}

TEST(URITemplateRouter, root_template_matches_root) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/", 1);
  EXPECT_ROUTER_MATCH(router, "/", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, root_template_no_match_empty) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/", 1);
  EXPECT_ROUTER_MATCH(router, "", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, root_template_no_match_path) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/", 1);
  EXPECT_ROUTER_MATCH(router, "/foo", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, empty_template_matches_empty) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", 1);
  EXPECT_ROUTER_MATCH(router, "", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, empty_template_no_match_root) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", 1);
  EXPECT_ROUTER_MATCH(router, "/", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, empty_template_no_match_path) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", 1);
  EXPECT_ROUTER_MATCH(router, "/foo", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, root_and_other_routes_match_root) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/", 1);
  router.add("/users", 2);
  router.add("/users/{id}", 3);
  EXPECT_ROUTER_MATCH(router, "/", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, root_and_other_routes_match_users) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/", 1);
  router.add("/users", 2);
  router.add("/users/{id}", 3);
  EXPECT_ROUTER_MATCH(router, "/users", 2, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, root_and_other_routes_match_users_id) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/", 1);
  router.add("/users", 2);
  router.add("/users/{id}", 3);
  EXPECT_ROUTER_MATCH(router, "/users/123", 3, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST(URITemplateRouter, empty_and_root_together_match_empty) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", 1);
  router.add("/", 2);
  EXPECT_ROUTER_MATCH(router, "", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, empty_and_root_together_match_root) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", 1);
  router.add("/", 2);
  EXPECT_ROUTER_MATCH(router, "/", 2, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, empty_and_root_together_no_match_path) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", 1);
  router.add("/", 2);
  EXPECT_ROUTER_MATCH(router, "/foo", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, empty_and_root_and_others_match_empty) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", 1);
  router.add("/", 2);
  router.add("/users", 3);
  router.add("/users/{id}", 4);
  EXPECT_ROUTER_MATCH(router, "", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, empty_and_root_and_others_match_root) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", 1);
  router.add("/", 2);
  router.add("/users", 3);
  router.add("/users/{id}", 4);
  EXPECT_ROUTER_MATCH(router, "/", 2, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, empty_and_root_and_others_match_users) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", 1);
  router.add("/", 2);
  router.add("/users", 3);
  router.add("/users/{id}", 4);
  EXPECT_ROUTER_MATCH(router, "/users", 3, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, empty_and_root_and_others_match_users_id) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", 1);
  router.add("/", 2);
  router.add("/users", 3);
  router.add("/users/{id}", 4);
  EXPECT_ROUTER_MATCH(router, "/users/42", 4, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

TEST(URITemplateRouter, binary_search_literals_gamma) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/alpha", 1);
  router.add("/beta", 2);
  router.add("/gamma", 3);
  router.add("/delta", 4);
  router.add("/epsilon", 5);
  EXPECT_ROUTER_MATCH(router, "/gamma", 3, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, binary_search_literals_alpha) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/alpha", 1);
  router.add("/beta", 2);
  router.add("/gamma", 3);
  router.add("/delta", 4);
  router.add("/epsilon", 5);
  EXPECT_ROUTER_MATCH(router, "/alpha", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, binary_search_literals_epsilon) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/alpha", 1);
  router.add("/beta", 2);
  router.add("/gamma", 3);
  router.add("/delta", 4);
  router.add("/epsilon", 5);
  EXPECT_ROUTER_MATCH(router, "/epsilon", 5, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, conflicting_variable_names_throws) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{user_id}/posts", 1);
  try {
    router.add("/users/{id}/comments", 2);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterVariableMismatchError &error) {
    EXPECT_EQ(error.left(), "user_id");
    EXPECT_EQ(error.right(), "id");
    SUCCEED();
  }
}

TEST(URITemplateRouter, same_variable_names_allowed) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}/posts", 1);
  router.add("/users/{id}/comments", 2);
  EXPECT_ROUTER_MATCH(router, "/users/123/posts", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST(URITemplateRouter, conflicting_expansion_variable_names_throws) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{id}", 1);
  try {
    router.add("/files/{+path}", 2);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterVariableMismatchError &error) {
    EXPECT_EQ(error.left(), "id");
    EXPECT_EQ(error.right(), "path");
    SUCCEED();
  }
}

TEST(URITemplateRouter, reserved_expansion_catch_all) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", 1);
  EXPECT_ROUTER_MATCH(router, "/files/foo/bar/baz.txt", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar/baz.txt");
}

TEST(URITemplateRouter, reserved_expansion_single_segment) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", 1);
  EXPECT_ROUTER_MATCH(router, "/files/readme.md", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "readme.md");
}

TEST(URITemplateRouter, reserved_expansion_with_literal_prefix) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/v1/proxy/{+url}", 1);
  EXPECT_ROUTER_MATCH(router, "/api/v1/proxy/https://example.com/path", 1,
                      captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "url", "https://example.com/path");
}

TEST(URITemplateRouter, reserved_expansion_matches_single_segment) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", 1);
  EXPECT_ROUTER_MATCH(router, "/files/123", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "123");
}

TEST(URITemplateRouter, reserved_expansion_matches_multi_segment) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", 1);
  EXPECT_ROUTER_MATCH(router, "/files/foo/bar", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar");
}

TEST(URITemplateRouter, expansion_takes_priority_over_variable) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{path}", 1);
  router.add("/files/{+path}", 2);
  EXPECT_ROUTER_MATCH(router, "/files/readme.md", 2, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "readme.md");
}

TEST(URITemplateRouter, expansion_takes_priority_multi_segment) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{path}", 1);
  router.add("/files/{+path}", 2);
  EXPECT_ROUTER_MATCH(router, "/files/foo/bar/baz", 2, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar/baz");
}

TEST(URITemplateRouter, expansion_first_then_variable) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", 1);
  router.add("/files/{path}", 2);
  EXPECT_ROUTER_MATCH(router, "/files/readme.md", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "readme.md");
}

TEST(URITemplateRouter, literal_takes_priority_over_expansion) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", 1);
  router.add("/files/special", 2);
  EXPECT_ROUTER_MATCH(router, "/files/special", 2, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, expansion_fallback_from_literal) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", 1);
  router.add("/files/special", 2);
  EXPECT_ROUTER_MATCH(router, "/files/other", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "other");
}

TEST(URITemplateRouter, literal_suffix_after_variable_throws) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar}.json", 1, "{bar}.json");
}

TEST(URITemplateRouter, literal_prefix_before_variable_throws) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/prefix{bar}", 1, "prefix{bar}");
}

TEST(URITemplateRouter, mixed_segment_variable_then_literal_suffix) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/{id}.html", 1, "{id}.html");
}

TEST(URITemplateRouter, mixed_segment_expansion_then_literal_suffix) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/files/{+path}.bak", 1, "{+path}.bak");
}

TEST(URITemplateRouter, mixed_segment_prefix_before_expansion_throws) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/api/v1{+rest}", 1, "v1{+rest}");
}

TEST(URITemplateRouter, mixed_segment_suffix_in_middle) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar}.json/baz", 1, "{bar}.json");
}

TEST(URITemplateRouter, mixed_segment_prefix_in_middle) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/prefix{bar}/baz", 1, "prefix{bar}");
}

TEST(URITemplateRouter, mixed_segment_expansion_suffix_in_middle) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/files/{+path}.txt/meta", 1,
                              "{+path}.txt");
}

TEST(URITemplateRouter, mixed_segment_expansion_prefix_in_middle) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/api/v2{+path}/end", 1, "v2{+path}");
}

TEST(URITemplateRouter, error_unclosed_brace) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar", 1, "{bar");
}

TEST(URITemplateRouter, error_unclosed_brace_at_end) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/bar{", 1, "bar{");
}

TEST(URITemplateRouter, error_empty_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{}", 1, "{}");
}

TEST(URITemplateRouter, error_empty_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{+}", 1, "{+}");
}

TEST(URITemplateRouter, error_nested_brace) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{{bar}}", 1, "{{bar}}");
}

TEST(URITemplateRouter, error_unmatched_close_brace) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/bar}", 1, "bar}");
}

TEST(URITemplateRouter, error_space_before_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{ bar}", 1, "{ bar}");
}

TEST(URITemplateRouter, error_space_after_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar }", 1, "{bar }");
}

TEST(URITemplateRouter, error_space_in_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar baz}", 1, "{bar baz}");
}

TEST(URITemplateRouter, error_invalid_variable_start) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{-bar}", 1, "{-bar}");
}

TEST(URITemplateRouter, error_invalid_character_in_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar!baz}", 1, "{bar!baz}");
}

TEST(URITemplateRouter, error_reserved_operator_equals) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{=bar}", 1, "{=bar}");
}

TEST(URITemplateRouter, error_reserved_operator_pipe) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{|bar}", 1, "{|bar}");
}

TEST(URITemplateRouter, error_reserved_operator_exclamation) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{!bar}", 1, "{!bar}");
}

TEST(URITemplateRouter, error_reserved_operator_at) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{@bar}", 1, "{@bar}");
}

TEST(URITemplateRouter, error_bracket_in_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar[0]}", 1, "{bar[0]}");
}

TEST(URITemplateRouter, error_multiple_variables) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar,baz}", 1, "{bar,baz}");
}

TEST(URITemplateRouter, error_prefix_modifier) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar:3}", 1, "{bar:3}");
}

TEST(URITemplateRouter, error_explode_modifier) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar*}", 1, "{bar*}");
}

TEST(URITemplateRouter, error_unsupported_fragment_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{#bar}", 1, "{#bar}");
}

TEST(URITemplateRouter, error_unsupported_label_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{.bar}", 1, "{.bar}");
}

TEST(URITemplateRouter, error_unsupported_path_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{/bar}", 1, "{/bar}");
}

TEST(URITemplateRouter, error_unsupported_path_parameter_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{;bar}", 1, "{;bar}");
}

TEST(URITemplateRouter, error_unsupported_query_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{?bar}", 1, "{?bar}");
}

TEST(URITemplateRouter, error_unsupported_query_continuation_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{&bar}", 1, "{&bar}");
}

TEST(URITemplateRouter, error_expansion_not_last_segment_literal) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{+bar}/baz", 1, "{+bar}");
}

TEST(URITemplateRouter, error_expansion_not_last_segment_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{+bar}/{baz}", 1, "{+bar}");
}

TEST(URITemplateRouter, error_expansion_not_last_segment_trailing_slash) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{+bar}/", 1, "{+bar}");
}

TEST(URITemplateRouter, trailing_slash_no_match) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", 1);
  EXPECT_ROUTER_MATCH(router, "/users/", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, multiple_trailing_slashes_no_match) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", 1);
  EXPECT_ROUTER_MATCH(router, "/users///", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, leading_double_slash_no_match) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", 1);
  EXPECT_ROUTER_MATCH(router, "//users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, internal_double_slashes_no_match) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/posts", 1);
  EXPECT_ROUTER_MATCH(router, "/users//posts", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, trailing_slash_with_variable_no_match) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}", 1);
  EXPECT_ROUTER_MATCH(router, "/users/123/", 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST(URITemplateRouter, internal_double_slash_with_variable_no_match) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}/posts", 1);
  EXPECT_ROUTER_MATCH(router, "/users//123//posts", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouter, expansion_matches_trailing_slash) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", 1);
  EXPECT_ROUTER_MATCH(router, "/files/foo/bar/", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar/");
}

TEST(URITemplateRouter, expansion_matches_double_slashes) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", 1);
  EXPECT_ROUTER_MATCH(router, "/files/foo//bar", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo//bar");
}

TEST(URITemplateRouter, add_with_single_string_argument) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"responseSchema", std::string_view{"some/path"}}}};
  router.add("/test", 1, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0].first, "responseSchema");
  EXPECT_EQ(std::get<std::string_view>(collected[0].second), "some/path");
}

TEST(URITemplateRouter, add_with_single_integer_argument) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"maxItems", std::int64_t{42}}}};
  router.add("/test", 1, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0].first, "maxItems");
  EXPECT_EQ(std::get<std::int64_t>(collected[0].second), 42);
}

TEST(URITemplateRouter, add_with_single_boolean_argument_true) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"verbose", true}}};
  router.add("/test", 1, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0].first, "verbose");
  EXPECT_TRUE(std::get<bool>(collected[0].second));
}

TEST(URITemplateRouter, add_with_single_boolean_argument_false) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"verbose", false}}};
  router.add("/test", 1, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0].first, "verbose");
  EXPECT_FALSE(std::get<bool>(collected[0].second));
}

TEST(URITemplateRouter, add_with_multiple_arguments) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 3> arguments{
      {{"responseSchema", std::string_view{"some/path"}},
       {"maxItems", std::int64_t{100}},
       {"verbose", true}}};
  router.add("/test", 1, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_EQ(collected.size(), 3);
  EXPECT_EQ(collected[0].first, "responseSchema");
  EXPECT_EQ(std::get<std::string_view>(collected[0].second), "some/path");
  EXPECT_EQ(collected[1].first, "maxItems");
  EXPECT_EQ(std::get<std::int64_t>(collected[1].second), 100);
  EXPECT_EQ(collected[2].first, "verbose");
  EXPECT_TRUE(std::get<bool>(collected[2].second));
}

TEST(URITemplateRouter, add_with_empty_arguments_span) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/test", 1,
             std::span<const sourcemeta::core::URITemplateRouter::Argument>{});
  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_TRUE(collected.empty());
}

TEST(URITemplateRouter, add_without_arguments_parameter) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/test", 1);
  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_TRUE(collected.empty());
}

TEST(URITemplateRouter, add_multiple_routes_with_arguments) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
      arguments_one{{{"schema", std::string_view{"path/one"}}}};
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
      arguments_two{{{"limit", std::int64_t{50}}}};
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
      arguments_three{{{"active", false}}};

  router.add("/alpha", 1, arguments_one);
  router.add("/beta", 2, arguments_two);
  router.add("/gamma", 3, arguments_three);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_one;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_one.emplace_back(name, value);
      });
  EXPECT_EQ(collected_one.size(), 1);
  EXPECT_EQ(collected_one[0].first, "schema");
  EXPECT_EQ(std::get<std::string_view>(collected_one[0].second), "path/one");

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_two;
  router.arguments(
      2, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_two.emplace_back(name, value);
      });
  EXPECT_EQ(collected_two.size(), 1);
  EXPECT_EQ(collected_two[0].first, "limit");
  EXPECT_EQ(std::get<std::int64_t>(collected_two[0].second), 50);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_three;
  router.arguments(
      3, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_three.emplace_back(name, value);
      });
  EXPECT_EQ(collected_three.size(), 1);
  EXPECT_EQ(collected_three[0].first, "active");
  EXPECT_FALSE(std::get<bool>(collected_three[0].second));
}

TEST(URITemplateRouter, add_arguments_negative_integer) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"offset", std::int64_t{INT64_MIN}}}};
  router.add("/test", 1, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0].first, "offset");
  EXPECT_EQ(std::get<std::int64_t>(collected[0].second), INT64_MIN);
}

TEST(URITemplateRouter, add_arguments_zero_integer) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"count", std::int64_t{0}}}};
  router.add("/test", 1, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0].first, "count");
  EXPECT_EQ(std::get<std::int64_t>(collected[0].second), 0);
}

TEST(URITemplateRouter, add_arguments_max_integer) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"limit", std::int64_t{INT64_MAX}}}};
  router.add("/test", 1, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0].first, "limit");
  EXPECT_EQ(std::get<std::int64_t>(collected[0].second), INT64_MAX);
}

TEST(URITemplateRouter, add_arguments_empty_string_value) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"description", std::string_view{""}}}};
  router.add("/test", 1, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0].first, "description");
  EXPECT_EQ(std::get<std::string_view>(collected[0].second), "");
}

TEST(URITemplateRouter, add_arguments_empty_string_name) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"", std::string_view{"some_value"}}}};
  router.add("/test", 1, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0].first, "");
  EXPECT_EQ(std::get<std::string_view>(collected[0].second), "some_value");
}

TEST(URITemplateRouter, match_still_works_with_arguments) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"responseSchema", std::string_view{"schemas/user"}}}};
  router.add("/users/{id}", 1, arguments);
  EXPECT_ROUTER_MATCH(router, "/users/42", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

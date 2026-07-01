#include <sourcemeta/core/test.h>
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
    router.add(input, "op", identifier);                                       \
    FAIL();                                                                    \
  } catch (                                                                    \
      const sourcemeta::core::URITemplateRouterInvalidSegmentError &error) {   \
    EXPECT_EQ(error.segment(), expected);                                      \
  } catch (...) {                                                              \
    FAIL();                                                                    \
  }

TEST(single_literal_route) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users", "op_2", 1);
  EXPECT_ROUTER_MATCH(router, "/users", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(single_literal_route_no_match) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users", "op_3", 1);
  EXPECT_ROUTER_MATCH(router, "/posts", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(multi_segment_literal) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users/list", "op_4", 1);
  EXPECT_ROUTER_MATCH(router, "/users/list", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(single_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users/{id}", "op_5", 1);
  EXPECT_ROUTER_MATCH(router, "/users/123", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST(multiple_variables) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users/{id}/posts/{post_id}", "op_6", 1);
  EXPECT_ROUTER_MATCH(router, "/users/42/posts/99", 1, 0, captures);
  EXPECT_EQ(captures.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
  EXPECT_ROUTER_CAPTURE(captures, 1, "post_id", "99");
}

TEST(literal_before_variable_precedence) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users/me", "op_7", 1);
  router.add("/users/{id}", "op_8", 2);
  EXPECT_ROUTER_MATCH(router, "/users/me", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(variable_fallback) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users/me", "op_9", 1);
  router.add("/users/{id}", "op_10", 2);
  EXPECT_ROUTER_MATCH(router, "/users/123", 2, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST(multiple_routes_match_users) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users", "op_11", 1);
  router.add("/users/{id}", "op_12", 2);
  router.add("/posts", "op_13", 3);
  router.add("/posts/{id}", "op_14", 4);
  EXPECT_ROUTER_MATCH(router, "/users", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(multiple_routes_match_users_id) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users", "op_15", 1);
  router.add("/users/{id}", "op_16", 2);
  router.add("/posts", "op_17", 3);
  router.add("/posts/{id}", "op_18", 4);
  EXPECT_ROUTER_MATCH(router, "/users/42", 2, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

TEST(multiple_routes_match_posts) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users", "op_19", 1);
  router.add("/users/{id}", "op_20", 2);
  router.add("/posts", "op_21", 3);
  router.add("/posts/{id}", "op_22", 4);
  EXPECT_ROUTER_MATCH(router, "/posts", 3, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(multiple_routes_match_posts_id) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users", "op_23", 1);
  router.add("/users/{id}", "op_24", 2);
  router.add("/posts", "op_25", 3);
  router.add("/posts/{id}", "op_26", 4);
  EXPECT_ROUTER_MATCH(router, "/posts/99", 4, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "99");
}

TEST(no_match_partial_path) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users/{id}/posts", "op_27", 1);
  EXPECT_ROUTER_MATCH(router, "/users/123", 0, 0, captures);
}

TEST(no_match_extra_segments) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users", "op_28", 1);
  EXPECT_ROUTER_MATCH(router, "/users/123", 0, 0, captures);
}

TEST(empty_path) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users", "op_29", 1);
  EXPECT_ROUTER_MATCH(router, "", 0, 0, captures);
}

TEST(root_template_matches_root) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/", "op_30", 1);
  EXPECT_ROUTER_MATCH(router, "/", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(root_template_no_match_empty) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/", "op_31", 1);
  EXPECT_ROUTER_MATCH(router, "", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(root_template_no_match_path) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/", "op_32", 1);
  EXPECT_ROUTER_MATCH(router, "/foo", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(empty_template_matches_empty) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("", "op_33", 1);
  EXPECT_ROUTER_MATCH(router, "", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(empty_template_no_match_root) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("", "op_34", 1);
  EXPECT_ROUTER_MATCH(router, "/", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(empty_template_no_match_path) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("", "op_35", 1);
  EXPECT_ROUTER_MATCH(router, "/foo", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(root_and_other_routes_match_root) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/", "op_36", 1);
  router.add("/users", "op_37", 2);
  router.add("/users/{id}", "op_38", 3);
  EXPECT_ROUTER_MATCH(router, "/", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(root_and_other_routes_match_users) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/", "op_39", 1);
  router.add("/users", "op_40", 2);
  router.add("/users/{id}", "op_41", 3);
  EXPECT_ROUTER_MATCH(router, "/users", 2, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(root_and_other_routes_match_users_id) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/", "op_42", 1);
  router.add("/users", "op_43", 2);
  router.add("/users/{id}", "op_44", 3);
  EXPECT_ROUTER_MATCH(router, "/users/123", 3, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST(empty_and_root_together_match_empty) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("", "op_45", 1);
  router.add("/", "op_46", 2);
  EXPECT_ROUTER_MATCH(router, "", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(empty_and_root_together_match_root) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("", "op_47", 1);
  router.add("/", "op_48", 2);
  EXPECT_ROUTER_MATCH(router, "/", 2, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(empty_and_root_together_no_match_path) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("", "op_49", 1);
  router.add("/", "op_50", 2);
  EXPECT_ROUTER_MATCH(router, "/foo", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(empty_and_root_and_others_match_empty) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("", "op_51", 1);
  router.add("/", "op_52", 2);
  router.add("/users", "op_53", 3);
  router.add("/users/{id}", "op_54", 4);
  EXPECT_ROUTER_MATCH(router, "", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(empty_and_root_and_others_match_root) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("", "op_55", 1);
  router.add("/", "op_56", 2);
  router.add("/users", "op_57", 3);
  router.add("/users/{id}", "op_58", 4);
  EXPECT_ROUTER_MATCH(router, "/", 2, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(empty_and_root_and_others_match_users) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("", "op_59", 1);
  router.add("/", "op_60", 2);
  router.add("/users", "op_61", 3);
  router.add("/users/{id}", "op_62", 4);
  EXPECT_ROUTER_MATCH(router, "/users", 3, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(empty_and_root_and_others_match_users_id) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("", "op_63", 1);
  router.add("/", "op_64", 2);
  router.add("/users", "op_65", 3);
  router.add("/users/{id}", "op_66", 4);
  EXPECT_ROUTER_MATCH(router, "/users/42", 4, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

TEST(binary_search_literals_gamma) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/alpha", "op_67", 1);
  router.add("/beta", "op_68", 2);
  router.add("/gamma", "op_69", 3);
  router.add("/delta", "op_70", 4);
  router.add("/epsilon", "op_71", 5);
  EXPECT_ROUTER_MATCH(router, "/gamma", 3, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(binary_search_literals_alpha) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/alpha", "op_72", 1);
  router.add("/beta", "op_73", 2);
  router.add("/gamma", "op_74", 3);
  router.add("/delta", "op_75", 4);
  router.add("/epsilon", "op_76", 5);
  EXPECT_ROUTER_MATCH(router, "/alpha", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(binary_search_literals_epsilon) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/alpha", "op_77", 1);
  router.add("/beta", "op_78", 2);
  router.add("/gamma", "op_79", 3);
  router.add("/delta", "op_80", 4);
  router.add("/epsilon", "op_81", 5);
  EXPECT_ROUTER_MATCH(router, "/epsilon", 5, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(conflicting_variable_names_throws) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users/{user_id}/posts", "op_82", 1);
  try {
    router.add("/users/{id}/comments", "op_83", 2);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterVariableMismatchError &error) {
    EXPECT_EQ(error.left(), "user_id");
    EXPECT_EQ(error.right(), "id");
  }
}

TEST(same_variable_names_allowed) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users/{id}/posts", "op_84", 1);
  router.add("/users/{id}/comments", "op_85", 2);
  EXPECT_ROUTER_MATCH(router, "/users/123/posts", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST(conflicting_expansion_variable_names_throws) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/files/{id}", "op_86", 1);
  try {
    router.add("/files/{+path}", "op_87", 2);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterVariableMismatchError &error) {
    EXPECT_EQ(error.left(), "id");
    EXPECT_EQ(error.right(), "path");
  }
}

TEST(reserved_expansion_catch_all) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/files/{+path}", "op_88", 1);
  EXPECT_ROUTER_MATCH(router, "/files/foo/bar/baz.txt", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar/baz.txt");
}

TEST(reserved_expansion_single_segment) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/files/{+path}", "op_89", 1);
  EXPECT_ROUTER_MATCH(router, "/files/readme.md", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "readme.md");
}

TEST(reserved_expansion_with_literal_prefix) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/api/v1/proxy/{+url}", "op_90", 1);
  EXPECT_ROUTER_MATCH(router, "/api/v1/proxy/https://example.com/path", 1, 0,
                      captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "url", "https://example.com/path");
}

TEST(reserved_expansion_matches_single_segment) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/files/{+path}", "op_91", 1);
  EXPECT_ROUTER_MATCH(router, "/files/123", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "123");
}

TEST(reserved_expansion_matches_multi_segment) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/files/{+path}", "op_92", 1);
  EXPECT_ROUTER_MATCH(router, "/files/foo/bar", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar");
}

TEST(expansion_takes_priority_over_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/files/{path}", "op_93", 1);
  router.add("/files/{+path}", "op_94", 2);
  EXPECT_ROUTER_MATCH(router, "/files/readme.md", 2, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "readme.md");
}

TEST(expansion_takes_priority_multi_segment) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/files/{path}", "op_95", 1);
  router.add("/files/{+path}", "op_96", 2);
  EXPECT_ROUTER_MATCH(router, "/files/foo/bar/baz", 2, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar/baz");
}

TEST(expansion_first_then_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/files/{+path}", "op_97", 1);
  router.add("/files/{path}", "op_98", 2);
  EXPECT_ROUTER_MATCH(router, "/files/readme.md", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "readme.md");
}

TEST(literal_takes_priority_over_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/files/{+path}", "op_99", 1);
  router.add("/files/special", "op_100", 2);
  EXPECT_ROUTER_MATCH(router, "/files/special", 2, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(expansion_fallback_from_literal) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/files/{+path}", "op_101", 1);
  router.add("/files/special", "op_102", 2);
  EXPECT_ROUTER_MATCH(router, "/files/other", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "other");
}

TEST(literal_suffix_after_variable_throws) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar}.json", 1, "{bar}.json");
}

TEST(literal_prefix_before_variable_throws) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/prefix{bar}", 1, "prefix{bar}");
}

TEST(mixed_segment_variable_then_literal_suffix) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/{id}.html", 1, "{id}.html");
}

TEST(mixed_segment_expansion_then_literal_suffix) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/files/{+path}.bak", 1, "{+path}.bak");
}

TEST(mixed_segment_prefix_before_expansion_throws) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/api/v1{+rest}", 1, "v1{+rest}");
}

TEST(mixed_segment_suffix_in_middle) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar}.json/baz", 1, "{bar}.json");
}

TEST(mixed_segment_prefix_in_middle) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/prefix{bar}/baz", 1, "prefix{bar}");
}

TEST(mixed_segment_expansion_suffix_in_middle) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/files/{+path}.txt/meta", 1,
                              "{+path}.txt");
}

TEST(mixed_segment_expansion_prefix_in_middle) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/api/v2{+path}/end", 1, "v2{+path}");
}

TEST(error_unclosed_brace) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar", 1, "{bar");
}

TEST(error_unclosed_brace_at_end) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/bar{", 1, "bar{");
}

TEST(error_empty_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{}", 1, "{}");
}

TEST(error_empty_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{+}", 1, "{+}");
}

TEST(error_nested_brace) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{{bar}}", 1, "{{bar}}");
}

TEST(error_unmatched_close_brace) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/bar}", 1, "bar}");
}

TEST(error_space_before_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{ bar}", 1, "{ bar}");
}

TEST(error_space_after_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar }", 1, "{bar }");
}

TEST(error_space_in_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar baz}", 1, "{bar baz}");
}

TEST(error_invalid_variable_start) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{-bar}", 1, "{-bar}");
}

TEST(error_invalid_character_in_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar!baz}", 1, "{bar!baz}");
}

TEST(error_reserved_operator_equals) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{=bar}", 1, "{=bar}");
}

TEST(error_reserved_operator_pipe) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{|bar}", 1, "{|bar}");
}

TEST(error_reserved_operator_exclamation) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{!bar}", 1, "{!bar}");
}

TEST(error_reserved_operator_at) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{@bar}", 1, "{@bar}");
}

TEST(error_bracket_in_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar[0]}", 1, "{bar[0]}");
}

TEST(error_multiple_variables) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar,baz}", 1, "{bar,baz}");
}

TEST(error_prefix_modifier) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar:3}", 1, "{bar:3}");
}

TEST(error_explode_modifier) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{bar*}", 1, "{bar*}");
}

TEST(error_unsupported_fragment_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{#bar}", 1, "{#bar}");
}

TEST(error_unsupported_label_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{.bar}", 1, "{.bar}");
}

TEST(error_unsupported_path_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{/bar}", 1, "{/bar}");
}

TEST(error_unsupported_path_parameter_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{;bar}", 1, "{;bar}");
}

TEST(error_unsupported_query_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{?bar}", 1, "{?bar}");
}

TEST(error_unsupported_query_continuation_expansion) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{&bar}", 1, "{&bar}");
}

TEST(error_expansion_not_last_segment_literal) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{+bar}/baz", 1, "{+bar}");
}

TEST(error_expansion_not_last_segment_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{+bar}/{baz}", 1, "{+bar}");
}

TEST(error_expansion_not_last_segment_trailing_slash) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/foo/{+bar}/", 1, "{+bar}");
}

TEST(trailing_slash_no_match) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users", "op_103", 1);
  EXPECT_ROUTER_MATCH(router, "/users/", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(multiple_trailing_slashes_no_match) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users", "op_104", 1);
  EXPECT_ROUTER_MATCH(router, "/users///", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(leading_double_slash_no_match) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users", "op_105", 1);
  EXPECT_ROUTER_MATCH(router, "//users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(internal_double_slashes_no_match) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users/posts", "op_106", 1);
  EXPECT_ROUTER_MATCH(router, "/users//posts", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_with_variable_no_match) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users/{id}", "op_107", 1);
  EXPECT_ROUTER_MATCH(router, "/users/123/", 0, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST(internal_double_slash_with_variable_no_match) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/users/{id}/posts", "op_108", 1);
  EXPECT_ROUTER_MATCH(router, "/users//123//posts", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(expansion_matches_trailing_slash) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/files/{+path}", "op_109", 1);
  EXPECT_ROUTER_MATCH(router, "/files/foo/bar/", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar/");
}

TEST(expansion_matches_double_slashes) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/files/{+path}", "op_110", 1);
  EXPECT_ROUTER_MATCH(router, "/files/foo//bar", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo//bar");
}

TEST(add_with_single_string_argument) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"responseSchema", std::string_view{"some/path"}}}};
  router.add("/test", "op_111", 1, 0, arguments);

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

TEST(add_with_single_integer_argument) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"maxItems", std::int64_t{42}}}};
  router.add("/test", "op_112", 1, 0, arguments);

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

TEST(add_with_single_boolean_argument_true) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"verbose", true}}};
  router.add("/test", "op_113", 1, 0, arguments);

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

TEST(add_with_single_boolean_argument_false) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"verbose", false}}};
  router.add("/test", "op_114", 1, 0, arguments);

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

TEST(add_with_multiple_arguments) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 3> arguments{
      {{"responseSchema", std::string_view{"some/path"}},
       {"maxItems", std::int64_t{100}},
       {"verbose", true}}};
  router.add("/test", "op_115", 1, 0, arguments);

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

TEST(add_with_empty_arguments_span) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/test", "op_116", 1, 0,
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

TEST(add_without_arguments_parameter) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  router.add("/test", "op_117", 1);
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

TEST(add_multiple_routes_with_arguments) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
      arguments_one{{{"schema", std::string_view{"path/one"}}}};
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
      arguments_two{{{"limit", std::int64_t{50}}}};
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
      arguments_three{{{"active", false}}};

  router.add("/alpha", "op_118", 1, 0, arguments_one);
  router.add("/beta", "op_119", 2, 0, arguments_two);
  router.add("/gamma", "op_120", 3, 0, arguments_three);

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

TEST(add_arguments_negative_integer) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"offset", std::int64_t{INT64_MIN}}}};
  router.add("/test", "op_121", 1, 0, arguments);

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

TEST(add_arguments_zero_integer) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"count", std::int64_t{0}}}};
  router.add("/test", "op_122", 1, 0, arguments);

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

TEST(add_arguments_max_integer) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"limit", std::int64_t{INT64_MAX}}}};
  router.add("/test", "op_123", 1, 0, arguments);

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

TEST(add_arguments_empty_string_value) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"description", std::string_view{""}}}};
  router.add("/test", "op_124", 1, 0, arguments);

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

TEST(add_arguments_empty_string_name) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"", std::string_view{"some_value"}}}};
  router.add("/test", "op_125", 1, 0, arguments);

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

TEST(match_still_works_with_arguments) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_path().empty());
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"responseSchema", std::string_view{"schemas/user"}}}};
  router.add("/users/{id}", "op_126", 1, 0, arguments);
  EXPECT_ROUTER_MATCH(router, "/users/42", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

TEST(base_path_single_segment) {
  sourcemeta::core::URITemplateRouter router{"/prefix"};
  EXPECT_EQ(router.base_path(), "/prefix");
  router.add("/foo", "op_127", 1);
  EXPECT_ROUTER_MATCH(router, "/prefix/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(base_path_without_prefix_no_match) {
  sourcemeta::core::URITemplateRouter router{"/prefix"};
  EXPECT_EQ(router.base_path(), "/prefix");
  router.add("/foo", "op_128", 1);
  EXPECT_ROUTER_MATCH(router, "/foo", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(base_path_multi_segment) {
  sourcemeta::core::URITemplateRouter router{"/v1/catalog"};
  EXPECT_EQ(router.base_path(), "/v1/catalog");
  router.add("/api/list", "op_129", 1);
  router.add("/{+path}", "op_130", 2);
  EXPECT_ROUTER_MATCH(router, "/v1/catalog/api/list", 1, 0, captures_list);
  EXPECT_EQ(captures_list.size(), 0);
  EXPECT_ROUTER_MATCH(router, "/v1/catalog/foo/bar", 2, 0, captures_expansion);
  EXPECT_EQ(captures_expansion.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_expansion, 0, "path", "foo/bar");
}

TEST(base_path_with_variable) {
  sourcemeta::core::URITemplateRouter router{"/prefix"};
  EXPECT_EQ(router.base_path(), "/prefix");
  router.add("/users/{id}", "op_131", 1);
  EXPECT_ROUTER_MATCH(router, "/prefix/users/42", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

TEST(base_path_prefix_boundary_no_match) {
  sourcemeta::core::URITemplateRouter router{"/prefix"};
  EXPECT_EQ(router.base_path(), "/prefix");
  router.add("/foo", "op_132", 1);
  EXPECT_ROUTER_MATCH(router, "/prefixfoo", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(base_path_with_empty_template) {
  sourcemeta::core::URITemplateRouter router{"/v1/catalog"};
  EXPECT_EQ(router.base_path(), "/v1/catalog");
  router.add("", "op_133", 1);
  EXPECT_ROUTER_MATCH(router, "/v1/catalog", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(base_path_slash_only_is_no_base_path) {
  sourcemeta::core::URITemplateRouter router{"/"};
  EXPECT_TRUE(router.base_path().empty());
  router.add("/foo", "op_134", 1);
  EXPECT_ROUTER_MATCH(router, "/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(base_path_trailing_slash_normalized) {
  sourcemeta::core::URITemplateRouter router{"/prefix/"};
  EXPECT_EQ(router.base_path(), "/prefix");
  router.add("/foo", "op_135", 1);
  EXPECT_ROUTER_MATCH(router, "/prefix/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(base_path_multiple_trailing_slashes_normalized) {
  sourcemeta::core::URITemplateRouter router{"/prefix///"};
  EXPECT_EQ(router.base_path(), "/prefix");
  router.add("/foo", "op_136", 1);
  EXPECT_ROUTER_MATCH(router, "/prefix/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(base_path_expansion) {
  sourcemeta::core::URITemplateRouter router{"/api"};
  EXPECT_EQ(router.base_path(), "/api");
  router.add("/files/{+path}", "op_137", 1);
  EXPECT_ROUTER_MATCH(router, "/api/files/a/b/c", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "a/b/c");
}

TEST(base_path_trailing_slash_on_request_no_match) {
  sourcemeta::core::URITemplateRouter router{"/v1/catalog"};
  EXPECT_EQ(router.base_path(), "/v1/catalog");
  router.add("/foo", "op_138", 1);
  EXPECT_ROUTER_MATCH(router, "/v1/catalog/foo/", 0, 0, captures);
}

TEST(base_path_empty_string_is_no_base_path) {
  sourcemeta::core::URITemplateRouter router{""};
  EXPECT_TRUE(router.base_path().empty());
  router.add("/foo", "op_139", 1);
  EXPECT_ROUTER_MATCH(router, "/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(base_path_wrong_prefix_no_match) {
  sourcemeta::core::URITemplateRouter router{"/v1/catalog"};
  EXPECT_EQ(router.base_path(), "/v1/catalog");
  router.add("/api/list", "op_140", 1);
  EXPECT_ROUTER_MATCH(router, "/v2/catalog/api/list", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(base_path_partial_prefix_no_match) {
  sourcemeta::core::URITemplateRouter router{"/v1/catalog"};
  EXPECT_EQ(router.base_path(), "/v1/catalog");
  router.add("/api/list", "op_141", 1);
  EXPECT_ROUTER_MATCH(router, "/v1/api/list", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(base_url_empty_by_default) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_TRUE(router.base_url().empty());
}

TEST(base_url_empty_when_only_base_path) {
  sourcemeta::core::URITemplateRouter router{"/prefix"};
  EXPECT_EQ(router.base_path(), "/prefix");
  EXPECT_TRUE(router.base_url().empty());
}

TEST(base_url_set_with_base_path) {
  sourcemeta::core::URITemplateRouter router{"/v1", "https://api.example.com"};
  EXPECT_EQ(router.base_path(), "/v1");
  EXPECT_EQ(router.base_url(), "https://api.example.com");
}

TEST(base_url_set_without_base_path) {
  sourcemeta::core::URITemplateRouter router{"", "https://api.example.com"};
  EXPECT_TRUE(router.base_path().empty());
  EXPECT_EQ(router.base_url(), "https://api.example.com");
}

TEST(base_url_trailing_slash_normalized) {
  sourcemeta::core::URITemplateRouter router{"", "https://api.example.com/"};
  EXPECT_EQ(router.base_url(), "https://api.example.com");
}

TEST(base_url_multiple_trailing_slashes_normalized) {
  sourcemeta::core::URITemplateRouter router{"", "https://api.example.com///"};
  EXPECT_EQ(router.base_url(), "https://api.example.com");
}

TEST(base_url_only_slash_collapses_to_empty) {
  sourcemeta::core::URITemplateRouter router{"", "/"};
  EXPECT_TRUE(router.base_url().empty());
}

TEST(base_url_not_used_for_matching) {
  sourcemeta::core::URITemplateRouter router{"/v1", "https://api.example.com"};
  router.add("/foo", "op_base_url_match", 1);
  EXPECT_ROUTER_MATCH(router, "/v1/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
  EXPECT_ROUTER_MATCH(router, "https://api.example.com/v1/foo", 0, 0,
                      captures_absolute);
  EXPECT_EQ(captures_absolute.size(), 0);
}

TEST(add_with_context_literal_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_142", 1, 7);
  EXPECT_ROUTER_MATCH(router, "/users", 1, 7, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(add_with_context_variable_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}", "op_143", 1, 42);
  EXPECT_ROUTER_MATCH(router, "/users/123", 1, 42, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST(add_with_context_default_zero) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_144", 1);
  EXPECT_ROUTER_MATCH(router, "/users", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(add_multiple_routes_different_contexts) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_145", 1, 1);
  router.add("/posts", "op_146", 2, 2);
  {
    EXPECT_ROUTER_MATCH(router, "/users", 1, 1, captures);
    EXPECT_EQ(captures.size(), 0);
  }
  {
    EXPECT_ROUTER_MATCH(router, "/posts", 2, 2, captures);
    EXPECT_EQ(captures.size(), 0);
  }
}

TEST(add_same_context_multiple_routes) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_147", 1, 99);
  router.add("/posts", "op_148", 2, 99);
  {
    EXPECT_ROUTER_MATCH(router, "/users", 1, 99, captures);
    EXPECT_EQ(captures.size(), 0);
  }
  {
    EXPECT_ROUTER_MATCH(router, "/posts", 2, 99, captures);
    EXPECT_EQ(captures.size(), 0);
  }
}

TEST(add_with_context_and_arguments) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 2> arguments{{
      {"schema", std::string_view{"schemas/health"}},
      {"enabled", true},
  }};
  router.add("/api/health", "op_149", 1, 11, arguments);
  EXPECT_ROUTER_MATCH(router, "/api/health", 1, 11, captures);
  EXPECT_EQ(captures.size(), 0);

  std::vector<std::pair<std::string, std::string>> seen_string;
  std::vector<std::pair<std::string, bool>> seen_bool;
  router.arguments(
      1, [&seen_string, &seen_bool](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        if (std::holds_alternative<std::string_view>(value)) {
          seen_string.emplace_back(
              std::string{name},
              std::string{std::get<std::string_view>(value)});
        } else if (std::holds_alternative<bool>(value)) {
          seen_bool.emplace_back(std::string{name}, std::get<bool>(value));
        }
      });

  EXPECT_EQ(seen_string.size(), 1);
  EXPECT_EQ(seen_string.at(0).first, "schema");
  EXPECT_EQ(seen_string.at(0).second, "schemas/health");
  EXPECT_EQ(seen_bool.size(), 1);
  EXPECT_EQ(seen_bool.at(0).first, "enabled");
  EXPECT_TRUE(seen_bool.at(0).second);
}

TEST(add_context_expansion_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", "op_150", 1, 5);
  EXPECT_ROUTER_MATCH(router, "/files/a/b/c", 1, 5, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "a/b/c");
}

TEST(add_context_base_path) {
  sourcemeta::core::URITemplateRouter router{"/v1/catalog"};
  router.add("/api/list", "op_151", 1, 33);
  EXPECT_ROUTER_MATCH(router, "/v1/catalog/api/list", 1, 33, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(add_with_context_no_match_returns_zero_pair) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_152", 1, 7);
  EXPECT_ROUTER_MATCH(router, "/posts", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(add_with_context_overwrites_previous_context) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_153", 1, 10);
  router.add("/users", "op_154", 1, 20);
  EXPECT_ROUTER_MATCH(router, "/users", 1, 20, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(size_empty_router) {
  const sourcemeta::core::URITemplateRouter router;
  EXPECT_EQ(router.size(), 0);
}

TEST(size_single_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_155", 1);
  EXPECT_EQ(router.size(), 1);
}

TEST(size_multiple_routes) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_156", 1);
  router.add("/users/{id}", "op_157", 2);
  router.add("/posts", "op_158", 3);
  router.add("/posts/{id}", "op_159", 4);
  EXPECT_EQ(router.size(), 4);
}

TEST(size_duplicate_route_does_not_increase) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_160", 1);
  router.add("/users", "op_161", 2);
  EXPECT_EQ(router.size(), 1);
}

TEST(size_with_context_overwrite_does_not_increase) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_162", 1, 10);
  router.add("/users", "op_163", 1, 20);
  EXPECT_EQ(router.size(), 1);
}

TEST(size_root_template) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", "op_164", 1);
  EXPECT_EQ(router.size(), 1);
}

TEST(size_with_base_path) {
  sourcemeta::core::URITemplateRouter router{"/v1"};
  router.add("/users", "op_165", 1);
  router.add("/posts", "op_166", 2);
  EXPECT_EQ(router.size(), 2);
}

TEST(otherwise_default_is_zero_context) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_167", 1);
  EXPECT_ROUTER_MATCH(router, "/unknown", 0, 0, captures);
}

TEST(otherwise_sets_context) {
  sourcemeta::core::URITemplateRouter router;
  router.otherwise(42);
  EXPECT_ROUTER_MATCH(router, "/anything", 0, 42, captures);
}

TEST(otherwise_returned_from_match_on_unknown_path) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_168", 1, 5);
  router.otherwise(99);
  EXPECT_ROUTER_MATCH(router, "/unknown", 0, 99, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(otherwise_not_returned_from_matching_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_169", 1, 5);
  router.otherwise(99);
  EXPECT_ROUTER_MATCH(router, "/users", 1, 5, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(otherwise_returned_for_empty_segment) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_170", 1);
  router.otherwise(77);
  EXPECT_ROUTER_MATCH(router, "/users//", 0, 77, captures);
}

TEST(otherwise_returned_for_root_slash_no_match) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_171", 1);
  router.otherwise(88);
  EXPECT_ROUTER_MATCH(router, "/", 0, 88, captures);
}

TEST(otherwise_without_registration_returns_zero_context) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_172", 1);
  EXPECT_ROUTER_MATCH(router, "/unknown", 0, 0, captures);
}

TEST(otherwise_arguments_lookup) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 2> arguments{
      {{"status", std::int64_t{404}},
       {"message", std::string_view{"Not Found"}}}};
  router.otherwise(55, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      0, [&collected](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });

  EXPECT_EQ(collected.size(), 2);
  EXPECT_EQ(collected[0].first, "status");
  EXPECT_TRUE(std::holds_alternative<std::int64_t>(collected[0].second));
  EXPECT_EQ(std::get<std::int64_t>(collected[0].second), 404);
  EXPECT_EQ(collected[1].first, "message");
  EXPECT_TRUE(std::holds_alternative<std::string_view>(collected[1].second));
  EXPECT_EQ(std::get<std::string_view>(collected[1].second), "Not Found");
}

TEST(otherwise_overwrite_context) {
  sourcemeta::core::URITemplateRouter router;
  router.otherwise(10);
  router.otherwise(20);
  EXPECT_ROUTER_MATCH(router, "/nope", 0, 20, captures);
}

TEST(otherwise_overwrite_arguments) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> first{
      {{"version", std::int64_t{1}}}};
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> second{
      {{"version", std::int64_t{2}}}};
  router.otherwise(10, first);
  router.otherwise(20, second);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      0, [&collected](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });

  EXPECT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0].first, "version");
  EXPECT_EQ(std::get<std::int64_t>(collected[0].second), 2);
}

TEST(otherwise_overwrite_with_empty_clears_arguments) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"key", std::string_view{"value"}}}};
  router.otherwise(1, arguments);
  router.otherwise(2);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      0, [&collected](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });

  EXPECT_TRUE(collected.empty());
  EXPECT_ROUTER_MATCH(router, "/nope", 0, 2, captures);
}

TEST(otherwise_boolean_argument) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> arguments{
      {{"cached", true}}};
  router.otherwise(3, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      0, [&collected](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });

  EXPECT_EQ(collected.size(), 1);
  EXPECT_TRUE(std::holds_alternative<bool>(collected[0].second));
  EXPECT_EQ(std::get<bool>(collected[0].second), true);
}

TEST(otherwise_does_not_affect_other_arguments) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> route_args{
      {{"schema", std::string_view{"user.json"}}}};
  router.add("/users", "op_173", 1, 0, route_args);

  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
      default_args{{{"message", std::string_view{"not found"}}}};
  router.otherwise(99, default_args);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      route_collected;
  router.arguments(
      1, [&route_collected](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        route_collected.emplace_back(name, value);
      });
  EXPECT_EQ(route_collected.size(), 1);
  EXPECT_EQ(route_collected[0].first, "schema");

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      default_collected;
  router.arguments(
      0, [&default_collected](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        default_collected.emplace_back(name, value);
      });
  EXPECT_EQ(default_collected.size(), 1);
  EXPECT_EQ(default_collected[0].first, "message");
}

TEST(otherwise_does_not_count_as_route_in_size) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_174", 1);
  router.otherwise(99);
  EXPECT_EQ(router.size(), 1);
}

TEST(otherwise_with_base_path_and_unmatched) {
  sourcemeta::core::URITemplateRouter router{"/v1"};
  router.add("/users", "op_175", 1);
  router.otherwise(42);
  EXPECT_ROUTER_MATCH(router, "/v1/other", 0, 42, captures);
}

TEST(otherwise_with_partial_trie_walk) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}/posts", "op_176", 1);
  router.otherwise(42);
  EXPECT_ROUTER_MATCH(router, "/users/123", 0, 42, captures);
}

TEST(listing_at_returns_identifiers_in_add_order) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_177", 7);
  router.add("/posts/{id}", "op_178", 3);
  router.add("/{+rest}", "op_179", 9);
  EXPECT_EQ(router.size(), 3);
  EXPECT_EQ(router.at(0), 7);
  EXPECT_EQ(router.at(1), 3);
  EXPECT_EQ(router.at(2), 9);
}

TEST(listing_context_returns_associated_context) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_180", 1, 100);
  router.add("/posts/{id}", "op_181", 2, 200);
  router.add("/comments", "op_182", 3, 300);
  EXPECT_EQ(router.context(1), 100);
  EXPECT_EQ(router.context(2), 200);
  EXPECT_EQ(router.context(3), 300);
}

TEST(listing_context_default_zero) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_183", 1);
  router.add("/posts", "op_184", 2);
  EXPECT_EQ(router.context(1), 0);
  EXPECT_EQ(router.context(2), 0);
}

TEST(listing_path_for_literal_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_185", 1);
  EXPECT_EQ(router.path(1), "/users");
}

TEST(listing_path_for_variable_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}", "op_186", 1);
  EXPECT_EQ(router.path(1), "/users/{id}");
}

TEST(listing_path_for_multi_variable_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}/posts/{post_id}", "op_187", 1);
  EXPECT_EQ(router.path(1), "/users/{id}/posts/{post_id}");
}

TEST(listing_path_for_expansion_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+rest}", "op_188", 1);
  EXPECT_EQ(router.path(1), "/files/{+rest}");
}

TEST(listing_path_for_root_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/", "op_189", 1);
  EXPECT_EQ(router.path(1), "/");
}

TEST(listing_path_excludes_base_path) {
  sourcemeta::core::URITemplateRouter router{"/api/v1"};
  router.add("/users/{id}", "op_190", 1);
  EXPECT_EQ(router.path(1), "/users/{id}");
}

TEST(listing_path_excludes_base_path_for_root_template) {
  sourcemeta::core::URITemplateRouter router{"/api"};
  router.add("/", "op_191", 1);
  EXPECT_EQ(router.path(1), "/");
}

TEST(listing_path_excludes_base_path_for_empty_template) {
  sourcemeta::core::URITemplateRouter router{"/api"};
  router.add("", "op_192", 1);
  EXPECT_EQ(router.path(1), "");
}

TEST(listing_path_for_multiple_routes_each_correct) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_193", 1);
  router.add("/users/{id}", "op_194", 2);
  router.add("/posts/{id}/comments/{comment_id}", "op_195", 3);
  router.add("/files/{+rest}", "op_196", 4);
  EXPECT_EQ(router.path(1), "/users");
  EXPECT_EQ(router.path(2), "/users/{id}");
  EXPECT_EQ(router.path(3), "/posts/{id}/comments/{comment_id}");
  EXPECT_EQ(router.path(4), "/files/{+rest}");
}

TEST(listing_size_does_not_count_otherwise) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_197", 1);
  router.add("/posts", "op_198", 2);
  router.otherwise(99);
  EXPECT_EQ(router.size(), 2);
  EXPECT_EQ(router.at(0), 1);
  EXPECT_EQ(router.at(1), 2);
}

TEST(listing_path_returns_freshly_allocated_string) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}", "op_199", 1);
  const auto first = router.path(1);
  const auto second = router.path(1);
  EXPECT_EQ(first, second);
  EXPECT_NE(first.data(), second.data());
}

TEST(listing_iterate_via_size_and_at) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/a", "op_200", 10);
  router.add("/b/{id}", "op_201", 20);
  router.add("/c/{+rest}", "op_202", 30);

  std::vector<sourcemeta::core::URITemplateRouter::Identifier> seen;
  for (std::size_t index = 0; index < router.size(); ++index) {
    seen.push_back(router.at(index));
  }

  EXPECT_EQ(seen.size(), 3);
  EXPECT_EQ(seen[0], 10);
  EXPECT_EQ(seen[1], 20);
  EXPECT_EQ(seen[2], 30);
}

TEST(listing_size_overwrite_does_not_grow) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_203", 1);
  EXPECT_EQ(router.size(), 1);
  router.add("/users", "op_204", 2);
  EXPECT_EQ(router.size(), 1);
  EXPECT_EQ(router.at(0), 2);
  EXPECT_EQ(router.path(2), "/users");
}

TEST(operation_id_minimum_length) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/a", "x", 1);
  const auto result = router.operation("x");
  EXPECT_EQ(result.first, 1);
  EXPECT_EQ(result.second, 0);
}

TEST(operation_id_maximum_length) {
  sourcemeta::core::URITemplateRouter router;
  const std::string operation_id(64, 'a');
  router.add("/a", operation_id, 1);
  const auto result = router.operation(operation_id);
  EXPECT_EQ(result.first, 1);
  EXPECT_EQ(result.second, 0);
}

TEST(operation_id_full_character_set) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/a", "aZ0_-", 7, 42);
  const auto result = router.operation("aZ0_-");
  EXPECT_EQ(result.first, 7);
  EXPECT_EQ(result.second, 42);
}

TEST(operation_id_reject_empty) {
  sourcemeta::core::URITemplateRouter router;
  try {
    router.add("/a", "", 1);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterInvalidOperationIdError &error) {
    EXPECT_EQ(error.operation_id(), "");
  }
}

TEST(operation_id_reject_too_long) {
  sourcemeta::core::URITemplateRouter router;
  const std::string operation_id(65, 'a');
  try {
    router.add("/a", operation_id, 1);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterInvalidOperationIdError &error) {
    EXPECT_EQ(error.operation_id(), operation_id);
  } catch (...) {
    FAIL();
  }
}

TEST(operation_id_reject_leading_digit) {
  sourcemeta::core::URITemplateRouter router;
  try {
    router.add("/a", "1foo", 1);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterInvalidOperationIdError &error) {
    EXPECT_EQ(error.operation_id(), "1foo");
  }
}

TEST(operation_id_reject_leading_underscore) {
  sourcemeta::core::URITemplateRouter router;
  try {
    router.add("/a", "_foo", 1);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterInvalidOperationIdError &error) {
    EXPECT_EQ(error.operation_id(), "_foo");
  }
}

TEST(operation_id_reject_leading_hyphen) {
  sourcemeta::core::URITemplateRouter router;
  try {
    router.add("/a", "-foo", 1);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterInvalidOperationIdError &error) {
    EXPECT_EQ(error.operation_id(), "-foo");
  }
}

TEST(operation_id_reject_invalid_characters) {
  sourcemeta::core::URITemplateRouter router;
  try {
    router.add("/a", "foo.bar", 1);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterInvalidOperationIdError &error) {
    EXPECT_EQ(error.operation_id(), "foo.bar");
  }
  try {
    router.add("/a", "foo/bar", 1);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterInvalidOperationIdError &error) {
    EXPECT_EQ(error.operation_id(), "foo/bar");
  }
  try {
    router.add("/a", "foo bar", 1);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterInvalidOperationIdError &error) {
    EXPECT_EQ(error.operation_id(), "foo bar");
  }
}

TEST(operation_id_duplicate_throws) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/a", "listUsers", 1);
  try {
    router.add("/b", "listUsers", 2);
    FAIL();
  } catch (const sourcemeta::core::URITemplateRouterDuplicateOperationIdError
               &error) {
    EXPECT_EQ(error.operation_id(), "listUsers");
  } catch (...) {
    FAIL();
  }
}

TEST(operation_unknown_returns_zero_zero) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/a", "listUsers", 1, 11);
  const auto result = router.operation("missing");
  EXPECT_EQ(result.first, 0);
  EXPECT_EQ(result.second, 0);
}

TEST(operation_unknown_ignores_otherwise) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/a", "listUsers", 1, 11);
  router.otherwise(99);
  const auto result = router.operation("missing");
  EXPECT_EQ(result.first, 0);
  EXPECT_EQ(result.second, 0);
}

TEST(operation_returns_identifier_and_context) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/a", "alpha", 1, 11);
  router.add("/b", "beta", 2, 22);
  router.add("/c", "gamma", 3, 33);

  const auto alpha = router.operation("alpha");
  EXPECT_EQ(alpha.first, 1);
  EXPECT_EQ(alpha.second, 11);

  const auto beta = router.operation("beta");
  EXPECT_EQ(beta.first, 2);
  EXPECT_EQ(beta.second, 22);

  const auto gamma = router.operation("gamma");
  EXPECT_EQ(gamma.first, 3);
  EXPECT_EQ(gamma.second, 33);
}

TEST(operation_id_not_reserved_when_add_throws) {
  sourcemeta::core::URITemplateRouter router;
  try {
    router.add("/foo/{bar}.json", "listFoo", 1);
    FAIL();
  } catch (const sourcemeta::core::URITemplateRouterInvalidSegmentError &) {
    router.add("/users", "listFoo", 2);
    const auto result = router.operation("listFoo");
    EXPECT_EQ(result.first, 2);
  } catch (...) {
    FAIL();
  }
}

TEST(operation_id_overwrite_removes_previous_mapping) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "first", 1, 11);
  router.add("/users", "second", 2, 22);

  const auto stale = router.operation("first");
  EXPECT_EQ(stale.first, 0);
  EXPECT_EQ(stale.second, 0);

  const auto live = router.operation("second");
  EXPECT_EQ(live.first, 2);
  EXPECT_EQ(live.second, 22);
}

TEST(optional_expansion_accepted_simple) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/list{/path*}", "op_oex_1", 1);
  EXPECT_EQ(router.size(), 1);
}

TEST(optional_expansion_accepted_with_explicit_slash) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list/{/path*}", "op_oex_2", 1);
  EXPECT_EQ(router.size(), 1);
}

TEST(optional_expansion_accepted_at_root) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/{/path*}", "op_oex_3", 1);
  EXPECT_EQ(router.size(), 1);
}

TEST(optional_expansion_accepted_with_deep_literal_prefix) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/v1/users/list{/path*}", "op_oex_4", 1);
  EXPECT_EQ(router.size(), 1);
}

TEST(optional_expansion_accepted_no_literal_prefix) {
  sourcemeta::core::URITemplateRouter router;
  router.add("{/path*}", "op_oex_5", 1);
  EXPECT_EQ(router.size(), 1);
}

TEST(optional_expansion_reject_no_explode) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/list{/path}", 1, "{/path}");
}

TEST(optional_expansion_reject_no_explode_at_root) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "{/path}", 1, "{/path}");
}

TEST(optional_expansion_reject_empty_name) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/list{/}", 1, "{/}");
}

TEST(optional_expansion_reject_empty_name_with_explode) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/list{/*}", 1, "{/*}");
}

TEST(optional_expansion_reject_unclosed) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/list{/path*", 1, "{/path*");
}

TEST(optional_expansion_reject_not_last_segment_literal) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/list{/path*}/more", 1, "{/path*}");
}

TEST(optional_expansion_reject_not_last_segment_variable) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/list{/path*}/{tail}", 1, "{/path*}");
}

TEST(optional_expansion_reject_not_last_segment_trailing_slash) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/list{/path*}/", 1, "{/path*}");
}

TEST(optional_expansion_reject_extra_after_explode) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/list{/path*abc}", 1, "{/path*abc}");
}

TEST(optional_expansion_reject_comma_after_explode) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/list{/path*,more}", 1, "{/path*,more}");
}

TEST(optional_expansion_reject_invalid_varname_char) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/list{/pa-th*}", 1, "{/pa-th*}");
}

TEST(simple_explode_still_rejected) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/list/{var*}", 1, "{var*}");
}

TEST(reserved_explode_still_rejected) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/list/{+var*}", 1, "{+var*}");
}

TEST(optional_expansion_dotted_varname_accepted) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/list{/foo.bar*}", "op_oex_dot", 1);
  EXPECT_ROUTER_MATCH(router, "/list/x", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "foo.bar", "x");
}

TEST(literal_followed_by_path_segment_no_slash) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_join", 1);
  EXPECT_ROUTER_MATCH(router, "/api/list/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo");
}

TEST(literal_followed_by_non_path_segment_still_throws) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/api/list{path}", 1, "list{path}");
}

TEST(literal_followed_by_reserved_expansion_still_throws) {
  sourcemeta::core::URITemplateRouter router;
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/api/list{+path}", 1, "list{+path}");
}

TEST(optional_expansion_match_empty_capture) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m1", 1);
  EXPECT_ROUTER_MATCH(router, "/api/list", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "");
}

TEST(optional_expansion_match_single_segment) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m2", 1);
  EXPECT_ROUTER_MATCH(router, "/api/list/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo");
}

TEST(optional_expansion_match_two_segments) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m3", 1);
  EXPECT_ROUTER_MATCH(router, "/api/list/foo/bar", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar");
}

TEST(optional_expansion_match_many_segments) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m4", 1);
  EXPECT_ROUTER_MATCH(router, "/api/list/foo/bar/baz/qux", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar/baz/qux");
}

TEST(optional_expansion_match_percent_encoded_segment) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m5", 1);
  EXPECT_ROUTER_MATCH(router, "/api/list/foo%2Fbar", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo%2Fbar");
}

TEST(optional_expansion_match_dot_segment) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m6", 1);
  EXPECT_ROUTER_MATCH(router, "/api/list/.well-known/version", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", ".well-known/version");
}

TEST(optional_expansion_reject_trailing_slash) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m7", 1);
  EXPECT_ROUTER_MATCH(router, "/api/list/", 0, 0, captures);
}

TEST(optional_expansion_reject_double_trailing_slash) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m8", 1);
  EXPECT_ROUTER_MATCH(router, "/api/list//", 0, 0, captures);
}

TEST(optional_expansion_reject_internal_empty_segment) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m9", 1);
  EXPECT_ROUTER_MATCH(router, "/api/list//foo", 0, 0, captures);
}

TEST(optional_expansion_reject_non_prefix) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m10", 1);
  EXPECT_ROUTER_MATCH(router, "/api/listing", 0, 0, captures);
}

TEST(optional_expansion_reject_unrelated_root) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m11", 1);
  EXPECT_ROUTER_MATCH(router, "/other", 0, 0, captures);
}

TEST(optional_expansion_reject_empty_path) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m12", 1);
  EXPECT_ROUTER_MATCH(router, "", 0, 0, captures);
}

TEST(optional_expansion_reject_root_path) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_m13", 1);
  EXPECT_ROUTER_MATCH(router, "/", 0, 0, captures);
}

TEST(optional_expansion_root_template_does_not_match_slash) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/{/path*}", "op_oex_m14", 1);
  EXPECT_ROUTER_MATCH(router, "/", 0, 0, captures);
}

TEST(optional_expansion_root_template_matches_segment) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/{/path*}", "op_oex_m15", 1);
  EXPECT_ROUTER_MATCH(router, "/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo");
}

TEST(optional_expansion_root_template_matches_multiple_segments) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/{/path*}", "op_oex_m16", 1);
  EXPECT_ROUTER_MATCH(router, "/foo/bar", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar");
}

TEST(optional_expansion_callback_index_zero) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_idx", 1);
  EXPECT_ROUTER_MATCH(router, "/api/list/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_EQ(std::get<0>(captures.at(0)), 0);
}

TEST(optional_expansion_callback_index_after_variables) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/{namespace}/list{/path*}", "op_oex_idx2", 1);
  EXPECT_ROUTER_MATCH(router, "/api/users/list/foo/bar", 1, 0, captures);
  EXPECT_EQ(captures.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures, 0, "namespace", "users");
  EXPECT_ROUTER_CAPTURE(captures, 1, "path", "foo/bar");
}

TEST(optional_expansion_empty_callback_index_after_variables) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/{namespace}/list{/path*}", "op_oex_idx3", 1);
  EXPECT_ROUTER_MATCH(router, "/api/users/list", 1, 0, captures);
  EXPECT_EQ(captures.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures, 0, "namespace", "users");
  EXPECT_ROUTER_CAPTURE(captures, 1, "path", "");
}

TEST(optional_expansion_with_context) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_ctx", 1, 42);
  EXPECT_ROUTER_MATCH(router, "/api/list/foo", 1, 42, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo");
}

TEST(optional_expansion_empty_with_context) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_ctx2", 1, 42);
  EXPECT_ROUTER_MATCH(router, "/api/list", 1, 42, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "");
}

TEST(optional_expansion_literal_sibling_takes_priority) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_p1", 1);
  router.add("/api/list/special", "op_oex_p2", 2);
  EXPECT_ROUTER_MATCH(router, "/api/list/special", 2, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(optional_expansion_literal_sibling_fallback) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_p3", 1);
  router.add("/api/list/special", "op_oex_p4", 2);
  EXPECT_ROUTER_MATCH(router, "/api/list/other", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "other");
}

TEST(optional_expansion_literal_sibling_fallback_multi) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_p5", 1);
  router.add("/api/list/special", "op_oex_p6", 2);
  EXPECT_ROUTER_MATCH(router, "/api/list/other/nested", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "other/nested");
}

TEST(optional_expansion_literal_parent_route_takes_priority) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list", "op_oex_pp1", 1);
  router.add("/api/list{/path*}", "op_oex_pp2", 2);
  EXPECT_ROUTER_MATCH(router, "/api/list", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(optional_expansion_literal_parent_route_other_order) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_pp3", 2);
  router.add("/api/list", "op_oex_pp4", 1);
  EXPECT_ROUTER_MATCH(router, "/api/list", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(optional_expansion_with_literal_parent_match_nested) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list", "op_oex_pn1", 1);
  router.add("/api/list{/path*}", "op_oex_pn2", 2);
  EXPECT_ROUTER_MATCH(router, "/api/list/foo/bar", 2, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar");
}

TEST(optional_expansion_versus_simple_variable_upgrade) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list/{path}", "op_oex_v1", 1);
  router.add("/api/list{/path*}", "op_oex_v2", 2);
  EXPECT_ROUTER_MATCH(router, "/api/list/foo", 2, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo");
}

TEST(optional_expansion_versus_simple_variable_absorbs) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_v3", 1);
  router.add("/api/list/{path}", "op_oex_v4", 2);
  EXPECT_ROUTER_MATCH(router, "/api/list/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo");
}

TEST(optional_expansion_conflicts_with_reserved_expansion) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list/{+path}", "op_oex_c1", 1);
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/api/list{/path*}", 2, "{/path*}");
}

TEST(optional_expansion_conflicts_with_reserved_expansion_other_order) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_c2", 1);
  EXPECT_ROUTER_SEGMENT_ERROR(router, "/api/list/{+path}", 2, "{+path}");
}

TEST(optional_expansion_variable_name_mismatch) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list/{id}", "op_oex_mm1", 1);
  try {
    router.add("/api/list{/path*}", "op_oex_mm2", 2);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterVariableMismatchError &error) {
    EXPECT_EQ(error.left(), "id");
    EXPECT_EQ(error.right(), "path");
  }
}

TEST(optional_expansion_variable_name_mismatch_with_expansion) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list/{+abc}", "op_oex_mm3", 1);
  try {
    router.add("/api/list{/xyz*}", "op_oex_mm4", 2);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterVariableMismatchError &error) {
    EXPECT_EQ(error.left(), "abc");
    EXPECT_EQ(error.right(), "xyz");
  }
}

TEST(optional_expansion_distinct_parents_independent_registration) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/a{/x*}", "op_oex_d1", 1);
  router.add("/api/b{/y*}", "op_oex_d2", 2);
  EXPECT_ROUTER_MATCH(router, "/api/a/foo", 1, 0, captures_a);
  EXPECT_EQ(captures_a.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_a, 0, "x", "foo");
  EXPECT_ROUTER_MATCH(router, "/api/b/bar", 2, 0, captures_b);
  EXPECT_EQ(captures_b.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_b, 0, "y", "bar");
}

TEST(optional_expansion_distinct_parents_both_empty) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/a{/x*}", "op_oex_d3", 1);
  router.add("/api/b{/y*}", "op_oex_d4", 2);
  EXPECT_ROUTER_MATCH(router, "/api/a", 1, 0, captures_a);
  EXPECT_EQ(captures_a.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_a, 0, "x", "");
  EXPECT_ROUTER_MATCH(router, "/api/b", 2, 0, captures_b);
  EXPECT_EQ(captures_b.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_b, 0, "y", "");
}

TEST(optional_expansion_otherwise_fallback) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_o1", 1);
  router.otherwise(99);
  EXPECT_ROUTER_MATCH(router, "/api/list/", 0, 99, captures);
  EXPECT_ROUTER_MATCH(router, "/api/listing", 0, 99, no_captures);
}

TEST(optional_expansion_under_intermediate_variable) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/{tenant}{/path*}", "op_oex_iv1", 1);
  EXPECT_ROUTER_MATCH(router, "/api/acme", 1, 0, captures);
  EXPECT_EQ(captures.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures, 0, "tenant", "acme");
  EXPECT_ROUTER_CAPTURE(captures, 1, "path", "");
}

TEST(optional_expansion_under_intermediate_variable_with_payload) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/{tenant}{/path*}", "op_oex_iv2", 1);
  EXPECT_ROUTER_MATCH(router, "/api/acme/foo/bar", 1, 0, captures);
  EXPECT_EQ(captures.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures, 0, "tenant", "acme");
  EXPECT_ROUTER_CAPTURE(captures, 1, "path", "foo/bar");
}

TEST(optional_expansion_path_method_round_trips) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_pth", 1);
  EXPECT_EQ(router.path(1), "/api/list{/path*}");
}

TEST(optional_expansion_operation_lookup) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "list_directory", 1, 42);
  const auto result = router.operation("list_directory");
  EXPECT_EQ(result.first, 1);
  EXPECT_EQ(result.second, 42);
}

TEST(optional_expansion_size_counts_one) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_sz", 1);
  EXPECT_EQ(router.size(), 1);
}

TEST(optional_expansion_size_counts_with_sibling_literal) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list", "op_oex_sz1", 1);
  router.add("/api/list{/path*}", "op_oex_sz2", 2);
  EXPECT_EQ(router.size(), 2);
}

TEST(optional_expansion_with_arguments) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 2> arguments{
      {{"max", std::int64_t{10}}, {"flag", true}}};
  router.add("/api/list{/path*}", "op_oex_args", 1, 0, arguments);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  router.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_EQ(collected.size(), 2);
  EXPECT_EQ(collected[0].first, "max");
  EXPECT_EQ(std::get<std::int64_t>(collected[0].second), 10);
  EXPECT_EQ(collected[1].first, "flag");
  EXPECT_EQ(std::get<bool>(collected[1].second), true);
}

TEST(optional_expansion_with_base_path) {
  sourcemeta::core::URITemplateRouter router{"/v1"};
  router.add("/api/list{/path*}", "op_oex_bp", 1);
  EXPECT_ROUTER_MATCH(router, "/v1/api/list", 1, 0, captures_empty);
  EXPECT_EQ(captures_empty.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_empty, 0, "path", "");
  EXPECT_ROUTER_MATCH(router, "/v1/api/list/foo", 1, 0, captures_one);
  EXPECT_EQ(captures_one.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_one, 0, "path", "foo");
  EXPECT_ROUTER_MATCH(router, "/v1/api/list/foo/bar", 1, 0, captures_two);
  EXPECT_EQ(captures_two.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_two, 0, "path", "foo/bar");
}

TEST(optional_expansion_matches_after_variable_segment) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}/files{/path*}", "op_oex_uf", 1);
  EXPECT_ROUTER_MATCH(router, "/users/42/files", 1, 0, captures_empty);
  EXPECT_EQ(captures_empty.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures_empty, 0, "id", "42");
  EXPECT_ROUTER_CAPTURE(captures_empty, 1, "path", "");
  EXPECT_ROUTER_MATCH(router, "/users/42/files/readme.md", 1, 0, captures_one);
  EXPECT_EQ(captures_one.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures_one, 0, "id", "42");
  EXPECT_ROUTER_CAPTURE(captures_one, 1, "path", "readme.md");
}

TEST(optional_expansion_motivating_use_case) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/self/v1/api/list{/path*}", "list_directory", 1, 7);
  EXPECT_ROUTER_MATCH(router, "/self/v1/api/list", 1, 7, captures_root);
  EXPECT_EQ(captures_root.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_root, 0, "path", "");
  EXPECT_ROUTER_MATCH(router, "/self/v1/api/list/foo", 1, 7, captures_one);
  EXPECT_EQ(captures_one.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_one, 0, "path", "foo");
  EXPECT_ROUTER_MATCH(router, "/self/v1/api/list/foo/bar/baz/qux", 1, 7,
                      captures_deep);
  EXPECT_EQ(captures_deep.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_deep, 0, "path", "foo/bar/baz/qux");
  EXPECT_ROUTER_MATCH(router, "/self/v1/api/listing", 0, 0, captures_nope);
  EXPECT_EQ(captures_nope.size(), 0);
  EXPECT_ROUTER_MATCH(router, "/self/v1/api/list/", 0, 0, captures_trailing);
}

TEST(optional_expansion_listing_does_not_count_node) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_oex_lc", 1);
  EXPECT_EQ(router.size(), 1);
  EXPECT_EQ(router.at(0), 1);
  EXPECT_EQ(router.path(1), "/api/list{/path*}");
  EXPECT_EQ(router.context(1), 0);
}

TEST(optional_expansion_overwrite_replaces_identifier) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "first", 1, 11);
  router.add("/api/list{/path*}", "second", 2, 22);

  EXPECT_EQ(router.size(), 1);
  EXPECT_ROUTER_MATCH(router, "/api/list/foo", 2, 22, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo");

  const auto stale = router.operation("first");
  EXPECT_EQ(stale.first, 0);
  const auto live = router.operation("second");
  EXPECT_EQ(live.first, 2);
  EXPECT_EQ(live.second, 22);
}

TEST(variable_does_not_consume_multiple_segments) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}", "var_no_consume_1", 1);
  EXPECT_ROUTER_MATCH(router, "/users/42/extra", 0, 0, captures);
}

TEST(variable_does_not_consume_with_literal_suffix) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}/posts", "var_no_consume_2", 1);
  EXPECT_ROUTER_MATCH(router, "/users/42/extra/posts", 0, 0, captures);
}

TEST(variable_segment_does_not_match_empty_capture) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}", "var_no_empty", 1);
  EXPECT_ROUTER_MATCH(router, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(expansion_consumes_unlike_plain_variable) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/a/{x}", "plain_var", 1);
  router.add("/b/{+y}", "reserved_exp", 2);
  router.add("/c{/z*}", "optional_exp", 3);
  EXPECT_ROUTER_MATCH(router, "/a/one/two", 0, 0, captures_a);
  EXPECT_ROUTER_MATCH(router, "/b/one/two", 2, 0, captures_b);
  EXPECT_EQ(captures_b.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_b, 0, "y", "one/two");
  EXPECT_ROUTER_MATCH(router, "/c/one/two", 3, 0, captures_c);
  EXPECT_EQ(captures_c.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_c, 0, "z", "one/two");
}

TEST(operation_id_round_trip_single_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "list_users", 1);
  EXPECT_EQ(router.operation_id(1), "list_users");
}

TEST(operation_id_round_trip_multiple_routes) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "list_users", 1, 11);
  router.add("/posts/{id}", "show_post", 2, 22);
  router.add("/files/{+rest}", "fetch_files", 3, 33);
  EXPECT_EQ(router.operation_id(1), "list_users");
  EXPECT_EQ(router.operation_id(2), "show_post");
  EXPECT_EQ(router.operation_id(3), "fetch_files");
}

TEST(operation_id_inverse_of_operation) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/a", "alpha", 1, 11);
  router.add("/b", "beta", 2, 22);
  router.add("/c", "gamma", 3, 33);

  EXPECT_EQ(router.operation_id(1), "alpha");
  EXPECT_EQ(router.operation_id(2), "beta");
  EXPECT_EQ(router.operation_id(3), "gamma");

  const auto resolved_alpha = router.operation(router.operation_id(1));
  EXPECT_EQ(resolved_alpha.first, 1);
  EXPECT_EQ(resolved_alpha.second, 11);

  const auto resolved_beta = router.operation(router.operation_id(2));
  EXPECT_EQ(resolved_beta.first, 2);
  EXPECT_EQ(resolved_beta.second, 22);

  const auto resolved_gamma = router.operation(router.operation_id(3));
  EXPECT_EQ(resolved_gamma.first, 3);
  EXPECT_EQ(resolved_gamma.second, 33);
}

TEST(operation_id_zero_returns_empty) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "list_users", 1);
  EXPECT_TRUE(router.operation_id(0).empty());
}

TEST(operation_id_unregistered_returns_empty) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "list_users", 1);
  EXPECT_TRUE(router.operation_id(99).empty());
}

TEST(operation_id_after_overwrite) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "list_users", 1);
  router.add("/users", "list_users_v2", 2);
  EXPECT_TRUE(router.operation_id(1).empty());
  EXPECT_EQ(router.operation_id(2), "list_users_v2");
}

TEST(trailing_slash_distinct_from_bare_match_bare) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo", "op_700", 1);
  EXPECT_ROUTER_MATCH(router, "/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_distinct_from_bare_no_match_slashed) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo", "op_701", 1);
  EXPECT_ROUTER_MATCH(router, "/foo/", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_only_registration_matches_slashed) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo/", "op_702", 1);
  EXPECT_ROUTER_MATCH(router, "/foo/", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_only_registration_no_match_bare) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo/", "op_703", 1);
  EXPECT_ROUTER_MATCH(router, "/foo", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_both_forms_registered_match_bare) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo", "op_704", 1);
  router.add("/foo/", "op_705", 2);
  EXPECT_ROUTER_MATCH(router, "/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_both_forms_registered_match_slashed) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo", "op_706", 1);
  router.add("/foo/", "op_707", 2);
  EXPECT_ROUTER_MATCH(router, "/foo/", 2, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_both_forms_registered_size_is_two) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo", "op_708", 1);
  router.add("/foo/", "op_709", 2);
  EXPECT_EQ(router.size(), 2);
}

TEST(trailing_slash_both_forms_registered_lookup_by_operation_id) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo", "op_710", 1);
  router.add("/foo/", "op_711", 2);
  const auto bare = router.operation("op_710");
  const auto slashed = router.operation("op_711");
  EXPECT_EQ(bare.first, 1);
  EXPECT_EQ(slashed.first, 2);
}

TEST(trailing_slash_both_forms_registered_path_reconstruction) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo", "op_712", 1);
  router.add("/foo/", "op_713", 2);
  EXPECT_EQ(router.path(1), "/foo");
  EXPECT_EQ(router.path(2), "/foo/");
}

TEST(trailing_slash_multi_segment_match_slashed) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo/bar/", "op_714", 1);
  EXPECT_ROUTER_MATCH(router, "/foo/bar/", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_multi_segment_no_match_bare) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo/bar/", "op_715", 1);
  EXPECT_ROUTER_MATCH(router, "/foo/bar", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_after_variable_match_slashed) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}/", "op_716", 1);
  EXPECT_ROUTER_MATCH(router, "/users/42/", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

TEST(trailing_slash_after_variable_no_match_bare) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}/", "op_717", 1);
  EXPECT_ROUTER_MATCH(router, "/users/42", 0, 0, captures);
}

TEST(trailing_slash_both_forms_with_variable) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}", "op_718", 1);
  router.add("/users/{id}/", "op_719", 2);
  EXPECT_ROUTER_MATCH(router, "/users/42", 1, 0, captures_bare);
  EXPECT_EQ(captures_bare.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_bare, 0, "id", "42");
  EXPECT_ROUTER_MATCH(router, "/users/42/", 2, 0, captures_slashed);
  EXPECT_EQ(captures_slashed.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_slashed, 0, "id", "42");
}

TEST(trailing_slash_does_not_relax_internal_double_slash) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo/bar", "op_720", 1);
  EXPECT_ROUTER_MATCH(router, "/foo//bar", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_with_base_path_match_slashed) {
  sourcemeta::core::URITemplateRouter router{"/api"};
  router.add("/foo/", "op_721", 1);
  EXPECT_ROUTER_MATCH(router, "/api/foo/", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_with_base_path_no_match_bare) {
  sourcemeta::core::URITemplateRouter router{"/api"};
  router.add("/foo/", "op_722", 1);
  EXPECT_ROUTER_MATCH(router, "/api/foo", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_with_base_path_both_forms) {
  sourcemeta::core::URITemplateRouter router{"/api"};
  router.add("/foo", "op_723", 1);
  router.add("/foo/", "op_724", 2);
  EXPECT_ROUTER_MATCH(router, "/api/foo", 1, 0, captures_bare);
  EXPECT_EQ(captures_bare.size(), 0);
  EXPECT_ROUTER_MATCH(router, "/api/foo/", 2, 0, captures_slashed);
  EXPECT_EQ(captures_slashed.size(), 0);
}

TEST(trailing_slash_multiple_trailing_slashes_in_request_no_match) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo/", "op_726", 1);
  EXPECT_ROUTER_MATCH(router, "/foo//", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(trailing_slash_both_forms_each_has_distinct_arguments) {
  sourcemeta::core::URITemplateRouter router;
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> bare_args{
      {sourcemeta::core::URITemplateRouter::Argument{
          "kind", sourcemeta::core::URITemplateRouter::ArgumentValue{
                      std::string_view{"bare"}}}}};
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
      slashed_args{{sourcemeta::core::URITemplateRouter::Argument{
          "kind", sourcemeta::core::URITemplateRouter::ArgumentValue{
                      std::string_view{"slashed"}}}}};
  router.add("/foo", "op_727", 1, 0, bare_args);
  router.add("/foo/", "op_728", 2, 0, slashed_args);
  std::string bare_kind;
  router.arguments(
      1, [&bare_kind](const std::string_view name, const auto &value) {
        if (name == "kind" && std::holds_alternative<std::string_view>(value)) {
          bare_kind = std::get<std::string_view>(value);
        }
      });
  std::string slashed_kind;
  router.arguments(
      2, [&slashed_kind](const std::string_view name, const auto &value) {
        if (name == "kind" && std::holds_alternative<std::string_view>(value)) {
          slashed_kind = std::get<std::string_view>(value);
        }
      });
  EXPECT_EQ(bare_kind, "bare");
  EXPECT_EQ(slashed_kind, "slashed");
}

TEST(strict_internal_double_slash_registers_and_matches_only_itself) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo//bar", "op_800", 1);
  EXPECT_ROUTER_MATCH(router, "/foo//bar", 1, 0, captures_verbatim);
  EXPECT_EQ(captures_verbatim.size(), 0);
  EXPECT_ROUTER_MATCH(router, "/foo/bar", 0, 0, captures_canonical);
}

TEST(strict_only_slashes_template_registers_four_empty_segments) {
  sourcemeta::core::URITemplateRouter router;
  router.add("////", "op_801", 1);
  EXPECT_EQ(router.size(), 1);
  EXPECT_ROUTER_MATCH(router, "////", 1, 0, captures_match);
  EXPECT_EQ(captures_match.size(), 0);
  EXPECT_ROUTER_MATCH(router, "///", 0, 0, captures_short);
  EXPECT_ROUTER_MATCH(router, "/////", 0, 0, captures_long);
}

TEST(strict_three_slashes_template_registers_three_empty_segments) {
  sourcemeta::core::URITemplateRouter router;
  router.add("///", "op_802", 1);
  EXPECT_ROUTER_MATCH(router, "///", 1, 0, captures_match);
  EXPECT_EQ(captures_match.size(), 0);
  EXPECT_ROUTER_MATCH(router, "//", 0, 0, captures_short);
  EXPECT_ROUTER_MATCH(router, "////", 0, 0, captures_long);
}

TEST(strict_double_slash_template_registers_two_empty_segments) {
  sourcemeta::core::URITemplateRouter router;
  router.add("//", "op_803", 1);
  EXPECT_ROUTER_MATCH(router, "//", 1, 0, captures_match);
  EXPECT_EQ(captures_match.size(), 0);
  EXPECT_ROUTER_MATCH(router, "/", 0, 0, captures_short);
}

TEST(strict_internal_empty_with_variable_after) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo//{id}", "op_804", 1);
  EXPECT_ROUTER_MATCH(router, "/foo//42", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

TEST(strict_variable_does_not_bind_empty_segment) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/{id}", "op_805", 1);
  EXPECT_ROUTER_MATCH(router, "/users/", 0, 0, captures);
}

TEST(strict_missing_leading_slash_throws) {
  sourcemeta::core::URITemplateRouter router;
  try {
    router.add("foo", "op_806", 1);
    FAIL();
  } catch (
      const sourcemeta::core::URITemplateRouterInvalidSegmentError &error) {
    EXPECT_EQ(error.segment(), "foo");
  }
}

TEST(strict_empty_template_still_special) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", "op_807", 1);
  EXPECT_ROUTER_MATCH(router, "", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(strict_internal_empty_then_literal) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/a", "op_810", 1);
  router.add("/a//b", "op_811", 2);
  EXPECT_ROUTER_MATCH(router, "/a", 1, 0, captures_a);
  EXPECT_EQ(captures_a.size(), 0);
  EXPECT_ROUTER_MATCH(router, "/a//b", 2, 0, captures_aab);
  EXPECT_EQ(captures_aab.size(), 0);
  EXPECT_ROUTER_MATCH(router, "/a/b", 0, 0, captures_ab);
  EXPECT_ROUTER_MATCH(router, "/a/", 0, 0, captures_a_slash);
}

TEST(strict_root_template_still_works) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/", "op_812", 1);
  EXPECT_ROUTER_MATCH(router, "/", 1, 0, captures_match);
  EXPECT_EQ(captures_match.size(), 0);
  EXPECT_ROUTER_MATCH(router, "", 0, 0, captures_empty);
  EXPECT_ROUTER_MATCH(router, "//", 0, 0, captures_double);
}

TEST(strict_path_reconstruction_preserves_input) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo//bar", "op_813", 1);
  router.add("////", "op_814", 2);
  EXPECT_EQ(router.path(1), "/foo//bar");
  EXPECT_EQ(router.path(2), "////");
}

TEST(describes_worked_example_table) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/self/v1/api/schemas/health/{+schema}", "op_815", 1);
  router.add("/self/v1/api/{+any}", "op_816", 2);
  router.add("/self/v1/mcp", "op_817", 3);
  router.add("/self/v1/health", "op_818", 4);
  router.add("/self/v1/static/{+path}", "op_819", 5);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/self/v1/api"));
  EXPECT_TRUE(router.describes("/self/v1/api/schemas/health"));
  EXPECT_TRUE(router.describes("/self/v1/api/schemas/health/acme"));
  EXPECT_TRUE(router.describes("/self/v1/mcp"));
  EXPECT_TRUE(router.describes("/self/v1/static/css/app.css"));
  EXPECT_TRUE(router.describes("/self/v1"));
  EXPECT_FALSE(router.describes("/self/v1/mpc"));
  EXPECT_FALSE(router.describes("/self/v1/healthz"));
  EXPECT_FALSE(router.describes("/acme/foo"));
  EXPECT_TRUE(router.describes("/"));
}

TEST(describes_intermediate_prefixes) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/self/v1/mcp", "op_820", 1);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/self"));
  EXPECT_TRUE(router.describes("/self/v1"));
  EXPECT_TRUE(router.describes("/self/v1/mcp"));
  EXPECT_FALSE(router.describes("/self/v1/mcp/extra"));
  EXPECT_FALSE(router.describes("/self/v2"));
  EXPECT_FALSE(router.describes("/other"));
}

TEST(describes_expansion_capture) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", "op_821", 1);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/files"));
  EXPECT_TRUE(router.describes("/files/a"));
  EXPECT_TRUE(router.describes("/files/a/b/c"));
  EXPECT_FALSE(router.describes("/file"));
  EXPECT_FALSE(router.describes("/filesx"));
}

TEST(describes_optional_expansion_capture) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/docs/{/rest*}", "op_822", 1);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/docs"));
  EXPECT_TRUE(router.describes("/docs/a"));
  EXPECT_TRUE(router.describes("/docs/a/b/c"));
  EXPECT_FALSE(router.describes("/doc"));
  EXPECT_FALSE(router.describes("/docsx"));
}

TEST(describes_single_segment_variable_does_not_over_absorb) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/foo/{bar}/baz", "op_823", 1);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/foo"));
  EXPECT_TRUE(router.describes("/foo/anything"));
  EXPECT_TRUE(router.describes("/foo/anything/baz"));
  EXPECT_FALSE(router.describes("/foo/anything/qux"));
  EXPECT_FALSE(router.describes("/foo/anything/baz/extra"));
}

TEST(describes_whole_segment_discipline) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/internalish", "op_824", 1);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/internalish"));
  EXPECT_FALSE(router.describes("/internal"));
  EXPECT_FALSE(router.describes("/internalisher"));
}

TEST(describes_excludes_otherwise_fallback) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/known", "op_825", 1);
  router.otherwise(7);

  EXPECT_TRUE(router.describes("/known"));
  EXPECT_FALSE(router.describes("/unknown"));
  EXPECT_FALSE(router.describes("/known/deeper"));
}

TEST(describes_empty_router_describes_nothing) {
  sourcemeta::core::URITemplateRouter router;
  router.otherwise(3);

  EXPECT_FALSE(router.describes("/"));
  EXPECT_FALSE(router.describes(""));
  EXPECT_FALSE(router.describes("/anything"));
}

TEST(describes_root_and_empty_path) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_826", 1);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/"));
  EXPECT_TRUE(router.describes(""));
}

TEST(describes_requires_leading_slash) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "op_827", 1);
  router.otherwise(0);

  EXPECT_FALSE(router.describes("users"));
  EXPECT_FALSE(router.describes("users/list"));
}

TEST(describes_with_base_path) {
  sourcemeta::core::URITemplateRouter router{"/api"};
  router.add("/foo", "op_828", 1);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/api/foo"));
  EXPECT_TRUE(router.describes("/api"));
  EXPECT_FALSE(router.describes("/foo"));
  EXPECT_FALSE(router.describes("/api/bar"));
}

TEST(describes_literal_preferred_over_variable) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users/me", "op_829", 1);
  router.add("/users/{id}/posts", "op_830", 2);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/users/me"));
  EXPECT_TRUE(router.describes("/users/42"));
  EXPECT_TRUE(router.describes("/users/42/posts"));
  EXPECT_FALSE(router.describes("/users/42/comments"));
  EXPECT_FALSE(router.describes("/users/me/posts"));
}

TEST(describes_empty_template_root_route) {
  sourcemeta::core::URITemplateRouter router;
  router.add("", "op_831", 1);
  router.otherwise(0);

  EXPECT_TRUE(router.describes(""));
  EXPECT_TRUE(router.describes("/"));
  EXPECT_FALSE(router.describes("/foo"));
}

TEST(describes_with_base_argument_equivalent_to_concat) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/self/v1/api/{+any}", "op_832", 1);
  router.add("/self/v1/mcp", "op_833", 2);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/api", "/self/v1"));
  EXPECT_TRUE(router.describes("/self/v1/api"));
  EXPECT_TRUE(router.describes("/api/foo", "/self/v1"));
  EXPECT_TRUE(router.describes("/mcp", "/self/v1"));
  EXPECT_FALSE(router.describes("/mpc", "/self/v1"));
  EXPECT_FALSE(router.describes("/api", "/self/v2"));
}

TEST(describes_with_base_argument_prefix_and_capture) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", "op_834", 1);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/files", ""));
  EXPECT_TRUE(router.describes("/files/a/b", ""));
  EXPECT_TRUE(router.describes("", "/files"));
  EXPECT_TRUE(router.describes("/", "/files"));
  EXPECT_TRUE(router.describes("/a/b", "/files"));
  EXPECT_FALSE(router.describes("/a/b", "/file"));
}

TEST(describes_with_base_argument_captured_in_base) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/files/{+path}", "op_835", 1);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/anything", "/files/already"));
}

TEST(describes_with_base_argument_base_mismatch) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/self/v1/mcp", "op_836", 1);
  router.otherwise(0);

  EXPECT_FALSE(router.describes("/mcp", "/other"));
  EXPECT_FALSE(router.describes("/mcp", "/self/v2"));
}

TEST(describes_with_router_base_path_split_argument) {
  sourcemeta::core::URITemplateRouter router{"/prefix"};
  router.add("/foo", "op_837", 1);
  router.otherwise(0);

  EXPECT_TRUE(router.describes("/foo", "/prefix"));
  EXPECT_TRUE(router.describes("/prefix/foo"));
  EXPECT_TRUE(router.describes("", "/prefix"));
  EXPECT_FALSE(router.describes("/bar", "/prefix"));
}

TEST(describes_with_base_argument_empty_router) {
  sourcemeta::core::URITemplateRouter router;
  router.otherwise(0);

  EXPECT_FALSE(router.describes("/foo", "/bar"));
  EXPECT_FALSE(router.describes("", "/bar"));
}

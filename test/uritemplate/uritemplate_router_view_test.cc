#include <gtest/gtest.h>

#include <sourcemeta/core/uritemplate.h>

#include "uritemplate_helpers.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class URITemplateRouterViewTest : public ::testing::Test {
protected:
  void TearDown() override { std::filesystem::remove(this->path); }
  // The tests are always sequential, so using the same path is safe
  std::filesystem::path path{std::filesystem::temp_directory_path() /
                             "sourcemeta_core_uritemplate_router_test.bin"};
};

TEST_F(URITemplateRouterViewTest, single_literal_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, single_literal_route_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/posts", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, multi_segment_literal) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/list", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/list", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, single_variable) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/123", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST_F(URITemplateRouterViewTest, multiple_variables) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}/posts/{post_id}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/42/posts/99", 1, captures);
  EXPECT_EQ(captures.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
  EXPECT_ROUTER_CAPTURE(captures, 1, "post_id", "99");
}

TEST_F(URITemplateRouterViewTest, literal_before_variable_precedence) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/me", 1);
    router.add("/users/{id}", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/me", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, variable_fallback) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/me", 1);
    router.add("/users/{id}", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/123", 2, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST_F(URITemplateRouterViewTest, multiple_routes_match_users) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    router.add("/users/{id}", 2);
    router.add("/posts", 3);
    router.add("/posts/{id}", 4);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, multiple_routes_match_users_id) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    router.add("/users/{id}", 2);
    router.add("/posts", 3);
    router.add("/posts/{id}", 4);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/42", 2, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

TEST_F(URITemplateRouterViewTest, multiple_routes_match_posts) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    router.add("/users/{id}", 2);
    router.add("/posts", 3);
    router.add("/posts/{id}", 4);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/posts", 3, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, multiple_routes_match_posts_id) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    router.add("/users/{id}", 2);
    router.add("/posts", 3);
    router.add("/posts/{id}", 4);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/posts/99", 4, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "99");
}

TEST_F(URITemplateRouterViewTest, binary_search_literals_gamma) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/alpha", 1);
    router.add("/beta", 2);
    router.add("/gamma", 3);
    router.add("/delta", 4);
    router.add("/epsilon", 5);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/gamma", 3, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, binary_search_literals_alpha) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/alpha", 1);
    router.add("/beta", 2);
    router.add("/gamma", 3);
    router.add("/delta", 4);
    router.add("/epsilon", 5);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/alpha", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, binary_search_literals_epsilon) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/alpha", 1);
    router.add("/beta", 2);
    router.add("/gamma", 3);
    router.add("/delta", 4);
    router.add("/epsilon", 5);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/epsilon", 5, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, root_template_matches_root) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, root_template_no_match_empty) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, root_template_no_match_path) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/foo", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, empty_template_matches_empty) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, empty_template_no_match_root) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, empty_template_no_match_path) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/foo", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, root_and_other_routes_match_root) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/", 1);
    router.add("/users", 2);
    router.add("/users/{id}", 3);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, root_and_other_routes_match_users) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/", 1);
    router.add("/users", 2);
    router.add("/users/{id}", 3);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users", 2, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, root_and_other_routes_match_users_id) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/", 1);
    router.add("/users", 2);
    router.add("/users/{id}", 3);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/123", 3, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST_F(URITemplateRouterViewTest, empty_and_root_together_match_empty) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    router.add("/", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, empty_and_root_together_match_root) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    router.add("/", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/", 2, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, empty_and_root_together_no_match_path) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    router.add("/", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/foo", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, empty_and_root_and_others_match_empty) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    router.add("/", 2);
    router.add("/users", 3);
    router.add("/users/{id}", 4);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "", 1, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, empty_and_root_and_others_match_root) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    router.add("/", 2);
    router.add("/users", 3);
    router.add("/users/{id}", 4);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/", 2, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, empty_and_root_and_others_match_users) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    router.add("/", 2);
    router.add("/users", 3);
    router.add("/users/{id}", 4);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users", 3, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, empty_and_root_and_others_match_users_id) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    router.add("/", 2);
    router.add("/users", 3);
    router.add("/users/{id}", 4);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/42", 4, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

TEST_F(URITemplateRouterViewTest, same_variable_names_allowed) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}/posts", 1);
    router.add("/users/{id}/comments", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/123/comments", 2, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST_F(URITemplateRouterViewTest, reserved_expansion_catch_all) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+path}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/files/foo/bar/baz.txt", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar/baz.txt");
}

TEST_F(URITemplateRouterViewTest, reserved_expansion_with_literal_prefix) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/v1/proxy/{+url}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/v1/proxy/https://example.com/path", 1,
                      captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "url", "https://example.com/path");
}

TEST_F(URITemplateRouterViewTest, reserved_expansion_matches_multi_segment) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+path}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/files/foo/bar", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar");
}

TEST_F(URITemplateRouterViewTest, expansion_takes_priority_over_variable) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{path}", 1);
    router.add("/files/{+path}", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/files/readme.md", 2, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "readme.md");
}

TEST_F(URITemplateRouterViewTest, expansion_takes_priority_multi_segment) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{path}", 1);
    router.add("/files/{+path}", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/files/foo/bar/baz", 2, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar/baz");
}

TEST_F(URITemplateRouterViewTest, expansion_first_then_variable) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+path}", 1);
    router.add("/files/{path}", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/files/readme.md", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "readme.md");
}

TEST_F(URITemplateRouterViewTest, literal_takes_priority_over_expansion) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+path}", 1);
    router.add("/files/special", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/files/special", 2, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, expansion_fallback_from_literal) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+path}", 1);
    router.add("/files/special", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/files/other", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "other");
}

TEST_F(URITemplateRouterViewTest, trailing_slash_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, multiple_trailing_slashes_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users///", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, leading_double_slash_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "//users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, internal_double_slashes_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/posts", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users//posts", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, trailing_slash_with_variable_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/123/", 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST_F(URITemplateRouterViewTest,
       internal_double_slash_with_variable_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}/posts", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users//123//posts", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, expansion_matches_trailing_slash) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+path}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/files/foo/bar/", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar/");
}

TEST_F(URITemplateRouterViewTest, expansion_matches_double_slashes) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+path}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/files/foo//bar", 1, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo//bar");
}

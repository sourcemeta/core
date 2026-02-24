#include <gtest/gtest.h>

#include <sourcemeta/core/uritemplate.h>

#include "uritemplate_helpers.h"

#include <cstdint>
#include <cstring>
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

TEST(URITemplateRouterView, corrupt_empty_data) {
  const sourcemeta::core::URITemplateRouterView view{nullptr, 0};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_too_small_for_header) {
  const std::uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03};
  const sourcemeta::core::URITemplateRouterView view{data, sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_wrong_magic) {
  const std::uint32_t data[] = {0xDEADBEEF, 1, 1, 40, 0, 0, 0, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_wrong_version) {
  const std::uint32_t data[] = {0x52544552, 99, 1, 40, 0, 0, 0, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_node_count_exceeds_file) {
  const std::uint32_t data[] = {0x52544552, 1, 10, 16};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_literal_child_out_of_bounds) {
  const std::uint32_t data[] = {0x52544552, 1,   1, 40,         0,
                                0,          999, 1, 0xFFFFFFFF, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_variable_child_out_of_bounds) {
  const std::uint32_t data[] = {0x52544552, 1,          1, 40,  0,
                                0,          0xFFFFFFFF, 0, 500, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_string_offset_out_of_bounds) {
  const std::uint32_t data[] = {0x52544552, 1, 2,          64, 0,    0,
                                1,          1, 0xFFFFFFFF, 0,  9999, 5,
                                0xFFFFFFFF, 0, 0xFFFFFFFF, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_variable_string_offset_out_of_bounds) {
  const std::uint32_t data[] = {0x52544552, 1, 2,          64, 0,    0,
                                0xFFFFFFFF, 0, 1,          0,  9999, 100,
                                0xFFFFFFFF, 0, 0xFFFFFFFF, 2};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_all_zeroes) {
  const std::uint8_t data[128] = {};
  const sourcemeta::core::URITemplateRouterView view{data, sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_all_ones) {
  std::uint8_t data[128];
  std::memset(data, 0xFF, sizeof(data));
  const sourcemeta::core::URITemplateRouterView view{data, sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_string_table_offset_overlaps_header) {
  const std::uint32_t data[] = {0x52544552, 1, 1, 4, 0, 0, 0, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_string_table_offset_past_end) {
  const std::uint32_t data[] = {0x52544552, 1, 1, 99999, 0, 0, 0, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_zero_node_count) {
  const std::uint32_t data[] = {0x52544552, 1, 0, 16};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_empty_data_match_empty_path) {
  const sourcemeta::core::URITemplateRouterView view{nullptr, 0};
  EXPECT_ROUTER_MATCH(view, "", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_empty_data_match_root) {
  const sourcemeta::core::URITemplateRouterView view{nullptr, 0};
  EXPECT_ROUTER_MATCH(view, "/", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_literal_child_count_overflow) {
  const std::uint32_t data[] = {0x52544552, 1, 2, 64, 0, 0, 1, 0xFFFFFFFF,
                                0xFFFFFFFF, 0, 0, 0,  0, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_root_literal_child_oob_match_root) {
  const std::uint32_t data[] = {0x52544552, 1,   1, 40,         0,
                                0,          999, 1, 0xFFFFFFFF, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_deep_node_variable_child_oob) {
  std::vector<std::uint8_t> data;
  const std::uint32_t header[] = {0x52544552, 1, 2, 64};
  const std::uint32_t root[] = {0, 0, 1, 1, 0xFFFFFFFF, 0};
  const std::uint32_t child[] = {0, 5, 0xFFFFFFFF, 0, 999, 0};
  data.insert(data.end(), reinterpret_cast<const std::uint8_t *>(header),
              reinterpret_cast<const std::uint8_t *>(header) + sizeof(header));
  data.insert(data.end(), reinterpret_cast<const std::uint8_t *>(root),
              reinterpret_cast<const std::uint8_t *>(root) + sizeof(root));
  data.insert(data.end(), reinterpret_cast<const std::uint8_t *>(child),
              reinterpret_cast<const std::uint8_t *>(child) + sizeof(child));
  data.insert(data.end(), {'u', 's', 'e', 'r', 's'});

  const sourcemeta::core::URITemplateRouterView view{data.data(), data.size()};
  EXPECT_ROUTER_MATCH(view, "/users/123", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_expansion_string_oob) {
  const std::uint32_t data[] = {0x52544552, 1, 2,          64, 0,    0,
                                0xFFFFFFFF, 0, 1,          0,  5000, 200,
                                0xFFFFFFFF, 0, 0xFFFFFFFF, 3};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/files/foo/bar", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_empty_string_table_with_string_ref) {
  const std::uint32_t data[] = {0x52544552, 1, 2,          64, 0, 0,
                                1,          1, 0xFFFFFFFF, 0,  0, 10,
                                0xFFFFFFFF, 0, 0xFFFFFFFF, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_node_count_max_uint32) {
  const std::uint32_t data[] = {0x52544552, 1, 0xFFFFFFFF, 40, 0,
                                0,          0, 0,          0,  0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_string_offset_plus_length_overflow) {
  const std::uint32_t data[] = {
      0x52544552, 1, 2,          64,         0,          0, 0xFFFFFFFF, 0,
      1,          0, 0x80000000, 0x80000001, 0xFFFFFFFF, 0, 0xFFFFFFFF, 2};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView,
     corrupt_string_offset_plus_length_overflow_with_data) {
  std::vector<std::uint8_t> data;
  const std::uint32_t header[] = {0x52544552, 1, 2, 64};
  const std::uint32_t root[] = {0, 0, 0xFFFFFFFF, 0, 1, 0};
  const std::uint32_t variable[] = {0xFFFFFFFF, 2,          0xFFFFFFFF,
                                    0,          0xFFFFFFFF, 2};
  data.insert(data.end(), reinterpret_cast<const std::uint8_t *>(header),
              reinterpret_cast<const std::uint8_t *>(header) + sizeof(header));
  data.insert(data.end(), reinterpret_cast<const std::uint8_t *>(root),
              reinterpret_cast<const std::uint8_t *>(root) + sizeof(root));
  data.insert(data.end(), reinterpret_cast<const std::uint8_t *>(variable),
              reinterpret_cast<const std::uint8_t *>(variable) +
                  sizeof(variable));
  data.push_back('x');

  const sourcemeta::core::URITemplateRouterView view{data.data(), data.size()};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView,
     corrupt_literal_string_offset_plus_length_overflow) {
  std::vector<std::uint8_t> data;
  const std::uint32_t header[] = {0x52544552, 1, 2, 64};
  const std::uint32_t root[] = {0, 0, 1, 1, 0xFFFFFFFF, 0};
  const std::uint32_t child[] = {0xFFFFFFFF, 2, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0};
  data.insert(data.end(), reinterpret_cast<const std::uint8_t *>(header),
              reinterpret_cast<const std::uint8_t *>(header) + sizeof(header));
  data.insert(data.end(), reinterpret_cast<const std::uint8_t *>(root),
              reinterpret_cast<const std::uint8_t *>(root) + sizeof(root));
  data.insert(data.end(), reinterpret_cast<const std::uint8_t *>(child),
              reinterpret_cast<const std::uint8_t *>(child) + sizeof(child));
  data.push_back('x');

  const sourcemeta::core::URITemplateRouterView view{data.data(), data.size()};
  EXPECT_ROUTER_MATCH(view, "/users", 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

#include <gtest/gtest.h>

#include <sourcemeta/core/uritemplate.h>

#include "uritemplate_helpers.h"

#include <array>      // std::array
#include <climits>    // INT64_MIN, INT64_MAX
#include <cstdint>    // std::uint8_t, std::int64_t
#include <cstring>    // std::memset
#include <filesystem> // std::filesystem::path, std::filesystem::remove, std::filesystem::temp_directory_path
#include <fstream>    // std::ifstream
#include <span>       // std::span
#include <string>     // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <variant>     // std::holds_alternative, std::get
#include <vector>      // std::vector

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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, single_literal_route_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/posts", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, multi_segment_literal) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/list", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users/list", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, single_variable) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users/123", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users/42/posts/99", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users/me", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users/123", 2, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users/42", 2, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/posts", 3, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/posts/99", 4, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/gamma", 3, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/alpha", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/epsilon", 5, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, root_template_matches_root) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, root_template_no_match_empty) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, root_template_no_match_path) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/foo", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, empty_template_matches_empty) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, empty_template_no_match_root) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, empty_template_no_match_path) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/foo", 0, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users", 2, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users/123", 3, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/", 2, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/foo", 0, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/", 2, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users", 3, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users/42", 4, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users/123/comments", 2, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/files/foo/bar/baz.txt", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/api/v1/proxy/https://example.com/path", 1, 0,
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/files/foo/bar", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/files/readme.md", 2, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/files/foo/bar/baz", 2, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/files/readme.md", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/files/special", 2, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/files/other", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users/", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, multiple_trailing_slashes_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users///", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, leading_double_slash_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "//users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, internal_double_slashes_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/posts", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users//posts", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, trailing_slash_with_variable_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users/123/", 0, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users//123//posts", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, expansion_matches_trailing_slash) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+path}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/files/foo/bar/", 1, 0, captures);
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
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/files/foo//bar", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo//bar");
}

TEST(URITemplateRouterView, corrupt_empty_data) {
  const sourcemeta::core::URITemplateRouterView view{nullptr, 0};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_too_small_for_header) {
  const std::uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03};
  const sourcemeta::core::URITemplateRouterView view{data, sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_wrong_magic) {
  const std::uint32_t data[] = {0xDEADBEEF, 1, 1, 40, 0, 0, 0, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_wrong_version) {
  const std::uint32_t data[] = {0x52544552, 99, 1, 40, 0, 0, 0, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_node_count_exceeds_file) {
  const std::uint32_t data[] = {0x52544552, 1, 10, 16};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_literal_child_out_of_bounds) {
  const std::uint32_t data[] = {0x52544552, 1,   1, 40,         0,
                                0,          999, 1, 0xFFFFFFFF, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_variable_child_out_of_bounds) {
  const std::uint32_t data[] = {0x52544552, 1,          1, 40,  0,
                                0,          0xFFFFFFFF, 0, 500, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_string_offset_out_of_bounds) {
  const std::uint32_t data[] = {0x52544552, 1, 2,          64, 0,    0,
                                1,          1, 0xFFFFFFFF, 0,  9999, 5,
                                0xFFFFFFFF, 0, 0xFFFFFFFF, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_variable_string_offset_out_of_bounds) {
  const std::uint32_t data[] = {0x52544552, 1, 2,          64, 0,    0,
                                0xFFFFFFFF, 0, 1,          0,  9999, 100,
                                0xFFFFFFFF, 0, 0xFFFFFFFF, 2};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_all_zeroes) {
  const std::uint8_t data[128] = {};
  const sourcemeta::core::URITemplateRouterView view{data, sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_all_ones) {
  std::uint8_t data[128];
  std::memset(data, 0xFF, sizeof(data));
  const sourcemeta::core::URITemplateRouterView view{data, sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_string_table_offset_overlaps_header) {
  const std::uint32_t data[] = {0x52544552, 1, 1, 4, 0, 0, 0, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_string_table_offset_past_end) {
  const std::uint32_t data[] = {0x52544552, 1, 1, 99999, 0, 0, 0, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_zero_node_count) {
  const std::uint32_t data[] = {0x52544552, 1, 0, 16};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_empty_data_match_empty_path) {
  const sourcemeta::core::URITemplateRouterView view{nullptr, 0};
  EXPECT_ROUTER_MATCH(view, "", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_empty_data_match_root) {
  const sourcemeta::core::URITemplateRouterView view{nullptr, 0};
  EXPECT_ROUTER_MATCH(view, "/", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_literal_child_count_overflow) {
  const std::uint32_t data[] = {0x52544552, 1, 2, 64, 0, 0, 1, 0xFFFFFFFF,
                                0xFFFFFFFF, 0, 0, 0,  0, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_root_literal_child_oob_match_root) {
  const std::uint32_t data[] = {0x52544552, 1,   1, 40,         0,
                                0,          999, 1, 0xFFFFFFFF, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/", 0, 0, captures);
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
  EXPECT_ROUTER_MATCH(view, "/users/123", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_expansion_string_oob) {
  const std::uint32_t data[] = {0x52544552, 1, 2,          64, 0,    0,
                                0xFFFFFFFF, 0, 1,          0,  5000, 200,
                                0xFFFFFFFF, 0, 0xFFFFFFFF, 3};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/files/foo/bar", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_empty_string_table_with_string_ref) {
  const std::uint32_t data[] = {0x52544552, 1, 2,          64, 0, 0,
                                1,          1, 0xFFFFFFFF, 0,  0, 10,
                                0xFFFFFFFF, 0, 0xFFFFFFFF, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_node_count_max_uint32) {
  const std::uint32_t data[] = {0x52544552, 1, 0xFFFFFFFF, 40, 0,
                                0,          0, 0,          0,  0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_string_offset_plus_length_overflow) {
  const std::uint32_t data[] = {
      0x52544552, 1, 2,          64,         0,          0, 0xFFFFFFFF, 0,
      1,          0, 0x80000000, 0x80000001, 0xFFFFFFFF, 0, 0xFFFFFFFF, 2};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
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
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
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
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, arguments_single_string) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string argument_value{"some/response/schema"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"responseSchema", std::string_view{argument_value}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "responseSchema");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::string_view>(collected_arguments[0].second),
            "some/response/schema");
}

TEST_F(URITemplateRouterViewTest, arguments_single_integer) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"count", std::int64_t{42}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "count");
  EXPECT_TRUE(
      std::holds_alternative<std::int64_t>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::int64_t>(collected_arguments[0].second), 42);
}

TEST_F(URITemplateRouterViewTest, arguments_single_boolean_true) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"enabled", true}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "enabled");
  EXPECT_TRUE(std::holds_alternative<bool>(collected_arguments[0].second));
  EXPECT_EQ(std::get<bool>(collected_arguments[0].second), true);
}

TEST_F(URITemplateRouterViewTest, arguments_single_boolean_false) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"disabled", false}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "disabled");
  EXPECT_TRUE(std::holds_alternative<bool>(collected_arguments[0].second));
  EXPECT_EQ(std::get<bool>(collected_arguments[0].second), false);
}

TEST_F(URITemplateRouterViewTest, arguments_integer_zero) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"value", std::int64_t{0}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "value");
  EXPECT_TRUE(
      std::holds_alternative<std::int64_t>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::int64_t>(collected_arguments[0].second), 0);
}

TEST_F(URITemplateRouterViewTest, arguments_integer_negative) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"value", std::int64_t{-1}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "value");
  EXPECT_TRUE(
      std::holds_alternative<std::int64_t>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::int64_t>(collected_arguments[0].second), -1);
}

TEST_F(URITemplateRouterViewTest, arguments_integer_min) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"value", INT64_MIN}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "value");
  EXPECT_TRUE(
      std::holds_alternative<std::int64_t>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::int64_t>(collected_arguments[0].second), INT64_MIN);
}

TEST_F(URITemplateRouterViewTest, arguments_integer_max) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"value", INT64_MAX}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "value");
  EXPECT_TRUE(
      std::holds_alternative<std::int64_t>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::int64_t>(collected_arguments[0].second), INT64_MAX);
}

TEST_F(URITemplateRouterViewTest, arguments_empty_string_value) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string argument_value{""};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"key", std::string_view{argument_value}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "key");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::string_view>(collected_arguments[0].second), "");
}

TEST_F(URITemplateRouterViewTest, arguments_string_with_slashes) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string argument_value{"/some/path/here"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"path", std::string_view{argument_value}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "path");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::string_view>(collected_arguments[0].second),
            "/some/path/here");
}

TEST_F(URITemplateRouterViewTest, arguments_string_with_utf8) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string argument_value{"\xC3\xA9"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"letter", std::string_view{argument_value}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "letter");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::string_view>(collected_arguments[0].second),
            "\xC3\xA9");
}

TEST_F(URITemplateRouterViewTest, arguments_string_with_nulls) {
  const std::string null_string{"\0foo\0", 5};
  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"binary", std::string_view{null_string.data(), 5}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "binary");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[0].second));
  const auto restored_value{
      std::get<std::string_view>(collected_arguments[0].second)};
  EXPECT_EQ(restored_value.size(), 5);
  EXPECT_EQ(restored_value, std::string_view(null_string.data(), 5));
}

TEST_F(URITemplateRouterViewTest, arguments_long_string_value) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string argument_value(10000, 'x');
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"payload", std::string_view{argument_value}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "payload");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[0].second));
  const auto restored_value{
      std::get<std::string_view>(collected_arguments[0].second)};
  EXPECT_EQ(restored_value.size(), 10000);
  EXPECT_EQ(restored_value, std::string(10000, 'x'));
}

TEST_F(URITemplateRouterViewTest, arguments_multiple_mixed_types) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string string_value{"hello"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 3>
        arguments{{{"name", std::string_view{string_value}},
                   {"count", std::int64_t{99}},
                   {"active", true}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 3);

  EXPECT_EQ(collected_arguments[0].first, "name");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::string_view>(collected_arguments[0].second), "hello");

  EXPECT_EQ(collected_arguments[1].first, "count");
  EXPECT_TRUE(
      std::holds_alternative<std::int64_t>(collected_arguments[1].second));
  EXPECT_EQ(std::get<std::int64_t>(collected_arguments[1].second), 99);

  EXPECT_EQ(collected_arguments[2].first, "active");
  EXPECT_TRUE(std::holds_alternative<bool>(collected_arguments[2].second));
  EXPECT_EQ(std::get<bool>(collected_arguments[2].second), true);
}

TEST_F(URITemplateRouterViewTest, arguments_multiple_strings) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string first_value{"alpha"};
    const std::string second_value{"beta"};
    const std::string third_value{"gamma"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 3>
        arguments{{{"first", std::string_view{first_value}},
                   {"second", std::string_view{second_value}},
                   {"third", std::string_view{third_value}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 3);

  EXPECT_EQ(collected_arguments[0].first, "first");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::string_view>(collected_arguments[0].second), "alpha");

  EXPECT_EQ(collected_arguments[1].first, "second");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[1].second));
  EXPECT_EQ(std::get<std::string_view>(collected_arguments[1].second), "beta");

  EXPECT_EQ(collected_arguments[2].first, "third");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[2].second));
  EXPECT_EQ(std::get<std::string_view>(collected_arguments[2].second), "gamma");
}

TEST_F(URITemplateRouterViewTest, arguments_multiple_integers) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 3>
        arguments{{{"first", std::int64_t{10}},
                   {"second", std::int64_t{20}},
                   {"third", std::int64_t{30}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 3);

  EXPECT_EQ(collected_arguments[0].first, "first");
  EXPECT_TRUE(
      std::holds_alternative<std::int64_t>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::int64_t>(collected_arguments[0].second), 10);

  EXPECT_EQ(collected_arguments[1].first, "second");
  EXPECT_TRUE(
      std::holds_alternative<std::int64_t>(collected_arguments[1].second));
  EXPECT_EQ(std::get<std::int64_t>(collected_arguments[1].second), 20);

  EXPECT_EQ(collected_arguments[2].first, "third");
  EXPECT_TRUE(
      std::holds_alternative<std::int64_t>(collected_arguments[2].second));
  EXPECT_EQ(std::get<std::int64_t>(collected_arguments[2].second), 30);
}

TEST_F(URITemplateRouterViewTest, arguments_five_arguments) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string string_one{"hello"};
    const std::string string_two{"world"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 5>
        arguments{{{"string_one", std::string_view{string_one}},
                   {"integer_one", std::int64_t{100}},
                   {"bool_one", true},
                   {"string_two", std::string_view{string_two}},
                   {"integer_two", std::int64_t{-50}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 5);

  EXPECT_EQ(collected_arguments[0].first, "string_one");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::string_view>(collected_arguments[0].second), "hello");

  EXPECT_EQ(collected_arguments[1].first, "integer_one");
  EXPECT_TRUE(
      std::holds_alternative<std::int64_t>(collected_arguments[1].second));
  EXPECT_EQ(std::get<std::int64_t>(collected_arguments[1].second), 100);

  EXPECT_EQ(collected_arguments[2].first, "bool_one");
  EXPECT_TRUE(std::holds_alternative<bool>(collected_arguments[2].second));
  EXPECT_EQ(std::get<bool>(collected_arguments[2].second), true);

  EXPECT_EQ(collected_arguments[3].first, "string_two");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[3].second));
  EXPECT_EQ(std::get<std::string_view>(collected_arguments[3].second), "world");

  EXPECT_EQ(collected_arguments[4].first, "integer_two");
  EXPECT_TRUE(
      std::holds_alternative<std::int64_t>(collected_arguments[4].second));
  EXPECT_EQ(std::get<std::int64_t>(collected_arguments[4].second), -50);
}

TEST_F(URITemplateRouterViewTest, arguments_two_routes_different_args) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string first_route_value{"route_one_data"};
    const std::string second_route_value{"route_two_data"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        first_arguments{{{"data", std::string_view{first_route_value}}}};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        second_arguments{{{"data", std::string_view{second_route_value}}}};
    router.add("/first", 1, 0, first_arguments);
    router.add("/second", 2, 0, second_arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());

  EXPECT_ROUTER_MATCH(restored, "/first", 1, 0, captures_first);
  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_first;
  restored.arguments(
      1, [&collected_first](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_first.emplace_back(name, value);
      });
  EXPECT_EQ(collected_first.size(), 1);
  EXPECT_EQ(collected_first[0].first, "data");
  EXPECT_EQ(std::get<std::string_view>(collected_first[0].second),
            "route_one_data");

  EXPECT_ROUTER_MATCH(restored, "/second", 2, 0, captures_second);
  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_second;
  restored.arguments(
      2, [&collected_second](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_second.emplace_back(name, value);
      });
  EXPECT_EQ(collected_second.size(), 1);
  EXPECT_EQ(collected_second[0].first, "data");
  EXPECT_EQ(std::get<std::string_view>(collected_second[0].second),
            "route_two_data");
}

TEST_F(URITemplateRouterViewTest, arguments_three_routes_one_without_args) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string first_value{"alpha_data"};
    const std::string third_value{"gamma_data"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        first_arguments{{{"info", std::string_view{first_value}}}};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        third_arguments{{{"info", std::string_view{third_value}}}};
    router.add("/alpha", 1, 0, first_arguments);
    router.add("/beta", 2);
    router.add("/gamma", 3, 0, third_arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());

  EXPECT_ROUTER_MATCH(restored, "/alpha", 1, 0, captures_alpha);
  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_alpha;
  restored.arguments(
      1, [&collected_alpha](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_alpha.emplace_back(name, value);
      });
  EXPECT_EQ(collected_alpha.size(), 1);
  EXPECT_EQ(std::get<std::string_view>(collected_alpha[0].second),
            "alpha_data");

  EXPECT_ROUTER_MATCH(restored, "/beta", 2, 0, captures_beta);
  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_beta;
  restored.arguments(
      2, [&collected_beta](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_beta.emplace_back(name, value);
      });
  EXPECT_EQ(collected_beta.size(), 0);

  EXPECT_ROUTER_MATCH(restored, "/gamma", 3, 0, captures_gamma);
  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_gamma;
  restored.arguments(
      3, [&collected_gamma](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_gamma.emplace_back(name, value);
      });
  EXPECT_EQ(collected_gamma.size(), 1);
  EXPECT_EQ(std::get<std::string_view>(collected_gamma[0].second),
            "gamma_data");
}

TEST_F(URITemplateRouterViewTest, arguments_empty_router) {
  {
    sourcemeta::core::URITemplateRouter router;
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      0, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });
  EXPECT_TRUE(collected_arguments.empty());
}

TEST_F(URITemplateRouterViewTest, arguments_route_without_arguments) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/test", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/test", 1, 0, captures);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });
  EXPECT_TRUE(collected_arguments.empty());
}

TEST_F(URITemplateRouterViewTest, arguments_identifier_not_found) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string argument_value{"data"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"key", std::string_view{argument_value}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      99, [&collected_arguments](
              const std::string_view name,
              const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });
  EXPECT_TRUE(collected_arguments.empty());
}

TEST_F(URITemplateRouterViewTest, arguments_identifier_zero) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string argument_value{"data"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"key", std::string_view{argument_value}}}};
    router.add("/test", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      0, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });
  EXPECT_TRUE(collected_arguments.empty());
}

TEST_F(URITemplateRouterViewTest, arguments_with_variable_capture) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string argument_value{"user_metadata"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"metadata", std::string_view{argument_value}}}};
    router.add("/users/{id}", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/users/42", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "metadata");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::string_view>(collected_arguments[0].second),
            "user_metadata");
}

TEST_F(URITemplateRouterViewTest, arguments_with_expansion) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string argument_value{"file_info"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"info", std::string_view{argument_value}}}};
    router.add("/files/{+path}", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/files/a/b/c", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "a/b/c");

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_arguments;
  restored.arguments(
      1, [&collected_arguments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_arguments.emplace_back(name, value);
      });

  EXPECT_EQ(collected_arguments.size(), 1);
  EXPECT_EQ(collected_arguments[0].first, "info");
  EXPECT_TRUE(
      std::holds_alternative<std::string_view>(collected_arguments[0].second));
  EXPECT_EQ(std::get<std::string_view>(collected_arguments[0].second),
            "file_info");
}

TEST_F(URITemplateRouterViewTest, arguments_multiple_routes_match_and_args) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string user_value{"user_data"};
    const std::string post_value{"post_data"};
    const std::string comment_value{"comment_data"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        user_arguments{{{"tag", std::string_view{user_value}}}};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        post_arguments{{{"tag", std::string_view{post_value}}}};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        comment_arguments{{{"tag", std::string_view{comment_value}}}};
    router.add("/users/{id}", 1, 0, user_arguments);
    router.add("/posts/{id}", 2, 0, post_arguments);
    router.add("/comments/{id}", 3, 0, comment_arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());

  EXPECT_ROUTER_MATCH(restored, "/users/1", 1, 0, captures_users);
  EXPECT_EQ(captures_users.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_users, 0, "id", "1");
  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_users;
  restored.arguments(
      1, [&collected_users](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_users.emplace_back(name, value);
      });
  EXPECT_EQ(collected_users.size(), 1);
  EXPECT_EQ(std::get<std::string_view>(collected_users[0].second), "user_data");

  EXPECT_ROUTER_MATCH(restored, "/posts/2", 2, 0, captures_posts);
  EXPECT_EQ(captures_posts.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_posts, 0, "id", "2");
  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_posts;
  restored.arguments(
      2, [&collected_posts](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_posts.emplace_back(name, value);
      });
  EXPECT_EQ(collected_posts.size(), 1);
  EXPECT_EQ(std::get<std::string_view>(collected_posts[0].second), "post_data");

  EXPECT_ROUTER_MATCH(restored, "/comments/3", 3, 0, captures_comments);
  EXPECT_EQ(captures_comments.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_comments, 0, "id", "3");
  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_comments;
  restored.arguments(
      3, [&collected_comments](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_comments.emplace_back(name, value);
      });
  EXPECT_EQ(collected_comments.size(), 1);
  EXPECT_EQ(std::get<std::string_view>(collected_comments[0].second),
            "comment_data");
}

TEST_F(URITemplateRouterViewTest, arguments_do_not_affect_match_precedence) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string me_value{"special_user"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        me_arguments{{{"role", std::string_view{me_value}}}};
    router.add("/users/me", 1, 0, me_arguments);
    router.add("/users/{id}", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());

  EXPECT_ROUTER_MATCH(restored, "/users/me", 1, 0, captures_me);
  EXPECT_EQ(captures_me.size(), 0);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_me;
  restored.arguments(
      1, [&collected_me](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_me.emplace_back(name, value);
      });
  EXPECT_EQ(collected_me.size(), 1);
  EXPECT_EQ(collected_me[0].first, "role");
  EXPECT_EQ(std::get<std::string_view>(collected_me[0].second), "special_user");

  EXPECT_ROUTER_MATCH(restored, "/users/123", 2, 0, captures_other);
  EXPECT_EQ(captures_other.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_other, 0, "id", "123");

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_no_args;
  restored.arguments(
      2, [&collected_no_args](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_no_args.emplace_back(name, value);
      });
  EXPECT_TRUE(collected_no_args.empty());
}

TEST_F(URITemplateRouterViewTest, arguments_survive_large_trie) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string payload_a{"data_for_a"};
    const std::string payload_b{"data_for_b"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> args_a{
        {{"payload", std::string_view{payload_a}}}};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1> args_b{
        {{"payload", std::string_view{payload_b}}}};

    router.add("/api/v1/alpha", 1, 0, args_a);
    router.add("/api/v1/beta", 2, 0, args_b);
    router.add("/api/v1/gamma", 3);
    router.add("/api/v1/delta", 4);
    router.add("/api/v1/epsilon", 5);
    router.add("/api/v1/zeta", 6);
    router.add("/api/v1/eta", 7);
    router.add("/api/v1/theta", 8);
    router.add("/api/v1/iota", 9);
    router.add("/api/v1/kappa", 10);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());

  EXPECT_ROUTER_MATCH(restored, "/api/v1/alpha", 1, 0, captures_alpha);
  EXPECT_ROUTER_MATCH(restored, "/api/v1/beta", 2, 0, captures_beta);
  EXPECT_ROUTER_MATCH(restored, "/api/v1/gamma", 3, 0, captures_gamma);
  EXPECT_ROUTER_MATCH(restored, "/api/v1/kappa", 10, 0, captures_kappa);

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_a;
  restored.arguments(
      1, [&collected_a](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_a.emplace_back(name, value);
      });
  EXPECT_EQ(collected_a.size(), 1);
  EXPECT_EQ(collected_a[0].first, "payload");
  EXPECT_EQ(std::get<std::string_view>(collected_a[0].second), "data_for_a");

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_b;
  restored.arguments(
      2, [&collected_b](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_b.emplace_back(name, value);
      });
  EXPECT_EQ(collected_b.size(), 1);
  EXPECT_EQ(collected_b[0].first, "payload");
  EXPECT_EQ(std::get<std::string_view>(collected_b[0].second), "data_for_b");

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_gamma;
  restored.arguments(
      3, [&collected_gamma](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_gamma.emplace_back(name, value);
      });
  EXPECT_TRUE(collected_gamma.empty());

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected_kappa;
  restored.arguments(
      10, [&collected_kappa](
              const std::string_view name,
              const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected_kappa.emplace_back(name, value);
      });
  EXPECT_TRUE(collected_kappa.empty());
}

TEST_F(URITemplateRouterViewTest, base_path_single_segment) {
  {
    sourcemeta::core::URITemplateRouter router{"/prefix"};
    router.add("/foo", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/prefix");
  EXPECT_ROUTER_MATCH(restored, "/prefix/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, base_path_without_prefix_no_match) {
  {
    sourcemeta::core::URITemplateRouter router{"/prefix"};
    router.add("/foo", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/prefix");
  EXPECT_ROUTER_MATCH(restored, "/foo", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, base_path_multi_segment) {
  {
    sourcemeta::core::URITemplateRouter router{"/v1/catalog"};
    router.add("/api/list", 1);
    router.add("/{+path}", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/v1/catalog");
  EXPECT_ROUTER_MATCH(restored, "/v1/catalog/api/list", 1, 0, captures_list);
  EXPECT_EQ(captures_list.size(), 0);
  EXPECT_ROUTER_MATCH(restored, "/v1/catalog/foo/bar", 2, 0,
                      captures_expansion);
  EXPECT_EQ(captures_expansion.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_expansion, 0, "path", "foo/bar");
}

TEST_F(URITemplateRouterViewTest, base_path_with_variable) {
  {
    sourcemeta::core::URITemplateRouter router{"/prefix"};
    router.add("/users/{id}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/prefix");
  EXPECT_ROUTER_MATCH(restored, "/prefix/users/42", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

TEST_F(URITemplateRouterViewTest, base_path_prefix_boundary_no_match) {
  {
    sourcemeta::core::URITemplateRouter router{"/prefix"};
    router.add("/foo", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/prefix");
  EXPECT_ROUTER_MATCH(restored, "/prefixfoo", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, base_path_with_empty_template) {
  {
    sourcemeta::core::URITemplateRouter router{"/v1/catalog"};
    router.add("", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/v1/catalog");
  EXPECT_ROUTER_MATCH(restored, "/v1/catalog", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, base_path_no_base_path_unchanged) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_ROUTER_MATCH(restored, "/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, base_path_expansion) {
  {
    sourcemeta::core::URITemplateRouter router{"/api"};
    router.add("/files/{+path}", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/api");
  EXPECT_ROUTER_MATCH(restored, "/api/files/a/b/c", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "a/b/c");
}

TEST_F(URITemplateRouterViewTest, base_path_trailing_slash_normalized) {
  {
    sourcemeta::core::URITemplateRouter router{"/prefix/"};
    router.add("/foo", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/prefix");
  EXPECT_ROUTER_MATCH(restored, "/prefix/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest,
       base_path_multiple_trailing_slashes_normalized) {
  {
    sourcemeta::core::URITemplateRouter router{"/prefix///"};
    router.add("/foo", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/prefix");
  EXPECT_ROUTER_MATCH(restored, "/prefix/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, add_with_context_literal_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1, 7);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users", 1, 7, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, add_with_context_variable_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}", 1, 42);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/123", 1, 42, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "123");
}

TEST_F(URITemplateRouterViewTest, add_with_context_default_zero) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, add_multiple_routes_different_contexts) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1, 1);
    router.add("/posts", 2, 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  {
    EXPECT_ROUTER_MATCH(restored, "/users", 1, 1, captures);
    EXPECT_EQ(captures.size(), 0);
  }
  {
    EXPECT_ROUTER_MATCH(restored, "/posts", 2, 2, captures);
    EXPECT_EQ(captures.size(), 0);
  }
}

TEST_F(URITemplateRouterViewTest, add_same_context_multiple_routes) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1, 99);
    router.add("/posts", 2, 99);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  {
    EXPECT_ROUTER_MATCH(restored, "/users", 1, 99, captures);
    EXPECT_EQ(captures.size(), 0);
  }
  {
    EXPECT_ROUTER_MATCH(restored, "/posts", 2, 99, captures);
    EXPECT_EQ(captures.size(), 0);
  }
}

TEST_F(URITemplateRouterViewTest, add_with_context_and_arguments) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 2>
        arguments{{
            {"schema", std::string_view{"schemas/health"}},
            {"enabled", true},
        }};
    router.add("/api/health", 1, 11, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/health", 1, 11, captures);
  EXPECT_EQ(captures.size(), 0);

  std::vector<std::pair<std::string, std::string>> seen_string;
  std::vector<std::pair<std::string, bool>> seen_bool;
  restored.arguments(
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

  ASSERT_EQ(seen_string.size(), 1);
  EXPECT_EQ(seen_string.at(0).first, "schema");
  EXPECT_EQ(seen_string.at(0).second, "schemas/health");
  ASSERT_EQ(seen_bool.size(), 1);
  EXPECT_EQ(seen_bool.at(0).first, "enabled");
  EXPECT_TRUE(seen_bool.at(0).second);
}

TEST_F(URITemplateRouterViewTest, add_context_expansion_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+path}", 1, 5);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/files/a/b/c", 1, 5, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "a/b/c");
}

TEST_F(URITemplateRouterViewTest, add_context_base_path) {
  {
    sourcemeta::core::URITemplateRouter router{"/v1/catalog"};
    router.add("/api/list", 1, 33);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/v1/catalog");
  EXPECT_ROUTER_MATCH(restored, "/v1/catalog/api/list", 1, 33, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, add_with_context_no_match_returns_zero_pair) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1, 7);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/posts", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest,
       add_with_context_overwrites_previous_context) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", 1, 10);
    router.add("/users", 1, 20);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users", 1, 20, captures);
  EXPECT_EQ(captures.size(), 0);
}

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
    router.add("/users", "op_1", 1);
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
    router.add("/users", "op_2", 1);
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
    router.add("/users/list", "op_3", 1);
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
    router.add("/users/{id}", "op_4", 1);
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
    router.add("/users/{id}/posts/{post_id}", "op_5", 1);
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
    router.add("/users/me", "op_6", 1);
    router.add("/users/{id}", "op_7", 2);
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
    router.add("/users/me", "op_8", 1);
    router.add("/users/{id}", "op_9", 2);
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
    router.add("/users", "op_10", 1);
    router.add("/users/{id}", "op_11", 2);
    router.add("/posts", "op_12", 3);
    router.add("/posts/{id}", "op_13", 4);
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
    router.add("/users", "op_14", 1);
    router.add("/users/{id}", "op_15", 2);
    router.add("/posts", "op_16", 3);
    router.add("/posts/{id}", "op_17", 4);
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
    router.add("/users", "op_18", 1);
    router.add("/users/{id}", "op_19", 2);
    router.add("/posts", "op_20", 3);
    router.add("/posts/{id}", "op_21", 4);
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
    router.add("/users", "op_22", 1);
    router.add("/users/{id}", "op_23", 2);
    router.add("/posts", "op_24", 3);
    router.add("/posts/{id}", "op_25", 4);
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
    router.add("/alpha", "op_26", 1);
    router.add("/beta", "op_27", 2);
    router.add("/gamma", "op_28", 3);
    router.add("/delta", "op_29", 4);
    router.add("/epsilon", "op_30", 5);
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
    router.add("/alpha", "op_31", 1);
    router.add("/beta", "op_32", 2);
    router.add("/gamma", "op_33", 3);
    router.add("/delta", "op_34", 4);
    router.add("/epsilon", "op_35", 5);
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
    router.add("/alpha", "op_36", 1);
    router.add("/beta", "op_37", 2);
    router.add("/gamma", "op_38", 3);
    router.add("/delta", "op_39", 4);
    router.add("/epsilon", "op_40", 5);
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
    router.add("/", "op_41", 1);
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
    router.add("/", "op_42", 1);
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
    router.add("/", "op_43", 1);
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
    router.add("", "op_44", 1);
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
    router.add("", "op_45", 1);
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
    router.add("", "op_46", 1);
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
    router.add("/", "op_47", 1);
    router.add("/users", "op_48", 2);
    router.add("/users/{id}", "op_49", 3);
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
    router.add("/", "op_50", 1);
    router.add("/users", "op_51", 2);
    router.add("/users/{id}", "op_52", 3);
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
    router.add("/", "op_53", 1);
    router.add("/users", "op_54", 2);
    router.add("/users/{id}", "op_55", 3);
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
    router.add("", "op_56", 1);
    router.add("/", "op_57", 2);
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
    router.add("", "op_58", 1);
    router.add("/", "op_59", 2);
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
    router.add("", "op_60", 1);
    router.add("/", "op_61", 2);
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
    router.add("", "op_62", 1);
    router.add("/", "op_63", 2);
    router.add("/users", "op_64", 3);
    router.add("/users/{id}", "op_65", 4);
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
    router.add("", "op_66", 1);
    router.add("/", "op_67", 2);
    router.add("/users", "op_68", 3);
    router.add("/users/{id}", "op_69", 4);
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
    router.add("", "op_70", 1);
    router.add("/", "op_71", 2);
    router.add("/users", "op_72", 3);
    router.add("/users/{id}", "op_73", 4);
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
    router.add("", "op_74", 1);
    router.add("/", "op_75", 2);
    router.add("/users", "op_76", 3);
    router.add("/users/{id}", "op_77", 4);
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
    router.add("/users/{id}/posts", "op_78", 1);
    router.add("/users/{id}/comments", "op_79", 2);
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
    router.add("/files/{+path}", "op_80", 1);
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
    router.add("/api/v1/proxy/{+url}", "op_81", 1);
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
    router.add("/files/{+path}", "op_82", 1);
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
    router.add("/files/{path}", "op_83", 1);
    router.add("/files/{+path}", "op_84", 2);
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
    router.add("/files/{path}", "op_85", 1);
    router.add("/files/{+path}", "op_86", 2);
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
    router.add("/files/{+path}", "op_87", 1);
    router.add("/files/{path}", "op_88", 2);
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
    router.add("/files/{+path}", "op_89", 1);
    router.add("/files/special", "op_90", 2);
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
    router.add("/files/{+path}", "op_91", 1);
    router.add("/files/special", "op_92", 2);
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
    router.add("/users", "op_93", 1);
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
    router.add("/users", "op_94", 1);
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
    router.add("/users", "op_95", 1);
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
    router.add("/users/posts", "op_96", 1);
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
    router.add("/users/{id}", "op_97", 1);
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
    router.add("/users/{id}/posts", "op_98", 1);
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
    router.add("/files/{+path}", "op_99", 1);
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
    router.add("/files/{+path}", "op_100", 1);
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
  const std::uint32_t data[] = {0xDEADBEEF, 5, 1, 64, 64, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_wrong_version) {
  const std::uint32_t data[] = {0x52544552, 99, 1, 64, 64, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_node_count_exceeds_file) {
  const std::uint32_t data[] = {0x52544552, 5, 10, 32, 32, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_literal_child_out_of_bounds) {
  const std::uint32_t data[] = {0x52544552, 5, 1,   64, 64,         0, 0, 0,
                                0,          0, 999, 1,  0xFFFFFFFF, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_variable_child_out_of_bounds) {
  const std::uint32_t data[] = {0x52544552, 5, 1,          64, 64,  0, 0, 0,
                                0,          0, 0xFFFFFFFF, 0,  500, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_string_offset_out_of_bounds) {
  const std::uint32_t data[] = {
      0x52544552, 5, 2, 96, 96,   0, 0,          0, 0,          0, 0, 1, 1,
      0xFFFFFFFF, 0, 0, 0,  9999, 5, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_variable_string_offset_out_of_bounds) {
  const std::uint32_t data[] = {
      0x52544552, 5,   2,          96, 96,         0,          0, 0,
      0,          0,   0xFFFFFFFF, 0,  1,          0,          0, 0,
      9999,       100, 0xFFFFFFFF, 0,  0xFFFFFFFF, 0x00000002, 0, 0};
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
  const std::uint32_t data[] = {0x52544552, 5, 1,          4, 64, 0, 0, 0, 0, 0,
                                0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 0,  0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_string_table_offset_past_end) {
  const std::uint32_t data[] = {0x52544552, 5, 1, 99999, 99999,      0,
                                0,          0, 0, 0,     0xFFFFFFFF, 0,
                                0xFFFFFFFF, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_zero_node_count) {
  const std::uint32_t data[] = {0x52544552, 5, 0, 32, 32, 0, 0, 0};
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
  const std::uint32_t data[] = {
      0x52544552, 5, 2, 96, 96, 0, 0,          0, 0,          0, 1, 0xFFFFFFFF,
      0xFFFFFFFF, 0, 0, 0,  0,  0, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_root_literal_child_oob_match_root) {
  const std::uint32_t data[] = {0x52544552, 5, 1,   64, 64,         0, 0, 0,
                                0,          0, 999, 1,  0xFFFFFFFF, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_deep_node_variable_child_oob) {
  std::vector<std::uint8_t> data;
  const std::uint32_t header[] = {0x52544552, 5, 2, 96, 101, 0, 0, 0};
  const std::uint32_t root[] = {0, 0, 1, 1, 0xFFFFFFFF, 0, 0, 0};
  const std::uint32_t child[] = {0, 5, 0xFFFFFFFF, 0, 999, 0x00000001, 0, 0};
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
  const std::uint32_t data[] = {
      0x52544552, 5,   2,          96, 96,         0,          0, 0,
      0,          0,   0xFFFFFFFF, 0,  1,          0,          0, 0,
      5000,       200, 0xFFFFFFFF, 0,  0xFFFFFFFF, 0x00000003, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/files/foo/bar", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_empty_string_table_with_string_ref) {
  const std::uint32_t data[] = {
      0x52544552, 5, 2, 96, 96, 0,  0,          0, 0,          0, 1, 1,
      0xFFFFFFFF, 0, 0, 0,  0,  10, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_node_count_max_uint32) {
  const std::uint32_t data[] = {0x52544552, 5, 0xFFFFFFFF, 44, 44, 0,
                                0,          0, 0,          0,  0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView, corrupt_string_offset_plus_length_overflow) {
  const std::uint32_t data[] = {
      0x52544552, 5,          2,          96, 96,         0,          0, 0,
      0,          0,          0xFFFFFFFF, 0,  1,          0,          0, 0,
      0x80000000, 0x80000001, 0xFFFFFFFF, 0,  0xFFFFFFFF, 0x00000002, 0, 0};
  const sourcemeta::core::URITemplateRouterView view{
      reinterpret_cast<const std::uint8_t *>(data), sizeof(data)};
  EXPECT_ROUTER_MATCH(view, "/users", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplateRouterView,
     corrupt_string_offset_plus_length_overflow_with_data) {
  std::vector<std::uint8_t> data;
  const std::uint32_t header[] = {0x52544552, 5, 2, 96, 97, 0, 0, 0};
  const std::uint32_t root[] = {0, 0, 0xFFFFFFFF, 0, 1, 0, 0, 0};
  const std::uint32_t variable[] = {0xFFFFFFFF, 2,          0xFFFFFFFF, 0,
                                    0xFFFFFFFF, 0x00000002, 0,          0};
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
  const std::uint32_t header[] = {0x52544552, 5, 2, 96, 97, 0, 0, 0};
  const std::uint32_t root[] = {0, 0, 1, 1, 0xFFFFFFFF, 0, 0, 0};
  const std::uint32_t child[] = {0xFFFFFFFF, 2, 0xFFFFFFFF, 0,
                                 0xFFFFFFFF, 0, 0,          0};
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
    router.add("/test", "op_101", 1, 0, arguments);
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
    router.add("/test", "op_102", 1, 0, arguments);
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
    router.add("/test", "op_103", 1, 0, arguments);
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
    router.add("/test", "op_104", 1, 0, arguments);
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
    router.add("/test", "op_105", 1, 0, arguments);
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
    router.add("/test", "op_106", 1, 0, arguments);
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
    router.add("/test", "op_107", 1, 0, arguments);
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
    router.add("/test", "op_108", 1, 0, arguments);
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
    router.add("/test", "op_109", 1, 0, arguments);
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
    router.add("/test", "op_110", 1, 0, arguments);
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
    router.add("/test", "op_111", 1, 0, arguments);
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
    router.add("/test", "op_112", 1, 0, arguments);
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
    router.add("/test", "op_113", 1, 0, arguments);
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
    router.add("/test", "op_114", 1, 0, arguments);
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
    router.add("/test", "op_115", 1, 0, arguments);
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
    router.add("/test", "op_116", 1, 0, arguments);
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
    router.add("/test", "op_117", 1, 0, arguments);
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
    router.add("/first", "op_118", 1, 0, first_arguments);
    router.add("/second", "op_119", 2, 0, second_arguments);
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
    router.add("/alpha", "op_120", 1, 0, first_arguments);
    router.add("/beta", "op_121", 2);
    router.add("/gamma", "op_122", 3, 0, third_arguments);
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
    router.add("/test", "op_123", 1);
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
    router.add("/test", "op_124", 1, 0, arguments);
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
    router.add("/test", "op_125", 1, 0, arguments);
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
    router.add("/users/{id}", "op_126", 1, 0, arguments);
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
    router.add("/files/{+path}", "op_127", 1, 0, arguments);
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
    router.add("/users/{id}", "op_128", 1, 0, user_arguments);
    router.add("/posts/{id}", "op_129", 2, 0, post_arguments);
    router.add("/comments/{id}", "op_130", 3, 0, comment_arguments);
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
    router.add("/users/me", "op_131", 1, 0, me_arguments);
    router.add("/users/{id}", "op_132", 2);
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

    router.add("/api/v1/alpha", "op_133", 1, 0, args_a);
    router.add("/api/v1/beta", "op_134", 2, 0, args_b);
    router.add("/api/v1/gamma", "op_135", 3);
    router.add("/api/v1/delta", "op_136", 4);
    router.add("/api/v1/epsilon", "op_137", 5);
    router.add("/api/v1/zeta", "op_138", 6);
    router.add("/api/v1/eta", "op_139", 7);
    router.add("/api/v1/theta", "op_140", 8);
    router.add("/api/v1/iota", "op_141", 9);
    router.add("/api/v1/kappa", "op_142", 10);
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
    router.add("/foo", "op_143", 1);
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
    router.add("/foo", "op_144", 1);
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
    router.add("/api/list", "op_145", 1);
    router.add("/{+path}", "op_146", 2);
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
    router.add("/users/{id}", "op_147", 1);
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
    router.add("/foo", "op_148", 1);
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
    router.add("", "op_149", 1);
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
    router.add("/foo", "op_150", 1);
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
    router.add("/files/{+path}", "op_151", 1);
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
    router.add("/foo", "op_152", 1);
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
    router.add("/foo", "op_153", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/prefix");
  EXPECT_ROUTER_MATCH(restored, "/prefix/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, base_url_empty_by_default) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo", "op_base_url_default", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_TRUE(restored.base_url().empty());
  EXPECT_ROUTER_MATCH(restored, "/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, base_url_empty_when_only_base_path) {
  {
    sourcemeta::core::URITemplateRouter router{"/prefix"};
    router.add("/foo", "op_base_url_only_path", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/prefix");
  EXPECT_TRUE(restored.base_url().empty());
}

TEST_F(URITemplateRouterViewTest, base_url_round_trip_with_base_path) {
  {
    sourcemeta::core::URITemplateRouter router{"/v1",
                                               "https://api.example.com"};
    router.add("/foo", "op_base_url_round_trip", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/v1");
  EXPECT_EQ(restored.base_url(), "https://api.example.com");
  EXPECT_ROUTER_MATCH(restored, "/v1/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, base_url_round_trip_without_base_path) {
  {
    sourcemeta::core::URITemplateRouter router{"", "https://api.example.com"};
    router.add("/foo", "op_base_url_no_path", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.base_path().empty());
  EXPECT_EQ(restored.base_url(), "https://api.example.com");
  EXPECT_ROUTER_MATCH(restored, "/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, base_url_trailing_slash_normalized) {
  {
    sourcemeta::core::URITemplateRouter router{"", "https://api.example.com/"};
    router.add("/foo", "op_base_url_trailing", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_url(), "https://api.example.com");
}

TEST_F(URITemplateRouterViewTest, base_url_arguments_still_resolve) {
  {
    sourcemeta::core::URITemplateRouter router{"/v1",
                                               "https://api.example.com"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{
            {"schema", std::string_view{"schemas/health"}},
        }};
    router.add("/health", "op_base_url_args", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/v1");
  EXPECT_EQ(restored.base_url(), "https://api.example.com");

  std::vector<std::pair<std::string, std::string>> seen;
  restored.arguments(
      1,
      [&seen](const std::string_view name,
              const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        if (std::holds_alternative<std::string_view>(value)) {
          seen.emplace_back(std::string{name},
                            std::string{std::get<std::string_view>(value)});
        }
      });
  ASSERT_EQ(seen.size(), 1);
  EXPECT_EQ(seen.at(0).first, "schema");
  EXPECT_EQ(seen.at(0).second, "schemas/health");
}

TEST_F(URITemplateRouterViewTest, add_with_context_literal_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_154", 1, 7);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users", 1, 7, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, add_with_context_variable_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}", "op_155", 1, 42);
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
    router.add("/users", "op_156", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, add_multiple_routes_different_contexts) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_157", 1, 1);
    router.add("/posts", "op_158", 2, 2);
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
    router.add("/users", "op_159", 1, 99);
    router.add("/posts", "op_160", 2, 99);
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
    router.add("/api/health", "op_161", 1, 11, arguments);
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
    router.add("/files/{+path}", "op_162", 1, 5);
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
    router.add("/api/list", "op_163", 1, 33);
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
    router.add("/users", "op_164", 1, 7);
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
    router.add("/users", "op_165", 1, 10);
    router.add("/users", "op_166", 1, 20);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users", 1, 20, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, size_empty_router) {
  {
    const sourcemeta::core::URITemplateRouter router;
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 0);
}

TEST_F(URITemplateRouterViewTest, size_single_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_167", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 1);
}

TEST_F(URITemplateRouterViewTest, size_multiple_routes) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_168", 1);
    router.add("/users/{id}", "op_169", 2);
    router.add("/posts", "op_170", 3);
    router.add("/posts/{id}", "op_171", 4);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 4);
}

TEST_F(URITemplateRouterViewTest, size_duplicate_route_does_not_increase) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_172", 1);
    router.add("/users", "op_173", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 1);
}

TEST_F(URITemplateRouterViewTest, size_with_base_path) {
  {
    sourcemeta::core::URITemplateRouter router{"/v1"};
    router.add("/users", "op_174", 1);
    router.add("/posts", "op_175", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 2);
}

TEST_F(URITemplateRouterViewTest, otherwise_returned_from_unknown_path) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_176", 1, 5);
    router.otherwise(99);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/unknown", 0, 99, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, otherwise_not_returned_from_matching_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_177", 1, 5);
    router.otherwise(99);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users", 1, 5, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, otherwise_returned_for_empty_segment) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_178", 1);
    router.otherwise(77);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users//", 0, 77, captures);
}

TEST_F(URITemplateRouterViewTest, otherwise_returned_for_root_slash_no_match) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_179", 1);
    router.otherwise(88);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/", 0, 88, captures);
}

TEST_F(URITemplateRouterViewTest,
       otherwise_without_registration_returns_zero_context) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_180", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/unknown", 0, 0, captures);
}

TEST_F(URITemplateRouterViewTest, otherwise_arguments_lookup) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string message_value{"Not Found"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 2>
        arguments{{{"status", std::int64_t{404}},
                   {"message", std::string_view{message_value}}}};
    router.otherwise(55, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  restored.arguments(
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

TEST_F(URITemplateRouterViewTest, otherwise_boolean_argument) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        arguments{{{"cached", true}}};
    router.otherwise(3, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  restored.arguments(
      0, [&collected](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });

  EXPECT_EQ(collected.size(), 1);
  EXPECT_TRUE(std::holds_alternative<bool>(collected[0].second));
  EXPECT_EQ(std::get<bool>(collected[0].second), true);
}

TEST_F(URITemplateRouterViewTest, otherwise_does_not_affect_other_arguments) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::string schema_value{"user.json"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        route_args{{{"schema", std::string_view{schema_value}}}};
    router.add("/users", "op_181", 1, 0, route_args);

    const std::string message_value{"not found"};
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        default_args{{{"message", std::string_view{message_value}}}};
    router.otherwise(99, default_args);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};

  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      route_collected;
  restored.arguments(
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
  restored.arguments(
      0, [&default_collected](
             const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        default_collected.emplace_back(name, value);
      });
  EXPECT_EQ(default_collected.size(), 1);
  EXPECT_EQ(default_collected[0].first, "message");
}

TEST_F(URITemplateRouterViewTest, otherwise_does_not_count_as_route_in_size) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_182", 1);
    router.otherwise(99);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 1);
}

TEST_F(URITemplateRouterViewTest, otherwise_with_base_path_and_unmatched) {
  {
    sourcemeta::core::URITemplateRouter router{"/v1"};
    router.add("/users", "op_183", 1);
    router.otherwise(42);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/v1/other", 0, 42, captures);
}

TEST_F(URITemplateRouterViewTest, otherwise_with_partial_trie_walk) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}/posts", "op_184", 1);
    router.otherwise(42);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/123", 0, 42, captures);
}

TEST_F(URITemplateRouterViewTest, otherwise_overwrite_context) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.otherwise(10);
    router.otherwise(20);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/nope", 0, 20, captures);
}

TEST_F(URITemplateRouterViewTest, listing_size_matches_router) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_185", 1);
    router.add("/users/{id}", "op_186", 2);
    router.add("/posts/{id}/comments/{comment_id}", "op_187", 3);
    router.add("/files/{+rest}", "op_188", 4);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 4);
}

TEST_F(URITemplateRouterViewTest, listing_size_excludes_otherwise) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_189", 1);
    router.add("/posts", "op_190", 2);
    router.otherwise(99);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 2);
}

TEST_F(URITemplateRouterViewTest, operation_returns_identifier_and_context) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/a", "alpha", 1, 11);
    router.add("/b", "beta", 2, 22);
    router.add("/c", "gamma", 3, 33);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};

  const auto alpha = restored.operation("alpha");
  EXPECT_EQ(alpha.first, 1);
  EXPECT_EQ(alpha.second, 11);

  const auto beta = restored.operation("beta");
  EXPECT_EQ(beta.first, 2);
  EXPECT_EQ(beta.second, 22);

  const auto gamma = restored.operation("gamma");
  EXPECT_EQ(gamma.first, 3);
  EXPECT_EQ(gamma.second, 33);
}

TEST_F(URITemplateRouterViewTest, operation_unknown_returns_zero_zero) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/a", "alpha", 1, 11);
    router.otherwise(99);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  const auto result = restored.operation("missing");
  EXPECT_EQ(result.first, 0);
  EXPECT_EQ(result.second, 0);
}

TEST_F(URITemplateRouterViewTest, operation_full_character_set_round_trip) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/a", "aZ0_-", 1);
    router.add("/b", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-09",
               2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.operation("aZ0_-").first, 1);
  EXPECT_EQ(
      restored
          .operation("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
                     "_-09")
          .first,
      2);
}

TEST_F(URITemplateRouterViewTest, operation_empty_router) {
  {
    sourcemeta::core::URITemplateRouter router;
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  const auto result = restored.operation("anything");
  EXPECT_EQ(result.first, 0);
  EXPECT_EQ(result.second, 0);
}

TEST_F(URITemplateRouterViewTest, operation_rejects_v5_blob) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/a", "alpha", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  std::vector<std::uint8_t> blob;
  {
    std::ifstream file{this->path, std::ios::binary | std::ios::ate};
    const auto size = static_cast<std::size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    blob.resize(size);
    file.read(reinterpret_cast<char *>(blob.data()),
              static_cast<std::streamsize>(size));
  }

  const std::uint32_t old_version = 5;
  std::memcpy(blob.data() + sizeof(std::uint32_t), &old_version,
              sizeof(old_version));

  const sourcemeta::core::URITemplateRouterView restored{blob.data(),
                                                         blob.size()};
  const auto result = restored.operation("alpha");
  EXPECT_EQ(result.first, 0);
  EXPECT_EQ(result.second, 0);
}

TEST_F(URITemplateRouterViewTest, operation_rejects_v6_blob) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/a", "alpha", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  std::vector<std::uint8_t> blob;
  {
    std::ifstream file{this->path, std::ios::binary | std::ios::ate};
    const auto size = static_cast<std::size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    blob.resize(size);
    file.read(reinterpret_cast<char *>(blob.data()),
              static_cast<std::streamsize>(size));
  }

  const std::uint32_t old_version = 6;
  std::memcpy(blob.data() + sizeof(std::uint32_t), &old_version,
              sizeof(old_version));

  const sourcemeta::core::URITemplateRouterView restored{blob.data(),
                                                         blob.size()};
  const auto result = restored.operation("alpha");
  EXPECT_EQ(result.first, 0);
  EXPECT_EQ(result.second, 0);
}

TEST_F(URITemplateRouterViewTest, optional_expansion_empty_capture) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "op_v_oex_1", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/list", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "");
}

TEST_F(URITemplateRouterViewTest, optional_expansion_single_segment) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "op_v_oex_2", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/list/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo");
}

TEST_F(URITemplateRouterViewTest, optional_expansion_two_segments) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "op_v_oex_3", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/list/foo/bar", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar");
}

TEST_F(URITemplateRouterViewTest, optional_expansion_many_segments) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "op_v_oex_4", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/list/foo/bar/baz/qux", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo/bar/baz/qux");
}

TEST_F(URITemplateRouterViewTest, optional_expansion_percent_encoded_segment) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "op_v_oex_5", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/list/foo%2Fbar", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "path", "foo%2Fbar");
}

TEST_F(URITemplateRouterViewTest, optional_expansion_rejects_trailing_slash) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "op_v_oex_6", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/list/", 0, 0, captures);
}

TEST_F(URITemplateRouterViewTest,
       optional_expansion_rejects_internal_double_slash) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "op_v_oex_7", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/list//foo", 0, 0, captures);
}

TEST_F(URITemplateRouterViewTest, optional_expansion_rejects_non_prefix) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "op_v_oex_8", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/listing", 0, 0, captures);
}

TEST_F(URITemplateRouterViewTest, optional_expansion_with_context) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "op_v_oex_9", 1, 7);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/list", 1, 7, captures_empty);
  EXPECT_EQ(captures_empty.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_empty, 0, "path", "");
  EXPECT_ROUTER_MATCH(restored, "/api/list/foo/bar", 1, 7, captures_two);
  EXPECT_EQ(captures_two.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_two, 0, "path", "foo/bar");
}

TEST_F(URITemplateRouterViewTest,
       optional_expansion_with_literal_sibling_priority) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "op_v_oex_p1", 1);
    router.add("/api/list/special", "op_v_oex_p2", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/list/special", 2, 0, captures_special);
  EXPECT_EQ(captures_special.size(), 0);
  EXPECT_ROUTER_MATCH(restored, "/api/list/other", 1, 0, captures_other);
  EXPECT_EQ(captures_other.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_other, 0, "path", "other");
}

TEST_F(URITemplateRouterViewTest,
       optional_expansion_with_literal_parent_route_priority) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list", "op_v_oex_pp1", 1);
    router.add("/api/list{/path*}", "op_v_oex_pp2", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/list", 1, 0, captures_root);
  EXPECT_EQ(captures_root.size(), 0);
  EXPECT_ROUTER_MATCH(restored, "/api/list/foo/bar", 2, 0, captures_nested);
  EXPECT_EQ(captures_nested.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_nested, 0, "path", "foo/bar");
}

TEST_F(URITemplateRouterViewTest,
       optional_expansion_callback_index_after_variables) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/{namespace}/list{/path*}", "op_v_oex_idx", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/users/list/foo/bar", 1, 0, captures);
  EXPECT_EQ(captures.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures, 0, "namespace", "users");
  EXPECT_ROUTER_CAPTURE(captures, 1, "path", "foo/bar");
}

TEST_F(URITemplateRouterViewTest,
       optional_expansion_empty_callback_index_after_variables) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/{namespace}/list{/path*}", "op_v_oex_idx2", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/users/list", 1, 0, captures);
  EXPECT_EQ(captures.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures, 0, "namespace", "users");
  EXPECT_ROUTER_CAPTURE(captures, 1, "path", "");
}

TEST_F(URITemplateRouterViewTest, optional_expansion_operation_lookup) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "list_directory", 1, 42);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  const auto result = restored.operation("list_directory");
  EXPECT_EQ(result.first, 1);
  EXPECT_EQ(result.second, 42);
}

TEST_F(URITemplateRouterViewTest, optional_expansion_size) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "op_v_oex_sz", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 1);
}

TEST_F(URITemplateRouterViewTest, optional_expansion_with_arguments) {
  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 2>
        arguments{
            {{"max", std::int64_t{99}}, {"name", std::string_view{"hello"}}}};
    router.add("/api/list{/path*}", "op_v_oex_args", 1, 0, arguments);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  std::vector<std::pair<std::string_view,
                        sourcemeta::core::URITemplateRouter::ArgumentValue>>
      collected;
  restored.arguments(
      1, [&](const std::string_view name,
             const sourcemeta::core::URITemplateRouter::ArgumentValue &value) {
        collected.emplace_back(name, value);
      });
  EXPECT_EQ(collected.size(), 2);
  EXPECT_EQ(collected[0].first, "max");
  EXPECT_EQ(std::get<std::int64_t>(collected[0].second), 99);
  EXPECT_EQ(collected[1].first, "name");
  EXPECT_EQ(std::get<std::string_view>(collected[1].second), "hello");
}

TEST_F(URITemplateRouterViewTest, optional_expansion_with_base_path) {
  {
    sourcemeta::core::URITemplateRouter router{"/v1"};
    router.add("/api/list{/path*}", "op_v_oex_bp", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.base_path(), "/v1");
  EXPECT_ROUTER_MATCH(restored, "/v1/api/list", 1, 0, captures_empty);
  EXPECT_EQ(captures_empty.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_empty, 0, "path", "");
  EXPECT_ROUTER_MATCH(restored, "/v1/api/list/foo/bar", 1, 0, captures_two);
  EXPECT_EQ(captures_two.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_two, 0, "path", "foo/bar");
}

TEST_F(URITemplateRouterViewTest, optional_expansion_otherwise_fallback) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "op_v_oex_o1", 1);
    router.otherwise(99);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/list/", 0, 99, captures_trailing);
  EXPECT_ROUTER_MATCH(restored, "/api/listing", 0, 99, captures_other);
}

TEST_F(URITemplateRouterViewTest, optional_expansion_root_template) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/{/path*}", "op_v_oex_root", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/foo", 1, 0, captures_one);
  EXPECT_EQ(captures_one.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_one, 0, "path", "foo");
  EXPECT_ROUTER_MATCH(restored, "/foo/bar", 1, 0, captures_two);
  EXPECT_EQ(captures_two.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_two, 0, "path", "foo/bar");
  EXPECT_ROUTER_MATCH(restored, "/", 0, 0, captures_slash);
}

TEST_F(URITemplateRouterViewTest, optional_expansion_under_variable_segment) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/{tenant}{/path*}", "op_v_oex_uv", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/acme", 1, 0, captures_empty);
  EXPECT_EQ(captures_empty.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures_empty, 0, "tenant", "acme");
  EXPECT_ROUTER_CAPTURE(captures_empty, 1, "path", "");
  EXPECT_ROUTER_MATCH(restored, "/api/acme/foo/bar", 1, 0, captures_payload);
  EXPECT_EQ(captures_payload.size(), 2);
  EXPECT_ROUTER_CAPTURE(captures_payload, 0, "tenant", "acme");
  EXPECT_ROUTER_CAPTURE(captures_payload, 1, "path", "foo/bar");
}

TEST_F(URITemplateRouterViewTest, optional_expansion_motivating_use_case) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/self/v1/api/list{/path*}", "list_directory", 1, 7);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/self/v1/api/list", 1, 7, captures_root);
  EXPECT_EQ(captures_root.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_root, 0, "path", "");
  EXPECT_ROUTER_MATCH(restored, "/self/v1/api/list/foo", 1, 7, captures_one);
  EXPECT_EQ(captures_one.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_one, 0, "path", "foo");
  EXPECT_ROUTER_MATCH(restored, "/self/v1/api/list/foo/bar/baz/qux", 1, 7,
                      captures_deep);
  EXPECT_EQ(captures_deep.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_deep, 0, "path", "foo/bar/baz/qux");
  EXPECT_ROUTER_MATCH(restored, "/self/v1/api/list/", 0, 0, captures_trailing);
  EXPECT_ROUTER_MATCH(restored, "/self/v1/api/listing", 0, 0, captures_nope);
}

TEST_F(URITemplateRouterViewTest,
       optional_expansion_coexists_with_mandatory_expansion_sibling) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/a{/x*}", "op_v_coex_a", 1);
    router.add("/api/b/{+y}", "op_v_coex_b", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/a", 1, 0, captures_a_empty);
  EXPECT_EQ(captures_a_empty.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_a_empty, 0, "x", "");
  EXPECT_ROUTER_MATCH(restored, "/api/a/foo/bar", 1, 0, captures_a_payload);
  EXPECT_EQ(captures_a_payload.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_a_payload, 0, "x", "foo/bar");
  EXPECT_ROUTER_MATCH(restored, "/api/b/baz", 2, 0, captures_b);
  EXPECT_EQ(captures_b.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_b, 0, "y", "baz");
  EXPECT_ROUTER_MATCH(restored, "/api/b", 0, 0, captures_b_empty);
}

TEST_F(URITemplateRouterViewTest, optional_expansion_path_method_round_trips) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/api/list{/path*}", "op_v_oex_pth", 1);
  EXPECT_EQ(router.path(1), "/api/list{/path*}");
}

TEST_F(URITemplateRouterViewTest, optional_expansion_dotted_varname) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/foo.bar*}", "op_v_oex_dot", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/list/x/y", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "foo.bar", "x/y");
}

TEST_F(URITemplateRouterViewTest, optional_expansion_many_routes) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/a{/x*}", "op_v_many_a", 1);
    router.add("/b{/y*}", "op_v_many_b", 2);
    router.add("/c{/z*}", "op_v_many_c", 3);
    router.add("/d{/w*}", "op_v_many_d", 4);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 4);
  EXPECT_ROUTER_MATCH(restored, "/a", 1, 0, captures_a);
  EXPECT_ROUTER_CAPTURE(captures_a, 0, "x", "");
  EXPECT_ROUTER_MATCH(restored, "/b/foo", 2, 0, captures_b);
  EXPECT_ROUTER_CAPTURE(captures_b, 0, "y", "foo");
  EXPECT_ROUTER_MATCH(restored, "/c/foo/bar", 3, 0, captures_c);
  EXPECT_ROUTER_CAPTURE(captures_c, 0, "z", "foo/bar");
  EXPECT_ROUTER_MATCH(restored, "/d", 4, 0, captures_d);
  EXPECT_ROUTER_CAPTURE(captures_d, 0, "w", "");
}

TEST_F(URITemplateRouterViewTest, variable_does_not_consume_multiple_segments) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}", "v_var_no_consume_1", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/42/extra", 0, 0, captures);
}

TEST_F(URITemplateRouterViewTest,
       expansion_types_distinct_from_plain_variable) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/a/{x}", "v_plain_var", 1);
    router.add("/b/{+y}", "v_reserved_exp", 2);
    router.add("/c{/z*}", "v_optional_exp", 3);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/a/one/two", 0, 0, captures_a);
  EXPECT_ROUTER_MATCH(restored, "/b/one/two", 2, 0, captures_b);
  EXPECT_EQ(captures_b.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_b, 0, "y", "one/two");
  EXPECT_ROUTER_MATCH(restored, "/c/one/two", 3, 0, captures_c);
  EXPECT_EQ(captures_c.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures_c, 0, "z", "one/two");
}

TEST_F(URITemplateRouterViewTest, listing_at_returns_identifiers_in_add_order) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "list_users", 7);
    router.add("/posts/{id}", "show_post", 3);
    router.add("/{+rest}", "fetch_all", 9);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 3);
  EXPECT_EQ(restored.at(0), 7);
  EXPECT_EQ(restored.at(1), 3);
  EXPECT_EQ(restored.at(2), 9);
}

TEST_F(URITemplateRouterViewTest, listing_context_returns_associated_context) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "list_users", 1, 100);
    router.add("/posts/{id}", "show_post", 2, 200);
    router.add("/comments", "list_comments", 3, 300);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.context(1), 100);
  EXPECT_EQ(restored.context(2), 200);
  EXPECT_EQ(restored.context(3), 300);
}

TEST_F(URITemplateRouterViewTest, listing_context_default_zero) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "list_users", 1);
    router.add("/posts", "list_posts", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.context(1), 0);
  EXPECT_EQ(restored.context(2), 0);
}

TEST_F(URITemplateRouterViewTest, listing_path_for_literal_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "list_users", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.path(1), "/users");
}

TEST_F(URITemplateRouterViewTest, listing_path_for_variable_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}", "show_user", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.path(1), "/users/{id}");
}

TEST_F(URITemplateRouterViewTest, listing_path_for_multi_variable_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}/posts/{post_id}", "show_post", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.path(1), "/users/{id}/posts/{post_id}");
}

TEST_F(URITemplateRouterViewTest, listing_path_for_expansion_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+rest}", "fetch_files", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.path(1), "/files/{+rest}");
}

TEST_F(URITemplateRouterViewTest, listing_path_for_optional_expansion_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/api/list{/path*}", "list_directory", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.path(1), "/api/list{/path*}");
}

TEST_F(URITemplateRouterViewTest, listing_path_for_root_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/", "root_route", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.path(1), "/");
}

TEST_F(URITemplateRouterViewTest,
       listing_path_for_multiple_routes_each_correct) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "list_users", 1);
    router.add("/users/{id}", "show_user", 2);
    router.add("/posts/{id}/comments/{comment_id}", "show_comment", 3);
    router.add("/files/{+rest}", "fetch_files", 4);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.path(1), "/users");
  EXPECT_EQ(restored.path(2), "/users/{id}");
  EXPECT_EQ(restored.path(3), "/posts/{id}/comments/{comment_id}");
  EXPECT_EQ(restored.path(4), "/files/{+rest}");
}

TEST_F(URITemplateRouterViewTest, listing_operation_id_round_trip) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "list_users", 1);
    router.add("/posts/{id}", "show_post", 2);
    router.add("/files/{+rest}", "fetch_files", 3);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.operation_id(1), "list_users");
  EXPECT_EQ(restored.operation_id(2), "show_post");
  EXPECT_EQ(restored.operation_id(3), "fetch_files");
}

TEST_F(URITemplateRouterViewTest, listing_operation_id_zero_returns_empty) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "list_users", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.operation_id(0).empty());
}

TEST_F(URITemplateRouterViewTest,
       listing_operation_id_unregistered_returns_empty) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "list_users", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.operation_id(99).empty());
}

TEST_F(URITemplateRouterViewTest, listing_operation_id_inverse_of_operation) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/a", "alpha", 1, 11);
    router.add("/b", "beta", 2, 22);
    router.add("/c", "gamma", 3, 33);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};

  EXPECT_EQ(restored.operation_id(1), "alpha");
  EXPECT_EQ(restored.operation_id(2), "beta");
  EXPECT_EQ(restored.operation_id(3), "gamma");

  const auto resolved_alpha = restored.operation(restored.operation_id(1));
  EXPECT_EQ(resolved_alpha.first, 1);
  EXPECT_EQ(resolved_alpha.second, 11);

  const auto resolved_beta = restored.operation(restored.operation_id(2));
  EXPECT_EQ(resolved_beta.first, 2);
  EXPECT_EQ(resolved_beta.second, 22);

  const auto resolved_gamma = restored.operation(restored.operation_id(3));
  EXPECT_EQ(resolved_gamma.first, 3);
  EXPECT_EQ(resolved_gamma.second, 33);
}

TEST_F(URITemplateRouterViewTest, listing_matches_writable_router) {
  sourcemeta::core::URITemplateRouter router;
  router.add("/users", "list_users", 1, 10);
  router.add("/posts/{id}", "show_post", 2, 20);
  router.add("/files/{+rest}", "fetch_files", 3, 30);
  router.add("/api/list{/path*}", "list_directory", 4, 40);
  sourcemeta::core::URITemplateRouterView::save(router, this->path);

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), router.size());

  EXPECT_EQ(restored.at(0), router.at(0));
  EXPECT_EQ(restored.at(1), router.at(1));
  EXPECT_EQ(restored.at(2), router.at(2));
  EXPECT_EQ(restored.at(3), router.at(3));

  EXPECT_EQ(restored.context(1), router.context(1));
  EXPECT_EQ(restored.context(2), router.context(2));
  EXPECT_EQ(restored.context(3), router.context(3));
  EXPECT_EQ(restored.context(4), router.context(4));

  EXPECT_EQ(restored.path(1), router.path(1));
  EXPECT_EQ(restored.path(2), router.path(2));
  EXPECT_EQ(restored.path(3), router.path(3));
  EXPECT_EQ(restored.path(4), router.path(4));

  EXPECT_EQ(restored.operation_id(1), router.operation_id(1));
  EXPECT_EQ(restored.operation_id(2), router.operation_id(2));
  EXPECT_EQ(restored.operation_id(3), router.operation_id(3));
  EXPECT_EQ(restored.operation_id(4), router.operation_id(4));
}

TEST_F(URITemplateRouterViewTest, listing_empty_router_size_zero) {
  {
    sourcemeta::core::URITemplateRouter router;
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 0);
  EXPECT_TRUE(restored.operation_id(0).empty());
  EXPECT_TRUE(restored.operation_id(1).empty());
}

TEST_F(URITemplateRouterViewTest, listing_only_otherwise_size_zero) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.otherwise(99);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 0);
  EXPECT_TRUE(restored.operation_id(0).empty());
}

TEST_F(URITemplateRouterViewTest, listing_path_excludes_base_path) {
  {
    sourcemeta::core::URITemplateRouter router{"/api/v1"};
    router.add("/users/{id}", "show_user", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.path(1), "/users/{id}");
}

TEST_F(URITemplateRouterViewTest,
       trailing_slash_distinct_from_bare_match_bare) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo", "op_900", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest,
       trailing_slash_distinct_from_bare_no_match_slashed) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo", "op_901", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/foo/", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest,
       trailing_slash_only_registration_matches_slashed) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo/", "op_902", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/foo/", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest,
       trailing_slash_only_registration_no_match_bare) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo/", "op_903", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/foo", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest,
       trailing_slash_both_forms_registered_match_bare) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo", "op_904", 1);
    router.add("/foo/", "op_905", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/foo", 1, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest,
       trailing_slash_both_forms_registered_match_slashed) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo", "op_906", 1);
    router.add("/foo/", "op_907", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/foo/", 2, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, trailing_slash_after_variable_match_slashed) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}/", "op_908", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/42/", 1, 0, captures);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_ROUTER_CAPTURE(captures, 0, "id", "42");
}

TEST_F(URITemplateRouterViewTest, trailing_slash_after_variable_no_match_bare) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}/", "op_909", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/42", 0, 0, captures);
}

TEST_F(URITemplateRouterViewTest,
       trailing_slash_does_not_relax_internal_double_slash) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo/bar", "op_910", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/foo//bar", 0, 0, captures);
  EXPECT_EQ(captures.size(), 0);
}

TEST_F(URITemplateRouterViewTest, trailing_slash_with_base_path_both_forms) {
  {
    sourcemeta::core::URITemplateRouter router{"/api"};
    router.add("/foo", "op_911", 1);
    router.add("/foo/", "op_912", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/api/foo", 1, 0, captures_bare);
  EXPECT_EQ(captures_bare.size(), 0);
  EXPECT_ROUTER_MATCH(restored, "/api/foo/", 2, 0, captures_slashed);
  EXPECT_EQ(captures_slashed.size(), 0);
}

TEST_F(URITemplateRouterViewTest, trailing_slash_path_reconstruction) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo", "op_913", 1);
    router.add("/foo/", "op_914", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.path(1), "/foo");
  EXPECT_EQ(restored.path(2), "/foo/");
}

TEST_F(URITemplateRouterViewTest, trailing_slash_size_is_two) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo", "op_915", 1);
    router.add("/foo/", "op_916", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.size(), 2);
}

TEST_F(URITemplateRouterViewTest,
       strict_internal_double_slash_matches_only_itself) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo//bar", "op_950", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/foo//bar", 1, 0, captures_verbatim);
  EXPECT_EQ(captures_verbatim.size(), 0);
  EXPECT_ROUTER_MATCH(restored, "/foo/bar", 0, 0, captures_canonical);
}

TEST_F(URITemplateRouterViewTest,
       strict_only_slashes_template_matches_only_itself) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("////", "op_951", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "////", 1, 0, captures_match);
  EXPECT_EQ(captures_match.size(), 0);
  EXPECT_ROUTER_MATCH(restored, "///", 0, 0, captures_short);
  EXPECT_ROUTER_MATCH(restored, "/////", 0, 0, captures_long);
}

TEST_F(URITemplateRouterViewTest, strict_variable_does_not_bind_empty_segment) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/{id}", "op_952", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/users/", 0, 0, captures);
}

TEST_F(URITemplateRouterViewTest, strict_path_reconstruction_preserves_input) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo//bar", "op_954", 1);
    router.add("////", "op_955", 2);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_EQ(restored.path(1), "/foo//bar");
  EXPECT_EQ(restored.path(2), "////");
}

TEST_F(URITemplateRouterViewTest, strict_root_template_still_works) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/", "op_956", 1);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }
  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_ROUTER_MATCH(restored, "/", 1, 0, captures_match);
  EXPECT_EQ(captures_match.size(), 0);
  EXPECT_ROUTER_MATCH(restored, "//", 0, 0, captures_double);
}

TEST_F(URITemplateRouterViewTest, describes_worked_example_table) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/self/v1/api/schemas/health/{+schema}", "op_957", 1);
    router.add("/self/v1/api/{+any}", "op_958", 2);
    router.add("/self/v1/mcp", "op_959", 3);
    router.add("/self/v1/health", "op_960", 4);
    router.add("/self/v1/static/{+path}", "op_961", 5);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/self/v1/api"));
  EXPECT_TRUE(restored.describes("/self/v1/api/schemas/health"));
  EXPECT_TRUE(restored.describes("/self/v1/api/schemas/health/acme"));
  EXPECT_TRUE(restored.describes("/self/v1/mcp"));
  EXPECT_TRUE(restored.describes("/self/v1/static/css/app.css"));
  EXPECT_TRUE(restored.describes("/self/v1"));
  EXPECT_FALSE(restored.describes("/self/v1/mpc"));
  EXPECT_FALSE(restored.describes("/self/v1/healthz"));
  EXPECT_FALSE(restored.describes("/acme/foo"));
  EXPECT_TRUE(restored.describes("/"));
}

TEST_F(URITemplateRouterViewTest, describes_intermediate_prefixes) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/self/v1/mcp", "op_962", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/self"));
  EXPECT_TRUE(restored.describes("/self/v1"));
  EXPECT_TRUE(restored.describes("/self/v1/mcp"));
  EXPECT_FALSE(restored.describes("/self/v1/mcp/extra"));
  EXPECT_FALSE(restored.describes("/self/v2"));
  EXPECT_FALSE(restored.describes("/other"));
}

TEST_F(URITemplateRouterViewTest, describes_expansion_capture) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+path}", "op_963", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/files"));
  EXPECT_TRUE(restored.describes("/files/a"));
  EXPECT_TRUE(restored.describes("/files/a/b/c"));
  EXPECT_FALSE(restored.describes("/file"));
  EXPECT_FALSE(restored.describes("/filesx"));
}

TEST_F(URITemplateRouterViewTest, describes_optional_expansion_capture) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/docs/{/rest*}", "op_964", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/docs"));
  EXPECT_TRUE(restored.describes("/docs/a"));
  EXPECT_TRUE(restored.describes("/docs/a/b/c"));
  EXPECT_FALSE(restored.describes("/doc"));
  EXPECT_FALSE(restored.describes("/docsx"));
}

TEST_F(URITemplateRouterViewTest,
       describes_single_segment_variable_does_not_over_absorb) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/foo/{bar}/baz", "op_965", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/foo"));
  EXPECT_TRUE(restored.describes("/foo/anything"));
  EXPECT_TRUE(restored.describes("/foo/anything/baz"));
  EXPECT_FALSE(restored.describes("/foo/anything/qux"));
  EXPECT_FALSE(restored.describes("/foo/anything/baz/extra"));
}

TEST_F(URITemplateRouterViewTest, describes_whole_segment_discipline) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/internalish", "op_966", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/internalish"));
  EXPECT_FALSE(restored.describes("/internal"));
  EXPECT_FALSE(restored.describes("/internalisher"));
}

TEST_F(URITemplateRouterViewTest, describes_excludes_otherwise_fallback) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/known", "op_967", 1);
    router.otherwise(7);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/known"));
  EXPECT_FALSE(restored.describes("/unknown"));
  EXPECT_FALSE(restored.describes("/known/deeper"));
}

TEST_F(URITemplateRouterViewTest, describes_empty_router_describes_nothing) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.otherwise(3);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_FALSE(restored.describes("/"));
  EXPECT_FALSE(restored.describes(""));
  EXPECT_FALSE(restored.describes("/anything"));
}

TEST_F(URITemplateRouterViewTest, describes_root_and_empty_path) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_968", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/"));
  EXPECT_TRUE(restored.describes(""));
}

TEST_F(URITemplateRouterViewTest, describes_requires_leading_slash) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users", "op_969", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_FALSE(restored.describes("users"));
  EXPECT_FALSE(restored.describes("users/list"));
}

TEST_F(URITemplateRouterViewTest, describes_with_base_path) {
  {
    sourcemeta::core::URITemplateRouter router{"/api"};
    router.add("/foo", "op_970", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/api/foo"));
  EXPECT_TRUE(restored.describes("/api"));
  EXPECT_FALSE(restored.describes("/foo"));
  EXPECT_FALSE(restored.describes("/api/bar"));
}

TEST_F(URITemplateRouterViewTest, describes_literal_preferred_over_variable) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/users/me", "op_971", 1);
    router.add("/users/{id}/posts", "op_972", 2);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/users/me"));
  EXPECT_TRUE(restored.describes("/users/42"));
  EXPECT_TRUE(restored.describes("/users/42/posts"));
  EXPECT_FALSE(restored.describes("/users/42/comments"));
  EXPECT_FALSE(restored.describes("/users/me/posts"));
}

TEST_F(URITemplateRouterViewTest, describes_empty_template_root_route) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("", "op_973", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes(""));
  EXPECT_TRUE(restored.describes("/"));
  EXPECT_FALSE(restored.describes("/foo"));
}

TEST_F(URITemplateRouterViewTest, describes_on_corrupt_buffer) {
  std::array<std::uint8_t, 4> buffer{};
  const sourcemeta::core::URITemplateRouterView view{buffer.data(),
                                                     buffer.size()};
  EXPECT_FALSE(view.describes("/anything"));
  EXPECT_FALSE(view.describes("/"));
}

TEST_F(URITemplateRouterViewTest,
       describes_with_base_argument_equivalent_to_concat) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/self/v1/api/{+any}", "op_974", 1);
    router.add("/self/v1/mcp", "op_975", 2);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/api", "/self/v1"));
  EXPECT_TRUE(restored.describes("/self/v1/api"));
  EXPECT_TRUE(restored.describes("/api/foo", "/self/v1"));
  EXPECT_TRUE(restored.describes("/mcp", "/self/v1"));
  EXPECT_FALSE(restored.describes("/mpc", "/self/v1"));
  EXPECT_FALSE(restored.describes("/api", "/self/v2"));
}

TEST_F(URITemplateRouterViewTest,
       describes_with_base_argument_prefix_and_capture) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+path}", "op_976", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/files", ""));
  EXPECT_TRUE(restored.describes("/files/a/b", ""));
  EXPECT_TRUE(restored.describes("", "/files"));
  EXPECT_TRUE(restored.describes("/", "/files"));
  EXPECT_TRUE(restored.describes("/a/b", "/files"));
  EXPECT_FALSE(restored.describes("/a/b", "/file"));
}

TEST_F(URITemplateRouterViewTest,
       describes_with_base_argument_captured_in_base) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/files/{+path}", "op_977", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/anything", "/files/already"));
}

TEST_F(URITemplateRouterViewTest, describes_with_base_argument_base_mismatch) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.add("/self/v1/mcp", "op_978", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_FALSE(restored.describes("/mcp", "/other"));
  EXPECT_FALSE(restored.describes("/mcp", "/self/v2"));
}

TEST_F(URITemplateRouterViewTest,
       describes_with_router_base_path_split_argument) {
  {
    sourcemeta::core::URITemplateRouter router{"/prefix"};
    router.add("/foo", "op_979", 1);
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_TRUE(restored.describes("/foo", "/prefix"));
  EXPECT_TRUE(restored.describes("/prefix/foo"));
  EXPECT_TRUE(restored.describes("", "/prefix"));
  EXPECT_FALSE(restored.describes("/bar", "/prefix"));
}

TEST_F(URITemplateRouterViewTest, describes_with_base_argument_empty_router) {
  {
    sourcemeta::core::URITemplateRouter router;
    router.otherwise(0);
    sourcemeta::core::URITemplateRouterView::save(router, this->path);
  }

  const sourcemeta::core::URITemplateRouterView restored{this->path};
  EXPECT_FALSE(restored.describes("/foo", "/bar"));
  EXPECT_FALSE(restored.describes("", "/bar"));
}

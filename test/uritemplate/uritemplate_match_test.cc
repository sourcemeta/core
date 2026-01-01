#include <gtest/gtest.h>

#include <sourcemeta/core/uritemplate.h>

#include <string>
#include <utility>
#include <vector>

TEST(URITemplate_match, empty_template_empty_uri) {
  const sourcemeta::core::URITemplate uri_template{""};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, empty_template_non_empty_uri) {
  const sourcemeta::core::URITemplate uri_template{""};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/foo", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, empty_template_root_uri) {
  const sourcemeta::core::URITemplate uri_template{""};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, root_template_matches_root) {
  const sourcemeta::core::URITemplate uri_template{"/"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, root_template_no_match_empty) {
  const sourcemeta::core::URITemplate uri_template{"/"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, root_template_no_match_path) {
  const sourcemeta::core::URITemplate uri_template{"/"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/foo", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, literal_only_match) {
  const sourcemeta::core::URITemplate uri_template{"/users/list"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/users/list", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, literal_only_no_match) {
  const sourcemeta::core::URITemplate uri_template{"/users/list"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/users/other", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, literal_uri_too_short) {
  const sourcemeta::core::URITemplate uri_template{"/users/list"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/users", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, literal_uri_too_long) {
  const sourcemeta::core::URITemplate uri_template{"/users"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/users/list", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, single_variable) {
  const sourcemeta::core::URITemplate uri_template{"/users/{id}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/users/123", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_EQ(captures[0].first, "id");
  EXPECT_EQ(captures[0].second, "123");
}

TEST(URITemplate_match, single_variable_empty_capture) {
  const sourcemeta::core::URITemplate uri_template{"/users/{id}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/users/", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, variable_at_start) {
  const sourcemeta::core::URITemplate uri_template{"{proto}://example.com"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "https://example.com", ':',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_EQ(captures[0].first, "proto");
  EXPECT_EQ(captures[0].second, "https");
}

TEST(URITemplate_match, variable_at_end) {
  const sourcemeta::core::URITemplate uri_template{"/files/{filename}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/files/document.pdf", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_EQ(captures[0].first, "filename");
  EXPECT_EQ(captures[0].second, "document.pdf");
}

TEST(URITemplate_match, variable_only) {
  const sourcemeta::core::URITemplate uri_template{"{path}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "some-value", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_EQ(captures[0].first, "path");
  EXPECT_EQ(captures[0].second, "some-value");
}

TEST(URITemplate_match, variable_only_empty_uri) {
  const sourcemeta::core::URITemplate uri_template{"{path}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, multiple_variables) {
  const sourcemeta::core::URITemplate uri_template{
      "/users/{id}/posts/{post_id}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/users/42/posts/100", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 2);
  EXPECT_EQ(captures[0].first, "id");
  EXPECT_EQ(captures[0].second, "42");
  EXPECT_EQ(captures[1].first, "post_id");
  EXPECT_EQ(captures[1].second, "100");
}

TEST(URITemplate_match, three_variables) {
  const sourcemeta::core::URITemplate uri_template{"{a}:{b}:{c}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "one:two:three", ':',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 3);
  EXPECT_EQ(captures[0].first, "a");
  EXPECT_EQ(captures[0].second, "one");
  EXPECT_EQ(captures[1].first, "b");
  EXPECT_EQ(captures[1].second, "two");
  EXPECT_EQ(captures[2].first, "c");
  EXPECT_EQ(captures[2].second, "three");
}

TEST(URITemplate_match, reserved_expansion_only) {
  const sourcemeta::core::URITemplate uri_template{"{+path}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/foo/bar/baz", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_EQ(captures[0].first, "path");
  EXPECT_EQ(captures[0].second, "/foo/bar/baz");
}

TEST(URITemplate_match, reserved_expansion_at_end) {
  const sourcemeta::core::URITemplate uri_template{"/files/{+path}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/files/dir/subdir/file.txt", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_EQ(captures[0].first, "path");
  EXPECT_EQ(captures[0].second, "dir/subdir/file.txt");
}

TEST(URITemplate_match, reserved_expansion_empty_capture) {
  const sourcemeta::core::URITemplate uri_template{"/files/{+path}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/files/", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, variable_and_reserved) {
  const sourcemeta::core::URITemplate uri_template{
      "/api/{version}/files/{+path}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/api/v2/files/docs/readme.md", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 2);
  EXPECT_EQ(captures[0].first, "version");
  EXPECT_EQ(captures[0].second, "v2");
  EXPECT_EQ(captures[1].first, "path");
  EXPECT_EQ(captures[1].second, "docs/readme.md");
}

TEST(URITemplate_match, variable_captures_percent_encoded) {
  const sourcemeta::core::URITemplate uri_template{"/search/{query}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/search/hello%20world", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_EQ(captures[0].first, "query");
  EXPECT_EQ(captures[0].second, "hello%20world");
}

TEST(URITemplate_match, variable_single_char) {
  const sourcemeta::core::URITemplate uri_template{"/x/{v}/y"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/x/a/y", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_EQ(captures[0].first, "v");
  EXPECT_EQ(captures[0].second, "a");
}

TEST(URITemplate_match, literal_prefix_mismatch) {
  const sourcemeta::core::URITemplate uri_template{"/users/{id}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/posts/123", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, literal_suffix_mismatch) {
  const sourcemeta::core::URITemplate uri_template{"/users/{id}/posts"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/users/123/comments", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_EQ(captures[0].first, "id");
  EXPECT_EQ(captures[0].second, "123");
}

TEST(URITemplate_match, trailing_content_in_uri) {
  const sourcemeta::core::URITemplate uri_template{"/users/{id}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/users/123/extra", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 1);
  EXPECT_EQ(captures[0].first, "id");
  EXPECT_EQ(captures[0].second, "123");
}

TEST(URITemplate_match, literal_case_sensitive) {
  const sourcemeta::core::URITemplate uri_template{"/Users/{id}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/users/123", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

TEST(URITemplate_match, variable_with_special_delimiters) {
  const sourcemeta::core::URITemplate uri_template{"{year}-{month}-{day}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "2024-12-25", '-',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_TRUE(result);
  EXPECT_EQ(captures.size(), 3);
  EXPECT_EQ(captures[0].first, "year");
  EXPECT_EQ(captures[0].second, "2024");
  EXPECT_EQ(captures[1].first, "month");
  EXPECT_EQ(captures[1].second, "12");
  EXPECT_EQ(captures[2].first, "day");
  EXPECT_EQ(captures[2].second, "25");
}

TEST(URITemplate_match, callback_not_called_on_prefix_mismatch) {
  const sourcemeta::core::URITemplate uri_template{"/users/{id}"};

  std::vector<std::pair<std::string, std::string>> captures;
  const auto result = uri_template.match(
      "/posts/123", '/',
      [&captures](const std::string_view name, const std::string_view value) {
        captures.emplace_back(name, value);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(captures.size(), 0);
}

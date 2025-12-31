#include <gtest/gtest.h>

#include <sourcemeta/core/uritemplate.h>

#include <map>           // std::map
#include <optional>      // std::optional, std::nullopt
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <tuple>         // std::make_tuple
#include <unordered_map> // std::unordered_map

TEST(URITemplate_expand, empty_template) {
  const sourcemeta::core::URITemplate uri_template{""};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(URITemplate_expand, literal_only) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com/path/to/resource"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "http://example.com/path/to/resource");
}

TEST(URITemplate_expand, single_variable) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                 false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "value");
}

TEST(URITemplate_expand, literal_then_variable) {
  const sourcemeta::core::URITemplate uri_template{"http://example.com/{id}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "id") {
          return std::make_tuple(std::string_view{"123"}, std::nullopt, false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "http://example.com/123");
}

TEST(URITemplate_expand, variable_then_literal) {
  const sourcemeta::core::URITemplate uri_template{"{id}/resource"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "id") {
          return std::make_tuple(std::string_view{"456"}, std::nullopt, false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "456/resource");
}

TEST(URITemplate_expand, literal_variable_literal) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com/~{username}/"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "username") {
          return std::make_tuple(std::string_view{"john"}, std::nullopt, false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "http://example.com/~john/");
}

TEST(URITemplate_expand, multiple_variables) {
  const sourcemeta::core::URITemplate uri_template{
      "/users/{id}/posts/{postId}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "id") {
          return std::make_tuple(std::string_view{"42"}, std::nullopt, false);
        } else if (name == "postId") {
          return std::make_tuple(std::string_view{"99"}, std::nullopt, false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "/users/42/posts/99");
}

TEST(URITemplate_expand, adjacent_variables) {
  const sourcemeta::core::URITemplate uri_template{"{x}{y}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "x") {
          return std::make_tuple(std::string_view{"hello"}, std::nullopt,
                                 false);
        } else if (name == "y") {
          return std::make_tuple(std::string_view{"world"}, std::nullopt,
                                 false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "helloworld");
}

// See RFC 6570 Section 3.2.1: undefined variables expand to empty string
// https://www.rfc-editor.org/rfc/rfc6570#section-3.2.1
TEST(URITemplate_expand, undefined_variable) {
  const sourcemeta::core::URITemplate uri_template{"{undefined}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(URITemplate_expand, partial_variables_defined) {
  const sourcemeta::core::URITemplate uri_template{
      "/users/{id}/posts/{postId}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "id") {
          return std::make_tuple(std::string_view{"42"}, std::nullopt, false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "/users/42/posts/");
}

TEST(URITemplate_expand, undefined_between_literals) {
  const sourcemeta::core::URITemplate uri_template{"start/{undefined}/end"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "start//end");
}

TEST(URITemplate_expand, empty_value) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{""}, std::nullopt, false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "");
}

TEST(URITemplate_expand, percent_encode_space) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"hello world"}, std::nullopt,
                                 false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "hello%20world");
}

TEST(URITemplate_expand, percent_encode_slash) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"foo/bar"}, std::nullopt,
                                 false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "foo%2Fbar");
}

TEST(URITemplate_expand, percent_encode_reserved_chars) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{":/?#[]@!$&'()*+,;="},
                                 std::nullopt, false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "%3A%2F%3F%23%5B%5D%40%21%24%26%27%28%29%2A%2B%2C%3B%3D");
}

TEST(URITemplate_expand, unreserved_chars_not_encoded) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"azAZ09-._~"}, std::nullopt,
                                 false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "azAZ09-._~");
}

TEST(URITemplate_expand, percent_encode_unicode) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"café"}, std::nullopt, false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "caf%C3%A9");
}

TEST(URITemplate_expand, with_std_map) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com/{resource}/{id}"};
  const std::map<std::string, std::string> variables{{"resource", "users"},
                                                     {"id", "123"}};
  const auto result = uri_template.expand(variables);
  EXPECT_EQ(result, "http://example.com/users/123");
}

TEST(URITemplate_expand, with_std_unordered_map) {
  const sourcemeta::core::URITemplate uri_template{
      "/users/{id}/posts/{postId}"};
  const std::unordered_map<std::string, std::string> variables{
      {"id", "42"}, {"postId", "99"}};
  const auto result = uri_template.expand(variables);
  EXPECT_EQ(result, "/users/42/posts/99");
}

TEST(URITemplate_expand, with_std_map_partial) {
  const sourcemeta::core::URITemplate uri_template{
      "/users/{id}/posts/{postId}"};
  const std::map<std::string, std::string> variables{{"id", "42"}};
  const auto result = uri_template.expand(variables);
  EXPECT_EQ(result, "/users/42/posts/");
}

TEST(URITemplate_expand, with_std_map_empty) {
  const sourcemeta::core::URITemplate uri_template{"/users/{id}"};
  const std::map<std::string, std::string> variables{};
  const auto result = uri_template.expand(variables);
  EXPECT_EQ(result, "/users/");
}

TEST(URITemplate_expand, rfc6570_level1_var) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "value");
}

TEST(URITemplate_expand, rfc6570_level1_hello) {
  const sourcemeta::core::URITemplate uri_template{"{hello}"};
  const std::map<std::string, std::string> variables{{"hello", "Hello World!"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "Hello%20World%21");
}

TEST(URITemplate_expand, reserved_expansion_simple) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                 false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "value");
}

TEST(URITemplate_expand, reserved_expansion_preserves_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+path}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "path") {
          return std::make_tuple(std::string_view{"/foo/bar"}, std::nullopt,
                                 false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "/foo/bar");
}

TEST(URITemplate_expand, reserved_expansion_all_reserved_chars) {
  const sourcemeta::core::URITemplate uri_template{"{+reserved}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "reserved") {
          return std::make_tuple(std::string_view{":/?#[]@!$&'()*+,;="},
                                 std::nullopt, false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, ":/?#[]@!$&'()*+,;=");
}

TEST(URITemplate_expand, reserved_expansion_encodes_non_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"hello world"}, std::nullopt,
                                 false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "hello%20world");
}

// See RFC 6570 Section 3.2.1: undefined variables expand to empty string
// https://www.rfc-editor.org/rfc/rfc6570#section-3.2.1
TEST(URITemplate_expand, reserved_expansion_undefined) {
  const sourcemeta::core::URITemplate uri_template{"{+undefined}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(URITemplate_expand, fragment_expansion_simple) {
  const sourcemeta::core::URITemplate uri_template{"{#var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                 false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "#value");
}

TEST(URITemplate_expand, fragment_expansion_preserves_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{#path}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "path") {
          return std::make_tuple(std::string_view{"/foo/bar"}, std::nullopt,
                                 false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "#/foo/bar");
}

TEST(URITemplate_expand, fragment_expansion_encodes_non_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{#var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"hello world"}, std::nullopt,
                                 false);
        } else {
          return std::nullopt;
        }
      });

  EXPECT_EQ(result, "#hello%20world");
}

// See RFC 6570 Section 3.2.1: undefined variables expand to empty string
// https://www.rfc-editor.org/rfc/rfc6570#section-3.2.1
TEST(URITemplate_expand, fragment_expansion_undefined) {
  const sourcemeta::core::URITemplate uri_template{"{#undefined}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(URITemplate_expand, rfc6570_level2_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+path}/here"};
  const std::map<std::string, std::string> variables{{"path", "/foo/bar"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/foo/bar/here");
}

TEST(URITemplate_expand, rfc6570_level2_fragment) {
  const sourcemeta::core::URITemplate uri_template{"X{#var}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "X#value");
}

TEST(URITemplate_expand, variable_list_two) {
  const sourcemeta::core::URITemplate uri_template{"{x,y}"};
  const std::map<std::string, std::string> variables{{"x", "1024"},
                                                     {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "1024,768");
}

TEST(URITemplate_expand, variable_list_three) {
  const sourcemeta::core::URITemplate uri_template{"{x,hello,y}"};
  const std::map<std::string, std::string> variables{
      {"x", "1024"}, {"hello", "Hello World!"}, {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "1024,Hello%20World%21,768");
}

TEST(URITemplate_expand, variable_list_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"map?{x,y}"};
  const std::map<std::string, std::string> variables{{"x", "1024"},
                                                     {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "map?1024,768");
}

TEST(URITemplate_expand, variable_list_partial_undefined) {
  const sourcemeta::core::URITemplate uri_template{"{x,undefined,y}"};
  const std::map<std::string, std::string> variables{{"x", "1024"},
                                                     {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "1024,768");
}

TEST(URITemplate_expand, variable_list_all_undefined) {
  const sourcemeta::core::URITemplate uri_template{"{a,b,c}"};
  const std::map<std::string, std::string> variables{};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "");
}

TEST(URITemplate_expand, reserved_expansion_variable_list) {
  const sourcemeta::core::URITemplate uri_template{"{+x,hello,y}"};
  const std::map<std::string, std::string> variables{
      {"x", "1024"}, {"hello", "Hello World!"}, {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "1024,Hello%20World!,768");
}

TEST(URITemplate_expand, reserved_expansion_variable_list_with_path) {
  const sourcemeta::core::URITemplate uri_template{"{+path,x}/here"};
  const std::map<std::string, std::string> variables{{"path", "/foo/bar"},
                                                     {"x", "1024"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/foo/bar,1024/here");
}

TEST(URITemplate_expand, fragment_expansion_variable_list) {
  const sourcemeta::core::URITemplate uri_template{"{#x,hello,y}"};
  const std::map<std::string, std::string> variables{
      {"x", "1024"}, {"hello", "Hello World!"}, {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "#1024,Hello%20World!,768");
}

TEST(URITemplate_expand, fragment_expansion_variable_list_with_path) {
  const sourcemeta::core::URITemplate uri_template{"{#path,x}/here"};
  const std::map<std::string, std::string> variables{{"path", "/foo/bar"},
                                                     {"x", "1024"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "#/foo/bar,1024/here");
}

TEST(URITemplate_expand, label_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"X{.var}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "X.value");
}

TEST(URITemplate_expand, label_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"X{.x,y}"};
  const std::map<std::string, std::string> variables{{"x", "1024"},
                                                     {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "X.1024.768");
}

TEST(URITemplate_expand, path_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"{/var}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/value");
}

TEST(URITemplate_expand, path_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{/var,x}/here"};
  const std::map<std::string, std::string> variables{{"var", "value"},
                                                     {"x", "1024"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/value/1024/here");
}

TEST(URITemplate_expand, path_parameter_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{;x,y}"};
  const std::map<std::string, std::string> variables{{"x", "1024"},
                                                     {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, ";x=1024;y=768");
}

TEST(URITemplate_expand, path_parameter_expansion_with_empty) {
  const sourcemeta::core::URITemplate uri_template{"{;x,y,empty}"};
  const std::map<std::string, std::string> variables{
      {"x", "1024"}, {"y", "768"}, {"empty", ""}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, ";x=1024;y=768;empty");
}

TEST(URITemplate_expand, query_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{?x,y}"};
  const std::map<std::string, std::string> variables{{"x", "1024"},
                                                     {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "?x=1024&y=768");
}

TEST(URITemplate_expand, query_expansion_with_empty) {
  const sourcemeta::core::URITemplate uri_template{"{?x,y,empty}"};
  const std::map<std::string, std::string> variables{
      {"x", "1024"}, {"y", "768"}, {"empty", ""}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "?x=1024&y=768&empty=");
}

TEST(URITemplate_expand, query_continuation_expansion) {
  const sourcemeta::core::URITemplate uri_template{"?fixed=yes{&x}"};
  const std::map<std::string, std::string> variables{{"x", "1024"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "?fixed=yes&x=1024");
}

TEST(URITemplate_expand, query_continuation_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{&x,y,empty}"};
  const std::map<std::string, std::string> variables{
      {"x", "1024"}, {"y", "768"}, {"empty", ""}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "&x=1024&y=768&empty=");
}

TEST(URITemplate_expand, prefix_modifier_short) {
  const sourcemeta::core::URITemplate uri_template{"{var:3}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "val");
}

TEST(URITemplate_expand, prefix_modifier_exact) {
  const sourcemeta::core::URITemplate uri_template{"{var:5}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "value");
}

TEST(URITemplate_expand, prefix_modifier_longer_than_value) {
  const sourcemeta::core::URITemplate uri_template{"{var:30}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "value");
}

TEST(URITemplate_expand, reserved_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{+path:6}/here"};
  const std::map<std::string, std::string> variables{{"path", "/foo/bar"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/foo/b/here");
}

TEST(URITemplate_expand, fragment_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{#path:6}/here"};
  const std::map<std::string, std::string> variables{{"path", "/foo/bar"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "#/foo/b/here");
}

TEST(URITemplate_expand, label_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"X{.var:3}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "X.val");
}

TEST(URITemplate_expand, path_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{/var:1,var}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/v/value");
}

TEST(URITemplate_expand, path_param_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{;hello:5}"};
  const std::map<std::string, std::string> variables{{"hello", "Hello World!"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, ";hello=Hello");
}

TEST(URITemplate_expand, query_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{?var:3}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "?var=val");
}

TEST(URITemplate_expand, query_continuation_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{&var:3}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "&var=val");
}

TEST(URITemplate_expand, list_no_explode) {
  const sourcemeta::core::URITemplate uri_template{"{list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"red"}, std::nullopt, true);
          } else if (call_count == 2) {
            return std::make_tuple(std::string_view{"green"}, std::nullopt,
                                   true);
          } else {
            return std::make_tuple(std::string_view{"blue"}, std::nullopt,
                                   false);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "red,green,blue");
}

TEST(URITemplate_expand, list_explode_path) {
  const sourcemeta::core::URITemplate uri_template{"{/list*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"red"}, std::nullopt, true);
          } else if (call_count == 2) {
            return std::make_tuple(std::string_view{"green"}, std::nullopt,
                                   true);
          } else {
            return std::make_tuple(std::string_view{"blue"}, std::nullopt,
                                   false);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "/red/green/blue");
}

TEST(URITemplate_expand, list_explode_query) {
  const sourcemeta::core::URITemplate uri_template{"{?list*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"red"}, std::nullopt, true);
          } else if (call_count == 2) {
            return std::make_tuple(std::string_view{"green"}, std::nullopt,
                                   true);
          } else {
            return std::make_tuple(std::string_view{"blue"}, std::nullopt,
                                   false);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "?list=red&list=green&list=blue");
}

TEST(URITemplate_expand, list_explode_label) {
  const sourcemeta::core::URITemplate uri_template{"{.list*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"red"}, std::nullopt, true);
          } else if (call_count == 2) {
            return std::make_tuple(std::string_view{"green"}, std::nullopt,
                                   true);
          } else {
            return std::make_tuple(std::string_view{"blue"}, std::nullopt,
                                   false);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ".red.green.blue");
}

TEST(URITemplate_expand, object_no_explode) {
  const sourcemeta::core::URITemplate uri_template{"{keys}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"1"},
                                   std::string_view{"one"}, true);
          } else {
            return std::make_tuple(std::string_view{"2"},
                                   std::string_view{"two"}, false);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "one,1,two,2");
}

TEST(URITemplate_expand, object_explode_query) {
  const sourcemeta::core::URITemplate uri_template{"{?keys*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"1"},
                                   std::string_view{"one"}, true);
          } else {
            return std::make_tuple(std::string_view{"2"},
                                   std::string_view{"two"}, false);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "?one=1&two=2");
}

TEST(URITemplate_expand, object_explode_path_param) {
  const sourcemeta::core::URITemplate uri_template{"{;keys*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{";"},
                                   std::string_view{"semi"}, true);
          } else {
            return std::make_tuple(std::string_view{"."},
                                   std::string_view{"dot"}, false);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ";semi=%3B;dot=.");
}

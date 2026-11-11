#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uritemplate.h>

#include <map>           // std::map
#include <optional>      // std::optional, std::nullopt
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <tuple>         // std::make_tuple
#include <unordered_map> // std::unordered_map

TEST(empty_template) {
  const sourcemeta::core::URITemplate uri_template{""};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(literal_only) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com/path/to/resource"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "http://example.com/path/to/resource");
}

TEST(single_variable) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "value");
}

TEST(literal_then_variable) {
  const sourcemeta::core::URITemplate uri_template{"http://example.com/{id}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "id") {
          return std::make_tuple(std::string_view{"123"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "http://example.com/123");
}

TEST(variable_then_literal) {
  const sourcemeta::core::URITemplate uri_template{"{id}/resource"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "id") {
          return std::make_tuple(std::string_view{"456"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "456/resource");
}

TEST(literal_variable_literal) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com/~{username}/"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "username") {
          return std::make_tuple(std::string_view{"john"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "http://example.com/~john/");
}

TEST(multiple_variables) {
  const sourcemeta::core::URITemplate uri_template{
      "/users/{id}/posts/{postId}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "id") {
          return std::make_tuple(std::string_view{"42"}, std::nullopt, false);
        }
        if (name == "postId") {
          return std::make_tuple(std::string_view{"99"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "/users/42/posts/99");
}

TEST(adjacent_variables) {
  const sourcemeta::core::URITemplate uri_template{"{x}{y}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "x") {
          return std::make_tuple(std::string_view{"hello"}, std::nullopt,
                                 false);
        }
        if (name == "y") {
          return std::make_tuple(std::string_view{"world"}, std::nullopt,
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "helloworld");
}

// See RFC 6570 Section 3.2.1: undefined variables expand to empty string
// https://www.rfc-editor.org/rfc/rfc6570#section-3.2.1
TEST(undefined_variable) {
  const sourcemeta::core::URITemplate uri_template{"{undefined}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(partial_variables_defined) {
  const sourcemeta::core::URITemplate uri_template{
      "/users/{id}/posts/{postId}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "id") {
          return std::make_tuple(std::string_view{"42"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "/users/42/posts/");
}

TEST(undefined_between_literals) {
  const sourcemeta::core::URITemplate uri_template{"start/{undefined}/end"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "start//end");
}

TEST(empty_value) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{""}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "");
}

TEST(percent_encode_space) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"hello world"}, std::nullopt,
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "hello%20world");
}

TEST(percent_encode_slash) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"foo/bar"}, std::nullopt,
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "foo%2Fbar");
}

TEST(percent_encode_reserved_chars) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{":/?#[]@!$&'()*+,;="},
                                 std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "%3A%2F%3F%23%5B%5D%40%21%24%26%27%28%29%2A%2B%2C%3B%3D");
}

TEST(unreserved_chars_not_encoded) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"azAZ09-._~"}, std::nullopt,
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "azAZ09-._~");
}

TEST(percent_encode_unicode) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"café"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "caf%C3%A9");
}

TEST(with_std_map) {
  const sourcemeta::core::URITemplate uri_template{
      "http://example.com/{resource}/{id}"};
  const std::map<std::string, std::string> variables{{"resource", "users"},
                                                     {"id", "123"}};
  const auto result = uri_template.expand(variables);
  EXPECT_EQ(result, "http://example.com/users/123");
}

TEST(with_std_unordered_map) {
  const sourcemeta::core::URITemplate uri_template{
      "/users/{id}/posts/{postId}"};
  const std::unordered_map<std::string, std::string> variables{
      {"id", "42"}, {"postId", "99"}};
  const auto result = uri_template.expand(variables);
  EXPECT_EQ(result, "/users/42/posts/99");
}

TEST(with_std_map_partial) {
  const sourcemeta::core::URITemplate uri_template{
      "/users/{id}/posts/{postId}"};
  const std::map<std::string, std::string> variables{{"id", "42"}};
  const auto result = uri_template.expand(variables);
  EXPECT_EQ(result, "/users/42/posts/");
}

TEST(with_std_map_empty) {
  const sourcemeta::core::URITemplate uri_template{"/users/{id}"};
  const std::map<std::string, std::string> variables{};
  const auto result = uri_template.expand(variables);
  EXPECT_EQ(result, "/users/");
}

TEST(rfc6570_level1_var) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "value");
}

TEST(rfc6570_level1_hello) {
  const sourcemeta::core::URITemplate uri_template{"{hello}"};
  const std::map<std::string, std::string> variables{{"hello", "Hello World!"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "Hello%20World%21");
}

TEST(reserved_expansion_simple) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "value");
}

TEST(reserved_expansion_preserves_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+path}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "path") {
          return std::make_tuple(std::string_view{"/foo/bar"}, std::nullopt,
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "/foo/bar");
}

TEST(reserved_expansion_all_reserved_chars) {
  const sourcemeta::core::URITemplate uri_template{"{+reserved}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "reserved") {
          return std::make_tuple(std::string_view{":/?#[]@!$&'()*+,;="},
                                 std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ":/?#[]@!$&'()*+,;=");
}

TEST(reserved_expansion_encodes_non_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"hello world"}, std::nullopt,
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "hello%20world");
}

// See RFC 6570 Section 3.2.1: undefined variables expand to empty string
// https://www.rfc-editor.org/rfc/rfc6570#section-3.2.1
TEST(reserved_expansion_undefined) {
  const sourcemeta::core::URITemplate uri_template{"{+undefined}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(fragment_expansion_simple) {
  const sourcemeta::core::URITemplate uri_template{"{#var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "#value");
}

TEST(fragment_expansion_preserves_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{#path}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "path") {
          return std::make_tuple(std::string_view{"/foo/bar"}, std::nullopt,
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "#/foo/bar");
}

TEST(fragment_expansion_encodes_non_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{#var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"hello world"}, std::nullopt,
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "#hello%20world");
}

// See RFC 6570 Section 3.2.1: undefined variables expand to empty string
// https://www.rfc-editor.org/rfc/rfc6570#section-3.2.1
TEST(fragment_expansion_undefined) {
  const sourcemeta::core::URITemplate uri_template{"{#undefined}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(rfc6570_level2_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+path}/here"};
  const std::map<std::string, std::string> variables{{"path", "/foo/bar"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/foo/bar/here");
}

TEST(rfc6570_level2_fragment) {
  const sourcemeta::core::URITemplate uri_template{"X{#var}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "X#value");
}

TEST(variable_list_two) {
  const sourcemeta::core::URITemplate uri_template{"{x,y}"};
  const std::map<std::string, std::string> variables{{"x", "1024"},
                                                     {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "1024,768");
}

TEST(variable_list_three) {
  const sourcemeta::core::URITemplate uri_template{"{x,hello,y}"};
  const std::map<std::string, std::string> variables{
      {"x", "1024"}, {"hello", "Hello World!"}, {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "1024,Hello%20World%21,768");
}

TEST(variable_list_with_literal) {
  const sourcemeta::core::URITemplate uri_template{"map?{x,y}"};
  const std::map<std::string, std::string> variables{{"x", "1024"},
                                                     {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "map?1024,768");
}

TEST(variable_list_partial_undefined) {
  const sourcemeta::core::URITemplate uri_template{"{x,undefined,y}"};
  const std::map<std::string, std::string> variables{{"x", "1024"},
                                                     {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "1024,768");
}

TEST(variable_list_all_undefined) {
  const sourcemeta::core::URITemplate uri_template{"{a,b,c}"};
  const std::map<std::string, std::string> variables{};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "");
}

TEST(reserved_expansion_variable_list) {
  const sourcemeta::core::URITemplate uri_template{"{+x,hello,y}"};
  const std::map<std::string, std::string> variables{
      {"x", "1024"}, {"hello", "Hello World!"}, {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "1024,Hello%20World!,768");
}

TEST(reserved_expansion_variable_list_with_path) {
  const sourcemeta::core::URITemplate uri_template{"{+path,x}/here"};
  const std::map<std::string, std::string> variables{{"path", "/foo/bar"},
                                                     {"x", "1024"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/foo/bar,1024/here");
}

TEST(fragment_expansion_variable_list) {
  const sourcemeta::core::URITemplate uri_template{"{#x,hello,y}"};
  const std::map<std::string, std::string> variables{
      {"x", "1024"}, {"hello", "Hello World!"}, {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "#1024,Hello%20World!,768");
}

TEST(fragment_expansion_variable_list_with_path) {
  const sourcemeta::core::URITemplate uri_template{"{#path,x}/here"};
  const std::map<std::string, std::string> variables{{"path", "/foo/bar"},
                                                     {"x", "1024"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "#/foo/bar,1024/here");
}

TEST(label_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"X{.var}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "X.value");
}

TEST(label_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"X{.x,y}"};
  const std::map<std::string, std::string> variables{{"x", "1024"},
                                                     {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "X.1024.768");
}

TEST(path_expansion_single) {
  const sourcemeta::core::URITemplate uri_template{"{/var}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/value");
}

TEST(path_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{/var,x}/here"};
  const std::map<std::string, std::string> variables{{"var", "value"},
                                                     {"x", "1024"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/value/1024/here");
}

TEST(path_parameter_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{;x,y}"};
  const std::map<std::string, std::string> variables{{"x", "1024"},
                                                     {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, ";x=1024;y=768");
}

TEST(path_parameter_expansion_with_empty) {
  const sourcemeta::core::URITemplate uri_template{"{;x,y,empty}"};
  const std::map<std::string, std::string> variables{
      {"x", "1024"}, {"y", "768"}, {"empty", ""}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, ";x=1024;y=768;empty");
}

TEST(query_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{?x,y}"};
  const std::map<std::string, std::string> variables{{"x", "1024"},
                                                     {"y", "768"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "?x=1024&y=768");
}

TEST(query_expansion_with_empty) {
  const sourcemeta::core::URITemplate uri_template{"{?x,y,empty}"};
  const std::map<std::string, std::string> variables{
      {"x", "1024"}, {"y", "768"}, {"empty", ""}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "?x=1024&y=768&empty=");
}

TEST(query_continuation_expansion) {
  const sourcemeta::core::URITemplate uri_template{"?fixed=yes{&x}"};
  const std::map<std::string, std::string> variables{{"x", "1024"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "?fixed=yes&x=1024");
}

TEST(query_continuation_expansion_multiple) {
  const sourcemeta::core::URITemplate uri_template{"{&x,y,empty}"};
  const std::map<std::string, std::string> variables{
      {"x", "1024"}, {"y", "768"}, {"empty", ""}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "&x=1024&y=768&empty=");
}

TEST(prefix_modifier_short) {
  const sourcemeta::core::URITemplate uri_template{"{var:3}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "val");
}

TEST(prefix_modifier_exact) {
  const sourcemeta::core::URITemplate uri_template{"{var:5}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "value");
}

TEST(prefix_modifier_longer_than_value) {
  const sourcemeta::core::URITemplate uri_template{"{var:30}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "value");
}

TEST(reserved_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{+path:6}/here"};
  const std::map<std::string, std::string> variables{{"path", "/foo/bar"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/foo/b/here");
}

TEST(fragment_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{#path:6}/here"};
  const std::map<std::string, std::string> variables{{"path", "/foo/bar"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "#/foo/b/here");
}

TEST(label_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"X{.var:3}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "X.val");
}

TEST(path_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{/var:1,var}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/v/value");
}

TEST(path_param_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{;hello:5}"};
  const std::map<std::string, std::string> variables{{"hello", "Hello World!"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, ";hello=Hello");
}

TEST(query_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{?var:3}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "?var=val");
}

TEST(query_continuation_with_prefix) {
  const sourcemeta::core::URITemplate uri_template{"{&var:3}"};
  const std::map<std::string, std::string> variables{{"var", "value"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "&var=val");
}

TEST(list_no_explode) {
  const sourcemeta::core::URITemplate uri_template{"{list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"red"}, std::nullopt, true);
          }
          if (call_count == 2) {
            return std::make_tuple(std::string_view{"green"}, std::nullopt,
                                   true);
          }
          return std::make_tuple(std::string_view{"blue"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "red,green,blue");
}

TEST(list_explode_path) {
  const sourcemeta::core::URITemplate uri_template{"{/list*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"red"}, std::nullopt, true);
          }
          if (call_count == 2) {
            return std::make_tuple(std::string_view{"green"}, std::nullopt,
                                   true);
          }
          return std::make_tuple(std::string_view{"blue"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "/red/green/blue");
}

TEST(list_explode_query) {
  const sourcemeta::core::URITemplate uri_template{"{?list*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"red"}, std::nullopt, true);
          }
          if (call_count == 2) {
            return std::make_tuple(std::string_view{"green"}, std::nullopt,
                                   true);
          }
          return std::make_tuple(std::string_view{"blue"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "?list=red&list=green&list=blue");
}

TEST(list_explode_label) {
  const sourcemeta::core::URITemplate uri_template{"{.list*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"red"}, std::nullopt, true);
          }
          if (call_count == 2) {
            return std::make_tuple(std::string_view{"green"}, std::nullopt,
                                   true);
          }
          return std::make_tuple(std::string_view{"blue"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ".red.green.blue");
}

TEST(object_no_explode) {
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
          }
          return std::make_tuple(std::string_view{"2"}, std::string_view{"two"},
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "one,1,two,2");
}

TEST(object_explode_query) {
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
          }
          return std::make_tuple(std::string_view{"2"}, std::string_view{"two"},
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "?one=1&two=2");
}

TEST(object_explode_path_param) {
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
          }
          return std::make_tuple(std::string_view{"."}, std::string_view{"dot"},
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ";semi=%3B;dot=.");
}

// RFC 6570 3.2.1 + Appendix A: for the ";" operator, an exploded
// associative-array pair with an empty value emits just the name (";a"), since
// "ifemp" is empty for ";". The "=" only applies to form-style (?/&).
TEST(object_explode_empty_value_semicolon) {
  const sourcemeta::core::URITemplate uri_template{"{;keys*}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          return std::make_tuple(std::string_view{""}, std::string_view{"a"},
                                 false);
        }
        return std::nullopt;
      });
  EXPECT_EQ(result, ";a");
}

TEST(object_explode_empty_value_query) {
  const sourcemeta::core::URITemplate uri_template{"{?keys*}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          return std::make_tuple(std::string_view{""}, std::string_view{"a"},
                                 false);
        }
        return std::nullopt;
      });
  EXPECT_EQ(result, "?a=");
}

TEST(object_no_explode_empty_value_semicolon) {
  const sourcemeta::core::URITemplate uri_template{"{;keys}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          return std::make_tuple(std::string_view{""}, std::string_view{"a"},
                                 false);
        }
        return std::nullopt;
      });
  EXPECT_EQ(result, ";keys=a,");
}

// RFC 6570 Section 2.4.1: "this numbering is in characters, not octets, in
// order to avoid splitting between the octets of a multi-octet-encoded
// character or within a pct-encoded triplet"
TEST(prefix_modifier_keeps_a_triplet_whole_under_reserved_expansion) {
  const sourcemeta::core::URITemplate uri_template{"{+var:1}"};
  const std::map<std::string, std::string> variables{{"var", "%41b"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "%41");
}

// The triplet counts as one character before the percent that opens it is
// itself encoded, so the prefix is taken over the value as it was given
TEST(prefix_modifier_keeps_a_triplet_whole_before_encoding_it) {
  const sourcemeta::core::URITemplate uri_template{"{var:1}"};
  const std::map<std::string, std::string> variables{{"var", "%41b"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "%2541");
}

// Section 2.4.1 counts a four octet character once, so a prefix of one
// character takes all four
TEST(prefix_modifier_keeps_a_four_octet_character_whole) {
  const sourcemeta::core::URITemplate uri_template{"{var:1}"};
  const std::map<std::string, std::string> variables{
      {"var", "\xF0\x9F\x98\x80X"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "%F0%9F%98%80");
}

// Every lead octet a well-formed encoding may carry is below %xF5, so the arm
// that takes one octet for a lead above the four octet range is only reachable
// with input that is not valid UTF-8 at all
TEST(prefix_modifier_takes_one_octet_for_a_lead_no_encoding_defines) {
  const sourcemeta::core::URITemplate uri_template{"{var:1}"};
  const std::map<std::string, std::string> variables{{"var", "\xF8"
                                                             "A"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "%F8");
}

// RFC 6570 Section 2.4.1 counts a prefix in characters rather than in bytes,
// which it does by stepping over a percent triplet or a UTF-8 sequence whole.
// A percent sign that begins neither is one character of its own, and so is
// encoded
TEST(prefix_modifier_over_a_trailing_percent_sign) {
  const sourcemeta::core::URITemplate uri_template{"{var:4}"};
  const std::map<std::string, std::string> variables{{"var", "ab%"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "ab%25");
}

TEST(prefix_modifier_over_a_percent_sign_with_no_hexadecimal) {
  const sourcemeta::core::URITemplate uri_template{"{var:4}"};
  const std::map<std::string, std::string> variables{{"var", "a%zzb"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "a%25zz");
}

TEST(prefix_modifier_over_a_three_byte_character) {
  const sourcemeta::core::URITemplate uri_template{"{var:2}"};
  const std::map<std::string, std::string> variables{
      {"var", "a\xE4\xB8\xAD b"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "a%E4%B8%AD");
}

// RFC 6570 Section 3.2.3 keeps an already encoded triplet as it stands under
// reserved expansion, which a percent sign that begins none of them is not
TEST(reserved_expansion_of_a_trailing_percent_sign) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const std::map<std::string, std::string> variables{{"var", "ab%"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "ab%25");
}

TEST(reserved_expansion_of_a_percent_sign_with_no_hexadecimal) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const std::map<std::string, std::string> variables{{"var", "a%zb"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "a%25zb");
}

// RFC 6570 Section 3.2.8 writes an exploded named expansion as a repetition of
// "name=value", so a member that is the empty string still contributes its
// name and the separator that would have introduced a value
TEST(list_explode_query_with_an_empty_member) {
  const sourcemeta::core::URITemplate uri_template{"{?list*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{""}, std::nullopt, true);
          }
          return std::make_tuple(std::string_view{"x"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "?list=&list=x");
}

// A caller answering that more values follow and then handing over none has
// nothing left to append, so the expansion stops where it was rather than
// writing a separator for a value that never arrives
TEST(list_explode_query_promising_more_than_it_gives) {
  const sourcemeta::core::URITemplate uri_template{"{?list*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"a"}, std::nullopt, true);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "?list=a");
}

// RFC 6570 Section 3.2.1 holds a zero-length string to be a defined value, so
// a list member that is one takes its place among the others and the name that
// precedes them still takes the "=" that separates it from them
TEST(list_no_explode_with_an_empty_first_member_query) {
  const sourcemeta::core::URITemplate uri_template{"{?list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{""}, std::nullopt, true);
          }
          return std::make_tuple(std::string_view{"x"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "?list=,x");
}

TEST(list_no_explode_with_an_empty_first_member_path_parameter) {
  const sourcemeta::core::URITemplate uri_template{"{;list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{""}, std::nullopt, true);
          }
          return std::make_tuple(std::string_view{"x"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ";list=,x");
}

TEST(list_no_explode_with_an_empty_first_member_simple) {
  const sourcemeta::core::URITemplate uri_template{"{list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{""}, std::nullopt, true);
          }
          return std::make_tuple(std::string_view{"x"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ",x");
}

// RFC 6570 Section 2.4.1: "Prefix modifiers are not applicable to variables
// that have composite values", and an associative array is one whether or not
// it holds more than a single pair
TEST(prefix_on_an_associative_array_of_one_pair) {
  const sourcemeta::core::URITemplate uri_template{"{var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"},
                                   std::string_view{"key"}, false);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

// RFC 6570 Section 3.2.1 writes an exploded pair as the name, "=" and the
// value, except that an empty value under an operator with no `ifemp` of its
// own leaves the name standing alone
TEST(associative_array_explode_with_an_empty_value_query) {
  const sourcemeta::core::URITemplate uri_template{"{?keys*}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          return std::make_tuple(std::string_view{""}, std::string_view{"a"},
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "?a=");
}

TEST(associative_array_explode_with_an_empty_value_path_parameter) {
  const sourcemeta::core::URITemplate uri_template{"{;keys*}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          return std::make_tuple(std::string_view{""}, std::string_view{"a"},
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ";a");
}

TEST(associative_array_explode_with_an_empty_value_simple) {
  const sourcemeta::core::URITemplate uri_template{"{keys*}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          return std::make_tuple(std::string_view{""}, std::string_view{"a"},
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "a");
}

TEST(list_explode_with_empty_members_query) {
  const sourcemeta::core::URITemplate uri_template{"{?list*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          return std::make_tuple(std::string_view{""}, std::nullopt,
                                 call_count == 1);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "?list=&list=");
}

TEST(list_explode_with_empty_members_path_parameter) {
  const sourcemeta::core::URITemplate uri_template{"{;list*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          return std::make_tuple(std::string_view{""}, std::nullopt,
                                 call_count == 1);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ";list;list");
}

// A caller that says more values follow and then names none leaves what was
// already written standing, as there is nothing further to join to it
TEST(list_whose_continuation_names_no_value_simple) {
  const sourcemeta::core::URITemplate uri_template{"{list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"a"}, std::nullopt, true);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "a");
}

TEST(list_whose_continuation_names_no_value_query) {
  const sourcemeta::core::URITemplate uri_template{"{?list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"a"}, std::nullopt, true);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "?list=a");
}

TEST(list_explode_whose_continuation_names_no_value_path) {
  const sourcemeta::core::URITemplate uri_template{"{/list*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"a"}, std::nullopt, true);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "/a");
}

// RFC 6570 Section 3.2.1: an undefined variable contributes nothing, which a
// container answers for by not holding the name at all
TEST(container_without_one_of_the_names) {
  const sourcemeta::core::URITemplate uri_template{"{a}{b}"};
  const std::map<std::string, std::string> variables{{"a", "1"}};
  EXPECT_EQ(uri_template.expand(variables), "1");
}

TEST(container_with_none_of_the_names) {
  const sourcemeta::core::URITemplate uri_template{"{a}"};
  const std::map<std::string, std::string> variables{{"z", "1"}};
  EXPECT_EQ(uri_template.expand(variables), "");
}

// RFC 6570 Section 2.4.1: "Prefix modifiers are not applicable to variables
// that have composite values", which holds for every expansion type

TEST(prefix_on_an_associative_array_simple) {
  const sourcemeta::core::URITemplate uri_template{"{var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"},
                                   std::string_view{"key"}, false);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_an_associative_array_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"},
                                   std::string_view{"key"}, false);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_an_associative_array_fragment) {
  const sourcemeta::core::URITemplate uri_template{"{#var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"},
                                   std::string_view{"key"}, false);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_an_associative_array_label) {
  const sourcemeta::core::URITemplate uri_template{"{.var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"},
                                   std::string_view{"key"}, false);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_an_associative_array_path) {
  const sourcemeta::core::URITemplate uri_template{"{/var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"},
                                   std::string_view{"key"}, false);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_an_associative_array_path_parameter) {
  const sourcemeta::core::URITemplate uri_template{"{;var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"},
                                   std::string_view{"key"}, false);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_an_associative_array_query) {
  const sourcemeta::core::URITemplate uri_template{"{?var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"},
                                   std::string_view{"key"}, false);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_an_associative_array_query_continuation) {
  const sourcemeta::core::URITemplate uri_template{"{&var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"},
                                   std::string_view{"key"}, false);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

// RFC 6570 Section 3.2.1 writes an exploded pair as the name, "=" and the
// value, except that an empty value under an operator that names no `ifemp`
// leaves the name standing alone

TEST(associative_array_explode_with_an_empty_value_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+keys*}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          return std::make_tuple(std::string_view{""}, std::string_view{"a"},
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "a");
}

TEST(associative_array_explode_with_an_empty_value_fragment) {
  const sourcemeta::core::URITemplate uri_template{"{#keys*}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          return std::make_tuple(std::string_view{""}, std::string_view{"a"},
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "#a");
}

TEST(associative_array_explode_with_an_empty_value_label) {
  const sourcemeta::core::URITemplate uri_template{"{.keys*}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          return std::make_tuple(std::string_view{""}, std::string_view{"a"},
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ".a");
}

TEST(associative_array_explode_with_an_empty_value_path) {
  const sourcemeta::core::URITemplate uri_template{"{/keys*}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          return std::make_tuple(std::string_view{""}, std::string_view{"a"},
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "/a");
}

TEST(associative_array_explode_with_an_empty_value_query_continuation) {
  const sourcemeta::core::URITemplate uri_template{"{&keys*}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "keys") {
          return std::make_tuple(std::string_view{""}, std::string_view{"a"},
                                 false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "&a=");
}

// RFC 6570 Section 3.2.1 holds a zero-length string to be a defined value, so
// a list member that is one takes its place among the others

TEST(list_no_explode_with_an_empty_first_member_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{""}, std::nullopt, true);
          }
          return std::make_tuple(std::string_view{"x"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ",x");
}

TEST(list_no_explode_with_an_empty_first_member_fragment) {
  const sourcemeta::core::URITemplate uri_template{"{#list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{""}, std::nullopt, true);
          }
          return std::make_tuple(std::string_view{"x"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "#,x");
}

TEST(list_no_explode_with_an_empty_first_member_label) {
  const sourcemeta::core::URITemplate uri_template{"{.list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{""}, std::nullopt, true);
          }
          return std::make_tuple(std::string_view{"x"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ".,x");
}

TEST(list_no_explode_with_an_empty_first_member_path) {
  const sourcemeta::core::URITemplate uri_template{"{/list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{""}, std::nullopt, true);
          }
          return std::make_tuple(std::string_view{"x"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "/,x");
}

TEST(list_no_explode_with_an_empty_first_member_query_continuation) {
  const sourcemeta::core::URITemplate uri_template{"{&list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{""}, std::nullopt, true);
          }
          return std::make_tuple(std::string_view{"x"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "&list=,x");
}

// A caller that says more values follow and then names none leaves what was
// already written standing

TEST(list_whose_continuation_names_no_value_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"a"}, std::nullopt, true);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "a");
}

TEST(list_whose_continuation_names_no_value_fragment) {
  const sourcemeta::core::URITemplate uri_template{"{#list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"a"}, std::nullopt, true);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "#a");
}

TEST(list_whose_continuation_names_no_value_label) {
  const sourcemeta::core::URITemplate uri_template{"{.list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"a"}, std::nullopt, true);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ".a");
}

TEST(list_whose_continuation_names_no_value_path) {
  const sourcemeta::core::URITemplate uri_template{"{/list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"a"}, std::nullopt, true);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "/a");
}

TEST(list_whose_continuation_names_no_value_path_parameter) {
  const sourcemeta::core::URITemplate uri_template{"{;list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"a"}, std::nullopt, true);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ";list=a");
}

TEST(list_whose_continuation_names_no_value_query_continuation) {
  const sourcemeta::core::URITemplate uri_template{"{&list}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "list") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"a"}, std::nullopt, true);
          }
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "&list=a");
}

// RFC 6570 Section 3.2.1: "if a variable is undefined, it is ignored", so an
// expression naming only undefined variables contributes nothing

TEST(undefined_variable_simple) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(undefined_variable_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(undefined_variable_fragment) {
  const sourcemeta::core::URITemplate uri_template{"{#var}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(undefined_variable_label) {
  const sourcemeta::core::URITemplate uri_template{"{.var}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(undefined_variable_path) {
  const sourcemeta::core::URITemplate uri_template{"{/var}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(undefined_variable_path_parameter) {
  const sourcemeta::core::URITemplate uri_template{"{;var}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(undefined_variable_query) {
  const sourcemeta::core::URITemplate uri_template{"{?var}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

TEST(undefined_variable_query_continuation) {
  const sourcemeta::core::URITemplate uri_template{"{&var}"};
  const auto result =
      uri_template.expand([](const std::string_view) { return std::nullopt; });

  EXPECT_EQ(result, "");
}

// RFC 6570 Section 2.4.1: "Prefix modifiers are not applicable to variables
// that have composite values", which a list is as much as an associative
// array is

TEST(prefix_on_a_list_simple) {
  const sourcemeta::core::URITemplate uri_template{"{var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                   true);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_a_list_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                   true);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_a_list_fragment) {
  const sourcemeta::core::URITemplate uri_template{"{#var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                   true);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_a_list_label) {
  const sourcemeta::core::URITemplate uri_template{"{.var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                   true);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_a_list_path) {
  const sourcemeta::core::URITemplate uri_template{"{/var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                   true);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_a_list_path_parameter) {
  const sourcemeta::core::URITemplate uri_template{"{;var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                   true);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_a_list_query) {
  const sourcemeta::core::URITemplate uri_template{"{?var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                   true);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

TEST(prefix_on_a_list_query_continuation) {
  const sourcemeta::core::URITemplate uri_template{"{&var:3}"};
  try {
    [[maybe_unused]] const auto result = uri_template.expand(
        [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
          if (name == "var") {
            return std::make_tuple(std::string_view{"value"}, std::nullopt,
                                   true);
          }
          return std::nullopt;
        });
    FAIL();
  } catch (const sourcemeta::core::URITemplateExpansionError &error) {
    EXPECT_STREQ(error.what(),
                 "Prefix modifier cannot be applied to composite values");
  } catch (...) {
    FAIL();
  }
}

// The prefix counts characters rather than bytes, and a percent sign that
// introduces no triplet is a character of its own rather than the first of
// three (RFC 6570 Section 2.4.1)
TEST(prefix_over_a_percent_sign_that_introduces_no_triplet) {
  const sourcemeta::core::URITemplate uri_template{"{var:2}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"a%4z"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "a%25");
}

TEST(prefix_over_a_percent_sign_with_one_hexadecimal_digit_left) {
  const sourcemeta::core::URITemplate uri_template{"{var:3}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{"%4"}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "%254");
}

// RFC 6570 Section 3.2.1: "if a variable is undefined, it is ignored", which a
// container answers for by not holding the name
TEST(unordered_map_without_the_name) {
  const sourcemeta::core::URITemplate uri_template{"{a}{b}"};
  const std::unordered_map<std::string, std::string> variables{{"a", "1"}};
  EXPECT_EQ(uri_template.expand(variables), "1");
}

// RFC 6570 Section 3.2.1 holds a zero-length string to be a defined value, so
// the expansion stands even though the value writes nothing, which is what the
// operator's own `ifemp` answers for

TEST(single_empty_value_simple) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{""}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "");
}

TEST(single_empty_value_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{""}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "");
}

TEST(single_empty_value_fragment) {
  const sourcemeta::core::URITemplate uri_template{"{#var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{""}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "#");
}

TEST(single_empty_value_label) {
  const sourcemeta::core::URITemplate uri_template{"{.var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{""}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ".");
}

TEST(single_empty_value_path) {
  const sourcemeta::core::URITemplate uri_template{"{/var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{""}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "/");
}

TEST(single_empty_value_path_parameter) {
  const sourcemeta::core::URITemplate uri_template{"{;var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{""}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, ";var");
}

TEST(single_empty_value_query) {
  const sourcemeta::core::URITemplate uri_template{"{?var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{""}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "?var=");
}

TEST(single_empty_value_query_continuation) {
  const sourcemeta::core::URITemplate uri_template{"{&var}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(std::string_view{""}, std::nullopt, false);
        }
        return std::nullopt;
      });

  EXPECT_EQ(result, "&var=");
}

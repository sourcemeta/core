#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <chrono> // std::chrono::seconds
#include <string> // std::string

TEST(simple_name_and_value) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "session", .value = "abc123"}));
}

TEST(empty_name_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "", .value = "abc"}));
}

TEST(name_with_space_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a b", .value = "abc"}));
}

TEST(name_with_semicolon_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a;b", .value = "abc"}));
}

TEST(name_with_equals_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a=b", .value = "abc"}));
}

TEST(name_with_comma_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a,b", .value = "abc"}));
}

TEST(name_with_dquote_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a\"b", .value = "abc"}));
}

TEST(name_with_parenthesis_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a(b", .value = "abc"}));
}

TEST(name_with_tab_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a\tb", .value = "abc"}));
}

TEST(name_with_all_token_specials_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "!#$%&'*+-.^_`|~", .value = "abc"}));
}

TEST(empty_value_is_valid) {
  EXPECT_TRUE(
      sourcemeta::core::http_cookie_valid({.name = "session", .value = ""}));
}

TEST(value_with_space_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = "a b"}));
}

TEST(value_with_semicolon_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = "a;b"}));
}

TEST(value_with_comma_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = "a,b"}));
}

TEST(value_with_dquote_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = "a\"b"}));
}

TEST(value_with_backslash_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = "a\\b"}));
}

TEST(value_with_newline_is_invalid) {
  const std::string value{"ab\ncd"};
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = value}));
}

TEST(value_with_carriage_return_is_invalid) {
  const std::string value{"ab\rcd"};
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = value}));
}

TEST(value_with_del_is_invalid) {
  const std::string value{"ab\x7f"
                          "cd"};
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = value}));
}

TEST(value_with_equals_is_valid) {
  EXPECT_TRUE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = "abc=def"}));
}

TEST(value_base64url_and_jwt_octets_are_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "sig", .value = "aGVsbG8-_x.YWJj:0="}));
}

TEST(value_allows_octets_that_are_not_token_characters) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "()/:<=>?@[]{}"}));
}

TEST(quoted_value_is_valid) {
  EXPECT_TRUE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = "\"abc\""}));
}

TEST(empty_quoted_value_is_valid) {
  EXPECT_TRUE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = "\"\""}));
}

TEST(lone_double_quote_value_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = "\""}));
}

TEST(value_with_only_leading_quote_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = "\"abc"}));
}

TEST(value_with_only_trailing_quote_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = "abc\""}));
}

TEST(quoted_value_with_inner_quote_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "a", .value = "\"a\"b\""}));
}

TEST(path_slash_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .path = "/"}));
}

TEST(path_with_space_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .path = "/a b"}));
}

TEST(path_with_semicolon_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .path = "/a;b"}));
}

TEST(path_with_carriage_return_is_invalid) {
  const std::string path{"/a\r\nb"};
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .path = path}));
}

TEST(path_with_del_is_invalid) {
  const std::string path{"/a\x7f"
                         "b"};
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .path = path}));
}

TEST(path_with_high_byte_is_invalid) {
  const std::string path{"/a\x80"
                         "b"};
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .path = path}));
}

TEST(empty_path_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .path = ""}));
}

TEST(path_with_space_and_punctuation_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .path = "/a b,c=d"}));
}

TEST(domain_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "example.com"}));
}

TEST(domain_with_subdomains_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "a.b.example.com"}));
}

TEST(domain_uppercase_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "EXAMPLE.COM"}));
}

TEST(domain_with_leading_dot_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = ".example.com"}));
}

TEST(domain_punycode_label_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "xn--mnchen-3ya.example.com"}));
}

TEST(domain_with_space_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "exa mple.com"}));
}

TEST(domain_with_underscore_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "exa_mple.com"}));
}

TEST(domain_with_trailing_dot_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "example.com."}));
}

TEST(domain_with_leading_hyphen_label_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "-bad.example.com"}));
}

TEST(domain_bare_dot_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "."}));
}

TEST(domain_empty_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = ""}));
}

TEST(domain_with_semicolon_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "example.com;evil"}));
}

TEST(domain_with_newline_is_invalid) {
  const std::string domain{"example.com\nSet-Cookie: x=y"};
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = domain}));
}

TEST(domain_with_high_byte_is_invalid) {
  const std::string domain{"exa\x80mple.com"};
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = domain}));
}

TEST(domain_single_label_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "localhost"}));
}

TEST(domain_numeric_labels_are_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "192.168.0.1"}));
}

TEST(domain_with_port_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "example.com:8080"}));
}

TEST(domain_with_empty_label_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .domain = "a..b"}));
}

TEST(zero_max_age_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .max_age = std::chrono::seconds{0}}));
}

TEST(positive_max_age_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .max_age = std::chrono::seconds{3600}}));
}

TEST(negative_max_age_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .max_age = std::chrono::seconds{-1}}));
}

TEST(large_max_age_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a", .value = "b", .max_age = std::chrono::seconds{999999999}}));
}

TEST(same_site_none_without_secure_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "a",
       .value = "b",
       .same_site = sourcemeta::core::HTTPCookieSameSite::None}));
}

TEST(same_site_none_with_secure_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a",
       .value = "b",
       .secure = true,
       .same_site = sourcemeta::core::HTTPCookieSameSite::None}));
}

TEST(same_site_lax_without_secure_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a",
       .value = "b",
       .same_site = sourcemeta::core::HTTPCookieSameSite::Lax}));
}

TEST(same_site_strict_without_secure_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "a",
       .value = "b",
       .same_site = sourcemeta::core::HTTPCookieSameSite::Strict}));
}

TEST(fully_populated_valid_cookie) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "sid",
       .value = "xyz",
       .path = "/",
       .domain = "example.com",
       .http_only = true,
       .secure = true,
       .same_site = sourcemeta::core::HTTPCookieSameSite::None}));
}

TEST(secure_prefix_without_secure_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "__Secure-session", .value = "b"}));
}

TEST(secure_prefix_with_secure_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "__Secure-session", .value = "b", .secure = true}));
}

TEST(secure_prefix_lowercase_without_secure_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "__secure-session", .value = "b"}));
}

TEST(secure_prefix_mixed_case_without_secure_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "__SeCuRe-session", .value = "b"}));
}

TEST(host_prefix_with_all_requirements_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "__Host-session", .value = "b", .path = "/", .secure = true}));
}

TEST(host_prefix_without_secure_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "__Host-session", .value = "b", .path = "/"}));
}

TEST(host_prefix_without_path_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "__Host-session", .value = "b", .secure = true}));
}

TEST(host_prefix_with_non_root_path_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid({.name = "__Host-session",
                                                    .value = "b",
                                                    .path = "/app",
                                                    .secure = true}));
}

TEST(host_prefix_with_domain_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid({.name = "__Host-session",
                                                    .value = "b",
                                                    .path = "/",
                                                    .domain = "example.com",
                                                    .secure = true}));
}

TEST(host_prefix_lowercase_without_requirements_is_invalid) {
  EXPECT_FALSE(sourcemeta::core::http_cookie_valid(
      {.name = "__host-session", .value = "b"}));
}

TEST(double_underscore_non_prefix_name_is_valid) {
  EXPECT_TRUE(
      sourcemeta::core::http_cookie_valid({.name = "__foo", .value = "b"}));
}

TEST(name_equal_to_secure_prefix_without_secure_is_invalid) {
  EXPECT_FALSE(
      sourcemeta::core::http_cookie_valid({.name = "__Secure-", .value = "b"}));
}

TEST(name_equal_to_secure_prefix_with_secure_is_valid) {
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(
      {.name = "__Secure-", .value = "b", .secure = true}));
}

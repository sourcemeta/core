#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <chrono> // std::chrono::seconds
#include <string> // std::string

TEST(name_and_value_only) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cookie(
                {.name = "session", .value = "abc123"}),
            "session=abc123");
}

TEST(empty_value) {
  EXPECT_EQ(
      sourcemeta::core::http_serialize_cookie({.name = "session", .value = ""}),
      "session=");
}

TEST(with_path) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cookie(
                {.name = "session", .value = "abc", .path = "/"}),
            "session=abc; Path=/");
}

TEST(with_domain) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cookie(
                {.name = "session", .value = "abc", .domain = "example.com"}),
            "session=abc; Domain=example.com");
}

TEST(with_max_age) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cookie(
                {.name = "session",
                 .value = "abc",
                 .max_age = std::chrono::seconds{3600}}),
            "session=abc; Max-Age=3600");
}

TEST(with_max_age_zero) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cookie(
                {.name = "session",
                 .value = "abc",
                 .max_age = std::chrono::seconds{0}}),
            "session=abc; Max-Age=0");
}

TEST(with_large_max_age) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cookie(
                {.name = "session",
                 .value = "abc",
                 .max_age = std::chrono::seconds{2147483648}}),
            "session=abc; Max-Age=2147483648");
}

TEST(quoted_value_round_trips) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cookie(
                {.name = "session", .value = "\"abc\""}),
            "session=\"abc\"");
}

TEST(negative_max_age_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_serialize_cookie(
                   {.name = "session",
                    .value = "abc",
                    .max_age = std::chrono::seconds{-1}})
                   .has_value());
}

TEST(with_secure) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cookie(
                {.name = "session", .value = "abc", .secure = true}),
            "session=abc; Secure");
}

TEST(with_http_only) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cookie(
                {.name = "session", .value = "abc", .http_only = true}),
            "session=abc; HttpOnly");
}

TEST(with_same_site_strict) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cookie(
                {.name = "session",
                 .value = "abc",
                 .same_site = sourcemeta::core::HTTPCookieSameSite::Strict}),
            "session=abc; SameSite=Strict");
}

TEST(with_same_site_lax) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cookie(
                {.name = "session",
                 .value = "abc",
                 .same_site = sourcemeta::core::HTTPCookieSameSite::Lax}),
            "session=abc; SameSite=Lax");
}

TEST(with_same_site_none) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cookie(
                {.name = "session",
                 .value = "abc",
                 .secure = true,
                 .same_site = sourcemeta::core::HTTPCookieSameSite::None}),
            "session=abc; Secure; SameSite=None");
}

TEST(all_attributes_in_order) {
  EXPECT_EQ(
      sourcemeta::core::http_serialize_cookie(
          {.name = "sid",
           .value = "xyz",
           .path = "/",
           .domain = "example.com",
           .max_age = std::chrono::seconds{60},
           .http_only = true,
           .secure = true,
           .same_site = sourcemeta::core::HTTPCookieSameSite::Lax}),
      "sid=xyz; Domain=example.com; Path=/; Max-Age=60; Secure; HttpOnly; "
      "SameSite=Lax");
}

TEST(append_form_preserves_existing_content) {
  std::string buffer{"Set-Cookie: "};
  const auto result{sourcemeta::core::http_serialize_cookie(
      {.name = "session", .value = "abc", .http_only = true}, buffer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(buffer, "Set-Cookie: session=abc; HttpOnly");
}

TEST(append_form_appends_twice) {
  std::string buffer;
  EXPECT_TRUE(sourcemeta::core::http_serialize_cookie(
      {.name = "a", .value = "1"}, buffer));
  EXPECT_TRUE(sourcemeta::core::http_serialize_cookie(
      {.name = "b", .value = "2"}, buffer));
  EXPECT_EQ(buffer, "a=1b=2");
}

TEST(returning_form_rejects_invalid_name) {
  EXPECT_FALSE(
      sourcemeta::core::http_serialize_cookie({.name = "a;b", .value = "abc"})
          .has_value());
}

TEST(returning_form_rejects_invalid_value) {
  EXPECT_FALSE(
      sourcemeta::core::http_serialize_cookie({.name = "a", .value = "a b"})
          .has_value());
}

TEST(returning_form_rejects_same_site_none_without_secure) {
  EXPECT_FALSE(sourcemeta::core::http_serialize_cookie(
                   {.name = "a",
                    .value = "b",
                    .same_site = sourcemeta::core::HTTPCookieSameSite::None})
                   .has_value());
}

TEST(append_form_rejects_invalid_and_leaves_buffer_unchanged) {
  std::string buffer{"Set-Cookie: "};
  const auto result{sourcemeta::core::http_serialize_cookie(
      {.name = "bad name", .value = "abc"}, buffer)};
  EXPECT_FALSE(result);
  EXPECT_EQ(buffer, "Set-Cookie: ");
}

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <chrono> // std::chrono::seconds

// RFC 6265bis §5.6.2: a max age of zero or less makes the expiry time "the
// earliest representable date and time", which removes the cookie
TEST(expire_cookie_clears_the_value_and_zeroes_the_max_age) {
  const auto cookie{sourcemeta::core::http_expire_cookie(
      {.name = "session", .value = "secret"})};
  EXPECT_EQ(cookie.name, "session");
  EXPECT_TRUE(cookie.value.empty());
  EXPECT_TRUE(cookie.max_age.has_value());
  EXPECT_EQ(cookie.max_age.value(), std::chrono::seconds{0});
}

TEST(expire_cookie_overrides_a_lifetime_already_set) {
  const auto cookie{sourcemeta::core::http_expire_cookie(
      {.name = "session",
       .value = "secret",
       .max_age = std::chrono::seconds{3600}})};
  EXPECT_EQ(cookie.max_age.value(), std::chrono::seconds{0});
}

// RFC 6265 §3.1: removal succeeds "only if the Path and the Domain attribute
// in the Set-Cookie header match the values used when the cookie was created"
TEST(expire_cookie_keeps_the_path_and_the_domain) {
  const auto cookie{
      sourcemeta::core::http_expire_cookie({.name = "session",
                                            .value = "secret",
                                            .path = "/admin",
                                            .domain = "example.com"})};
  EXPECT_TRUE(cookie.path.has_value());
  EXPECT_EQ(cookie.path.value(), "/admin");
  EXPECT_TRUE(cookie.domain.has_value());
  EXPECT_EQ(cookie.domain.value(), "example.com");
}

TEST(expire_cookie_keeps_the_remaining_attributes) {
  const auto cookie{sourcemeta::core::http_expire_cookie(
      {.name = "session",
       .value = "secret",
       .http_only = true,
       .secure = true,
       .same_site = sourcemeta::core::HTTPCookieSameSite::Strict})};
  EXPECT_TRUE(cookie.http_only);
  EXPECT_TRUE(cookie.secure);
  EXPECT_TRUE(cookie.same_site.has_value());
  EXPECT_TRUE(cookie.same_site.value() ==
              sourcemeta::core::HTTPCookieSameSite::Strict);
}

TEST(expire_cookie_serializes) {
  const auto value{sourcemeta::core::http_serialize_cookie(
      sourcemeta::core::http_expire_cookie({.name = "session",
                                            .value = "secret",
                                            .path = "/",
                                            .http_only = true,
                                            .secure = true}))};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "session=; Path=/; Max-Age=0; Secure; HttpOnly");
}

// RFC 6265bis §4.1.3.2: a cookie named with the host prefix must stay secure,
// scoped to the root path, and free of a domain, which preserving the
// attributes satisfies
TEST(expire_cookie_of_a_host_prefixed_cookie_stays_valid) {
  const auto cookie{
      sourcemeta::core::http_expire_cookie({.name = "__Host-session",
                                            .value = "secret",
                                            .path = "/",
                                            .secure = true})};
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(cookie));
}

// RFC 6265bis §5.7: a cookie whose same-site mode is none is ignored unless it
// is also secure, so the expiring twin has to carry both across
TEST(expire_cookie_of_a_cross_site_cookie_stays_valid) {
  const auto cookie{sourcemeta::core::http_expire_cookie(
      {.name = "session",
       .value = "secret",
       .secure = true,
       .same_site = sourcemeta::core::HTTPCookieSameSite::None})};
  EXPECT_TRUE(sourcemeta::core::http_cookie_valid(cookie));
}

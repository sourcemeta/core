#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::seconds
#include <string>      // std::string
#include <string_view> // std::string_view

// The spellings this replaces across a server, each built rather than written
TEST(cache_control_no_store) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control({.no_store = true}),
            "no-store");
}

TEST(cache_control_public_revalidated) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.visibility = sourcemeta::core::HTTPCacheVisibility::Public,
                 .max_age = std::chrono::seconds{0},
                 .must_revalidate = true}),
            "public, max-age=0, must-revalidate");
}

TEST(cache_control_public_short_lived) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.visibility = sourcemeta::core::HTTPCacheVisibility::Public,
                 .max_age = std::chrono::seconds{60}}),
            "public, max-age=60");
}

TEST(cache_control_public_immutable) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.visibility = sourcemeta::core::HTTPCacheVisibility::Public,
                 .max_age = std::chrono::seconds{31536000},
                 .immutable = true}),
            "public, max-age=31536000, immutable");
}

TEST(cache_control_private_short_lived) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.visibility = sourcemeta::core::HTTPCacheVisibility::Private,
                 .max_age = std::chrono::seconds{60}}),
            "private, max-age=60");
}

TEST(cache_control_private_immutable) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.visibility = sourcemeta::core::HTTPCacheVisibility::Private,
                 .max_age = std::chrono::seconds{31536000},
                 .immutable = true}),
            "private, max-age=31536000, immutable");
}

TEST(cache_control_private_revalidated) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.visibility = sourcemeta::core::HTTPCacheVisibility::Private,
                 .max_age = std::chrono::seconds{0},
                 .must_revalidate = true}),
            "private, max-age=0, must-revalidate");
}

// RFC 9111 §5.2.2.1: "This directive uses the token form of the argument
// syntax: e.g., 'max-age=5' not 'max-age="5"'. A sender MUST NOT generate the
// quoted-string form"
TEST(cache_control_max_age_is_never_quoted) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.max_age = std::chrono::seconds{5}}),
            "max-age=5");
}

// RFC 9111 §1.2.2: "The delta-seconds rule specifies a non-negative integer"
TEST(cache_control_refuses_a_negative_max_age) {
  EXPECT_FALSE(sourcemeta::core::http_serialize_cache_control(
                   {.max_age = std::chrono::seconds{-1}})
                   .has_value());
}

TEST(cache_control_refuses_a_negative_shared_max_age) {
  EXPECT_FALSE(sourcemeta::core::http_serialize_cache_control(
                   {.shared_max_age = std::chrono::seconds{-1}})
                   .has_value());
}

// RFC 9111 §5.2.2.10: s-maxage overrides max-age for a shared cache
TEST(cache_control_shared_max_age) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.visibility = sourcemeta::core::HTTPCacheVisibility::Public,
                 .max_age = std::chrono::seconds{60},
                 .shared_max_age = std::chrono::seconds{120}}),
            "public, max-age=60, s-maxage=120");
}

// RFC 9111 §5.2.2.4: the unqualified form
TEST(cache_control_no_cache) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control({.no_cache = true}),
            "no-cache");
}

// RFC 9111 §5.2.2.4: "This directive uses the quoted-string form of the
// argument syntax. A sender SHOULD NOT generate the token form (even if
// quoting appears not to be needed for single-entry lists)"
TEST(cache_control_qualified_no_cache_is_quoted) {
  const std::array<std::string_view, 1> fields{{"Set-Cookie"}};
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.no_cache_fields = fields}),
            "no-cache=\"Set-Cookie\"");
}

TEST(cache_control_qualified_no_cache_with_several_fields) {
  const std::array<std::string_view, 2> fields{{"Set-Cookie", "Authorization"}};
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.no_cache_fields = fields}),
            "no-cache=\"Set-Cookie, Authorization\"");
}

// RFC 9111 §5.2.2.7: the qualified private form carries the same requirement
TEST(cache_control_qualified_private_is_quoted) {
  const std::array<std::string_view, 1> fields{{"Set-Cookie"}};
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.visibility = sourcemeta::core::HTTPCacheVisibility::Private,
                 .private_fields = fields}),
            "private=\"Set-Cookie\"");
}

// A field list limiting nothing names no directive to qualify
TEST(cache_control_refuses_private_fields_without_private) {
  const std::array<std::string_view, 1> fields{{"Set-Cookie"}};
  EXPECT_FALSE(
      sourcemeta::core::http_serialize_cache_control({.private_fields = fields})
          .has_value());
}

TEST(cache_control_refuses_private_fields_alongside_public) {
  const std::array<std::string_view, 1> fields{{"Set-Cookie"}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_cache_control(
                   {.visibility = sourcemeta::core::HTTPCacheVisibility::Public,
                    .private_fields = fields})
                   .has_value());
}

// RFC 9110 §5.1: field-name = token, so a name outside that set cannot be sent
TEST(cache_control_refuses_a_field_name_that_is_not_a_token) {
  const std::array<std::string_view, 1> fields{{"Set Cookie"}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_cache_control(
                   {.no_cache_fields = fields})
                   .has_value());
}

TEST(cache_control_refuses_a_field_name_carrying_a_quote) {
  const std::array<std::string_view, 1> fields{{"a\"b"}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_cache_control(
                   {.no_cache_fields = fields})
                   .has_value());
}

// RFC 9111 §5.2: Cache-Control = #cache-directive, and a sender must not
// generate an empty list element, so a header naming no directive is not sent
TEST(cache_control_refuses_an_empty_directive_set) {
  EXPECT_FALSE(sourcemeta::core::http_serialize_cache_control({}).has_value());
}

// RFC 9111 §5.2.2.2, §5.2.2.5, §5.2.2.6, §5.2.2.8, and RFC 8246, each of which
// takes no argument
TEST(cache_control_every_valueless_directive) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.visibility = sourcemeta::core::HTTPCacheVisibility::Public,
                 .max_age = std::chrono::seconds{1},
                 .no_cache = true,
                 .must_revalidate = true,
                 .proxy_revalidate = true,
                 .must_understand = true,
                 .no_transform = true,
                 .immutable = true}),
            "public, max-age=1, no-cache, must-revalidate, proxy-revalidate, "
            "must-understand, no-transform, immutable");
}

// RFC 9111 §5.2.2.3: "A response that contains the must-understand directive
// SHOULD also contain the no-store directive", which is guidance rather than a
// prohibition, so the pair is emitted as asked
TEST(cache_control_must_understand_with_no_store) {
  EXPECT_EQ(sourcemeta::core::http_serialize_cache_control(
                {.no_store = true, .must_understand = true}),
            "no-store, must-understand");
}

TEST(cache_control_sink_overload_appends) {
  std::string buffer{"Cache-Control: "};
  EXPECT_TRUE(sourcemeta::core::http_serialize_cache_control({.no_store = true},
                                                             buffer));
  EXPECT_EQ(buffer, "Cache-Control: no-store");
}

TEST(cache_control_sink_overload_leaves_the_buffer_alone_on_failure) {
  std::string buffer{"Cache-Control: "};
  EXPECT_FALSE(sourcemeta::core::http_serialize_cache_control({}, buffer));
  EXPECT_EQ(buffer, "Cache-Control: ");
}

// What this module writes, its own parser reads back
TEST(cache_control_round_trips_through_the_parser) {
  const auto value{sourcemeta::core::http_serialize_cache_control(
      {.visibility = sourcemeta::core::HTTPCacheVisibility::Public,
       .max_age = std::chrono::seconds{60}})};
  EXPECT_TRUE(value.has_value());
  const auto parsed{
      sourcemeta::core::http_cache_control_max_age(value.value())};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed.value(), std::chrono::seconds{60});
}

#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_user_info, working_example) {
  sourcemeta::core::URI uri{"http://user:@host:80/"};
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user:");
}

// Note: Using the format "user:password" is depreacted.
// But we would like to support it for users that would still use it.
TEST(URI_user_info, depreacted_example) {
  sourcemeta::core::URI uri{"http://user:password@host:80/"};
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user:password");
}

TEST(URI_user_info, empty_example) {
  sourcemeta::core::URI uri{"http://host:80/"};
  EXPECT_FALSE(uri.userinfo().has_value());
}

TEST(URI_user_info, rfc3986_username_only) {
  sourcemeta::core::URI uri{"http://user@host/path"};
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user");
}

TEST(URI_user_info, rfc3986_username_empty_password) {
  sourcemeta::core::URI uri{"http://user:@host/path"};
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user:");
}

TEST(URI_user_info, rfc3986_username_password) {
  sourcemeta::core::URI uri{"http://user:secret@host/path"};
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user:secret");
}

TEST(URI_user_info, rfc3986_percent_encoded_userinfo) {
  sourcemeta::core::URI uri{"http://user%20name:pass%20word@host/path"};
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user%20name:pass%20word");
}

TEST(URI_user_info, rfc3986_userinfo_with_subdelims) {
  sourcemeta::core::URI uri{"http://user!$&'()*+,;=:pass@host/path"};
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user!$&'()*+,;=:pass");
}

TEST(URI_user_info, rfc3986_empty_userinfo_marker) {
  sourcemeta::core::URI uri{"http://@host/path"};
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "");
}

TEST(URI_user_info, rfc3986_no_authority) {
  sourcemeta::core::URI uri{"urn:example:resource"};
  EXPECT_FALSE(uri.userinfo().has_value());
}

TEST(URI_user_info, rfc3986_userinfo_with_at_sign) {
  sourcemeta::core::URI uri{"http://user%40domain:pass@host/path"};
  EXPECT_TRUE(uri.userinfo().has_value());
  // Per RFC 3986 Section 2.2, %40 (@) is a reserved gen-delim
  // and must not be decoded
  EXPECT_EQ(uri.userinfo().value(), "user%40domain:pass");
}

TEST(URI_user_info_setter, set_on_uri_without_userinfo) {
  sourcemeta::core::URI uri{"http://host/path"};
  uri.userinfo("user");
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user");
  EXPECT_EQ(uri.recompose(), "http://user@host/path");
}

TEST(URI_user_info_setter, replace_existing_userinfo) {
  sourcemeta::core::URI uri{"http://user:pass@host/path"};
  uri.userinfo("alice");
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "alice");
  EXPECT_EQ(uri.recompose(), "http://alice@host/path");
}

TEST(URI_user_info_setter, clear_with_empty_string) {
  sourcemeta::core::URI uri{"http://user:pass@host/path"};
  uri.userinfo("");
  EXPECT_FALSE(uri.userinfo().has_value());
  EXPECT_EQ(uri.recompose(), "http://host/path");
}

TEST(URI_user_info_setter, clear_when_already_absent) {
  sourcemeta::core::URI uri{"http://host/path"};
  uri.userinfo("");
  EXPECT_FALSE(uri.userinfo().has_value());
  EXPECT_EQ(uri.recompose(), "http://host/path");
}

TEST(URI_user_info_setter, set_with_colon) {
  sourcemeta::core::URI uri{"http://host/path"};
  uri.userinfo("user:secret");
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user:secret");
  EXPECT_EQ(uri.recompose(), "http://user:secret@host/path");
}

TEST(URI_user_info_setter, percent_encoding_applied_on_recompose) {
  sourcemeta::core::URI uri{"http://host/path"};
  uri.userinfo("user name");
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user name");
  EXPECT_EQ(uri.recompose(), "http://user%20name@host/path");
}

TEST(URI_user_info_setter, returns_reference_for_chaining) {
  sourcemeta::core::URI uri{"http://host"};
  uri.userinfo("user").fragment("section");
  EXPECT_EQ(uri.recompose(), "http://user@host#section");
}

TEST(URI_user_info_setter, normalizes_unreserved_percent_encoding) {
  sourcemeta::core::URI uri{"http://host/path"};
  uri.userinfo("user%7Ename");
  EXPECT_TRUE(uri.userinfo().has_value());
  // %7E encodes ~, an unreserved character, so it must be decoded
  EXPECT_EQ(uri.userinfo().value(), "user~name");
}

TEST(URI_user_info_setter, preserves_reserved_percent_encoding) {
  sourcemeta::core::URI uri{"http://host/path"};
  uri.userinfo("user%40name");
  EXPECT_TRUE(uri.userinfo().has_value());
  // %40 encodes @, a reserved gen-delim, so it must not be decoded
  EXPECT_EQ(uri.userinfo().value(), "user%40name");
}

TEST(URI_user_info_setter, matches_parsed_uri_for_unreserved_percent) {
  sourcemeta::core::URI built{"http://host/path"};
  built.userinfo("user%7Ename");
  const sourcemeta::core::URI parsed{"http://user%7Ename@host/path"};
  EXPECT_EQ(built.userinfo().value(), parsed.userinfo().value());
  EXPECT_EQ(built.recompose(), parsed.recompose());
}

TEST(URI_user_info_setter, percent_encodes_at_sign_on_recompose) {
  sourcemeta::core::URI uri{"http://host/path"};
  uri.userinfo("user@domain");
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user@domain");
  // @ is a gen-delim that terminates userinfo, so it must be encoded
  // on recompose to avoid producing an ambiguous URI
  EXPECT_EQ(uri.recompose(), "http://user%40domain@host/path");
}

TEST(URI_user_info_setter, recompose_roundtrip_after_at_sign_encoded) {
  sourcemeta::core::URI uri{"http://host/path"};
  uri.userinfo("user@domain");
  const auto recomposed{uri.recompose()};
  const sourcemeta::core::URI parsed{recomposed};
  EXPECT_EQ(parsed.recompose(), recomposed);
  EXPECT_TRUE(parsed.userinfo().has_value());
  EXPECT_EQ(parsed.userinfo().value(), "user%40domain");
}

TEST(URI_user_info, iri_unicode_userinfo) {
  const auto uri{
      sourcemeta::core::URI::from_iri("https://user\xE5\x90\x8D@example.com/")};
  ASSERT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user\xE5\x90\x8D");
}

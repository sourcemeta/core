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
  EXPECT_EQ(uri.userinfo().value(), "user%40domain:pass");
}

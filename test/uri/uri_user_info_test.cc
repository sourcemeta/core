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

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(secure_zero_clears_a_string) {
  std::string secret{"password"};
  sourcemeta::core::secure_zero(secret);
  EXPECT_EQ(secret, std::string(8, '\x00'));
}

TEST(secure_zero_of_an_empty_string_is_a_no_op) {
  std::string secret;
  sourcemeta::core::secure_zero(secret);
  EXPECT_TRUE(secret.empty());
}

TEST(secure_zero_clears_a_raw_buffer) {
  std::string buffer{"abcd"};
  sourcemeta::core::secure_zero(buffer.data(), buffer.size());
  EXPECT_EQ(buffer, std::string(4, '\x00'));
}

TEST(secure_scope_clears_on_exit) {
  std::string secret{"password"};
  {
    const sourcemeta::core::SecureStringScope scope{secret};
  }

  EXPECT_EQ(secret, std::string(8, '\x00'));
}

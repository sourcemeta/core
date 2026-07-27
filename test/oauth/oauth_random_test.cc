#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

TEST(random_token_is_43_characters) {
  const auto token{sourcemeta::core::oauth_random_token()};
  EXPECT_EQ((std::string_view{token.data(), token.size()}).size(), 43);
}

TEST(random_token_carries_no_base64_padding) {
  // The base64url of 32 octets is 43 characters with no "=" padding, which is
  // exactly what makes the array-sized mint allocation free
  const auto token{sourcemeta::core::oauth_random_token()};
  EXPECT_TRUE((std::string_view{token.data(), token.size()}).find('=') ==
              std::string_view::npos);
}

TEST(random_tokens_differ_between_calls) {
  const auto first{sourcemeta::core::oauth_random_token()};
  const auto second{sourcemeta::core::oauth_random_token()};
  EXPECT_FALSE((std::string_view{first.data(), first.size()}) ==
               (std::string_view{second.data(), second.size()}));
}

#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <string> // std::string

TEST(no_separator_after_the_opening_question_mark) {
  std::string sink{"https://example.com/authorize?"};
  sourcemeta::core::URI::append_query_parameter(sink, "response_type", "code");
  EXPECT_EQ(sink, "https://example.com/authorize?response_type=code");
}

TEST(joins_later_pairs_with_ampersand) {
  std::string sink{"https://example.com/authorize?"};
  sourcemeta::core::URI::append_query_parameter(sink, "response_type", "code");
  sourcemeta::core::URI::append_query_parameter(sink, "client_id",
                                                "s6BhdRkqt3");
  EXPECT_EQ(sink, "https://example.com/authorize?response_type=code"
                  "&client_id=s6BhdRkqt3");
}

TEST(the_first_pair_in_an_empty_sink_has_no_separator) {
  std::string sink;
  sourcemeta::core::URI::append_query_parameter(sink, "grant_type",
                                                "authorization_code");
  EXPECT_EQ(sink, "grant_type=authorization_code");
}

TEST(joins_form_body_pairs_with_ampersand) {
  std::string sink;
  sourcemeta::core::URI::append_query_parameter(sink, "grant_type",
                                                "authorization_code");
  sourcemeta::core::URI::append_query_parameter(sink, "code", "xyz");
  EXPECT_EQ(sink, "grant_type=authorization_code&code=xyz");
}

TEST(appends_after_an_existing_query_value) {
  std::string sink{"https://example.com/authorize?response_type=code"};
  sourcemeta::core::URI::append_query_parameter(sink, "state", "xyz");
  EXPECT_EQ(sink, "https://example.com/authorize?response_type=code&state=xyz");
}

TEST(no_separator_after_a_trailing_ampersand) {
  std::string sink{"grant_type=code&"};
  sourcemeta::core::URI::append_query_parameter(sink, "code", "xyz");
  EXPECT_EQ(sink, "grant_type=code&code=xyz");
}

TEST(escapes_the_value) {
  std::string sink;
  sourcemeta::core::URI::append_query_parameter(sink, "scope",
                                                "openid profile/email");
  EXPECT_EQ(sink, "scope=openid%20profile%2Femail");
}

TEST(escapes_the_name) {
  std::string sink;
  sourcemeta::core::URI::append_query_parameter(sink, "a b", "c");
  EXPECT_EQ(sink, "a%20b=c");
}

TEST(passes_unreserved_characters_through) {
  std::string sink;
  sourcemeta::core::URI::append_query_parameter(sink, "key", "ABCabc0123-._~");
  EXPECT_EQ(sink, "key=ABCabc0123-._~");
}

TEST(an_empty_value_still_writes_the_equals) {
  std::string sink;
  sourcemeta::core::URI::append_query_parameter(sink, "prompt", "");
  EXPECT_EQ(sink, "prompt=");
}

TEST(an_empty_name_writes_only_the_equals_and_value) {
  std::string sink;
  sourcemeta::core::URI::append_query_parameter(sink, "", "value");
  EXPECT_EQ(sink, "=value");
}

TEST(builds_a_full_authorization_query) {
  std::string sink{"https://server.example/authorize?"};
  sourcemeta::core::URI::append_query_parameter(sink, "response_type", "code");
  sourcemeta::core::URI::append_query_parameter(sink, "client_id",
                                                "s6BhdRkqt3");
  sourcemeta::core::URI::append_query_parameter(sink, "redirect_uri",
                                                "https://client.example/cb");
  sourcemeta::core::URI::append_query_parameter(sink, "scope", "openid");
  sourcemeta::core::URI::append_query_parameter(sink, "state", "xyz");
  EXPECT_EQ(sink, "https://server.example/authorize?response_type=code"
                  "&client_id=s6BhdRkqt3"
                  "&redirect_uri=https%3A%2F%2Fclient.example%2Fcb"
                  "&scope=openid&state=xyz");
}

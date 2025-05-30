#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <sstream>

TEST(URI_escape, string_example_1_skip_unreserved) {
  std::istringstream input{"foo bar"};
  std::ostringstream output;
  sourcemeta::core::uri_escape(input, output,
                               sourcemeta::core::URIEscapeMode::SkipUnreserved);
  EXPECT_EQ(output.str(), "foo%20bar");
}

TEST(URI_escape, character_reserved_skip_unreserved) {
  std::ostringstream output;
  sourcemeta::core::uri_escape('@', output,
                               sourcemeta::core::URIEscapeMode::SkipUnreserved);
  EXPECT_EQ(output.str(), "%40");
}

TEST(URI_escape, character_reserved_skip_sub_delims) {
  std::ostringstream output;
  sourcemeta::core::uri_escape('@', output,
                               sourcemeta::core::URIEscapeMode::SkipSubDelims);
  EXPECT_EQ(output.str(), "%40");
}

TEST(URI_escape, character_unreserved_skip_unreserved) {
  std::ostringstream output;
  sourcemeta::core::uri_escape('a', output,
                               sourcemeta::core::URIEscapeMode::SkipUnreserved);
  EXPECT_EQ(output.str(), "a");
}

TEST(URI_escape, character_unreserved_skip_sub_delims) {
  std::ostringstream output;
  sourcemeta::core::uri_escape('a', output,
                               sourcemeta::core::URIEscapeMode::SkipSubDelims);
  EXPECT_EQ(output.str(), "a");
}

// See https://www.rfc-editor.org/rfc/rfc3986#appendix-A
// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
TEST(URI_escape, string_unreserved_skip_unreserved) {
  std::istringstream input{
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~"};
  std::ostringstream output;
  sourcemeta::core::uri_escape(input, output,
                               sourcemeta::core::URIEscapeMode::SkipUnreserved);
  EXPECT_EQ(
      output.str(),
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~");
}

// See https://www.rfc-editor.org/rfc/rfc3986#appendix-A
// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
TEST(URI_escape, string_unreserved_skip_sub_delims) {
  std::istringstream input{
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~"};
  std::ostringstream output;
  sourcemeta::core::uri_escape(input, output,
                               sourcemeta::core::URIEscapeMode::SkipSubDelims);
  EXPECT_EQ(
      output.str(),
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~");
}

// See https://www.rfc-editor.org/rfc/rfc3986#appendix-A
// gen-delims = ":" / "/" / "?" / "#" / "[" / "]" / "@"
TEST(URI_escape, string_gen_delims_skip_unreserved) {
  std::istringstream input{":/?#[]@"};
  std::ostringstream output;
  sourcemeta::core::uri_escape(input, output,
                               sourcemeta::core::URIEscapeMode::SkipUnreserved);
  EXPECT_EQ(output.str(), "%3A%2F%3F%23%5B%5D%40");
}

// See https://www.rfc-editor.org/rfc/rfc3986#appendix-A
// gen-delims = ":" / "/" / "?" / "#" / "[" / "]" / "@"
TEST(URI_escape, string_gen_delims_skip_sub_delims) {
  std::istringstream input{":/?#[]@"};
  std::ostringstream output;
  sourcemeta::core::uri_escape(input, output,
                               sourcemeta::core::URIEscapeMode::SkipSubDelims);
  EXPECT_EQ(output.str(), "%3A%2F%3F%23%5B%5D%40");
}

// See https://www.rfc-editor.org/rfc/rfc3986#appendix-A
// sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
TEST(URI_escape, string_sub_delims_skip_unreserved) {
  std::istringstream input{"!$&'()*+,;="};
  std::ostringstream output;
  sourcemeta::core::uri_escape(input, output,
                               sourcemeta::core::URIEscapeMode::SkipUnreserved);
  EXPECT_EQ(output.str(), "%21%24%26%27%28%29%2A%2B%2C%3B%3D");
}

// See https://www.rfc-editor.org/rfc/rfc3986#appendix-A
// sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
TEST(URI_escape, string_sub_delims_skip_sub_delims) {
  std::istringstream input{"!$&'()*+,;="};
  std::ostringstream output;
  sourcemeta::core::uri_escape(input, output,
                               sourcemeta::core::URIEscapeMode::SkipSubDelims);
  EXPECT_EQ(output.str(), "!$&'()*+,;=");
}

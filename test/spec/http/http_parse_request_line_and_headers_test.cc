#include <sourcemeta/core/http_request.h>
#include <sourcemeta/core/test.h>

#include <cstddef>          // std::size_t
#include <initializer_list> // std::initializer_list
#include <string>           // std::string
#include <string_view>      // std::string_view
#include <utility>          // std::pair

TEST(
    request_line_and_headers_complete_preserves_method_target_and_field_order) {
  constexpr std::string_view INPUT{
      "custom! /path?key=value HTTP/1.1\r\nHost: example.com\r\n"
      "X-Value:\t one \t\r\nx-value:two\r\nEmpty: \t\r\n\r\n"};
  const auto result{
      sourcemeta::core::http_parse_request_line_and_headers(INPUT)};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Complete);
  EXPECT_EQ(result.consumed, INPUT.size());
  EXPECT_EQ(result.method, "custom!");
  EXPECT_EQ(result.target, "/path?key=value");
  EXPECT_EQ(result.headers.size(), 4);
  EXPECT_EQ(result.headers.at(0).first, "Host");
  EXPECT_EQ(result.headers.at(0).second, "example.com");
  EXPECT_EQ(result.headers.at(1).first, "X-Value");
  EXPECT_EQ(result.headers.at(1).second, "one");
  EXPECT_EQ(result.headers.at(2).first, "x-value");
  EXPECT_EQ(result.headers.at(2).second, "two");
  EXPECT_EQ(result.headers.at(3).first, "Empty");
  EXPECT_TRUE(result.headers.at(3).second.empty());
}

TEST(request_line_and_headers_stops_before_body) {
  constexpr std::string_view INPUT{
      "POST / HTTP/1.1\r\nHost: example.com\r\nContent-Length: 4\r\n\r\ndata"};
  const auto result{
      sourcemeta::core::http_parse_request_line_and_headers(INPUT)};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Complete);
  EXPECT_EQ(result.consumed, INPUT.size() - 4);
  EXPECT_EQ(result.method, "POST");
  EXPECT_EQ(result.target, "/");
  EXPECT_EQ(result.headers.size(), 2);
  EXPECT_EQ(result.headers.at(0).first, "Host");
  EXPECT_EQ(result.headers.at(0).second, "example.com");
  EXPECT_EQ(result.headers.at(1).first, "Content-Length");
  EXPECT_EQ(result.headers.at(1).second, "4");
  EXPECT_EQ(INPUT.substr(result.consumed), "data");
}

TEST(request_line_and_headers_empty_input_is_incomplete) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers("")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Incomplete);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
  EXPECT_TRUE(result.headers.empty());
}

TEST(request_line_and_headers_split_terminator_is_incomplete) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\r\nHost: example.com\r\n\r")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Incomplete);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.headers.empty());
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
}

TEST(request_line_and_headers_rejects_other_versions) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.0\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
  EXPECT_TRUE(result.headers.empty());
}

TEST(request_line_and_headers_rejects_missing_target) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET  HTTP/1.1\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
  EXPECT_TRUE(result.headers.empty());
}

TEST(request_line_and_headers_rejects_bare_lf) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\n\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
  EXPECT_TRUE(result.headers.empty());
}

TEST(request_line_and_headers_rejects_whitespace_before_colon) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\r\nHost : example.com\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.headers.empty());
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
}

TEST(request_line_and_headers_rejects_field_value_control_byte) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\r\nHost: example.com\r\nX-Value: \x01\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
  EXPECT_TRUE(result.headers.empty());
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
}

TEST(request_line_and_headers_rejects_obsolete_line_folding) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\r\nHost: example.com\r\n continued\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
  EXPECT_TRUE(result.headers.empty());
}

TEST(request_line_and_headers_partial_request_line_is_incomplete) {
  const auto result{
      sourcemeta::core::http_parse_request_line_and_headers("GET / HTTP/1.")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Incomplete);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
  EXPECT_TRUE(result.headers.empty());
}

TEST(request_line_and_headers_partial_field_is_incomplete) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\r\nHost: example.com")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Incomplete);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
  EXPECT_TRUE(result.headers.empty());
}

TEST(request_line_and_headers_rejects_carriage_return_without_line_feed) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\rHost: example.com\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
  EXPECT_TRUE(result.headers.empty());
}

// Adapted from llhttp's "Simple request" fixture (MIT):
// https://github.com/nodejs/llhttp/blob/85c5d42cd563d5426a478d32248a45ee81f4b635/test/request/sample.md#simple-request
TEST(llhttp_simple_request) {
  constexpr std::string_view INPUT{
      "OPTIONS /url HTTP/1.1\r\nHeader1: Value1\r\nHeader2:\t Value2\r\n\r\n"};
  const auto result{
      sourcemeta::core::http_parse_request_line_and_headers(INPUT)};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Complete);
  EXPECT_EQ(result.consumed, INPUT.size());
  EXPECT_EQ(result.method, "OPTIONS");
  EXPECT_EQ(result.target, "/url");
  EXPECT_EQ(result.headers.size(), 2);
  EXPECT_EQ(result.headers.at(0).first, "Header1");
  EXPECT_EQ(result.headers.at(0).second, "Value1");
  EXPECT_EQ(result.headers.at(1).first, "Header2");
  EXPECT_EQ(result.headers.at(1).second, "Value2");
}

// Adapted from llhttp's "No headers and no body" fixture (MIT):
// https://github.com/nodejs/llhttp/blob/85c5d42cd563d5426a478d32248a45ee81f4b635/test/request/sample.md#no-headers-and-no-body
TEST(llhttp_no_headers_and_no_body) {
  constexpr std::string_view INPUT{
      "GET /get_no_headers_no_body/world HTTP/1.1\r\n\r\n"};
  const auto result{
      sourcemeta::core::http_parse_request_line_and_headers(INPUT)};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Complete);
  EXPECT_EQ(result.consumed, INPUT.size());
  EXPECT_EQ(result.method, "GET");
  EXPECT_EQ(result.target, "/get_no_headers_no_body/world");
  EXPECT_TRUE(result.headers.empty());
}

namespace {
// RFC 9110 token alphabet, independent of the parser's syntax helper.
constexpr std::string_view TOKEN_CHARACTERS{
    "!#$%&'*+-.^_`|~"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};

auto expect_complete(
    const std::string_view input, const std::size_t consumed,
    const std::string_view method, const std::string_view target,
    const std::initializer_list<std::pair<std::string_view, std::string_view>>
        headers) -> sourcemeta::core::HTTPRequestLineAndHeaders {
  const auto result{
      sourcemeta::core::http_parse_request_line_and_headers(input)};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Complete);
  EXPECT_EQ(result.consumed, consumed);
  EXPECT_EQ(result.method, method);
  EXPECT_EQ(result.target, target);
  EXPECT_EQ(result.headers.size(), headers.size());
  std::size_t index{0};
  for (const auto &[name, value] : headers) {
    EXPECT_EQ(result.headers.at(index).first, name);
    EXPECT_EQ(result.headers.at(index).second, value);
    ++index;
  }
  return result;
}

auto expect_failure(
    const std::string_view input,
    const sourcemeta::core::HTTPRequestLineAndHeadersStatus status) -> void {
  const auto result{
      sourcemeta::core::http_parse_request_line_and_headers(input)};
  EXPECT_TRUE(result.status == status);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
  EXPECT_TRUE(result.headers.empty());
}
} // namespace

// Adapted from llhttp's "DUMBPACK" fixture (MIT); normalized to CRLF:
// https://github.com/nodejs/llhttp/blob/85c5d42cd563d5426a478d32248a45ee81f4b635/test/request/sample.md#dumbpack
TEST(llhttp_dumbpack) {
  constexpr std::string_view INPUT{
      "GET /dumbpack HTTP/1.1\r\naaaaaaaaaaaaa:++++++++++\r\n\r\n"};
  expect_complete(INPUT, INPUT.size(), "GET", "/dumbpack",
                  {{"aaaaaaaaaaaaa", "++++++++++"}});
}

// Adapted from llhttp's "Extended Characters" fixture (MIT); UTF-8 bytes:
// https://github.com/nodejs/llhttp/blob/85c5d42cd563d5426a478d32248a45ee81f4b635/test/request/sample.md#extended-characters
TEST(llhttp_extended_characters) {
  constexpr std::string_view INPUT{
      "GET / HTTP/1.1\r\nTest: D\xC3\xBCsseldorf\r\n\r\n"};
  expect_complete(INPUT, INPUT.size(), "GET", "/",
                  {{"Test", "D\xC3\xBCsseldorf"}});
}

// Adapted from llhttp's "255 ASCII in header value" fixture (MIT):
// https://github.com/nodejs/llhttp/blob/85c5d42cd563d5426a478d32248a45ee81f4b635/test/request/sample.md#255-ascii-in-header-value
TEST(llhttp_header_value_ff_octet) {
  constexpr std::string_view INPUT{"OPTIONS /url HTTP/1.1\r\nHeader1: "
                                   "Value1\r\nHeader2: \xFFValue2\r\n\r\n"};
  expect_complete(INPUT, INPUT.size(), "OPTIONS", "/url",
                  {{"Header1", "Value1"}, {"Header2", "\xFFValue2"}});
}

// Adapted from llhttp's "Headers separated by CR" fixture (MIT):
// https://github.com/nodejs/llhttp/blob/85c5d42cd563d5426a478d32248a45ee81f4b635/test/request/invalid.md#headers-separated-by-cr
TEST(llhttp_headers_separated_by_cr) {
  expect_failure("GET / HTTP/1.1\r\nFoo: 1\rBar: 2\r\n\r\n",
                 sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
}

// Adapted from llhttp's "Headers separated by dummy characters" fixture (MIT):
// https://github.com/nodejs/llhttp/blob/85c5d42cd563d5426a478d32248a45ee81f4b635/test/request/invalid.md#headers-separated-by-dummy-characters
TEST(llhttp_headers_separated_by_dummy_characters) {
  expect_failure("GET / HTTP/1.1\r\nConnection: close\r\nHost: a\r\n"
                 "\rZGET /evil: HTTP/1.1\r\nHost: a\r\n\r\n",
                 sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
}

// Adapted from llhttp's "Invalid header token #1" fixture (MIT):
// https://github.com/nodejs/llhttp/blob/85c5d42cd563d5426a478d32248a45ee81f4b635/test/request/invalid.md#invalid-header-token-1
TEST(llhttp_invalid_header_token) {
  expect_failure("GET / HTTP/1.1\r\nFo@: Failure\r\n\r\n",
                 sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
}

// Adapted from llhttp's "Invalid method" fixture (MIT). Core intentionally
// accepts extension method tokens instead of llhttp's fixed method inventory:
// https://github.com/nodejs/llhttp/blob/85c5d42cd563d5426a478d32248a45ee81f4b635/test/request/invalid.md#invalid-method
TEST(llhttp_extension_method_token) {
  constexpr std::string_view INPUT{"MKCOLA / HTTP/1.1\r\n\r\n"};
  expect_complete(INPUT, INPUT.size(), "MKCOLA", "/", {});
}

TEST(request_line_and_headers_retry_at_every_split) {
  // This API retries from the beginning; it does not retain incremental state.
  for (const std::string_view input :
       {"GET / HTTP/1.1\r\n\r\n",
        "PATCH /items?q=1 HTTP/1.1\r\nHost: example.com\r\n"
        "X-Id: one\r\nx-id:\ttwo \t\r\nEmpty:\t \r\n\r\n"}) {
    for (std::size_t split{0}; split <= input.size(); ++split) {
      std::string buffer{input.substr(0, split)};
      if (split < input.size()) {
        expect_failure(
            buffer,
            sourcemeta::core::HTTPRequestLineAndHeadersStatus::Incomplete);
      }
      buffer.append(input.substr(split));
      if (input.starts_with("GET")) {
        expect_complete(buffer, input.size(), "GET", "/", {});
      } else {
        expect_complete(buffer, input.size(), "PATCH", "/items?q=1",
                        {{"Host", "example.com"},
                         {"X-Id", "one"},
                         {"x-id", "two"},
                         {"Empty", ""}});
      }
    }
  }
}

TEST(request_line_and_headers_binary_body_and_next_request_at_every_split) {
  constexpr std::string_view HEAD{"POST /upload HTTP/1.1\r\nHost: "
                                  "example.com\r\nContent-Length: 256\r\n\r\n"};
  constexpr std::string_view NEXT{"GET /next HTTP/1.1\r\n\r\n"};
  std::string body;
  for (unsigned int byte{0}; byte <= 255; ++byte) {
    body.push_back(static_cast<char>(byte));
  }
  const auto suffix{body + std::string{NEXT}};
  const auto input{std::string{HEAD} + suffix};
  for (std::size_t split{0}; split <= input.size(); ++split) {
    std::string buffer{input.substr(0, split)};
    if (split < HEAD.size()) {
      expect_failure(
          buffer,
          sourcemeta::core::HTTPRequestLineAndHeadersStatus::Incomplete);
    } else {
      expect_complete(buffer, HEAD.size(), "POST", "/upload",
                      {{"Host", "example.com"}, {"Content-Length", "256"}});
    }
    buffer.append(std::string_view{input}.substr(split));
    const auto first{
        expect_complete(buffer, HEAD.size(), "POST", "/upload",
                        {{"Host", "example.com"}, {"Content-Length", "256"}})};
    EXPECT_EQ(std::string_view{buffer}.substr(first.consumed), suffix);
    // The caller, not this header parser, accounts for the declared body.
    const auto next{
        std::string_view{buffer}.substr(first.consumed + body.size())};
    expect_complete(next, NEXT.size(), "GET", "/next", {});
  }
}

TEST(request_line_and_headers_pipelined_header_boundary) {
  constexpr std::string_view FIRST{"GET /first HTTP/1.1\r\nHost: one\r\n\r\n"};
  constexpr std::string_view SECOND{
      "HEAD /second HTTP/1.1\r\nHost: two\r\nX-Id: 2\r\n\r\n"};
  const auto input{std::string{FIRST} + std::string{SECOND}};
  const auto first{
      expect_complete(input, FIRST.size(), "GET", "/first", {{"Host", "one"}})};
  const auto remaining{std::string_view{input}.substr(first.consumed)};
  EXPECT_EQ(remaining, SECOND);
  expect_complete(remaining, SECOND.size(), "HEAD", "/second",
                  {{"Host", "two"}, {"X-Id", "2"}});
}

TEST(request_line_and_headers_late_failure_discards_partial_result) {
  for (const std::string_view tail :
       {"Bad Header: value\r\n\r\n", "Missing-Colon\r\n\r\n", ": value\r\n\r\n",
        "\tcontinued\r\n\r\n", "X: value\n\r\n", "X: value\rX\r\n\r\n"}) {
    const auto input{std::string{"GET / HTTP/1.1\r\nHost: example.com\r\n"} +
                     std::string{tail}};
    expect_failure(
        input, sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
  }
}

TEST(request_line_and_headers_strict_request_line) {
  for (const std::string_view input :
       {"GET\r\n\r\n", "GET /target\r\n\r\n", " / HTTP/1.1\r\n\r\n",
        "\r\nGET / HTTP/1.1\r\n\r\n", "GET\t/ HTTP/1.1\r\n\r\n",
        "GET /\tHTTP/1.1\r\n\r\n", "GET /  HTTP/1.1\r\n\r\n",
        "GET / HTTP/1.1 \r\n\r\n", "GET / HTTP/1.1 extra\r\n\r\n",
        "GET / HTTP/2.0\r\n\r\n", "GET / http/1.1\r\n\r\n"}) {
    expect_failure(
        input, sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
  }
}

TEST(request_line_and_headers_method_octets) {
  for (unsigned int byte{0}; byte <= 255; ++byte) {
    const auto method{std::string{"X"} + static_cast<char>(byte) + "Y"};
    const auto input{method + " / HTTP/1.1\r\n\r\n"};
    if (TOKEN_CHARACTERS.find(static_cast<char>(byte)) !=
        std::string_view::npos) {
      expect_complete(input, input.size(), method, "/", {});
    } else {
      expect_failure(
          input, sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
    }
  }
}

TEST(request_line_and_headers_field_name_octets) {
  for (unsigned int byte{0}; byte <= 255; ++byte) {
    const auto name{std::string{"X"} + static_cast<char>(byte) + "Y"};
    const auto input{"GET / HTTP/1.1\r\n" + name + ": value\r\n\r\n"};
    if (byte == ':') {
      // A colon terminates the name; the remaining colon belongs to the value.
      expect_complete(input, input.size(), "GET", "/", {{"X", "Y: value"}});
    } else if (TOKEN_CHARACTERS.find(static_cast<char>(byte)) !=
               std::string_view::npos) {
      expect_complete(input, input.size(), "GET", "/", {{name, "value"}});
    } else {
      expect_failure(
          input, sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
    }
  }
}

TEST(request_line_and_headers_target_octets) {
  for (unsigned int byte{0}; byte <= 255; ++byte) {
    const auto target{std::string{"/"} + static_cast<char>(byte) + "path"};
    const auto input{"GET " + target + " HTTP/1.1\r\n\r\n"};
    if (byte >= 0x21 && byte <= 0x7E) {
      expect_complete(input, input.size(), "GET", target, {});
    } else {
      expect_failure(
          input, sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
    }
  }
}

TEST(request_line_and_headers_field_value_octets) {
  for (unsigned int byte{0}; byte <= 255; ++byte) {
    const auto value{std::string{"left"} + static_cast<char>(byte) + "right"};
    const auto input{"GET / HTTP/1.1\r\nX-Value: " + value + "\r\n\r\n"};
    if (byte == 0x09 || (byte >= 0x20 && byte != 0x7F)) {
      expect_complete(input, input.size(), "GET", "/", {{"X-Value", value}});
    } else {
      expect_failure(
          input, sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
    }
  }
}

TEST(request_line_and_headers_repeated_empty_and_colon_values) {
  constexpr std::string_view INPUT{
      "GET / HTTP/1.1\r\nX-Id: first\r\nx-id:\r\nX-Id:\t third \t\r\n"
      "X-Value: \t left:\tmiddle right \t\r\n\r\n"};
  expect_complete(INPUT, INPUT.size(), "GET", "/",
                  {{"X-Id", "first"},
                   {"x-id", ""},
                   {"X-Id", "third"},
                   {"X-Value", "left:\tmiddle right"}});
}

TEST(request_line_and_headers_views_borrow_non_terminated_input) {
  constexpr std::string_view HEAD{"GET /view HTTP/1.1\r\nX-Id: value\r\n\r\n"};
  const auto storage{std::string{"prefix"} + std::string{HEAD} + "suffix"};
  const auto input{std::string_view{storage}.substr(6, HEAD.size())};
  const auto result{
      expect_complete(input, HEAD.size(), "GET", "/view", {{"X-Id", "value"}})};
  EXPECT_TRUE(result.method.data() == input.data());
  EXPECT_TRUE(result.target.data() == input.data() + 4);
  EXPECT_TRUE(result.headers.at(0).first.data() ==
              input.data() + input.find("X-Id"));
  EXPECT_TRUE(result.headers.at(0).second.data() ==
              input.data() + input.find("value"));
  // The final LF is readable in storage, but is outside this input view.
  expect_failure(input.substr(0, input.size() - 1),
                 sourcemeta::core::HTTPRequestLineAndHeadersStatus::Incomplete);
}

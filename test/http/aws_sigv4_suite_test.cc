#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

namespace {

// The fixed inputs every case in the suite shares
constexpr std::string_view ACCESS_KEY_ID{"AKIDEXAMPLE"};
constexpr std::string_view SECRET_ACCESS_KEY{
    "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY"};
constexpr std::string_view REGION{"us-east-1"};
constexpr std::string_view SERVICE{"service"};

// Every field borrows from the request content that produced it
struct ParsedRequest {
  std::string_view method;
  std::string_view path;
  std::string_view query;
  std::string_view body;
  std::vector<std::pair<std::string_view, std::string_view>> headers;
};

auto parse_request(const std::string_view content) -> ParsedRequest {
  ParsedRequest request;

  // The header block and the body are separated by a blank line, and a request
  // without a body has no such separator
  std::string_view header_block{content};
  const auto separator{content.find("\n\n")};
  if (separator != std::string_view::npos) {
    header_block = content.substr(0, separator);
    request.body = content.substr(separator + 2);
  }

  std::size_t line_start{0};
  bool is_request_line{true};
  std::string_view last_name;
  while (line_start <= header_block.size()) {
    const auto line_end{header_block.find('\n', line_start)};
    const auto line{
        header_block.substr(line_start, line_end == std::string_view::npos
                                            ? std::string_view::npos
                                            : line_end - line_start)};

    if (line.empty()) {
      // A blank line, such as a trailing newline at the end of the file
    } else if (is_request_line) {
      // "<METHOD> <TARGET> HTTP/1.1", where the target may contain spaces
      request.method = sourcemeta::core::take_until(line, ' ');
      const auto rest{line.substr(request.method.size() + 1)};
      const auto target{rest.substr(0, rest.rfind(' '))};
      const auto split{sourcemeta::core::split_once(target, '?')};
      if (split.has_value()) {
        request.path = split->first;
        request.query = split->second;
      } else {
        request.path = target;
      }

      is_request_line = false;
    } else if (line.front() == ' ' || line.front() == '\t') {
      // A folded continuation line is signed as a separate value of the
      // previous header
      request.headers.emplace_back(last_name, line);
    } else if (const auto field{sourcemeta::core::split_once(line, ':')};
               field.has_value()) {
      last_name = field->first;
      request.headers.emplace_back(field->first, field->second);
    }

    if (line_end == std::string_view::npos) {
      break;
    }

    line_start = line_end + 1;
  }

  return request;
}

auto find_amz_date(
    const std::span<const std::pair<std::string_view, std::string_view>>
        headers) -> std::string_view {
  for (const auto &[name, value] : headers) {
    if (sourcemeta::core::equals_ignore_case(name, "x-amz-date")) {
      return sourcemeta::core::trim(value);
    }
  }

  return {};
}

auto run_case(const std::string_view content,
              const std::string_view expected_canonical,
              const std::string_view expected_string_to_sign,
              const std::string_view expected_authorization) -> void {
  const auto request{parse_request(content)};
  const auto payload_hash{sourcemeta::core::sha256(request.body)};
  const auto amz_date{find_amz_date(request.headers)};
  const auto date{amz_date.substr(0, 8)};

  const auto canonical{sourcemeta::core::http_aws_sigv4_canonical_request(
      request.method, request.path, request.query, request.headers,
      payload_hash)};
  EXPECT_EQ(canonical, expected_canonical);

  const auto scope{
      sourcemeta::core::http_aws_sigv4_credential_scope(date, REGION, SERVICE)};
  const auto string_to_sign{sourcemeta::core::http_aws_sigv4_string_to_sign(
      amz_date, scope, canonical)};
  EXPECT_EQ(string_to_sign, expected_string_to_sign);

  const auto signed_headers{
      sourcemeta::core::http_aws_sigv4_signed_headers(request.headers)};
  const auto key{sourcemeta::core::http_aws_sigv4_signing_key(
      SECRET_ACCESS_KEY, date, REGION, SERVICE)};
  const auto signature{
      sourcemeta::core::http_aws_sigv4_signature(key, string_to_sign)};
  const auto authorization{sourcemeta::core::http_aws_sigv4_authorization(
      ACCESS_KEY_ID, scope, signed_headers, signature)};
  EXPECT_EQ(authorization, expected_authorization);
}

auto register_suite_case(const std::filesystem::path &request_path) -> void {
  const auto name{request_path.stem().string()};
  const auto content{sourcemeta::core::read_file_to_string(request_path)};
  const auto expected_canonical{sourcemeta::core::read_file_to_string(
      std::filesystem::path{request_path}.replace_extension(".creq"))};
  const auto expected_string_to_sign{sourcemeta::core::read_file_to_string(
      std::filesystem::path{request_path}.replace_extension(".sts"))};
  const auto expected_authorization{sourcemeta::core::read_file_to_string(
      std::filesystem::path{request_path}.replace_extension(".authz"))};

  sourcemeta::core::test_register(
      "AWS_SigV4." + name, [content, expected_canonical,
                            expected_string_to_sign, expected_authorization]() {
        run_case(content, expected_canonical, expected_string_to_sign,
                 expected_authorization);
      });
}

} // namespace

TEST(parse_request_basic) {
  const std::string content{
      "POST /path?a=b HTTP/1.1\nHost:example.com\n\nbody"};
  const auto request{parse_request(content)};
  EXPECT_EQ(request.method, "POST");
  EXPECT_EQ(request.path, "/path");
  EXPECT_EQ(request.query, "a=b");
  EXPECT_EQ(request.body, "body");
  EXPECT_EQ(request.headers.size(), 1);
}

TEST(parse_request_tolerates_trailing_newline) {
  const std::string content{"GET / HTTP/1.1\nHost:example.amazonaws.com\n"
                            "X-Amz-Date:20150830T123600Z\n"};
  const auto request{parse_request(content)};
  EXPECT_EQ(request.method, "GET");
  EXPECT_EQ(request.path, "/");
  EXPECT_EQ(request.headers.size(), 2);
}

TEST(parse_request_ignores_line_without_colon) {
  const std::string content{"GET / HTTP/1.1\nHost:example.amazonaws.com\n"
                            "garbage-without-a-colon\n"
                            "X-Amz-Date:20150830T123600Z"};
  const auto request{parse_request(content)};
  EXPECT_EQ(request.headers.size(), 2);
}

auto main(int argc, char **argv) -> int {
  const std::filesystem::path base{AWS_SIGV4_TEST_SUITE_PATH};
  for (const auto &entry :
       std::filesystem::recursive_directory_iterator{base}) {
    if (entry.is_regular_file() && entry.path().extension() == ".req") {
      register_suite_case(entry.path());
    }
  }

  return sourcemeta::core::test_run(argc, argv);
}

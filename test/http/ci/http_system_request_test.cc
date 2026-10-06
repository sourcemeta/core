#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
// This also keeps the original Windows sockets header out of the Windows
// headers that the current one pulls in, as the two cannot coexist
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// SOCKET, INVALID_SOCKET, WSADATA, WSAStartup, WSACleanup, socket, bind,
// listen, getsockname, closesocket, htonl, htons, ntohs
#include <winsock2.h>
#else
// sockaddr_in, INADDR_LOOPBACK, IPPROTO_TCP, htonl, htons, ntohs
#include <netinet/in.h>
// socket, bind, listen, getsockname, socklen_t, AF_INET, SOCK_STREAM
#include <sys/socket.h>
#include <unistd.h> // close
#endif

#include <chrono>  // std::chrono::milliseconds
#include <cstdint> // std::uint16_t
#include <string>  // std::string, std::to_string

namespace {

#ifdef _WIN32
using ListenerDescriptor = SOCKET;
constexpr ListenerDescriptor LISTENER_INVALID{INVALID_SOCKET};
#else
using ListenerDescriptor = int;
constexpr ListenerDescriptor LISTENER_INVALID{-1};
#endif

// A loopback listener that completes the connection handshake but never
// accepts, reads, or writes, so a request against it can only end by
// exceeding its timeout. Leaving the connection in the kernel accept queue is
// what keeps the peer socket open, as a listener that accepted and then closed
// would make a backend report an empty response rather than a timeout
class SilentListener {
public:
  SilentListener() {
#ifdef _WIN32
    WSADATA information;
    if (WSAStartup(MAKEWORD(2, 2), &information) != 0) {
      return;
    }
#endif

    this->descriptor_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->descriptor_ == LISTENER_INVALID) {
      return;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(0);
    if (bind(this->descriptor_, reinterpret_cast<sockaddr *>(&address),
             sizeof(address)) != 0) {
      return;
    }

    if (listen(this->descriptor_, 1) != 0) {
      return;
    }

    sockaddr_in bound{};
#ifdef _WIN32
    int length{sizeof(bound)};
#else
    socklen_t length{sizeof(bound)};
#endif
    if (getsockname(this->descriptor_, reinterpret_cast<sockaddr *>(&bound),
                    &length) != 0) {
      return;
    }

    this->port_ = ntohs(bound.sin_port);
  }

  ~SilentListener() {
    if (this->descriptor_ != LISTENER_INVALID) {
#ifdef _WIN32
      closesocket(this->descriptor_);
#else
      close(this->descriptor_);
#endif
    }

#ifdef _WIN32
    WSACleanup();
#endif
  }

  SilentListener(const SilentListener &) = delete;
  auto operator=(const SilentListener &) -> SilentListener & = delete;
  SilentListener(SilentListener &&) = delete;
  auto operator=(SilentListener &&) -> SilentListener & = delete;

  // The port the listener bound to, or zero when it could not be set up
  [[nodiscard]] auto port() const noexcept -> std::uint16_t {
    return this->port_;
  }

private:
  ListenerDescriptor descriptor_{LISTENER_INVALID};
  std::uint16_t port_{0};
};

} // namespace

TEST(get_health_ok) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/health"};
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_OK);
  // Backends may normalise the effective URL differently, so match its
  // meaningful prefix rather than an exact string
  EXPECT_TRUE(response.url.starts_with(
      "https://schemas.sourcemeta.com/self/v1/health"));
}

TEST(head_health_ok) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/health",
      sourcemeta::core::HTTPMethod::HEAD};
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_OK);
  EXPECT_TRUE(response.body.empty());
}

TEST(get_list_json_body) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/api/list"};
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_OK);
  const auto content_type{
      sourcemeta::core::http_header_find(response.headers, "content-type")};
  EXPECT_TRUE(content_type.has_value());
  EXPECT_NE(content_type.value().find("application/json"),
            std::string_view::npos);
  const auto body{sourcemeta::core::parse_json(response.body)};
  EXPECT_TRUE(body.is_object());
  EXPECT_TRUE(body.defines("entries"));
}

TEST(get_missing_path_not_found) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/api/list/"
      "this-directory-does-not-exist-xyz"};
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_NOT_FOUND);
  const auto content_type{
      sourcemeta::core::http_header_find(response.headers, "content-type")};
  EXPECT_TRUE(content_type.has_value());
  EXPECT_NE(content_type.value().find("application/problem+json"),
            std::string_view::npos);
}

TEST(post_health_method_not_allowed) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/health",
      sourcemeta::core::HTTPMethod::POST};
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED);
}

TEST(post_evaluate_with_body) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/api/schemas/evaluate/"
      "cloudevents/v1.0.2/cloudevents",
      sourcemeta::core::HTTPMethod::POST};
  request.body("{}", "application/json");
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_OK);
  const auto body{sourcemeta::core::parse_json(response.body)};
  EXPECT_TRUE(body.is_object());
  EXPECT_TRUE(body.defines("valid"));
  EXPECT_TRUE(body.at("valid").is_boolean());
  EXPECT_FALSE(body.at("valid").to_boolean());
}

TEST(post_evaluate_missing_instance_bad_request) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/api/schemas/evaluate/"
      "cloudevents/v1.0.2/cloudevents",
      sourcemeta::core::HTTPMethod::POST};
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
}

TEST(follow_redirect_to_https) {
  sourcemeta::core::HTTPSystemRequest request{
      "http://schemas.sourcemeta.com/self/v1/health"};
  request.follow_redirects(true);
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_OK);
  // The redirect upgrades the scheme to HTTPS, which the effective URL
  // reflects regardless of backend-specific normalisation
  EXPECT_TRUE(response.url.starts_with(
      "https://schemas.sourcemeta.com/self/v1/health"));
}

TEST(no_follow_redirect_returns_redirect) {
  sourcemeta::core::HTTPSystemRequest request{
      "http://schemas.sourcemeta.com/self/v1/health"};
  request.follow_redirects(false);
  const auto response{request.send()};
  EXPECT_GE(response.status.code, 300);
  EXPECT_LT(response.status.code, 400);
}

TEST(unreachable_host_throws) {
  // RFC 5737 reserves this block for documentation and asks operators to treat
  // it as non-routeable, which leaves the failure mode up to the network: a
  // host that drops the traffic exceeds the timeout, while one that rejects it
  // fails immediately. Only the base error covers both, so the deterministic
  // timeout classification is asserted against a local listener instead
  sourcemeta::core::HTTPSystemRequest request{"https://192.0.2.1/"};
  request.timeout(std::chrono::milliseconds{1000});
  try {
    [[maybe_unused]] const auto response{request.send()};
    FAIL();
  } catch (const sourcemeta::core::HTTPError &error) {
    EXPECT_EQ(error.method(), sourcemeta::core::HTTPMethod::GET);
    EXPECT_EQ(error.url(), "https://192.0.2.1/");
  }
}

TEST(timeout_against_silent_listener_throws) {
  const SilentListener listener;
  EXPECT_NE(listener.port(), 0);
  const auto url{"http://127.0.0.1:" + std::to_string(listener.port()) + "/"};
  sourcemeta::core::HTTPSystemRequest request{url};
  request.timeout(std::chrono::milliseconds{1000});
  try {
    [[maybe_unused]] const auto response{request.send()};
    FAIL();
  } catch (const sourcemeta::core::HTTPTimeoutError &error) {
    EXPECT_EQ(error.method(), sourcemeta::core::HTTPMethod::GET);
    EXPECT_EQ(error.url(), url);
    EXPECT_EQ(error.timeout(), std::chrono::milliseconds{1000});
  }
}

TEST(unresolvable_host_throws) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://this-host-does-not-exist.sourcemeta.invalid/"};
  try {
    [[maybe_unused]] const auto response{request.send()};
    FAIL();
  } catch (const sourcemeta::core::HTTPTimeoutError &) {
    FAIL();
  } catch (const sourcemeta::core::HTTPError &error) {
    EXPECT_EQ(error.method(), sourcemeta::core::HTTPMethod::GET);
    EXPECT_EQ(error.url(),
              "https://this-host-does-not-exist.sourcemeta.invalid/");
  }
}

TEST(maximum_response_size_exceeded_throws) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/api/list"};
  request.maximum_response_size(1);
  try {
    [[maybe_unused]] const auto response{request.send()};
    FAIL();
  } catch (const sourcemeta::core::HTTPError &error) {
    EXPECT_EQ(error.method(), sourcemeta::core::HTTPMethod::GET);
    EXPECT_EQ(error.url(), "https://schemas.sourcemeta.com/self/v1/api/list");
  }
}

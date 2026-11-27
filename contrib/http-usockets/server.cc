#include <libusockets.h>
#include <sourcemeta/core/http_request.h>

#include <algorithm>   // std::min, std::ranges::equal
#include <cstddef>     // std::size_t
#include <iostream>    // std::cout, std::cerr
#include <new>         // std::nothrow
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {
constexpr int SSL{0};
constexpr std::size_t MAX_HEADERS{8192};
constexpr unsigned int IDLE_SECONDS{10};
constexpr std::string_view NO_CONTENT{
    "HTTP/1.1 204 No Content\r\nConnection: close\r\n\r\n"};
constexpr std::string_view BAD{"HTTP/1.1 400 Bad Request\r\nContent-Length: "
                               "0\r\nConnection: close\r\n\r\n"};
constexpr std::string_view TOO_LARGE{
    "HTTP/1.1 431 Request Header Fields Too Large\r\n"
    "Content-Length: 0\r\nConnection: close\r\n\r\n"};

struct Connection {
  std::string input;
  std::string_view pending;
  bool replied{false};
  bool peer_ended{false};
};

auto connection(us_socket_t *socket) -> Connection *& {
  return *static_cast<Connection **>(us_socket_ext(SSL, socket));
}

auto equals_header(const std::string_view actual, const std::string_view lower)
    -> bool {
  return std::ranges::equal(actual, lower, [](char left, const char right) {
    if (left >= 'A' && left <= 'Z') {
      left = static_cast<char>(left + ('a' - 'A'));
    }
    return left == right;
  });
}

auto accepts(const sourcemeta::core::HTTPRequestLineAndHeaders &request)
    -> bool {
  // Restrictive fixture policy, not request-target, Host, or framing
  // validation. Only a bodyless GET / with one nonempty Host is handled. In
  // particular, repeated lengths and spellings other than a single literal 0
  // are rejected.
  if (request.method != "GET" || request.target != "/") {
    return false;
  }

  bool host{false};
  bool content_length{false};
  for (const auto &[name, value] : request.headers) {
    if (equals_header(name, "host")) {
      if (host || value.empty()) {
        return false;
      }
      host = true;
    } else if (equals_header(name, "transfer-encoding")) {
      return false;
    } else if (equals_header(name, "content-length")) {
      if (content_length || value != "0") {
        return false;
      }
      content_length = true;
    }
  }
  return host;
}

auto writable(us_socket_t *socket) -> us_socket_t * {
  auto &state{*connection(socket)};
  if (!state.pending.empty()) {
    const auto written{us_socket_write(SSL, socket, state.pending.data(),
                                       static_cast<int>(state.pending.size()),
                                       0)};
    state.pending.remove_prefix(static_cast<std::size_t>(written));
    if (state.pending.empty()) {
      if (state.peer_ended) {
        return us_socket_close(SSL, socket, 0, nullptr);
      }
      us_socket_shutdown(SSL, socket);
    }
  }
  return socket;
}

auto data(us_socket_t *socket, char *bytes, int length) -> us_socket_t * {
  auto &state{*connection(socket)};
  if (state.replied) {
    return socket;
  }

  state.input.append(bytes, std::min(static_cast<std::size_t>(length),
                                     MAX_HEADERS - state.input.size()));
  using Status = sourcemeta::core::HTTPRequestLineAndHeadersStatus;
  {
    const auto request{
        sourcemeta::core::http_parse_request_line_and_headers(state.input)};
    if (request.status == Status::Incomplete &&
        state.input.size() < MAX_HEADERS) {
      return socket;
    }
    if (request.status == Status::Complete) {
      // This prototype closes after one response. Discard any suffix without
      // treating it as part of the header section or as a second request.
      state.pending = accepts(request) ? NO_CONTENT : BAD;
    } else {
      state.pending = request.status == Status::Malformed ? BAD : TOO_LARGE;
    }
  }
  state.replied = true;
  state.input.clear();
  us_socket_timeout(SSL, socket, IDLE_SECONDS);
  return writable(socket);
}

auto noop(us_loop_t *) -> void {}
} // namespace

auto main() -> int {
  auto *loop{us_create_loop(nullptr, noop, noop, noop, 0)};
  if (loop == nullptr) {
    std::cerr << "Could not create the uSockets loop\n";
    return 1;
  }
  auto *context{us_create_socket_context(SSL, loop, 0, {})};
  if (context == nullptr) {
    std::cerr << "Could not create the uSockets context\n";
    us_loop_free(loop);
    return 1;
  }
  us_socket_context_on_open(
      SSL, context, [](us_socket_t *socket, int, char *, int) {
        connection(socket) = new (std::nothrow) Connection{};
        if (connection(socket) == nullptr) {
          return us_socket_close(SSL, socket, 0, nullptr);
        }
        us_socket_timeout(SSL, socket, IDLE_SECONDS);
        return socket;
      });
  us_socket_context_on_close(SSL, context,
                             [](us_socket_t *socket, int, void *) {
                               delete connection(socket);
                               return socket;
                             });
  us_socket_context_on_data(SSL, context, data);
  us_socket_context_on_writable(SSL, context, writable);
  us_socket_context_on_end(SSL, context, [](us_socket_t *socket) {
    auto &state{*connection(socket)};
    state.peer_ended = true;
    if (!state.pending.empty()) {
      return socket;
    }
    return us_socket_close(SSL, socket, 0, nullptr);
  });
  us_socket_context_on_timeout(SSL, context, [](us_socket_t *socket) {
    return us_socket_close(SSL, socket, 0, nullptr);
  });

  auto *listener{us_socket_context_listen(SSL, context, "127.0.0.1", 0, 0,
                                          sizeof(Connection *))};
  if (listener == nullptr) {
    std::cerr << "Could not listen on IPv4 loopback\n";
    us_socket_context_free(SSL, context);
    us_loop_free(loop);
    return 1;
  }
  // uSockets represents a listening socket as a socket with extra state.
  const auto port{
      us_socket_local_port(SSL, reinterpret_cast<us_socket_t *>(listener))};
  if (port <= 0) {
    std::cerr << "Could not determine the loopback port\n";
    us_listen_socket_close(SSL, listener);
    us_socket_context_free(SSL, context);
    us_loop_free(loop);
    return 1;
  }
  // The test must receive the announcement before the loop starts waiting.
  std::cout << port << '\n' << std::flush;
  us_loop_run(loop);
  us_socket_context_free(SSL, context);
  us_loop_free(loop);
}

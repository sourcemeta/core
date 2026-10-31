// Application deployment policies, not additional MCP protocol requirements.
//
// Ingress: give read_message a transport-framed, decompressed message stream.
// Bound compressed bytes, decompression, headers, read time and concurrency in
// the transport too. Do not buffer an unbounded body before calling this code.
// The lexical depth check only counts containers outside strings; Core still
// parses and rejects malformed JSON. These limits are not a complete sandbox.
// After parsing, use mcp_validate_request_meta and the relevant method checks
// before dispatching any operation. None of these helpers authenticates users.
//
// Resources: principal comes from verified transport authentication, never from
// JSON arguments. Authorize the exact registered identifier before touching its
// backing resource. This example deliberately has no arbitrary URI dereference,
// path conversion, redirects or network access. A filesystem/HTTP handler needs
// its own containment, race and egress controls; URI syntax is not permission.
//
// Continuations: keep all authoritative state on the server. Issue an opaque,
// unguessable token (for example base64url_encode(random_bytes(32)) with Core's
// system crypto provider, NOT its non-cryptographic reference random backend).
// Fixed tokens below are test fixtures only. Authenticate each retry and bind
// its token to the principal, operation, expiry and saved inputRequests. The
// operation identifies the original parameters as well as the method. Do not
// put mutable business state in client-controlled requestState. Core checks the
// response shapes and token echo; the application controls ownership and use.
// Consume once before execution. A throwing handler burns the token too: this
// is at-most-once admission, not exactly-once side effects or a retry protocol.
// This example synchronizes one in-memory record. Multiple workers need shared,
// atomic compare-and-consume storage; loss/restart must fail closed. Bound and
// expire pending records, protect tokens from logs, and recheck authorization
// and application schemas before any continued side effects.
// The principal, operation identity and clock supplied to resume are trusted
// server inputs, not values taken from the client's JSON document.

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/test.h>

#include <atomic>
#include <chrono>
#include <cstddef>
#include <functional>
#include <istream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

namespace {

auto read_message(std::istream &input, const std::size_t max_bytes,
                  const std::size_t max_depth) -> sourcemeta::core::JSON {
  std::string body;
  std::size_t depth{0};
  bool quoted{false};
  bool escaped{false};
  char character{};
  while (input.get(character)) {
    // Read at most one sentinel byte past the limit, without appending it.
    if (body.size() == max_bytes) {
      throw std::length_error{"Message byte limit exceeded"};
    }
    if (quoted) {
      if (escaped) {
        escaped = false;
      } else if (character == '\\') {
        escaped = true;
      } else if (character == '"') {
        quoted = false;
      }
    } else if (character == '"') {
      quoted = true;
    } else if (character == '[' || character == '{') {
      if (depth == max_depth) {
        throw std::length_error{"Message depth limit exceeded"};
      }
      ++depth;
    } else if ((character == ']' || character == '}') && depth > 0) {
      --depth;
    }
    body.push_back(character);
  }
  if (input.bad() || !input.eof()) {
    throw std::runtime_error{"Message read failed"};
  }
  return sourcemeta::core::parse_json(body);
}

auto read_resource(const std::string_view principal, const std::string_view uri,
                   const std::function<void()> &execute) -> bool {
  // A deliberately small, exact ACL for a registered logical resource.
  if (principal != "alice" || uri != "urn:reports:quarterly") {
    return false;
  }
  execute();
  return true;
}

struct PendingContinuation {
  std::string token;
  std::string principal;
  std::string operation;
  std::chrono::steady_clock::time_point expires;
  sourcemeta::core::JSON input_requests;
  bool consumed{false};
  std::mutex mutex{};
};

auto resume(PendingContinuation &pending, const std::string_view principal,
            const std::string_view operation,
            const std::chrono::steady_clock::time_point now,
            const sourcemeta::core::JSON &parameters,
            const std::function<void()> &execute) -> bool {
  {
    const std::scoped_lock lock{pending.mutex};
    if (pending.consumed || now >= pending.expires ||
        principal != pending.principal || operation != pending.operation ||
        !parameters.is_object() || !parameters.defines("requestState") ||
        !parameters.at("requestState").is_string() ||
        !sourcemeta::core::secure_equals(
            pending.token, parameters.at("requestState").to_string()) ||
        !sourcemeta::core::mcp_validate_continuation(
            sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters,
            pending.input_requests, pending.token)) {
      return false;
    }
    pending.consumed = true;
  }
  execute();
  return true;
}

} // namespace

TEST(message_at_byte_and_depth_limits_parses) {
  std::istringstream input{"{\"x\":[]}"};
  EXPECT_EQ(read_message(input, 8, 2),
            sourcemeta::core::parse_json(R"JSON({ "x": [] })JSON"));
}

TEST(oversized_message_is_rejected_before_parsing) {
  // The invalid first byte would otherwise raise JSONParseError.
  std::istringstream input{"xxxxx"};
  try {
    [[maybe_unused]] const auto result{read_message(input, 4, 2)};
    FAIL();
  } catch (const std::length_error &) {
    // expected
  }
  EXPECT_EQ(input.tellg(), std::streampos{5});
}

TEST(valid_json_over_byte_limit_is_rejected) {
  std::istringstream input{"{\"x\":[]}"};
  try {
    [[maybe_unused]] const auto result{read_message(input, 7, 2)};
    FAIL();
  } catch (const std::length_error &) {
    // expected
  }
}

TEST(failed_transport_read_is_rejected) {
  std::istringstream input{"{}"};
  input.setstate(std::ios::badbit);
  try {
    [[maybe_unused]] const auto result{read_message(input, 32, 2)};
    FAIL();
  } catch (const std::runtime_error &) {
    // expected
  }
}

TEST(nested_message_is_rejected_before_parsing) {
  std::istringstream input{"[[[0]]]"};
  try {
    [[maybe_unused]] const auto result{read_message(input, 32, 2)};
    FAIL();
  } catch (const std::length_error &) {
    // expected
  }
  EXPECT_EQ(input.tellg(), std::streampos{3});
}

TEST(quoted_and_escaped_brackets_do_not_count_as_depth) {
  std::istringstream input{R"JSON({ "x": "[\"{\\[" })JSON"};
  EXPECT_EQ(read_message(input, 64, 1).at("x"),
            sourcemeta::core::JSON{"[\"{\\["});
}

TEST(bounded_malformed_json_is_still_rejected) {
  std::istringstream input{"{]"};
  try {
    [[maybe_unused]] const auto result{read_message(input, 32, 2)};
    FAIL();
  } catch (const sourcemeta::core::JSONParseError &) {
    // expected
  }
}

TEST(authorized_resource_executes_once) {
  unsigned int calls{0};
  EXPECT_TRUE(
      read_resource("alice", "urn:reports:quarterly", [&calls]() { ++calls; }));
  EXPECT_EQ(calls, 1U);
}

TEST(unauthorized_principal_never_reads_resource) {
  unsigned int calls{0};
  EXPECT_FALSE(
      read_resource("bob", "urn:reports:quarterly", [&calls]() { ++calls; }));
  EXPECT_EQ(calls, 0U);
}

TEST(unregistered_file_uri_never_reaches_handler) {
  unsigned int calls{0};
  EXPECT_FALSE(read_resource("alice", "file:///reports/../secret",
                             [&calls]() { ++calls; }));
  EXPECT_EQ(calls, 0U);
}

TEST(unregistered_network_uri_never_reaches_handler) {
  unsigned int calls{0};
  EXPECT_FALSE(read_resource("alice", "http://169.254.169.254/",
                             [&calls]() { ++calls; }));
  EXPECT_EQ(calls, 0U);
}

TEST(valid_continuation_executes_once_and_replay_is_rejected) {
  const auto now{std::chrono::steady_clock::now()};
  PendingContinuation pending{.token = "opaque-token",
                              .principal = "alice",
                              .operation = "read-quarterly",
                              .expires = now + std::chrono::seconds{60},
                              .input_requests =
                                  sourcemeta::core::parse_json(R"JSON({
    "roots": { "method": "roots/list" }
  })JSON")};
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "opaque-token",
    "inputResponses": {
      "roots": { "roots": [{ "uri": "file:///reports" }] }
    }
  })JSON")};
  unsigned int calls{0};
  EXPECT_TRUE(resume(pending, "alice", "read-quarterly", now, parameters,
                     [&calls]() { ++calls; }));
  // Core's protocol check alone intentionally has no replay memory.
  EXPECT_TRUE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters,
      pending.input_requests, pending.token));
  EXPECT_FALSE(resume(pending, "alice", "read-quarterly", now, parameters,
                      [&calls]() { ++calls; }));
  EXPECT_EQ(calls, 1U);
}

TEST(tampered_continuation_never_executes) {
  const auto now{std::chrono::steady_clock::now()};
  PendingContinuation pending{.token = "opaque-token",
                              .principal = "alice",
                              .operation = "read-quarterly",
                              .expires = now + std::chrono::seconds{60},
                              .input_requests =
                                  sourcemeta::core::JSON::make_object()};
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "opaque-tokeX"
  })JSON")};
  unsigned int calls{0};
  EXPECT_FALSE(resume(pending, "alice", "read-quarterly", now, parameters,
                      [&calls]() { ++calls; }));
  EXPECT_EQ(calls, 0U);
  EXPECT_FALSE(pending.consumed);
}

TEST(expired_continuation_never_executes) {
  const auto now{std::chrono::steady_clock::now()};
  PendingContinuation pending{.token = "opaque-token",
                              .principal = "alice",
                              .operation = "read-quarterly",
                              .expires = now,
                              .input_requests =
                                  sourcemeta::core::JSON::make_object()};
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "opaque-token"
  })JSON")};
  unsigned int calls{0};
  EXPECT_FALSE(resume(pending, "alice", "read-quarterly", now, parameters,
                      [&calls]() { ++calls; }));
  EXPECT_EQ(calls, 0U);
}

TEST(continuation_cannot_move_between_principals) {
  const auto now{std::chrono::steady_clock::now()};
  PendingContinuation pending{.token = "opaque-token",
                              .principal = "alice",
                              .operation = "read-quarterly",
                              .expires = now + std::chrono::seconds{60},
                              .input_requests =
                                  sourcemeta::core::JSON::make_object()};
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "opaque-token"
  })JSON")};
  unsigned int calls{0};
  EXPECT_FALSE(resume(pending, "bob", "read-quarterly", now, parameters,
                      [&calls]() { ++calls; }));
  EXPECT_EQ(calls, 0U);
}

TEST(continuation_cannot_move_between_operations) {
  const auto now{std::chrono::steady_clock::now()};
  PendingContinuation pending{.token = "opaque-token",
                              .principal = "alice",
                              .operation = "read-quarterly",
                              .expires = now + std::chrono::seconds{60},
                              .input_requests =
                                  sourcemeta::core::JSON::make_object()};
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "opaque-token"
  })JSON")};
  unsigned int calls{0};
  EXPECT_FALSE(resume(pending, "alice", "delete-quarterly", now, parameters,
                      [&calls]() { ++calls; }));
  EXPECT_EQ(calls, 0U);
}

TEST(invalid_continuation_does_not_consume_valid_state) {
  const auto now{std::chrono::steady_clock::now()};
  PendingContinuation pending{.token = "opaque-token",
                              .principal = "alice",
                              .operation = "read-quarterly",
                              .expires = now + std::chrono::seconds{60},
                              .input_requests =
                                  sourcemeta::core::parse_json(R"JSON({
    "roots": { "method": "roots/list" }
  })JSON")};
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "opaque-token",
    "inputResponses": {
      "roots": { "roots": "invalid" }
    }
  })JSON")};
  unsigned int calls{0};
  EXPECT_FALSE(resume(pending, "alice", "read-quarterly", now, parameters,
                      [&calls]() { ++calls; }));
  EXPECT_EQ(calls, 0U);
  EXPECT_FALSE(pending.consumed);
}

TEST(throwing_handler_does_not_make_continuation_reusable) {
  const auto now{std::chrono::steady_clock::now()};
  PendingContinuation pending{.token = "opaque-token",
                              .principal = "alice",
                              .operation = "read-quarterly",
                              .expires = now + std::chrono::seconds{60},
                              .input_requests =
                                  sourcemeta::core::JSON::make_object()};
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "opaque-token"
  })JSON")};
  unsigned int calls{0};
  try {
    [[maybe_unused]] const auto result{
        resume(pending, "alice", "read-quarterly", now, parameters, [&calls]() {
          ++calls;
          throw std::runtime_error{"Handler failed after a side effect"};
        })};
    FAIL();
  } catch (const std::runtime_error &) {
    // expected
  }
  EXPECT_FALSE(resume(pending, "alice", "read-quarterly", now, parameters,
                      [&calls]() { ++calls; }));
  EXPECT_EQ(calls, 1U);
}

TEST(concurrent_replay_admits_only_one_handler) {
  const auto now{std::chrono::steady_clock::now()};
  PendingContinuation pending{.token = "opaque-token",
                              .principal = "alice",
                              .operation = "read-quarterly",
                              .expires = now + std::chrono::seconds{60},
                              .input_requests =
                                  sourcemeta::core::JSON::make_object()};
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "opaque-token"
  })JSON")};
  std::atomic<unsigned int> calls{0};
  std::atomic<unsigned int> accepted{0};
  std::jthread first{[&]() {
    if (resume(pending, "alice", "read-quarterly", now, parameters,
               [&calls]() { ++calls; })) {
      ++accepted;
    }
  }};
  std::jthread second{[&]() {
    if (resume(pending, "alice", "read-quarterly", now, parameters,
               [&calls]() { ++calls; })) {
      ++accepted;
    }
  }};
  first.join();
  second.join();
  EXPECT_EQ(calls.load(), 1U);
  EXPECT_EQ(accepted.load(), 1U);
}

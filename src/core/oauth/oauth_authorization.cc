#include <sourcemeta/core/oauth_authorization.h>

#include <sourcemeta/core/uri.h>

#include "oauth_decode.h"

#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

namespace {

auto oauth_append_parameter(std::string &sink, char &separator,
                            const std::string_view name,
                            const std::string_view value) -> void {
  if (separator != '\0') {
    sink.push_back(separator);
  }

  separator = '&';
  // RFC 6749 Section 4.1.1: the query is application/x-www-form-urlencoded, so
  // the name is escaped too, which is a no-op for the fixed keys but keeps a
  // caller-supplied resource or extra name from corrupting the query
  URI::escape(name, sink);
  sink.push_back('=');
  URI::escape(value, sink);
}

} // namespace

auto oauth_build_authorization_url(const std::string_view endpoint,
                                   const OAuthAuthorizationRequest &request,
                                   std::string &sink) -> void {
  // A lower bound covering the endpoint, the known keys and separators, and the
  // raw value lengths, over which percent-escaping only ever grows
  sink.reserve(sink.size() + endpoint.size() + request.client_id.size() +
               request.redirect_uri.size() + request.scope.size() +
               request.state.size() + request.code_challenge.size() +
               request.code_challenge_method.size() +
               request.request_uri.size() + request.dpop_jkt.size() + 128);
  sink.append(endpoint);
  // The separator before the first parameter: '?' opens a query the endpoint
  // lacks, nothing when the endpoint already ends its query with '?' or '&',
  // and '&' continues an existing query (RFC 6749 Section 3.1)
  char separator{'?'};
  if (endpoint.find('?') != std::string_view::npos) {
    separator =
        (endpoint.empty() || endpoint.back() == '?' || endpoint.back() == '&')
            ? '\0'
            : '&';
  }

  // RFC 6749 Section 4.1.1: the authorization code flow is the only response
  // type this builder emits, since the implicit grant is not represented
  oauth_append_parameter(sink, separator, "response_type", "code");

  if (!request.client_id.empty()) {
    oauth_append_parameter(sink, separator, "client_id", request.client_id);
  }

  if (!request.redirect_uri.empty()) {
    oauth_append_parameter(sink, separator, "redirect_uri",
                           request.redirect_uri);
  }

  if (!request.scope.empty()) {
    oauth_append_parameter(sink, separator, "scope", request.scope);
  }

  if (!request.state.empty()) {
    oauth_append_parameter(sink, separator, "state", request.state);
  }

  if (!request.code_challenge.empty()) {
    oauth_append_parameter(sink, separator, "code_challenge",
                           request.code_challenge);
    // RFC 7636 Section 4.3: the method qualifies a challenge, so it is
    // meaningless and omitted when no challenge is present
    if (!request.code_challenge_method.empty()) {
      oauth_append_parameter(sink, separator, "code_challenge_method",
                             request.code_challenge_method);
    }
  }

  if (!request.request_uri.empty()) {
    oauth_append_parameter(sink, separator, "request_uri", request.request_uri);
  }

  if (!request.dpop_jkt.empty()) {
    oauth_append_parameter(sink, separator, "dpop_jkt", request.dpop_jkt);
  }

  for (const auto &resource : request.resources) {
    oauth_append_parameter(sink, separator, resource.name, resource.value);
  }

  for (const auto &parameter : request.extra) {
    oauth_append_parameter(sink, separator, parameter.name, parameter.value);
  }
}

auto oauth_parse_authorization_response(const std::string_view query,
                                        std::string &storage,
                                        OAuthAuthorizationResponse &result)
    -> bool {
  result = {};
  // A single up-front reserve keeps every later decode from reallocating the
  // arena and dangling an earlier borrowed view (design convention 1)
  storage.reserve(storage.size() + query.size());
  const URI::Query parsed{query};
  bool has_code{false};
  bool has_state{false};
  bool has_iss{false};
  bool has_error{false};
  bool has_error_description{false};
  bool has_error_uri{false};
  for (const auto &parameter : parsed) {
    const auto name{parameter.first};
    const auto value{parameter.second};
    // RFC 6749 Section 3.1: "Request and response parameters MUST NOT be
    // included more than once", so a duplicate is malformed, while an
    // unrecognized parameter is ignored (RFC 6749 Section 4.1.2)
    if (name == "code") {
      if (has_code) {
        return false;
      }

      has_code = true;
      if (!oauth_form_decode_into(value, storage, result.code)) {
        return false;
      }
    } else if (name == "state") {
      if (has_state) {
        return false;
      }

      has_state = true;
      if (!oauth_form_decode_into(value, storage, result.state)) {
        return false;
      }
    } else if (name == "iss") {
      if (has_iss) {
        return false;
      }

      has_iss = true;
      if (!oauth_form_decode_into(value, storage, result.iss)) {
        return false;
      }
    } else if (name == "error") {
      if (has_error) {
        return false;
      }

      has_error = true;
      if (!oauth_form_decode_into(value, storage, result.error)) {
        return false;
      }
    } else if (name == "error_description") {
      if (has_error_description) {
        return false;
      }

      has_error_description = true;
      if (!oauth_form_decode_into(value, storage, result.error_description)) {
        return false;
      }
    } else if (name == "error_uri") {
      if (has_error_uri) {
        return false;
      }

      has_error_uri = true;
      if (!oauth_form_decode_into(value, storage, result.error_uri)) {
        return false;
      }
    }
  }

  return true;
}

} // namespace sourcemeta::core

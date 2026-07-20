#include <sourcemeta/core/oauth_client_authentication.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/uri.h>

#include "oauth_encode.h"

#include <string_view> // std::string_view

namespace sourcemeta::core {

auto oauth_client_secret_basic(const std::string_view client_id,
                               const std::string_view client_secret,
                               SecureString &sink) -> void {
  // RFC 6749 Section 2.3.1: each half is percent-encoded before it is joined
  // with a colon, so a colon inside the identifier or secret cannot be mistaken
  // for the delimiter, and only then is the pair Base64 encoded
  SecureString credential;
  credential.reserve(client_id.size() + client_secret.size() + 1);
  URI::escape(client_id, credential);
  credential.push_back(':');
  URI::escape(client_secret, credential);

  sink.append("Basic ");
  base64_encode(std::string_view{credential}, sink);
}

auto oauth_client_secret_post(const std::string_view client_id,
                              const std::string_view client_secret,
                              SecureString &sink) -> void {
  oauth_append_form_parameter(sink, "client_id", client_id);
  oauth_append_form_parameter(sink, "client_secret", client_secret);
}

auto oauth_client_id_only(const std::string_view client_id, SecureString &sink)
    -> void {
  oauth_append_form_parameter(sink, "client_id", client_id);
}

} // namespace sourcemeta::core

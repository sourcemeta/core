#include <sourcemeta/core/oidc_subject.h>

#include <sourcemeta/core/crypto.h>

#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace sourcemeta::core {

auto oidc_subject_type_name(const OIDCSubjectType type) noexcept
    -> std::string_view {
  switch (type) {
    case OIDCSubjectType::Public:
      return "public";
    case OIDCSubjectType::Pairwise:
      return "pairwise";
  }

  std::unreachable();
}

auto to_oidc_subject_type(const std::string_view name) noexcept
    -> std::optional<OIDCSubjectType> {
  if (name == "public") {
    return OIDCSubjectType::Public;
  } else if (name == "pairwise") {
    return OIDCSubjectType::Pairwise;
  } else {
    return std::nullopt;
  }
}

auto oidc_pairwise_subject(const std::string_view sector_identifier,
                           const std::string_view local_account_identifier,
                           const std::string_view salt) -> std::string {
  // OpenID Connect Core 1.0 Section 8.1: the pairwise value is derived from the
  // sector identifier, a local account identifier, and a provider secret. A
  // keyed HMAC over the sector and account, separated by a space so the fields
  // cannot run together, is non-reversible and distinct per sector
  std::string message;
  message.reserve(sector_identifier.size() + 1 +
                  local_account_identifier.size());
  message.append(sector_identifier);
  message.push_back(' ');
  message.append(local_account_identifier);

  const auto digest{hmac_sha256_digest(salt, message)};
  return base64url_encode(digest);
}

} // namespace sourcemeta::core

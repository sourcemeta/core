#include <sourcemeta/core/oidc_claims.h>

#include <sourcemeta/core/json.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <functional>  // std::function
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::core {

namespace {

using namespace std::literals::string_view_literals;

constexpr auto HASH_USERINFO{JSON::Object::hash("userinfo"sv)};
constexpr auto HASH_ID_TOKEN{JSON::Object::hash("id_token"sv)};
constexpr auto HASH_ESSENTIAL{JSON::Object::hash("essential"sv)};

// OpenID Connect Core 1.0 Section 5.1: the standard claims
constexpr std::array<std::string_view, 20> STANDARD_CLAIMS{
    {"sub",
     "name",
     "given_name",
     "family_name",
     "middle_name",
     "nickname",
     "preferred_username",
     "profile",
     "picture",
     "website",
     "email",
     "email_verified",
     "gender",
     "birthdate",
     "zoneinfo",
     "locale",
     "phone_number",
     "phone_number_verified",
     "address",
     "updated_at"}};

// OpenID Connect Core 1.0 Section 5.4: the profile scope claim set
constexpr std::array<std::string_view, 14> PROFILE_CLAIMS{
    {"name", "family_name", "given_name", "middle_name", "nickname",
     "preferred_username", "profile", "picture", "website", "gender",
     "birthdate", "zoneinfo", "locale", "updated_at"}};

constexpr std::array<std::string_view, 2> EMAIL_CLAIMS{
    {"email", "email_verified"}};

constexpr std::array<std::string_view, 1> ADDRESS_CLAIMS{{"address"}};

constexpr std::array<std::string_view, 2> PHONE_CLAIMS{
    {"phone_number", "phone_number_verified"}};

auto emit_claims(const std::span<const std::string_view> claims,
                 const std::function<void(std::string_view)> &on_claim)
    -> void {
  for (const auto claim : claims) {
    on_claim(claim);
  }
}

auto assign_claim_requests(JSON &document, const std::string_view target,
                           const JSON::Object::hash_type hash,
                           const std::span<const OIDCClaimRequest> claims)
    -> void {
  if (claims.empty()) {
    return;
  }

  auto member{JSON::make_object()};
  for (const auto &request : claims) {
    if (request.essential) {
      auto specification{JSON::make_object()};
      specification.assign_assume_new("essential", JSON{true}, HASH_ESSENTIAL);
      member.assign(request.name, std::move(specification));
    } else {
      member.assign(request.name, JSON{nullptr});
    }
  }

  document.assign_assume_new(std::string{target}, std::move(member), hash);
}

} // namespace

auto oidc_is_standard_claim(const std::string_view name) noexcept -> bool {
  for (const auto claim : STANDARD_CLAIMS) {
    if (claim == name) {
      return true;
    }
  }

  return false;
}

auto oidc_scope_to_claims(const std::string_view scopes,
                          const std::function<void(std::string_view)> &on_claim)
    -> void {
  bool has_openid{false};
  bool has_profile{false};
  bool has_email{false};
  bool has_address{false};
  bool has_phone{false};

  std::size_t position{0};
  while (position <= scopes.size()) {
    const auto space{scopes.find(' ', position)};
    const auto value{space == std::string_view::npos
                         ? scopes.substr(position)
                         : scopes.substr(position, space - position)};
    if (value == "openid") {
      has_openid = true;
    } else if (value == "profile") {
      has_profile = true;
    } else if (value == "email") {
      has_email = true;
    } else if (value == "address") {
      has_address = true;
    } else if (value == "phone") {
      has_phone = true;
    }

    if (space == std::string_view::npos) {
      break;
    }

    position = space + 1;
  }

  // OpenID Connect Core 1.0 Section 5.4: openid yields sub, and each profile
  // scope yields its claim set. The standard scopes are disjoint, so a claim is
  // reported at most once
  if (has_openid) {
    on_claim("sub");
  }

  if (has_profile) {
    emit_claims(PROFILE_CLAIMS, on_claim);
  }

  if (has_email) {
    emit_claims(EMAIL_CLAIMS, on_claim);
  }

  if (has_address) {
    emit_claims(ADDRESS_CLAIMS, on_claim);
  }

  if (has_phone) {
    emit_claims(PHONE_CLAIMS, on_claim);
  }
}

auto oidc_build_claims_parameter(
    const std::span<const OIDCClaimRequest> userinfo_claims,
    const std::span<const OIDCClaimRequest> id_token_claims) -> JSON {
  auto document{JSON::make_object()};
  assign_claim_requests(document, "userinfo", HASH_USERINFO, userinfo_claims);
  assign_claim_requests(document, "id_token", HASH_ID_TOKEN, id_token_claims);
  return document;
}

auto oidc_claims_parameter_requests(const JSON &claims,
                                    const std::string_view target,
                                    const std::string_view claim) -> bool {
  if (!claims.is_object() || !claims.defines(target)) {
    return false;
  }

  const auto &target_object{claims.at(target)};
  return target_object.is_object() && target_object.defines(claim);
}

auto oidc_claims_parameter_is_essential(const JSON &claims,
                                        const std::string_view target,
                                        const std::string_view claim) -> bool {
  if (!claims.is_object() || !claims.defines(target)) {
    return false;
  }

  const auto &target_object{claims.at(target)};
  if (!target_object.is_object() || !target_object.defines(claim)) {
    return false;
  }

  const auto &specification{target_object.at(claim)};
  if (!specification.is_object() || !specification.defines("essential")) {
    return false;
  }

  const auto &essential{specification.at("essential")};
  return essential.is_boolean() && essential.to_boolean();
}

} // namespace sourcemeta::core

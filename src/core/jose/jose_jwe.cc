#include <sourcemeta/core/jose_jwe.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/text.h>

#include <optional>      // std::optional, std::nullopt
#include <string_view>   // std::string_view
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move

namespace {
using namespace std::string_view_literals;

constexpr auto HASH_ALG{sourcemeta::core::JSON::Object::hash("alg"sv)};
constexpr auto HASH_ENC{sourcemeta::core::JSON::Object::hash("enc"sv)};
constexpr auto HASH_CRIT{sourcemeta::core::JSON::Object::hash("crit"sv)};
constexpr auto HASH_KID{sourcemeta::core::JSON::Object::hash("kid"sv)};

// The JSON layer preserves repeated members rather than collapsing them, so
// uniqueness is checked here (RFC 7516 Section 4, RFC 8725 Section 2.4)
auto has_unique_members(const sourcemeta::core::JSON &object) -> bool {
  std::unordered_set<std::string_view> names;
  for (const auto &entry : object.as_object()) {
    if (!names.emplace(entry.first).second) {
      return false;
    }
  }

  return true;
}

} // namespace

namespace sourcemeta::core {

auto JWE::parse(const std::string_view input, JWE &result) -> bool {
  // The compact serialization is exactly five base64url segments joined by dots
  // (RFC 7516 Section 7.1)
  const auto first{split_once(input, '.')};
  if (!first.has_value()) {
    return false;
  }

  const auto second{split_once(first->second, '.')};
  if (!second.has_value()) {
    return false;
  }

  const auto third{split_once(second->second, '.')};
  if (!third.has_value()) {
    return false;
  }

  const auto fourth{split_once(third->second, '.')};
  if (!fourth.has_value()) {
    return false;
  }

  const auto header_segment{first->first};
  const auto encrypted_key_segment{second->first};
  const auto iv_segment{third->first};
  const auto ciphertext_segment{fourth->first};
  const auto tag_segment{fourth->second};
  if (tag_segment.find('.') != std::string_view::npos) {
    return false;
  }

  auto header_bytes{base64url_decode(header_segment)};
  auto encrypted_key_bytes{base64url_decode(encrypted_key_segment)};
  auto iv_bytes{base64url_decode(iv_segment)};
  auto ciphertext_bytes{base64url_decode(ciphertext_segment)};
  auto tag_bytes{base64url_decode(tag_segment)};
  if (!header_bytes.has_value() || !encrypted_key_bytes.has_value() ||
      !iv_bytes.has_value() || !ciphertext_bytes.has_value() ||
      !tag_bytes.has_value()) {
    return false;
  }

  auto header_json{try_parse_json(header_bytes.value())};
  if (!header_json.has_value() || !header_json.value().is_object() ||
      !has_unique_members(header_json.value())) {
    return false;
  }

  // The algorithm and content encryption header parameters are required and
  // must be strings (RFC 7516 Sections 4.1.1 and 4.1.2)
  const auto *algorithm{header_json.value().try_at("alg", HASH_ALG)};
  const auto *encryption{header_json.value().try_at("enc", HASH_ENC)};
  if (algorithm == nullptr || !algorithm->is_string() ||
      encryption == nullptr || !encryption->is_string()) {
    return false;
  }

  // Critical header extensions are not understood and must be rejected (RFC
  // 7516 Section 4.1.11)
  if (header_json.value().try_at("crit", HASH_CRIT) != nullptr) {
    return false;
  }

  result.algorithm_ = to_jwe_algorithm(algorithm->to_string());
  result.encryption_ = to_jwe_encryption(encryption->to_string());
  result.protected_header_ = header_segment;
  result.encrypted_key_ = std::move(encrypted_key_bytes).value();
  result.initialization_vector_ = std::move(iv_bytes).value();
  result.ciphertext_ = std::move(ciphertext_bytes).value();
  result.tag_ = std::move(tag_bytes).value();
  result.header_ = std::move(header_json).value();
  return true;
}

JWE::JWE(const std::string_view input) {
  if (!parse(input, *this)) {
    throw JWEParseError{};
  }
}

auto JWE::from(const std::string_view input) -> std::optional<JWE> {
  JWE result;
  if (parse(input, result)) {
    return result;
  }

  return std::nullopt;
}

auto JWE::key_id() const noexcept -> std::optional<std::string_view> {
  const auto *member{this->header_.try_at("kid"sv, HASH_KID)};
  if (member == nullptr || !member->is_string()) {
    return std::nullopt;
  }

  return std::string_view{member->to_string()};
}

} // namespace sourcemeta::core

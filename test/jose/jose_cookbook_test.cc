#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>

#include <filesystem>  // std::filesystem::path
#include <string_view> // std::string_view

// The IETF JOSE cookbook (RFC 7520) signs a plain text payload that is not a
// JSON object, so its examples are valid signatures but not tokens. They are
// verified here through the payload-agnostic signature engine rather than the
// token layer. Each file carries the public key, the algorithm, and the compact
// serialization whose first two segments are the signing input
static auto verify_cookbook_signature(const std::string_view filename) -> bool {
  const std::filesystem::path path{std::filesystem::path{JOSE_COOKBOOK_PATH} /
                                   "jws" / filename};
  auto stream{sourcemeta::core::read_file(path)};
  const auto document{sourcemeta::core::parse_json(stream)};

  const auto &compact{document.at("output").at("compact").to_string()};
  const auto separator{compact.rfind('.')};
  const std::string_view signing_input{
      std::string_view{compact}.substr(0, separator)};
  const auto signature{sourcemeta::core::base64url_decode(
      std::string_view{compact}.substr(separator + 1))};

  const auto &source_key{document.at("input").at("key")};
  auto key{sourcemeta::core::JSON::make_object()};
  key.assign("kty", source_key.at("kty"));
  if (source_key.at("kty").to_string() == "RSA") {
    key.assign("n", source_key.at("n"));
    key.assign("e", source_key.at("e"));
  } else {
    key.assign("crv", source_key.at("crv"));
    key.assign("x", source_key.at("x"));
    key.assign("y", source_key.at("y"));
  }

  const auto parsed_key{sourcemeta::core::JWK::from(key)};
  const auto algorithm{sourcemeta::core::to_jws_algorithm(
      document.at("input").at("alg").to_string())};

  return signature.has_value() && parsed_key.has_value() &&
         sourcemeta::core::jws_verify_signature(
             algorithm, signing_input, signature.value(), parsed_key.value());
}

TEST(JOSE_cookbook, rsa_pkcs1_v15_rs256) {
  EXPECT_TRUE(verify_cookbook_signature("4_1.rsa_v15_signature.json"));
}

TEST(JOSE_cookbook, rsa_pss_ps384) {
  EXPECT_TRUE(verify_cookbook_signature("4_2.rsa-pss_signature.json"));
}

TEST(JOSE_cookbook, ecdsa_es512) {
  EXPECT_TRUE(verify_cookbook_signature("4_3.ecdsa_signature.json"));
}

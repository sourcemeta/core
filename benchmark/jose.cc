#include <benchmark/benchmark.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>

#include <cassert>     // assert
#include <string_view> // std::string_view

namespace {
constexpr std::string_view RS256_TOKEN{
    "eyJhbGciOiJSUzI1NiIsInR5cCI6ImF0K2p3dCJ9."
    "eyJpc3MiOiJhY21lIiwiYXVkIjoiY2xpZW50IiwiZXhwIjoyMDAwMDAwMDAwfQ."
    "U3ZBo7MvSW0U099gJ_"
    "vIA5T8HJ2XnKSzYmqkx7SDxgxQfmxQyu3QZIeKT68AAH7wQjWRvNWQ7f3Es57UUNUQAMs-"
    "z5TWlVBKtYZf5ZcbYqc4KrQ-ApwpjoFGJxurnd1R_"
    "tz02WssnvrZNKnxNPuGoYIkJKNCl59yLFJwRLf3nK_Jcxs-"
    "1m2MvKsm647PuXqhYOKlZkHOvkIV0RV8cLJ56_gDVjj7TlKQgwbTdW_"
    "71QLwLWRFGftU2EAWuqayTSpPeUA6kB4sfn7JNsweqDs7uev30m6y8BE9uzwzHuuovaN1cZz0o"
    "TAGXcx64sfbPs6HEMp5_FoU0SccxArAbnHSjA"};
constexpr std::string_view RS256_JWK{
    R"JSON({ "kty": "RSA", "n": "oHTpl-jfNfBuXmBp58sW8s_77UP6j2jA0mjjKjhDkxhp7Agk-xLNGgfPCS_bjdZ6YU6FGeab8uVjkSgo9_0OCJUaF4vzEGwXmNuGawANxnZtiYjWvbJlq-2mn_L7rsqGQcSkMmyM0g4aX7dF8wB6DVrXShJ78fcrNtpeoU72YGEdjehA8qVclDFwBdpCGynxxnWJePk72lQb6gkVMqKMc3jBF8GkWf8oP_sjss-fpOjSUMR1c8_0JlTYWO46KWOZa0EO2t8H1V3imMyzbhoxRd_qZHmo46gJkG-ZdebjX0vGQllaCwu0z4kLcXIfAZhqPEkdssDGhC_txwJuhaPDFQ", "e": "AQAB" })JSON"};
constexpr std::string_view ES512_TOKEN{
    "eyJhbGciOiJFUzUxMiJ9."
    "eyJpc3MiOiJhY21lIiwiYXVkIjoiY2xpZW50IiwiZXhwIjoyMDAwMDAwMDAwfQ."
    "ALruSKdoKMV4CGm3rA18G2Jqc7fG0EhVVUVEBE6LXMJCDPDDFYUPxbddk_"
    "bMROd2FaVHX5MXL9UWPV-2ltnFpZ-qARxtA_gVaLAaLhYI2u-Sy74mR_-"
    "cCUUxtYG8qH09toBV2xQiMKPRLYp4O2RvpXZKYItp0dE-zaduwY6o_Z9wNoNJ"};
constexpr std::string_view ES512_JWK{
    R"JSON({ "kty": "EC", "crv": "P-521", "x": "ASTBdxhoeBK4-OqrpidcXbsz_Gsd-TRgqkZGD9ROv7r_kBHW1_is2HDctAOVBWC3ywSUS5MZJIl17xJuYCyZA-Rg", "y": "AOQCwwmrhHkVTm9BgloMWDc7T3hw5yQM5Z-YhVumpCYyFIGuyrziD5iqsH8uQvsNXGqnMKhOJ26X8ADhvP20RIAW" })JSON"};
} // namespace

static void JOSE_VerifySignature_RS256(benchmark::State &state) {
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN)};
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(RS256_JWK))};
  assert(token.has_value());
  assert(key.has_value());
  const auto public_key{sourcemeta::core::make_rsa_public_key(
      key.value().modulus(), key.value().exponent())};
  assert(public_key.has_value());
  for (auto _ : state) {
    auto result{sourcemeta::core::rsassa_pkcs1_v15_verify(
        public_key.value(), sourcemeta::core::SignatureHashFunction::SHA256,
        token.value().signing_input(), token.value().signature())};
    assert(result);
    benchmark::DoNotOptimize(result);
  }
}

static void JOSE_VerifySignature_ES512(benchmark::State &state) {
  const auto token{sourcemeta::core::JWT::from(ES512_TOKEN)};
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(ES512_JWK))};
  assert(token.has_value());
  assert(key.has_value());
  const auto public_key{sourcemeta::core::make_ec_public_key(
      sourcemeta::core::EllipticCurve::P521, key.value().coordinate_x(),
      key.value().coordinate_y())};
  assert(public_key.has_value());
  for (auto _ : state) {
    auto result{sourcemeta::core::ecdsa_verify(
        public_key.value(), sourcemeta::core::SignatureHashFunction::SHA512,
        token.value().signing_input(), token.value().signature())};
    assert(result);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(JOSE_VerifySignature_RS256);
BENCHMARK(JOSE_VerifySignature_ES512);

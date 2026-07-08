#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint64_t
#include <filesystem>  // std::filesystem::path
#include <functional>  // std::function
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {
// Every NIST CAVP response file ignores blank lines and "#" comments, leaving
// section headers and "Key = Value" records
template <typename Callback>
auto for_each_vector_line(const std::filesystem::path &path, Callback callback)
    -> void {
  auto stream{sourcemeta::core::read_file(path)};
  sourcemeta::core::for_each_line(stream,
                                  [&callback](const std::string_view line) {
                                    if (line.empty() || line.front() == '#') {
                                      return;
                                    }

                                    callback(line);
                                  });
}

auto to_signature_hash_function(const std::string_view name)
    -> std::optional<sourcemeta::core::SignatureHashFunction> {
  if (name == "SHA256" || name == "SHA-256") {
    return sourcemeta::core::SignatureHashFunction::SHA256;
  } else if (name == "SHA384" || name == "SHA-384") {
    return sourcemeta::core::SignatureHashFunction::SHA384;
  } else if (name == "SHA512" || name == "SHA-512") {
    return sourcemeta::core::SignatureHashFunction::SHA512;
  } else {
    return std::nullopt;
  }
}

struct CurveParameters {
  sourcemeta::core::EllipticCurve curve;
  std::size_t coordinate_bytes;
};

auto to_curve(const std::string_view name) -> std::optional<CurveParameters> {
  if (name == "P-256") {
    return CurveParameters{sourcemeta::core::EllipticCurve::P256, 32};
  } else if (name == "P-384") {
    return CurveParameters{sourcemeta::core::EllipticCurve::P384, 48};
  } else if (name == "P-521") {
    return CurveParameters{sourcemeta::core::EllipticCurve::P521, 66};
  } else {
    return std::nullopt;
  }
}

// The hash functions are overloaded, so the registrars name the digest variant
// that returns a hex string through this alias
using DigestFunction = auto (*)(const std::string_view) -> std::string;

// The MAC functions are overloaded, so the registrars name the variant that
// returns a hex string through this alias
using MacFunction = auto (*)(const std::string_view, const std::string_view)
    -> std::string;

// Every vector in this file reduces to running a closure that asserts on data
// captured at registration time, so a single runner carrying that closure
// stands in for every mechanism
auto run_pyca_test_case(const std::function<void()> &assertion) -> void {
  assertion();
}

auto verify_pkcs1(const sourcemeta::core::SignatureHashFunction hash,
                  const std::string_view modulus,
                  const std::string_view exponent,
                  const std::string_view message,
                  const std::string_view signature) -> bool {
  const auto key{sourcemeta::core::make_rsa_public_key(modulus, exponent)};
  return key.has_value() && sourcemeta::core::rsassa_pkcs1_v15_verify(
                                key.value(), hash, message, signature);
}

auto verify_pss(const sourcemeta::core::SignatureHashFunction hash,
                const std::string_view modulus, const std::string_view exponent,
                const std::string_view message,
                const std::string_view signature) -> bool {
  const auto key{sourcemeta::core::make_rsa_public_key(modulus, exponent)};
  return key.has_value() && sourcemeta::core::rsassa_pss_verify(
                                key.value(), hash, message, signature);
}

auto verify_ecdsa(const sourcemeta::core::EllipticCurve curve,
                  const sourcemeta::core::SignatureHashFunction hash,
                  const std::string_view coordinate_x,
                  const std::string_view coordinate_y,
                  const std::string_view message,
                  const std::string_view signature) -> bool {
  const auto key{
      sourcemeta::core::make_ec_public_key(curve, coordinate_x, coordinate_y)};
  return key.has_value() &&
         sourcemeta::core::ecdsa_verify(key.value(), hash, message, signature);
}

auto verify_eddsa(const sourcemeta::core::EdwardsCurve curve,
                  const std::string_view public_key,
                  const std::string_view message,
                  const std::string_view signature) -> bool {
  const auto key{sourcemeta::core::make_eddsa_public_key(curve, public_key)};
  return key.has_value() &&
         sourcemeta::core::eddsa_verify(key.value(), message, signature);
}

// ECDSA and Ed25519 signing use a random nonce, so the output is not a fixed
// known answer, and the aspect under test is that a fresh signature verifies
auto sign_and_verify_ecdsa(const sourcemeta::core::EllipticCurve curve,
                           const sourcemeta::core::SignatureHashFunction hash,
                           const std::string_view scalar,
                           const std::string_view coordinate_x,
                           const std::string_view coordinate_y,
                           const std::string_view message) -> bool {
  const auto key{sourcemeta::core::make_ec_private_key(
      curve, scalar, coordinate_x, coordinate_y)};
  if (!key.has_value()) {
    return false;
  }

  const auto signature{
      sourcemeta::core::ecdsa_sign(key.value(), hash, message)};
  const auto public_key{
      sourcemeta::core::make_ec_public_key(curve, coordinate_x, coordinate_y)};
  return signature.has_value() && public_key.has_value() &&
         sourcemeta::core::ecdsa_verify(public_key.value(), hash, message,
                                        signature.value());
}

auto sign_and_verify_eddsa(const sourcemeta::core::EdwardsCurve curve,
                           const std::string_view seed,
                           const std::string_view public_key,
                           const std::string_view message) -> bool {
  const auto key{sourcemeta::core::make_edwards_private_key(curve, seed)};
  if (!key.has_value()) {
    return false;
  }

  const auto signature{sourcemeta::core::eddsa_sign(key.value(), message)};
  const auto verification_key{
      sourcemeta::core::make_eddsa_public_key(curve, public_key)};
  return signature.has_value() && verification_key.has_value() &&
         sourcemeta::core::eddsa_verify(verification_key.value(), message,
                                        signature.value());
}

auto register_case(const std::string &suite_name, const std::string &test_name,
                   std::function<void()> assertion) -> void {
  sourcemeta::core::test_register(suite_name + "." + test_name,
                                  [assertion = std::move(assertion)]() -> void {
                                    run_pyca_test_case(assertion);
                                  });
}

auto register_sha_msg_tests(const std::filesystem::path &file_path,
                            const std::string &suite_name,
                            const DigestFunction digest) -> void {
  std::string current_length;
  std::string current_message_hex;

  for_each_vector_line(file_path, [&](const std::string_view line) {
    if (line.front() == '[') {
      return;
    }

    const auto field{
        sourcemeta::core::split_once(line, std::string_view{" = "})};
    if (!field.has_value()) {
      return;
    }

    if (field->first == "Len") {
      current_length = field->second;
    } else if (field->first == "Msg") {
      current_message_hex = field->second;
    } else if (field->first == "MD") {
      const auto expected_digest{std::string{field->second}};
      const auto empty_message{current_length == "0"};
      const auto message_hex{current_message_hex};

      register_case(suite_name, "len_" + current_length, [=]() {
        const auto message{
            empty_message
                ? std::string{}
                : sourcemeta::core::hex_to_bytes(message_hex).value()};
        EXPECT_EQ(digest(message), expected_digest);
      });
    }
  });
}

auto register_sha_monte_tests(const std::filesystem::path &file_path,
                              const std::string &suite_name,
                              const DigestFunction digest) -> void {
  std::string seed;
  std::string current_count;

  for_each_vector_line(file_path, [&](const std::string_view line) {
    if (line.front() == '[') {
      return;
    }

    const auto field{
        sourcemeta::core::split_once(line, std::string_view{" = "})};
    if (!field.has_value()) {
      return;
    }

    if (field->first == "Seed") {
      seed = field->second;
    } else if (field->first == "COUNT") {
      current_count = field->second;
    } else if (field->first == "MD") {
      const auto expected_digest{std::string{field->second}};
      const auto current_seed{seed};

      register_case(suite_name, "count_" + current_count, [=]() {
        // NIST SHAVS Monte Carlo pseudocode:
        // MD[0] = MD[1] = MD[2] = Seed
        // For i = 3 to 1002:
        //   M[i] = MD[i-3] || MD[i-2] || MD[i-1]
        //   MD[i] = SHA(M[i])
        // Output MD[1002]
        auto md_0{sourcemeta::core::hex_to_bytes(current_seed).value()};
        auto md_1{md_0};
        auto md_2{md_0};
        for (std::uint64_t iteration{3}; iteration <= 1002; ++iteration) {
          auto next{sourcemeta::core::hex_to_bytes(digest(md_0 + md_1 + md_2))
                        .value()};
          md_0 = std::move(md_1);
          md_1 = std::move(md_2);
          md_2 = std::move(next);
        }

        EXPECT_EQ(sourcemeta::core::bytes_to_hex(md_2), expected_digest);
      });

      // The output becomes the seed for the next checkpoint
      seed = expected_digest;
    }
  });
}

auto register_sha_tests(const std::filesystem::path &directory,
                        const std::string &name, const DigestFunction digest)
    -> void {
  register_sha_msg_tests(directory / (name + "ShortMsg.rsp"),
                         "PyCA_Cryptography_" + name + "_ShortMsg", digest);
  register_sha_msg_tests(directory / (name + "LongMsg.rsp"),
                         "PyCA_Cryptography_" + name + "_LongMsg", digest);
  register_sha_monte_tests(directory / (name + "Monte.rsp"),
                           "PyCA_Cryptography_" + name + "_Monte", digest);
}

auto register_hmac_tests(const std::filesystem::path &file_path,
                         const std::string &suite_name,
                         const MacFunction function) -> void {
  std::string key_hex;
  std::string message_hex;
  std::uint64_t case_count{0};

  for_each_vector_line(file_path, [&](const std::string_view line) {
    const auto field{
        sourcemeta::core::split_once(line, std::string_view{" = "})};
    if (!field.has_value()) {
      return;
    }

    if (field->first == "Key") {
      key_hex = field->second;
    } else if (field->first == "Msg") {
      message_hex = field->second;
    } else if (field->first == "MD") {
      const auto expected_tag{std::string{field->second}};
      const auto current_key{key_hex};
      const auto current_message{message_hex};
      case_count += 1;
      register_case(suite_name, "case_" + std::to_string(case_count), [=]() {
        EXPECT_EQ(
            function(sourcemeta::core::hex_to_bytes(current_key).value(),
                     sourcemeta::core::hex_to_bytes(current_message).value()),
            expected_tag);
      });
    }
  });
}

struct RSAVector {
  std::string modulus_bits;
  std::string modulus_hex;
  std::string sha_algorithm;
  std::string exponent_hex;
  std::string message_hex;
  std::string signature_hex;
};

template <typename Callback>
auto for_each_rsa_case(const std::filesystem::path &file_path,
                       Callback callback) -> void {
  RSAVector vector;
  std::uint64_t case_count{0};

  for_each_vector_line(file_path, [&](const std::string_view line) {
    if (line.front() == '[') {
      // The section header carries the modulus bit size, like "[mod = 1024]"
      const auto header{sourcemeta::core::strip_right(
          sourcemeta::core::strip_left(line, '['), ']')};
      const auto field{
          sourcemeta::core::split_once(header, std::string_view{" = "})};
      if (field.has_value()) {
        vector.modulus_bits = field->second;
      }

      return;
    }

    const auto field{
        sourcemeta::core::split_once(line, std::string_view{" = "})};
    if (!field.has_value()) {
      return;
    }

    if (field->first == "n") {
      vector.modulus_hex = field->second;
    } else if (field->first == "SHAAlg") {
      vector.sha_algorithm = field->second;
    } else if (field->first == "e") {
      vector.exponent_hex = field->second;
    } else if (field->first == "Msg") {
      vector.message_hex = field->second;
    } else if (field->first == "S") {
      vector.signature_hex = field->second;
    } else if (field->first == "Result") {
      case_count += 1;
      callback(vector, field->second, case_count);
    }
  });
}

auto register_rsa_sigver15_tests(const std::filesystem::path &file_path)
    -> void {
  for_each_rsa_case(file_path, [](const RSAVector &vector,
                                  const std::string_view result,
                                  const std::uint64_t case_count) {
    const auto hash{to_signature_hash_function(vector.sha_algorithm)};
    if (!hash.has_value()) {
      return;
    }

    const auto hash_function{hash.value()};
    const auto expected{result.starts_with("P")};
    const auto test_name{
        "mod" + vector.modulus_bits + "_" + vector.sha_algorithm + "_case_" +
        std::to_string(case_count) + "_" + (expected ? "pass" : "fail")};
    register_case(
        "PyCA_Cryptography_RSA_SigVer15", test_name,
        [vector, hash_function, expected]() {
          EXPECT_EQ(
              verify_pkcs1(
                  hash_function,
                  sourcemeta::core::hex_to_bytes(vector.modulus_hex).value(),
                  sourcemeta::core::hex_to_bytes(vector.exponent_hex).value(),
                  sourcemeta::core::hex_to_bytes(vector.message_hex).value(),
                  sourcemeta::core::hex_to_bytes(vector.signature_hex).value()),
              expected);
        });
  });
}

auto register_rsa_sigverpss_tests(const std::filesystem::path &file_path)
    -> void {
  // Every case in this suite uses a 10-byte salt, while RFC 7518 Section 3.5
  // fixes the salt size to the hash function output ("The size of the salt
  // value is the same size as the hash function output"). Verification must
  // therefore reject every case, including the ones whose signatures are
  // otherwise cryptographically valid. The suite still exercises the full
  // unmasking and consistency checking paths with realistic data
  for_each_rsa_case(file_path, [](const RSAVector &vector,
                                  const std::string_view,
                                  const std::uint64_t case_count) {
    const auto hash{to_signature_hash_function(vector.sha_algorithm)};
    if (!hash.has_value()) {
      return;
    }

    const auto hash_function{hash.value()};
    const auto test_name{"mod" + vector.modulus_bits + "_" +
                         vector.sha_algorithm + "_case_" +
                         std::to_string(case_count)};
    register_case(
        "PyCA_Cryptography_RSA_SigVerPSS", test_name,
        [vector, hash_function]() {
          EXPECT_FALSE(verify_pss(
              hash_function,
              sourcemeta::core::hex_to_bytes(vector.modulus_hex).value(),
              sourcemeta::core::hex_to_bytes(vector.exponent_hex).value(),
              sourcemeta::core::hex_to_bytes(vector.message_hex).value(),
              sourcemeta::core::hex_to_bytes(vector.signature_hex).value()));
        });
  });
}

auto register_ecdsa_sigver_tests(const std::filesystem::path &file_path)
    -> void {
  std::optional<CurveParameters> current_curve;
  std::optional<sourcemeta::core::SignatureHashFunction> current_hash;
  std::string message_hex;
  std::string coordinate_x_hex;
  std::string coordinate_y_hex;
  std::string r_hex;
  std::string s_hex;
  std::uint64_t case_count{0};

  for_each_vector_line(file_path, [&](const std::string_view line) {
    if (line.front() == '[') {
      // The section header pairs a curve and a hash, like "[P-256,SHA-256]"
      const auto header{sourcemeta::core::strip_right(
          sourcemeta::core::strip_left(line, '['), ']')};
      const auto parts{sourcemeta::core::split_once(header, ',')};
      if (parts.has_value()) {
        current_curve = to_curve(parts->first);
        current_hash = to_signature_hash_function(parts->second);
      }

      return;
    }

    const auto field{
        sourcemeta::core::split_once(line, std::string_view{" = "})};
    if (!field.has_value()) {
      return;
    }

    if (field->first == "Msg") {
      message_hex = field->second;
    } else if (field->first == "Qx") {
      coordinate_x_hex = field->second;
    } else if (field->first == "Qy") {
      coordinate_y_hex = field->second;
    } else if (field->first == "R") {
      r_hex = field->second;
    } else if (field->first == "S") {
      s_hex = field->second;
    } else if (field->first == "Result") {
      case_count += 1;
      if (!current_curve.has_value() || !current_hash.has_value()) {
        return;
      }

      const auto width{current_curve.value().coordinate_bytes};
      const auto curve{current_curve.value().curve};
      const auto hash{current_hash.value()};
      const auto expected{field->second.starts_with("P")};
      const auto test_name{"case_" + std::to_string(case_count) + "_" +
                           (expected ? "pass" : "fail")};
      register_case("PyCA_Cryptography_ECDSA_SigVer", test_name, [=]() {
        // The vectors elide leading zero nibbles, so an odd hex length means a
        // single zero was dropped, and each coordinate is padded to the curve
        // width
        const auto coordinate_x{sourcemeta::core::pad_left(
            sourcemeta::core::hex_to_bytes(coordinate_x_hex, true).value(),
            width, '\x00')};
        const auto coordinate_y{sourcemeta::core::pad_left(
            sourcemeta::core::hex_to_bytes(coordinate_y_hex, true).value(),
            width, '\x00')};
        const auto message{
            sourcemeta::core::hex_to_bytes(message_hex, true).value()};
        const auto signature{
            sourcemeta::core::pad_left(
                sourcemeta::core::hex_to_bytes(r_hex, true).value(), width,
                '\x00') +
            sourcemeta::core::pad_left(
                sourcemeta::core::hex_to_bytes(s_hex, true).value(), width,
                '\x00')};
        EXPECT_EQ(verify_ecdsa(curve, hash, coordinate_x, coordinate_y, message,
                               signature),
                  expected);
      });
    }
  });
}

auto register_ecdsa_siggen_tests(const std::filesystem::path &file_path)
    -> void {
  std::optional<CurveParameters> current_curve;
  std::optional<sourcemeta::core::SignatureHashFunction> current_hash;
  std::string message_hex;
  std::string scalar_hex;
  std::string coordinate_x_hex;
  std::string coordinate_y_hex;
  std::uint64_t case_count{0};

  for_each_vector_line(file_path, [&](const std::string_view line) {
    if (line.front() == '[') {
      // The section header pairs a curve and a hash, like "[P-256,SHA-256]"
      const auto header{sourcemeta::core::strip_right(
          sourcemeta::core::strip_left(line, '['), ']')};
      const auto parts{sourcemeta::core::split_once(header, ',')};
      if (parts.has_value()) {
        current_curve = to_curve(parts->first);
        current_hash = to_signature_hash_function(parts->second);
      }

      return;
    }

    const auto field{
        sourcemeta::core::split_once(line, std::string_view{" = "})};
    if (!field.has_value()) {
      return;
    }

    if (field->first == "Msg") {
      message_hex = field->second;
    } else if (field->first == "d") {
      scalar_hex = field->second;
    } else if (field->first == "Qx") {
      coordinate_x_hex = field->second;
    } else if (field->first == "Qy") {
      coordinate_y_hex = field->second;
    } else if (field->first == "S") {
      case_count += 1;
      if (!current_curve.has_value() || !current_hash.has_value()) {
        return;
      }

      const auto width{current_curve.value().coordinate_bytes};
      const auto curve{current_curve.value().curve};
      const auto hash{current_hash.value()};
      register_case(
          "PyCA_Cryptography_ECDSA_SigGen",
          "case_" + std::to_string(case_count), [=]() {
            // The vectors elide leading zero nibbles, so an odd hex
            // length means a single zero was dropped, and each value
            // is padded to the curve width
            const auto scalar{sourcemeta::core::pad_left(
                sourcemeta::core::hex_to_bytes(scalar_hex, true).value(), width,
                '\x00')};
            const auto coordinate_x{sourcemeta::core::pad_left(
                sourcemeta::core::hex_to_bytes(coordinate_x_hex, true).value(),
                width, '\x00')};
            const auto coordinate_y{sourcemeta::core::pad_left(
                sourcemeta::core::hex_to_bytes(coordinate_y_hex, true).value(),
                width, '\x00')};
            const auto message{
                sourcemeta::core::hex_to_bytes(message_hex, true).value()};
            EXPECT_TRUE(sign_and_verify_ecdsa(curve, hash, scalar, coordinate_x,
                                              coordinate_y, message));
          });
    }
  });
}

auto register_eddsa_25519_tests(const std::filesystem::path &file_path)
    -> void {
  std::uint64_t case_count{0};

  for_each_vector_line(file_path, [&](const std::string_view line) {
    // Each line is colon-separated: the secret (seed concatenated with the
    // public key), the public key, the message, and the signature
    // concatenated with the message (RFC 8032 reference format)
    const auto first{sourcemeta::core::split_once(line, ':')};
    if (!first.has_value()) {
      return;
    }

    const auto second{sourcemeta::core::split_once(first->second, ':')};
    if (!second.has_value()) {
      return;
    }

    const auto third{sourcemeta::core::split_once(second->second, ':')};
    if (!third.has_value()) {
      return;
    }

    const std::string public_key_hex{second->first};
    const std::string message_hex{third->first};
    const std::string signature_hex{third->second.substr(0, 128)};
    case_count += 1;
    register_case("PyCA_Cryptography_Ed25519_SigVer",
                  "case_" + std::to_string(case_count), [=]() {
                    EXPECT_TRUE(verify_eddsa(
                        sourcemeta::core::EdwardsCurve::Ed25519,
                        sourcemeta::core::hex_to_bytes(public_key_hex).value(),
                        sourcemeta::core::hex_to_bytes(message_hex).value(),
                        sourcemeta::core::hex_to_bytes(signature_hex).value()));
                  });
  });
}

auto register_eddsa_448_tests(const std::filesystem::path &file_path) -> void {
  std::string current_count;
  std::string public_key_hex;
  std::string message_hex;
  std::string signature_hex;
  bool has_context{false};

  for_each_vector_line(file_path, [&](const std::string_view line) {
    // The records are "Key = Value", but the message value may be empty, so
    // split on the first '=' and trim the surrounding spaces by hand
    const auto separator{line.find('=')};
    if (separator == std::string_view::npos) {
      return;
    }

    auto key{line.substr(0, separator)};
    while (!key.empty() && key.back() == ' ') {
      key.remove_suffix(1);
    }

    auto value{line.substr(separator + 1)};
    while (!value.empty() && value.front() == ' ') {
      value.remove_prefix(1);
    }

    if (key == "COUNT") {
      current_count = value;
      has_context = false;
    } else if (key == "PUBLIC") {
      public_key_hex = value;
    } else if (key == "MESSAGE") {
      message_hex = value;
    } else if (key == "CONTEXT") {
      // JWS uses Ed448 with an empty context (RFC 8037 Section 3.1), so the
      // contextful vectors are not applicable
      has_context = !value.empty();
    } else if (key == "SIGNATURE") {
      signature_hex = value;
      if (has_context) {
        return;
      }

      const auto public_key{public_key_hex};
      const auto message{message_hex};
      const auto signature{signature_hex};
      register_case("PyCA_Cryptography_Ed448_SigVer", "count_" + current_count,
                    [=]() {
                      EXPECT_TRUE(verify_eddsa(
                          sourcemeta::core::EdwardsCurve::Ed448,
                          sourcemeta::core::hex_to_bytes(public_key).value(),
                          sourcemeta::core::hex_to_bytes(message).value(),
                          sourcemeta::core::hex_to_bytes(signature).value()));
                    });
    }
  });
}

auto register_eddsa_25519_sign_tests(const std::filesystem::path &file_path)
    -> void {
  std::uint64_t case_count{0};

  for_each_vector_line(file_path, [&](const std::string_view line) {
    const auto first{sourcemeta::core::split_once(line, ':')};
    if (!first.has_value()) {
      return;
    }

    const auto second{sourcemeta::core::split_once(first->second, ':')};
    if (!second.has_value()) {
      return;
    }

    const auto third{sourcemeta::core::split_once(second->second, ':')};
    if (!third.has_value()) {
      return;
    }

    // The secret field is the 32-byte seed concatenated with the public key
    const std::string seed_hex{std::string{first->first}.substr(0, 64)};
    const std::string public_key_hex{second->first};
    const std::string message_hex{third->first};
    case_count += 1;
    register_case("PyCA_Cryptography_Ed25519_SigGen",
                  "case_" + std::to_string(case_count), [=]() {
                    EXPECT_TRUE(sign_and_verify_eddsa(
                        sourcemeta::core::EdwardsCurve::Ed25519,
                        sourcemeta::core::hex_to_bytes(seed_hex).value(),
                        sourcemeta::core::hex_to_bytes(public_key_hex).value(),
                        sourcemeta::core::hex_to_bytes(message_hex).value()));
                  });
  });
}

auto register_eddsa_448_sign_tests(const std::filesystem::path &file_path)
    -> void {
  std::string current_count;
  std::string secret_hex;
  std::string message_hex;
  bool has_context{false};

  for_each_vector_line(file_path, [&](const std::string_view line) {
    const auto separator{line.find('=')};
    if (separator == std::string_view::npos) {
      return;
    }

    auto key{line.substr(0, separator)};
    while (!key.empty() && key.back() == ' ') {
      key.remove_suffix(1);
    }

    auto value{line.substr(separator + 1)};
    while (!value.empty() && value.front() == ' ') {
      value.remove_prefix(1);
    }

    if (key == "COUNT") {
      current_count = value;
      has_context = false;
    } else if (key == "SECRET") {
      secret_hex = value;
    } else if (key == "MESSAGE") {
      message_hex = value;
    } else if (key == "CONTEXT") {
      // JWS uses Ed448 with an empty context (RFC 8037 Section 3.1), and only
      // an empty context yields the deterministic signature checked here
      has_context = !value.empty();
    } else if (key == "SIGNATURE") {
      if (has_context) {
        return;
      }

      const auto seed{secret_hex};
      const auto message{message_hex};
      const std::string expected_signature_hex{value};
      register_case(
          "PyCA_Cryptography_Ed448_SigGen", "count_" + current_count, [=]() {
            const auto key_pair{sourcemeta::core::make_edwards_private_key(
                sourcemeta::core::EdwardsCurve::Ed448,
                sourcemeta::core::hex_to_bytes(seed).value())};
            std::string signature_hex_got;
            if (key_pair.has_value()) {
              const auto signature{sourcemeta::core::eddsa_sign(
                  key_pair.value(),
                  sourcemeta::core::hex_to_bytes(message).value())};
              if (signature.has_value()) {
                signature_hex_got =
                    sourcemeta::core::bytes_to_hex(signature.value());
              }
            }

            EXPECT_EQ(signature_hex_got, expected_signature_hex);
          });
    }
  });
}
} // namespace

auto main(int argc, char **argv) -> int {
  const std::filesystem::path suite_path{PYCA_CRYPTOGRAPHY_PATH};
  const auto hashes_path{suite_path / "hashes"};

  register_sha_tests(hashes_path / "SHA1", "SHA1",
                     static_cast<DigestFunction>(sourcemeta::core::sha1));
  register_sha_tests(hashes_path / "SHA2", "SHA256",
                     static_cast<DigestFunction>(sourcemeta::core::sha256));
  register_sha_tests(hashes_path / "SHA2", "SHA384",
                     static_cast<DigestFunction>(sourcemeta::core::sha384));
  register_sha_tests(hashes_path / "SHA2", "SHA512",
                     static_cast<DigestFunction>(sourcemeta::core::sha512));

  register_hmac_tests(suite_path / "HMAC" / "rfc-4231-sha256.txt",
                      "PyCA_Cryptography_HMAC_SHA256",
                      sourcemeta::core::hmac_sha256);
  register_hmac_tests(suite_path / "HMAC" / "rfc-4231-sha384.txt",
                      "PyCA_Cryptography_HMAC_SHA384",
                      sourcemeta::core::hmac_sha384);
  register_hmac_tests(suite_path / "HMAC" / "rfc-4231-sha512.txt",
                      "PyCA_Cryptography_HMAC_SHA512",
                      sourcemeta::core::hmac_sha512);

  const auto rsa_path{suite_path / "asymmetric" / "RSA" / "FIPS_186-2"};
  register_rsa_sigver15_tests(rsa_path / "SigVer15_186-3.rsp");
  register_rsa_sigverpss_tests(rsa_path / "SigVerPSS_186-3.rsp");

  register_ecdsa_sigver_tests(suite_path / "asymmetric" / "ECDSA" /
                              "FIPS_186-3" / "SigVer.rsp");
  register_ecdsa_siggen_tests(suite_path / "asymmetric" / "ECDSA" /
                              "FIPS_186-3" / "SigGen.txt");

  register_eddsa_25519_tests(suite_path / "asymmetric" / "Ed25519" /
                             "sign.input");
  register_eddsa_25519_sign_tests(suite_path / "asymmetric" / "Ed25519" /
                                  "sign.input");

  register_eddsa_448_tests(suite_path / "asymmetric" / "Ed448" / "rfc8032.txt");
  register_eddsa_448_sign_tests(suite_path / "asymmetric" / "Ed448" /
                                "rfc8032.txt");

  return sourcemeta::core::test_run(argc, argv);
}

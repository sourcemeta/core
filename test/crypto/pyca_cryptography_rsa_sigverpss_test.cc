#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/text.h>

#include <cstdint>    // std::uint64_t
#include <filesystem> // std::filesystem
#include <optional>   // std::optional
#include <string>     // std::string, std::getline

static auto strip_cr(std::string &value) -> void {
  if (!value.empty() && value.back() == '\r') {
    value.pop_back();
  }
}

static auto to_hash_function(const std::string &name)
    -> std::optional<sourcemeta::core::SignatureHashFunction> {
  if (name == "SHA256") {
    return sourcemeta::core::SignatureHashFunction::SHA256;
  } else if (name == "SHA384") {
    return sourcemeta::core::SignatureHashFunction::SHA384;
  } else if (name == "SHA512") {
    return sourcemeta::core::SignatureHashFunction::SHA512;
  } else {
    return std::nullopt;
  }
}

// Every case in this suite uses a 10-byte salt, while RFC 7518 Section 3.5
// fixes the salt size to the hash function output ("The size of the salt
// value is the same size as the hash function output"). Verification must
// therefore reject every case, including the ones whose signatures are
// otherwise cryptographically valid. The suite still exercises the full
// unmasking and consistency checking paths with realistic data
class PyCA_Cryptography_RSA_SigVerPSS_Test : public testing::Test {
public:
  explicit PyCA_Cryptography_RSA_SigVerPSS_Test(
      const sourcemeta::core::SignatureHashFunction input_hash,
      std::string input_n, std::string input_e, std::string input_message,
      std::string input_signature)
      : hash{input_hash}, modulus{std::move(input_n)},
        exponent{std::move(input_e)}, message{std::move(input_message)},
        signature{std::move(input_signature)} {}

  auto TestBody() -> void override {
    EXPECT_FALSE(sourcemeta::core::rsassa_pss_verify(
        this->hash, this->modulus, this->exponent, this->message,
        this->signature));
  }

private:
  const sourcemeta::core::SignatureHashFunction hash;
  const std::string modulus;
  const std::string exponent;
  const std::string message;
  const std::string signature;
};

auto main(int argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  const std::filesystem::path suite_path{PYCA_CRYPTOGRAPHY_PATH};
  auto stream = sourcemeta::core::read_file(
      suite_path / "asymmetric" / "RSA" / "FIPS_186-2" / "SigVerPSS_186-3.rsp");

  std::string line;
  std::string modulus_bits;
  std::string modulus_hex;
  std::string sha_algorithm;
  std::string exponent_hex;
  std::string message_hex;
  std::string signature_hex;
  std::uint64_t case_count{0};

  while (std::getline(stream, line)) {
    strip_cr(line);
    if (line.empty() || line.front() == '#') {
      continue;
    }

    if (line.starts_with("[mod = ")) {
      modulus_bits = line.substr(7, line.size() - 8);
    } else if (line.starts_with("n = ")) {
      modulus_hex = line.substr(4);
    } else if (line.starts_with("SHAAlg = ")) {
      sha_algorithm = line.substr(9);
    } else if (line.starts_with("e = ")) {
      exponent_hex = line.substr(4);
    } else if (line.starts_with("Msg = ")) {
      message_hex = line.substr(6);
    } else if (line.starts_with("S = ")) {
      signature_hex = line.substr(4);
    } else if (line.starts_with("Result = ")) {
      case_count += 1;
      const auto hash{to_hash_function(sha_algorithm)};
      if (!hash.has_value()) {
        continue;
      }

      const auto modulus{sourcemeta::core::hex_to_bytes(modulus_hex).value()};
      const auto exponent{sourcemeta::core::hex_to_bytes(exponent_hex).value()};
      const auto message{sourcemeta::core::hex_to_bytes(message_hex).value()};
      const auto signature{
          sourcemeta::core::hex_to_bytes(signature_hex).value()};

      const auto test_name{"mod" + modulus_bits + "_" + sha_algorithm +
                           "_case_" + std::to_string(case_count)};
      testing::RegisterTest(
          "PyCA_Cryptography_RSA_SigVerPSS", test_name.c_str(), nullptr,
          nullptr, __FILE__, __LINE__,
          [=]() -> PyCA_Cryptography_RSA_SigVerPSS_Test * {
            return new PyCA_Cryptography_RSA_SigVerPSS_Test(
                hash.value(), modulus, exponent, message, signature);
          });
    }
  }

  return RUN_ALL_TESTS();
}

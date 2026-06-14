#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/text.h>

#include <cstddef>    // std::size_t
#include <cstdint>    // std::uint64_t
#include <filesystem> // std::filesystem
#include <optional>   // std::optional
#include <string>     // std::string, std::getline

static auto strip_cr(std::string &value) -> void {
  if (!value.empty() && value.back() == '\r') {
    value.pop_back();
  }
}

static auto left_pad(const std::string &value, const std::size_t width)
    -> std::string {
  if (value.size() >= width) {
    return value;
  }

  return std::string(width - value.size(), '\x00') + value;
}

// The vector file drops leading zero nibbles, so an odd hex length means a
// single leading zero was elided
static auto hex_field_to_bytes(const std::string &hex) -> std::string {
  const auto normalized{hex.size() % 2 == 0 ? hex : "0" + hex};
  return sourcemeta::core::hex_to_bytes(normalized).value();
}

struct CurveParameters {
  sourcemeta::core::EllipticCurve curve;
  std::size_t coordinate_bytes;
};

static auto to_curve(const std::string &name)
    -> std::optional<CurveParameters> {
  if (name == "[P-256") {
    return CurveParameters{sourcemeta::core::EllipticCurve::P256, 32};
  } else if (name == "[P-384") {
    return CurveParameters{sourcemeta::core::EllipticCurve::P384, 48};
  } else if (name == "[P-521") {
    return CurveParameters{sourcemeta::core::EllipticCurve::P521, 66};
  } else {
    return std::nullopt;
  }
}

static auto to_hash_function(const std::string &name)
    -> std::optional<sourcemeta::core::SignatureHashFunction> {
  if (name == "SHA-256]") {
    return sourcemeta::core::SignatureHashFunction::SHA256;
  } else if (name == "SHA-384]") {
    return sourcemeta::core::SignatureHashFunction::SHA384;
  } else if (name == "SHA-512]") {
    return sourcemeta::core::SignatureHashFunction::SHA512;
  } else {
    return std::nullopt;
  }
}

class PyCA_Cryptography_ECDSA_SigVer_Test : public testing::Test {
public:
  explicit PyCA_Cryptography_ECDSA_SigVer_Test(
      const sourcemeta::core::EllipticCurve input_curve,
      const sourcemeta::core::SignatureHashFunction input_hash,
      std::string input_x, std::string input_y, std::string input_message,
      std::string input_signature, const bool input_expected)
      : curve{input_curve}, hash{input_hash}, coordinate_x{std::move(input_x)},
        coordinate_y{std::move(input_y)}, message{std::move(input_message)},
        signature{std::move(input_signature)}, expected{input_expected} {}

  auto TestBody() -> void override {
    EXPECT_EQ(sourcemeta::core::ecdsa_verify(
                  this->curve, this->hash, this->coordinate_x,
                  this->coordinate_y, this->message, this->signature),
              this->expected);
  }

private:
  const sourcemeta::core::EllipticCurve curve;
  const sourcemeta::core::SignatureHashFunction hash;
  const std::string coordinate_x;
  const std::string coordinate_y;
  const std::string message;
  const std::string signature;
  const bool expected;
};

auto main(int argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  const std::filesystem::path suite_path{PYCA_CRYPTOGRAPHY_PATH};
  auto stream = sourcemeta::core::read_file(
      suite_path / "asymmetric" / "ECDSA" / "FIPS_186-3" / "SigVer.rsp");

  std::string line;
  std::optional<CurveParameters> current_curve;
  std::optional<sourcemeta::core::SignatureHashFunction> current_hash;
  std::string message_hex;
  std::string coordinate_x_hex;
  std::string coordinate_y_hex;
  std::string r_hex;
  std::string s_hex;
  std::uint64_t case_count{0};

  while (std::getline(stream, line)) {
    strip_cr(line);
    if (line.empty() || line.front() == '#') {
      continue;
    }

    if (line.front() == '[') {
      // The section header pairs a curve and a hash, like "[P-256,SHA-256]"
      const auto comma{line.find(',')};
      current_curve = to_curve(line.substr(0, comma));
      current_hash = to_hash_function(line.substr(comma + 1));
    } else if (line.starts_with("Msg = ")) {
      message_hex = line.substr(6);
    } else if (line.starts_with("Qx = ")) {
      coordinate_x_hex = line.substr(5);
    } else if (line.starts_with("Qy = ")) {
      coordinate_y_hex = line.substr(5);
    } else if (line.starts_with("R = ")) {
      r_hex = line.substr(4);
    } else if (line.starts_with("S = ")) {
      s_hex = line.substr(4);
    } else if (line.starts_with("Result = ")) {
      case_count += 1;
      if (!current_curve.has_value() || !current_hash.has_value()) {
        continue;
      }

      const auto width{current_curve.value().coordinate_bytes};
      const auto expected{line.substr(9).starts_with("P")};
      const auto coordinate_x{
          left_pad(hex_field_to_bytes(coordinate_x_hex), width)};
      const auto coordinate_y{
          left_pad(hex_field_to_bytes(coordinate_y_hex), width)};
      const auto message{hex_field_to_bytes(message_hex)};
      const auto signature{left_pad(hex_field_to_bytes(r_hex), width) +
                           left_pad(hex_field_to_bytes(s_hex), width)};

      const auto test_name{"case_" + std::to_string(case_count) + "_" +
                           (expected ? "pass" : "fail")};
      const auto curve{current_curve.value().curve};
      const auto hash{current_hash.value()};
      testing::RegisterTest("PyCA_Cryptography_ECDSA_SigVer", test_name.c_str(),
                            nullptr, nullptr, __FILE__, __LINE__,
                            [=]() -> PyCA_Cryptography_ECDSA_SigVer_Test * {
                              return new PyCA_Cryptography_ECDSA_SigVer_Test(
                                  curve, hash, coordinate_x, coordinate_y,
                                  message, signature, expected);
                            });
    }
  }

  return RUN_ALL_TESTS();
}

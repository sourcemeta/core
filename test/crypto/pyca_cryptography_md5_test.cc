#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>

#include <cassert>    // assert
#include <cstdint>    // std::uint64_t
#include <filesystem> // std::filesystem
#include <fstream>    // std::ifstream
#include <sstream>    // std::ostringstream, std::istringstream
#include <string>     // std::string, std::getline

static auto hex_to_bytes(const std::string &hex) -> std::string {
  std::string result;
  result.reserve(hex.size() / 2);
  for (std::string::size_type index = 0; index + 1 < hex.size(); index += 2) {
    result.push_back(static_cast<char>(static_cast<unsigned char>(
        std::stoi(hex.substr(index, 2), nullptr, 16))));
  }

  return result;
}

class PyCA_Cryptography_MD5_Test : public testing::Test {
public:
  explicit PyCA_Cryptography_MD5_Test(std::string input_message,
                                      std::string input_expected_digest)
      : message{std::move(input_message)},
        expected_digest{std::move(input_expected_digest)} {}

  auto TestBody() -> void override {
    std::ostringstream result;
    sourcemeta::core::md5(this->message, result);
    EXPECT_EQ(result.str(), this->expected_digest);
  }

private:
  const std::string message;
  const std::string expected_digest;
};

auto main(int argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  const std::filesystem::path suite_path{PYCA_CRYPTOGRAPHY_PATH};
  std::ifstream stream{suite_path / "hashes" / "MD5" / "rfc-1321.txt"};
  assert(stream.is_open());
  stream.exceptions(std::ios_base::badbit);

  std::string line;
  std::string current_message_hex;
  std::uint64_t current_length_bits{0};

  while (std::getline(stream, line)) {
    if (line.empty() || line.front() == '#') {
      continue;
    }

    if (line.starts_with("Len = ")) {
      current_length_bits =
          static_cast<std::uint64_t>(std::stoul(line.substr(6)));
    } else if (line.starts_with("Msg = ")) {
      current_message_hex = line.substr(6);
    } else if (line.starts_with("MD = ")) {
      const auto expected_digest{line.substr(5)};
      const auto message{current_length_bits == 0
                             ? std::string{}
                             : hex_to_bytes(current_message_hex)};

      const auto test_name{"rfc_1321_len_" +
                           std::to_string(current_length_bits)};
      testing::RegisterTest(
          "PyCA_Cryptography_MD5", test_name.c_str(), nullptr, nullptr,
          __FILE__, __LINE__, [=]() -> PyCA_Cryptography_MD5_Test * {
            return new PyCA_Cryptography_MD5_Test(message, expected_digest);
          });
    }
  }

  return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>

#include <cstdint>    // std::uint64_t
#include <filesystem> // std::filesystem
#include <sstream>    // std::ostringstream
#include <string>     // std::string, std::getline

static auto strip_cr(std::string &value) -> void {
  if (!value.empty() && value.back() == '\r') {
    value.pop_back();
  }
}

static auto hex_to_bytes(const std::string &hex) -> std::string {
  std::string result;
  result.reserve(hex.size() / 2);
  for (std::string::size_type index = 0; index + 1 < hex.size(); index += 2) {
    result.push_back(static_cast<char>(static_cast<unsigned char>(
        std::stoi(hex.substr(index, 2), nullptr, 16))));
  }

  return result;
}

static auto sha256_hex(const std::string &input) -> std::string {
  std::ostringstream result;
  sourcemeta::core::sha256(input, result);
  return result.str();
}

class PyCA_Cryptography_SHA256_Test : public testing::Test {
public:
  explicit PyCA_Cryptography_SHA256_Test(std::string input_message,
                                         std::string input_expected_digest)
      : message{std::move(input_message)},
        expected_digest{std::move(input_expected_digest)} {}

  auto TestBody() -> void override {
    std::ostringstream result;
    sourcemeta::core::sha256(this->message, result);
    EXPECT_EQ(result.str(), this->expected_digest);
  }

private:
  const std::string message;
  const std::string expected_digest;
};

class PyCA_Cryptography_SHA256_Monte_Test : public testing::Test {
public:
  explicit PyCA_Cryptography_SHA256_Monte_Test(
      std::string input_seed, std::string input_expected_digest)
      : seed{std::move(input_seed)},
        expected_digest{std::move(input_expected_digest)} {}

  auto TestBody() -> void override {
    // NIST SHAVS Monte Carlo pseudocode:
    // MD[0] = MD[1] = MD[2] = Seed
    // For i = 3 to 1002:
    //   M[i] = MD[i-3] || MD[i-2] || MD[i-1]
    //   MD[i] = SHA-256(M[i])
    // Output MD[1002]
    auto md_0 = this->seed;
    auto md_1 = this->seed;
    auto md_2 = this->seed;
    for (std::uint64_t iteration = 3; iteration <= 1002; ++iteration) {
      const auto message =
          hex_to_bytes(md_0) + hex_to_bytes(md_1) + hex_to_bytes(md_2);
      const auto digest = sha256_hex(message);
      md_0 = md_1;
      md_1 = md_2;
      md_2 = digest;
    }

    EXPECT_EQ(md_2, this->expected_digest);
  }

private:
  const std::string seed;
  const std::string expected_digest;
};

static auto register_msg_tests(const std::filesystem::path &file_path,
                               const std::string &suite_name) -> void {
  auto stream = sourcemeta::core::read_file(file_path);

  std::string line;
  std::string current_message_hex;
  std::uint64_t current_length_bits{0};

  while (std::getline(stream, line)) {
    strip_cr(line);
    if (line.empty() || line.front() == '#' || line.front() == '[') {
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

      const auto test_name{"len_" + std::to_string(current_length_bits)};
      testing::RegisterTest(
          suite_name.c_str(), test_name.c_str(), nullptr, nullptr, __FILE__,
          __LINE__, [=]() -> PyCA_Cryptography_SHA256_Test * {
            return new PyCA_Cryptography_SHA256_Test(message, expected_digest);
          });
    }
  }
}

static auto register_monte_tests(const std::filesystem::path &file_path)
    -> void {
  auto stream = sourcemeta::core::read_file(file_path);

  std::string line;
  std::string seed;
  std::string current_count;

  while (std::getline(stream, line)) {
    strip_cr(line);
    if (line.empty() || line.front() == '#' || line.front() == '[') {
      continue;
    }

    if (line.starts_with("Seed = ")) {
      seed = line.substr(7);
    } else if (line.starts_with("COUNT = ")) {
      current_count = line.substr(8);
    } else if (line.starts_with("MD = ")) {
      const auto expected_digest{line.substr(5)};
      const auto current_seed = seed;

      const auto test_name{"count_" + current_count};
      testing::RegisterTest("PyCA_Cryptography_SHA256_Monte", test_name.c_str(),
                            nullptr, nullptr, __FILE__, __LINE__,
                            [=]() -> PyCA_Cryptography_SHA256_Monte_Test * {
                              return new PyCA_Cryptography_SHA256_Monte_Test(
                                  current_seed, expected_digest);
                            });

      // The output becomes the seed for the next checkpoint
      seed = expected_digest;
    }
  }
}

auto main(int argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  const std::filesystem::path suite_path{PYCA_CRYPTOGRAPHY_PATH};
  const auto sha2_path = suite_path / "hashes" / "SHA2";

  register_msg_tests(sha2_path / "SHA256ShortMsg.rsp",
                     "PyCA_Cryptography_SHA256_ShortMsg");
  register_msg_tests(sha2_path / "SHA256LongMsg.rsp",
                     "PyCA_Cryptography_SHA256_LongMsg");
  register_monte_tests(sha2_path / "SHA256Monte.rsp");

  return RUN_ALL_TESTS();
}

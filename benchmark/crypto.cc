#include <sourcemeta/core/benchmark.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>

#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {

// Deterministic test keys and inputs for repeatable benchmarks
constexpr std::string_view KEY_128{"0123456789abcdef"};
constexpr std::string_view KEY_256{"0123456789abcdef0123456789abcdef"};
constexpr std::string_view IV_96{"123456789012"};
constexpr std::string_view AD_32{"associated-data-for-benchmark-01"};

} // namespace

BENCHMARK(Crypto_CRC32_Large_JSONL) {
  const sourcemeta::core::FileView view{
      std::filesystem::path{CURRENT_DIRECTORY} / "files" / "large.jsonl"};
  const std::string_view contents{
      reinterpret_cast<const char *>(view.as<std::uint8_t>()), view.size()};

  for (auto iteration : state) {
    auto result{sourcemeta::core::crc32(contents)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_GCM_Encrypt_128_1KB) {
  const std::string payload(1024, 'y');

  for (auto iteration : state) {
    auto result{
        sourcemeta::core::aes_gcm_encrypt(KEY_128, IV_96, AD_32, payload)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_GCM_Decrypt_128_1KB) {
  const std::string payload(1024, 'y');
  const auto sealed{
      sourcemeta::core::aes_gcm_encrypt(KEY_128, IV_96, AD_32, payload)};
  const auto ciphertext{sealed->ciphertext()};
  const auto tag{sealed->tag()};

  for (auto iteration : state) {
    auto result{sourcemeta::core::aes_gcm_decrypt(KEY_128, IV_96, AD_32,
                                                  ciphertext, tag)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_GCM_Encrypt_256_1KB) {
  const std::string payload(1024, 'b');

  for (auto iteration : state) {
    auto result{
        sourcemeta::core::aes_gcm_encrypt(KEY_256, IV_96, AD_32, payload)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

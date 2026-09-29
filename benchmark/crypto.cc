#include <sourcemeta/core/benchmark.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>

#include "crypto_aes_block.h"
#include "crypto_ghash.h"

#include <array>       // std::array
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {

// Deterministic test keys and inputs for repeatable benchmarks
constexpr std::string_view KEY_128{"0123456789abcdef"};
constexpr std::string_view KEY_192{"0123456789abcdef01234567"};
constexpr std::string_view KEY_256{"0123456789abcdef0123456789abcdef"};
constexpr std::string_view IV_96{"123456789012"};
constexpr std::string_view AD_32{"associated-data-for-benchmark-01"};

const sourcemeta::core::AesBlock SAMPLE_BLOCK{
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,
     0xcc, 0xdd, 0xee, 0xff}};

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

// AES Key Expansion microbenchmarks
BENCHMARK(Crypto_AES_ExpandKey_128) {
  for (auto iteration : state) {
    auto schedule{sourcemeta::core::aes_expand_key(KEY_128)};
    sourcemeta::core::benchmark_do_not_optimize(schedule);
  }
}

BENCHMARK(Crypto_AES_ExpandKey_192) {
  for (auto iteration : state) {
    auto schedule{sourcemeta::core::aes_expand_key(KEY_192)};
    sourcemeta::core::benchmark_do_not_optimize(schedule);
  }
}

BENCHMARK(Crypto_AES_ExpandKey_256) {
  for (auto iteration : state) {
    auto schedule{sourcemeta::core::aes_expand_key(KEY_256)};
    sourcemeta::core::benchmark_do_not_optimize(schedule);
  }
}

// AES Block Encryption microbenchmarks with pre-expanded keys
BENCHMARK(Crypto_AES_EncryptBlock_128) {
  const auto schedule{sourcemeta::core::aes_expand_key(KEY_128)};
  const auto block{SAMPLE_BLOCK};

  for (auto iteration : state) {
    auto encrypted{sourcemeta::core::aes_encrypt_block(schedule, block)};
    sourcemeta::core::benchmark_do_not_optimize(encrypted);
  }
}

BENCHMARK(Crypto_AES_EncryptBlock_192) {
  const auto schedule{sourcemeta::core::aes_expand_key(KEY_192)};
  const auto block{SAMPLE_BLOCK};

  for (auto iteration : state) {
    auto encrypted{sourcemeta::core::aes_encrypt_block(schedule, block)};
    sourcemeta::core::benchmark_do_not_optimize(encrypted);
  }
}

BENCHMARK(Crypto_AES_EncryptBlock_256) {
  const auto schedule{sourcemeta::core::aes_expand_key(KEY_256)};
  const auto block{SAMPLE_BLOCK};

  for (auto iteration : state) {
    auto encrypted{sourcemeta::core::aes_encrypt_block(schedule, block)};
    sourcemeta::core::benchmark_do_not_optimize(encrypted);
  }
}

// AES Block Decryption microbenchmarks with pre-expanded keys
BENCHMARK(Crypto_AES_DecryptBlock_128) {
  const auto schedule{sourcemeta::core::aes_expand_key(KEY_128)};
  const auto block{SAMPLE_BLOCK};

  for (auto iteration : state) {
    auto decrypted{sourcemeta::core::aes_decrypt_block(schedule, block)};
    sourcemeta::core::benchmark_do_not_optimize(decrypted);
  }
}

BENCHMARK(Crypto_AES_DecryptBlock_192) {
  const auto schedule{sourcemeta::core::aes_expand_key(KEY_192)};
  const auto block{SAMPLE_BLOCK};

  for (auto iteration : state) {
    auto decrypted{sourcemeta::core::aes_decrypt_block(schedule, block)};
    sourcemeta::core::benchmark_do_not_optimize(decrypted);
  }
}

BENCHMARK(Crypto_AES_DecryptBlock_256) {
  const auto schedule{sourcemeta::core::aes_expand_key(KEY_256)};
  const auto block{SAMPLE_BLOCK};

  for (auto iteration : state) {
    auto decrypted{sourcemeta::core::aes_decrypt_block(schedule, block)};
    sourcemeta::core::benchmark_do_not_optimize(decrypted);
  }
}

// Baseline reference implementation for direct before/after comparison
auto gf_multiply_baseline(const sourcemeta::core::AesBlock &left,
                          const sourcemeta::core::AesBlock &right)
    -> sourcemeta::core::AesBlock {
  sourcemeta::core::AesBlock product{};
  sourcemeta::core::AesBlock value{left};
  for (std::size_t bit = 0; bit < 128; ++bit) {
    if (((right[bit / 8] >> (7 - (bit % 8))) & 1u) != 0) {
      for (std::size_t index = 0; index < 16; ++index) {
        product[index] ^= value[index];
      }
    }

    const auto carry_out{static_cast<std::uint8_t>(value[15] & 1u)};
    std::uint8_t carry_in{0};
    for (auto &byte : value) {
      const auto next_carry{static_cast<std::uint8_t>(byte & 1u)};
      byte = static_cast<std::uint8_t>((byte >> 1u) | (carry_in << 7u));
      carry_in = next_carry;
    }

    if (carry_out != 0) {
      value[0] ^= 0xe1u;
    }
  }

  return product;
}

// GHASH field multiplication baseline microbenchmark
BENCHMARK(Crypto_GHASH_Multiply_Baseline) {
  const auto left{SAMPLE_BLOCK};
  const auto right{SAMPLE_BLOCK};

  for (auto iteration : state) {
    auto product{gf_multiply_baseline(left, right)};
    sourcemeta::core::benchmark_do_not_optimize(product);
  }
}

// GHASH field multiplication optimized microbenchmark
BENCHMARK(Crypto_GHASH_Multiply_Optimized) {
  const auto left{SAMPLE_BLOCK};
  const auto right{SAMPLE_BLOCK};

  for (auto iteration : state) {
    auto product{sourcemeta::core::gf_multiply(left, right)};
    sourcemeta::core::benchmark_do_not_optimize(product);
  }
}

// AES-GCM 128-bit End-to-End benchmarks: small (32B), medium (1KB), large
// (64KB)
BENCHMARK(Crypto_AES_GCM_Encrypt_128_Small_32B) {
  const std::string payload(32, 'x');

  for (auto iteration : state) {
    auto result{
        sourcemeta::core::aes_gcm_encrypt(KEY_128, IV_96, AD_32, payload)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_GCM_Decrypt_128_Small_32B) {
  const std::string payload(32, 'x');
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

BENCHMARK(Crypto_AES_GCM_Encrypt_128_Medium_1KB) {
  const std::string payload(1024, 'y');

  for (auto iteration : state) {
    auto result{
        sourcemeta::core::aes_gcm_encrypt(KEY_128, IV_96, AD_32, payload)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_GCM_Decrypt_128_Medium_1KB) {
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

BENCHMARK(Crypto_AES_GCM_Encrypt_128_Large_64KB) {
  const std::string payload(65536, 'z');

  for (auto iteration : state) {
    auto result{
        sourcemeta::core::aes_gcm_encrypt(KEY_128, IV_96, AD_32, payload)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_GCM_Decrypt_128_Large_64KB) {
  const std::string payload(65536, 'z');
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

// AES-GCM 256-bit End-to-End benchmarks: small (32B), medium (1KB), large
// (64KB)
BENCHMARK(Crypto_AES_GCM_Encrypt_256_Small_32B) {
  const std::string payload(32, 'a');

  for (auto iteration : state) {
    auto result{
        sourcemeta::core::aes_gcm_encrypt(KEY_256, IV_96, AD_32, payload)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_GCM_Decrypt_256_Small_32B) {
  const std::string payload(32, 'a');
  const auto sealed{
      sourcemeta::core::aes_gcm_encrypt(KEY_256, IV_96, AD_32, payload)};
  const auto ciphertext{sealed->ciphertext()};
  const auto tag{sealed->tag()};

  for (auto iteration : state) {
    auto result{sourcemeta::core::aes_gcm_decrypt(KEY_256, IV_96, AD_32,
                                                  ciphertext, tag)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_GCM_Encrypt_256_Medium_1KB) {
  const std::string payload(1024, 'b');

  for (auto iteration : state) {
    auto result{
        sourcemeta::core::aes_gcm_encrypt(KEY_256, IV_96, AD_32, payload)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_GCM_Decrypt_256_Medium_1KB) {
  const std::string payload(1024, 'b');
  const auto sealed{
      sourcemeta::core::aes_gcm_encrypt(KEY_256, IV_96, AD_32, payload)};
  const auto ciphertext{sealed->ciphertext()};
  const auto tag{sealed->tag()};

  for (auto iteration : state) {
    auto result{sourcemeta::core::aes_gcm_decrypt(KEY_256, IV_96, AD_32,
                                                  ciphertext, tag)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_GCM_Encrypt_256_Large_64KB) {
  const std::string payload(65536, 'c');

  for (auto iteration : state) {
    auto result{
        sourcemeta::core::aes_gcm_encrypt(KEY_256, IV_96, AD_32, payload)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_GCM_Decrypt_256_Large_64KB) {
  const std::string payload(65536, 'c');
  const auto sealed{
      sourcemeta::core::aes_gcm_encrypt(KEY_256, IV_96, AD_32, payload)};
  const auto ciphertext{sealed->ciphertext()};
  const auto tag{sealed->tag()};

  for (auto iteration : state) {
    auto result{sourcemeta::core::aes_gcm_decrypt(KEY_256, IV_96, AD_32,
                                                  ciphertext, tag)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

// AES Key Wrap benchmarks
BENCHMARK(Crypto_AES_KeyWrap_Wrap_128) {
  for (auto iteration : state) {
    auto result{sourcemeta::core::aes_key_wrap(KEY_128, KEY_128)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_KeyWrap_Unwrap_128) {
  const auto wrapped{sourcemeta::core::aes_key_wrap(KEY_128, KEY_128)};
  const auto &wrapped_data{wrapped.value()};

  for (auto iteration : state) {
    auto result{sourcemeta::core::aes_key_unwrap(KEY_128, wrapped_data)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_KeyWrap_Wrap_256) {
  for (auto iteration : state) {
    auto result{sourcemeta::core::aes_key_wrap(KEY_256, KEY_256)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Crypto_AES_KeyWrap_Unwrap_256) {
  const auto wrapped{sourcemeta::core::aes_key_wrap(KEY_256, KEY_256)};
  const auto &wrapped_data{wrapped.value()};

  for (auto iteration : state) {
    auto result{sourcemeta::core::aes_key_unwrap(KEY_256, wrapped_data)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

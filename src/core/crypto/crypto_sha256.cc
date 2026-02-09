#include <sourcemeta/core/crypto_sha256.h>

#include <array>   // std::array
#include <cstdint> // std::uint32_t, std::uint64_t
#include <cstring> // std::memcpy
#include <iomanip> // std::hex, std::setfill

namespace {

inline constexpr auto rotate_right(std::uint32_t value,
                                   std::uint64_t count) noexcept
    -> std::uint32_t {
  return (value >> count) | (value << (32u - count));
}

inline auto sha256_process_block(const unsigned char *block,
                                 std::array<std::uint32_t, 8> &state) noexcept
    -> void {
  // First 32 bits of the fractional parts of the cube roots
  // of the first 64 prime numbers (FIPS 180-4 Section 4.2.2)
  static constexpr std::array<std::uint32_t, 64> k = {
      {0x428a2f98U, 0x71374491U, 0xb5c0fbcfU, 0xe9b5dba5U, 0x3956c25bU,
       0x59f111f1U, 0x923f82a4U, 0xab1c5ed5U, 0xd807aa98U, 0x12835b01U,
       0x243185beU, 0x550c7dc3U, 0x72be5d74U, 0x80deb1feU, 0x9bdc06a7U,
       0xc19bf174U, 0xe49b69c1U, 0xefbe4786U, 0x0fc19dc6U, 0x240ca1ccU,
       0x2de92c6fU, 0x4a7484aaU, 0x5cb0a9dcU, 0x76f988daU, 0x983e5152U,
       0xa831c66dU, 0xb00327c8U, 0xbf597fc7U, 0xc6e00bf3U, 0xd5a79147U,
       0x06ca6351U, 0x14292967U, 0x27b70a85U, 0x2e1b2138U, 0x4d2c6dfcU,
       0x53380d13U, 0x650a7354U, 0x766a0abbU, 0x81c2c92eU, 0x92722c85U,
       0xa2bfe8a1U, 0xa81a664bU, 0xc24b8b70U, 0xc76c51a3U, 0xd192e819U,
       0xd6990624U, 0xf40e3585U, 0x106aa070U, 0x19a4c116U, 0x1e376c08U,
       0x2748774cU, 0x34b0bcb5U, 0x391c0cb3U, 0x4ed8aa4aU, 0x5b9cca4fU,
       0x682e6ff3U, 0x748f82eeU, 0x78a5636fU, 0x84c87814U, 0x8cc70208U,
       0x90befffaU, 0xa4506cebU, 0xbef9a3f7U, 0xc67178f2U}};

  // Decode 16 big-endian 32-bit words from the block
  std::array<std::uint32_t, 64> schedule{};
  for (std::uint64_t word_index = 0; word_index < 16u; ++word_index) {
    const std::uint64_t byte_index = word_index * 4u;
    schedule[word_index] =
        (static_cast<std::uint32_t>(block[byte_index]) << 24u) |
        (static_cast<std::uint32_t>(block[byte_index + 1u]) << 16u) |
        (static_cast<std::uint32_t>(block[byte_index + 2u]) << 8u) |
        static_cast<std::uint32_t>(block[byte_index + 3u]);
  }

  // Extend the message schedule (FIPS 180-4 Section 6.2.2 step 1)
  for (std::uint64_t index = 16u; index < 64u; ++index) {
    const auto s0 = rotate_right(schedule[index - 15u], 7u) ^
                    rotate_right(schedule[index - 15u], 18u) ^
                    (schedule[index - 15u] >> 3u);
    const auto s1 = rotate_right(schedule[index - 2u], 17u) ^
                    rotate_right(schedule[index - 2u], 19u) ^
                    (schedule[index - 2u] >> 10u);
    schedule[index] = schedule[index - 16u] + s0 + schedule[index - 7u] + s1;
  }

  auto a = state[0];
  auto b = state[1];
  auto c = state[2];
  auto d = state[3];
  auto e = state[4];
  auto f = state[5];
  auto g = state[6];
  auto h = state[7];

  // Compression function (FIPS 180-4 Section 6.2.2 step 3)
  for (std::uint64_t round_index = 0u; round_index < 64u; ++round_index) {
    const auto big_sigma_1 =
        rotate_right(e, 6u) ^ rotate_right(e, 11u) ^ rotate_right(e, 25u);
    const auto ch = (e & f) ^ ((~e) & g);
    const auto temp_1 =
        h + big_sigma_1 + ch + k[round_index] + schedule[round_index];
    const auto big_sigma_0 =
        rotate_right(a, 2u) ^ rotate_right(a, 13u) ^ rotate_right(a, 22u);
    const auto maj = (a & b) ^ (a & c) ^ (b & c);
    const auto temp_2 = big_sigma_0 + maj;

    h = g;
    g = f;
    f = e;
    e = d + temp_1;
    d = c;
    c = b;
    b = a;
    a = temp_1 + temp_2;
  }

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
  state[5] += f;
  state[6] += g;
  state[7] += h;
}

} // namespace

namespace sourcemeta::core {

auto sha256(const std::string_view input, std::ostream &output) -> void {
  // Initial hash values: first 32 bits of the fractional parts of the
  // square roots of the first 8 primes (FIPS 180-4 Section 5.3.3)
  std::array<std::uint32_t, 8> state{};
  state[0] = 0x6a09e667U;
  state[1] = 0xbb67ae85U;
  state[2] = 0x3c6ef372U;
  state[3] = 0xa54ff53aU;
  state[4] = 0x510e527fU;
  state[5] = 0x9b05688cU;
  state[6] = 0x1f83d9abU;
  state[7] = 0x5be0cd19U;

  const auto *const input_bytes =
      reinterpret_cast<const unsigned char *>(input.data());
  const std::size_t input_length = input.size();

  // Process all full 64-byte blocks directly from the input (streaming)
  std::size_t processed_bytes = 0u;
  while (input_length - processed_bytes >= 64u) {
    sha256_process_block(input_bytes + processed_bytes, state);
    processed_bytes += 64u;
  }

  // Prepare the final block(s) (one or two 64-byte blocks)
  std::array<unsigned char, 128> final_block{};
  const std::size_t remaining_bytes = input_length - processed_bytes;
  if (remaining_bytes > 0u) {
    std::memcpy(final_block.data(), input_bytes + processed_bytes,
                remaining_bytes);
  }

  // Append the 0x80 byte after the message data
  final_block[remaining_bytes] = 0x80u;

  // Append length in bits as big-endian 64-bit at the end of the padding
  const std::uint64_t message_length_bits =
      static_cast<std::uint64_t>(input_length) * 8ull;

  if (remaining_bytes < 56u) {
    // Enough room for length in the first final block
    // Place length at final_block[56..63] in big-endian
    for (std::uint64_t index = 0u; index < 8u; ++index) {
      final_block[56u + index] = static_cast<unsigned char>(
          (message_length_bits >> (8u * (7u - index))) & 0xffu);
    }
    sha256_process_block(final_block.data(), state);
  } else {
    // Need two blocks: process final_block[0..63] then final_block[64..127]
    // with length
    for (std::uint64_t index = 0u; index < 8u; ++index) {
      final_block[64u + 56u + index] = static_cast<unsigned char>(
          (message_length_bits >> (8u * (7u - index))) & 0xffu);
    }

    sha256_process_block(final_block.data(), state);
    sha256_process_block(final_block.data() + 64u, state);
  }

  // Produce the final digest (big-endian)
  std::array<unsigned char, 32> digest;
  for (std::uint64_t state_index = 0u; state_index < 8u; ++state_index) {
    const std::uint32_t value = state[state_index];
    const std::uint64_t base_index = state_index * 4u;
    digest[base_index + 0u] =
        static_cast<unsigned char>((value >> 24u) & 0xffu);
    digest[base_index + 1u] =
        static_cast<unsigned char>((value >> 16u) & 0xffu);
    digest[base_index + 2u] = static_cast<unsigned char>((value >> 8u) & 0xffu);
    digest[base_index + 3u] = static_cast<unsigned char>(value & 0xffu);
  }

  output << std::hex << std::setfill('0');
  for (const unsigned char octet : digest) {
    output << std::setw(2) << static_cast<std::uint64_t>(octet);
  }

  output.unsetf(std::ios_base::hex);
}

} // namespace sourcemeta::core

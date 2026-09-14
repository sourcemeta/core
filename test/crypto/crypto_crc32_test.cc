#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint32_t
#include <string>      // std::string
#include <string_view> // std::string_view

static auto repeat(const std::string_view pattern, const std::size_t count)
    -> std::string {
  std::string result;
  result.reserve(pattern.size() * count);
  for (std::size_t index = 0; index < count; ++index) {
    result.append(pattern);
  }

  return result;
}

TEST(empty_string) { EXPECT_EQ(sourcemeta::core::crc32(""), 0x00000000u); }

// Try $ printf '%s' "a" | gzip -c | tail -c 8 | head -c 4 | xxd
TEST(single_byte_a) { EXPECT_EQ(sourcemeta::core::crc32("a"), 0xE8B7BE43u); }

// Try $ printf '%s' "abc" | gzip -c | tail -c 8 | head -c 4 | xxd
TEST(abc_string) { EXPECT_EQ(sourcemeta::core::crc32("abc"), 0x352441C2u); }

// ITU-T V.42 canonical CRC-32 check value
TEST(check_string_123456789) {
  EXPECT_EQ(sourcemeta::core::crc32("123456789"), 0xCBF43926u);
}

TEST(alphabet_lowercase) {
  EXPECT_EQ(sourcemeta::core::crc32("abcdefghijklmnopqrstuvwxyz"), 0x4C2750BDu);
}

TEST(quick_brown_fox) {
  EXPECT_EQ(
      sourcemeta::core::crc32("The quick brown fox jumps over the lazy dog"),
      0x414FA339u);
}

TEST(embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  EXPECT_EQ(sourcemeta::core::crc32(input), 0x0854897Fu);
}

TEST(all_zero_bytes_eight) {
  const std::string input(8, '\0');
  EXPECT_EQ(sourcemeta::core::crc32(input), 0x6522DF69u);
}

// Try $ head -c 64 /dev/zero | tr '\0' 'a' | gzip -c | tail -c 8 | head -c 4 |
// xxd
TEST(sixty_four_bytes_a) {
  const std::string input(64, 'a');
  EXPECT_EQ(sourcemeta::core::crc32(input), 0x89B46555u);
}

// Try $ head -c 1000000 /dev/zero | tr '\0' 'a' | gzip -c | tail -c 8 |
// head -c 4 | xxd
TEST(one_million_bytes_a) {
  const std::string input(1000000, 'a');
  EXPECT_EQ(sourcemeta::core::crc32(input), 0xDC25BFBCu);
}

// Try $ for index in $(seq 1 2); do printf '%s' "The quick brown fox jumps
// over the lazy dog"; done | gzip -c | tail -c 8 | head -c 4 | xxd
TEST(quick_brown_fox_2_times) {
  const auto input{repeat("The quick brown fox jumps over the lazy dog", 2)};
  EXPECT_EQ(sourcemeta::core::crc32(input), 0x53EC60A9u);
}

// Try $ for index in $(seq 1 13); do printf '%s' "The quick brown fox jumps
// over the lazy dog"; done | gzip -c | tail -c 8 | head -c 4 | xxd
TEST(quick_brown_fox_13_times) {
  const auto input{repeat("The quick brown fox jumps over the lazy dog", 13)};
  EXPECT_EQ(sourcemeta::core::crc32(input), 0x8C25BA7Cu);
}

// Try $ for index in $(seq 1 14); do printf '%s' "The quick brown fox jumps
// over the lazy dog"; done | gzip -c | tail -c 8 | head -c 4 | xxd
TEST(quick_brown_fox_14_times) {
  const auto input{repeat("The quick brown fox jumps over the lazy dog", 14)};
  EXPECT_EQ(sourcemeta::core::crc32(input), 0x24E60824u);
}

// Try $ for index in $(seq 1 22); do printf '%s' "The quick brown fox jumps
// over the lazy dog"; done | gzip -c | tail -c 8 | head -c 4 | xxd
TEST(quick_brown_fox_22_times) {
  const auto input{repeat("The quick brown fox jumps over the lazy dog", 22)};
  EXPECT_EQ(sourcemeta::core::crc32(input), 0x239385B1u);
}

// Try $ for index in $(seq 1 1000); do printf '%s' "The quick brown fox jumps
// over the lazy dog"; done | gzip -c | tail -c 8 | head -c 4 | xxd
TEST(quick_brown_fox_1000_times) {
  const auto input{repeat("The quick brown fox jumps over the lazy dog", 1000)};
  EXPECT_EQ(sourcemeta::core::crc32(input), 0x89BFA256u);
}

TEST(update_zero_initial_matches_single_shot) {
  EXPECT_EQ(sourcemeta::core::crc32_update(0u, "abc"),
            sourcemeta::core::crc32("abc"));
}

TEST(update_composes_across_chunks) {
  const auto single{sourcemeta::core::crc32("hello world")};
  auto running{sourcemeta::core::crc32_update(0u, "hello")};
  running = sourcemeta::core::crc32_update(running, " world");
  EXPECT_EQ(running, single);
}

TEST(update_composes_across_three_chunks) {
  const auto single{sourcemeta::core::crc32("The quick brown fox")};
  auto running{sourcemeta::core::crc32_update(0u, "The ")};
  running = sourcemeta::core::crc32_update(running, "quick ");
  running = sourcemeta::core::crc32_update(running, "brown fox");
  EXPECT_EQ(running, single);
}

TEST(update_composes_across_long_chunks) {
  const auto input{repeat("The quick brown fox jumps over the lazy dog", 22)};
  const std::string_view view{input};
  auto running{sourcemeta::core::crc32_update(0u, view.substr(0, 5))};
  running = sourcemeta::core::crc32_update(running, view.substr(5, 600));
  running = sourcemeta::core::crc32_update(running, view.substr(605));
  EXPECT_EQ(running, 0x239385B1u);
}

TEST(update_with_empty_input_returns_previous) {
  EXPECT_EQ(sourcemeta::core::crc32_update(0xCAFEBABEu, ""), 0xCAFEBABEu);
}

TEST(update_byte_by_byte_matches_single_shot) {
  const std::string input{"hello"};
  const auto single{sourcemeta::core::crc32(input)};
  std::uint32_t running{0u};
  running = sourcemeta::core::crc32_update(running, "h");
  running = sourcemeta::core::crc32_update(running, "e");
  running = sourcemeta::core::crc32_update(running, "l");
  running = sourcemeta::core::crc32_update(running, "l");
  running = sourcemeta::core::crc32_update(running, "o");
  EXPECT_EQ(running, single);
}

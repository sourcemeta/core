#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/test.h>

#include <array>   // std::array
#include <cstddef> // std::byte, std::size_t
#include <cstdint> // std::uint64_t
#include <limits>  // std::numeric_limits

TEST(encode_0) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  EXPECT_EQ(sourcemeta::core::varint_encode(0, buffer.data()), 1);
  EXPECT_EQ(buffer[0], std::byte{0x00});
}

TEST(encode_1) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  EXPECT_EQ(sourcemeta::core::varint_encode(1, buffer.data()), 1);
  EXPECT_EQ(buffer[0], std::byte{0x01});
}

TEST(encode_127) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  EXPECT_EQ(sourcemeta::core::varint_encode(127, buffer.data()), 1);
  EXPECT_EQ(buffer[0], std::byte{0x7F});
}

TEST(encode_128) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  EXPECT_EQ(sourcemeta::core::varint_encode(128, buffer.data()), 2);
  EXPECT_EQ(buffer[0], std::byte{0x80});
  EXPECT_EQ(buffer[1], std::byte{0x01});
}

TEST(encode_300) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  EXPECT_EQ(sourcemeta::core::varint_encode(300, buffer.data()), 2);
  EXPECT_EQ(buffer[0], std::byte{0xAC});
  EXPECT_EQ(buffer[1], std::byte{0x02});
}

TEST(encode_16383) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  EXPECT_EQ(sourcemeta::core::varint_encode(16383, buffer.data()), 2);
  EXPECT_EQ(buffer[0], std::byte{0xFF});
  EXPECT_EQ(buffer[1], std::byte{0x7F});
}

TEST(encode_50399) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  EXPECT_EQ(sourcemeta::core::varint_encode(50399, buffer.data()), 3);
  EXPECT_EQ(buffer[0], std::byte{0xDF});
  EXPECT_EQ(buffer[1], std::byte{0x89});
  EXPECT_EQ(buffer[2], std::byte{0x03});
}

TEST(encode_16384) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  EXPECT_EQ(sourcemeta::core::varint_encode(16384, buffer.data()), 3);
  EXPECT_EQ(buffer[0], std::byte{0x80});
  EXPECT_EQ(buffer[1], std::byte{0x80});
  EXPECT_EQ(buffer[2], std::byte{0x01});
}

TEST(encode_uint32_maximum) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  EXPECT_EQ(sourcemeta::core::varint_encode(4294967295, buffer.data()), 5);
  EXPECT_EQ(buffer[0], std::byte{0xFF});
  EXPECT_EQ(buffer[1], std::byte{0xFF});
  EXPECT_EQ(buffer[2], std::byte{0xFF});
  EXPECT_EQ(buffer[3], std::byte{0xFF});
  EXPECT_EQ(buffer[4], std::byte{0x0F});
}

TEST(encode_uint64_maximum) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  EXPECT_EQ(sourcemeta::core::varint_encode(
                std::numeric_limits<std::uint64_t>::max(), buffer.data()),
            sourcemeta::core::VARINT_MAXIMUM_BYTES);
  EXPECT_EQ(buffer[0], std::byte{0xFF});
  EXPECT_EQ(buffer[1], std::byte{0xFF});
  EXPECT_EQ(buffer[2], std::byte{0xFF});
  EXPECT_EQ(buffer[3], std::byte{0xFF});
  EXPECT_EQ(buffer[4], std::byte{0xFF});
  EXPECT_EQ(buffer[5], std::byte{0xFF});
  EXPECT_EQ(buffer[6], std::byte{0xFF});
  EXPECT_EQ(buffer[7], std::byte{0xFF});
  EXPECT_EQ(buffer[8], std::byte{0xFF});
  EXPECT_EQ(buffer[9], std::byte{0x01});
}

TEST(decode_0) {
  const std::array<std::byte, 1> bytes{{std::byte{0x00}}};
  EXPECT_EQ(sourcemeta::core::varint_decode(bytes.data(), bytes.size()), 0);
}

TEST(decode_1) {
  const std::array<std::byte, 1> bytes{{std::byte{0x01}}};
  EXPECT_EQ(sourcemeta::core::varint_decode(bytes.data(), bytes.size()), 1);
}

TEST(decode_127) {
  const std::array<std::byte, 1> bytes{{std::byte{0x7F}}};
  EXPECT_EQ(sourcemeta::core::varint_decode(bytes.data(), bytes.size()), 127);
}

TEST(decode_128) {
  const std::array<std::byte, 2> bytes{{std::byte{0x80}, std::byte{0x01}}};
  EXPECT_EQ(sourcemeta::core::varint_decode(bytes.data(), bytes.size()), 128);
}

TEST(decode_300) {
  const std::array<std::byte, 2> bytes{{std::byte{0xAC}, std::byte{0x02}}};
  EXPECT_EQ(sourcemeta::core::varint_decode(bytes.data(), bytes.size()), 300);
}

TEST(decode_16383) {
  const std::array<std::byte, 2> bytes{{std::byte{0xFF}, std::byte{0x7F}}};
  EXPECT_EQ(sourcemeta::core::varint_decode(bytes.data(), bytes.size()), 16383);
}

TEST(decode_16384) {
  const std::array<std::byte, 3> bytes{
      {std::byte{0x80}, std::byte{0x80}, std::byte{0x01}}};
  EXPECT_EQ(sourcemeta::core::varint_decode(bytes.data(), bytes.size()), 16384);
}

TEST(decode_50399) {
  const std::array<std::byte, 3> bytes{
      {std::byte{0xDF}, std::byte{0x89}, std::byte{0x03}}};
  EXPECT_EQ(sourcemeta::core::varint_decode(bytes.data(), bytes.size()), 50399);
}

TEST(decode_uint32_maximum) {
  const std::array<std::byte, 5> bytes{{std::byte{0xFF}, std::byte{0xFF},
                                        std::byte{0xFF}, std::byte{0xFF},
                                        std::byte{0x0F}}};
  EXPECT_EQ(sourcemeta::core::varint_decode(bytes.data(), bytes.size()),
            4294967295);
}

TEST(decode_uint64_maximum) {
  const std::array<std::byte, 10> bytes{
      {std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
       std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
       std::byte{0xFF}, std::byte{0x01}}};
  EXPECT_EQ(sourcemeta::core::varint_decode(bytes.data(), bytes.size()),
            std::numeric_limits<std::uint64_t>::max());
}

TEST(decode_non_canonical_zero) {
  const std::array<std::byte, 2> bytes{{std::byte{0x80}, std::byte{0x00}}};
  EXPECT_EQ(sourcemeta::core::varint_decode(bytes.data(), bytes.size()), 0);
}

TEST(decode_empty_throws) {
  const std::array<std::byte, 1> bytes{{std::byte{0x00}}};
  try {
    [[maybe_unused]] const auto value{
        sourcemeta::core::varint_decode(bytes.data(), 0)};
    FAIL();
  } catch (const sourcemeta::core::VarintParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid variable-length integer");
  }
}

TEST(decode_unterminated_throws) {
  const std::array<std::byte, 1> bytes{{std::byte{0x80}}};
  try {
    [[maybe_unused]] const auto value{
        sourcemeta::core::varint_decode(bytes.data(), bytes.size())};
    FAIL();
  } catch (const sourcemeta::core::VarintParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid variable-length integer");
  }
}

TEST(decode_too_many_bytes_throws) {
  const std::array<std::byte, 11> bytes{
      {std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
       std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
       std::byte{0xFF}, std::byte{0xFF}, std::byte{0x01}}};
  try {
    [[maybe_unused]] const auto value{
        sourcemeta::core::varint_decode(bytes.data(), bytes.size())};
    FAIL();
  } catch (const sourcemeta::core::VarintParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid variable-length integer");
  }
}

TEST(decode_past_uint64_maximum_throws) {
  const std::array<std::byte, 10> bytes{
      {std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
       std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
       std::byte{0xFF}, std::byte{0x02}}};
  try {
    [[maybe_unused]] const auto value{
        sourcemeta::core::varint_decode(bytes.data(), bytes.size())};
    FAIL();
  } catch (const sourcemeta::core::NumericOutOfRangeError &error) {
    EXPECT_STREQ(error.what(), "Numeric value is out of range");
  }
}

TEST(roundtrip_0) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  const auto size{sourcemeta::core::varint_encode(0, buffer.data())};
  EXPECT_EQ(sourcemeta::core::varint_decode(buffer.data(), size), 0);
}

TEST(roundtrip_1000000) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  const auto size{sourcemeta::core::varint_encode(1000000, buffer.data())};
  EXPECT_EQ(size, 3);
  EXPECT_EQ(sourcemeta::core::varint_decode(buffer.data(), size), 1000000);
}

TEST(roundtrip_uint64_maximum) {
  std::array<std::byte, sourcemeta::core::VARINT_MAXIMUM_BYTES> buffer{};
  const auto size{sourcemeta::core::varint_encode(
      std::numeric_limits<std::uint64_t>::max(), buffer.data())};
  EXPECT_EQ(size, sourcemeta::core::VARINT_MAXIMUM_BYTES);
  EXPECT_EQ(sourcemeta::core::varint_decode(buffer.data(), size),
            std::numeric_limits<std::uint64_t>::max());
}

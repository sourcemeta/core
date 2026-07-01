#include <sourcemeta/core/test.h>

#include <sourcemeta/core/io.h>

#include <cstddef> // std::byte
#include <cstdint> // std::uint8_t
#include <string>  // std::string
#include <vector>  // std::vector

TEST(input_empty_list) {
  sourcemeta::core::InputByteStream stream{};
  EXPECT_EQ(stream.peek(), std::char_traits<char>::eof());
}

TEST(input_single_byte_read) {
  sourcemeta::core::InputByteStream stream{0x42};
  EXPECT_EQ(static_cast<std::uint8_t>(stream.get()), 0x42u);
  EXPECT_EQ(stream.get(), std::char_traits<char>::eof());
}

TEST(input_reads_bytes_in_order) {
  sourcemeta::core::InputByteStream stream{0x01, 0x02, 0x03};
  EXPECT_EQ(static_cast<std::uint8_t>(stream.get()), 0x01u);
  EXPECT_EQ(static_cast<std::uint8_t>(stream.get()), 0x02u);
  EXPECT_EQ(static_cast<std::uint8_t>(stream.get()), 0x03u);
  EXPECT_EQ(stream.get(), std::char_traits<char>::eof());
}

TEST(input_preserves_high_bit_bytes) {
  sourcemeta::core::InputByteStream stream{0xff, 0x80, 0x7f};
  EXPECT_EQ(static_cast<std::uint8_t>(stream.get()), 0xffu);
  EXPECT_EQ(static_cast<std::uint8_t>(stream.get()), 0x80u);
  EXPECT_EQ(static_cast<std::uint8_t>(stream.get()), 0x7fu);
}

TEST(input_preserves_embedded_nuls) {
  sourcemeta::core::InputByteStream stream{0x00, 0x01, 0x00};
  EXPECT_EQ(static_cast<std::uint8_t>(stream.get()), 0x00u);
  EXPECT_EQ(static_cast<std::uint8_t>(stream.get()), 0x01u);
  EXPECT_EQ(static_cast<std::uint8_t>(stream.get()), 0x00u);
  EXPECT_EQ(stream.get(), std::char_traits<char>::eof());
}

TEST(input_peek_does_not_advance) {
  sourcemeta::core::InputByteStream stream{0xaa, 0xbb};
  EXPECT_EQ(static_cast<std::uint8_t>(stream.peek()), 0xaau);
  EXPECT_EQ(static_cast<std::uint8_t>(stream.peek()), 0xaau);
  EXPECT_EQ(static_cast<std::uint8_t>(stream.get()), 0xaau);
  EXPECT_EQ(static_cast<std::uint8_t>(stream.peek()), 0xbbu);
}

TEST(output_starts_empty) {
  const sourcemeta::core::OutputByteStream stream;
  EXPECT_TRUE(stream.bytes().empty());
}

TEST(output_captures_put_calls) {
  sourcemeta::core::OutputByteStream stream;
  stream.put('A');
  stream.put('B');
  const std::vector<std::byte> expected{std::byte{0x41}, std::byte{0x42}};
  EXPECT_EQ(stream.bytes(), expected);
}

TEST(output_captures_high_bit_bytes) {
  sourcemeta::core::OutputByteStream stream;
  stream.put(static_cast<char>(0xff));
  stream.put(static_cast<char>(0x80));
  const std::vector<std::byte> expected{std::byte{0xff}, std::byte{0x80}};
  EXPECT_EQ(stream.bytes(), expected);
}

TEST(output_captures_embedded_nuls) {
  sourcemeta::core::OutputByteStream stream;
  stream.put('\0');
  stream.put('\x01');
  stream.put('\0');
  const std::vector<std::byte> expected{std::byte{0x00}, std::byte{0x01},
                                        std::byte{0x00}};
  EXPECT_EQ(stream.bytes(), expected);
}

TEST(output_captures_write_calls) {
  sourcemeta::core::OutputByteStream stream;
  const std::string payload{"hello"};
  stream.write(payload.data(), static_cast<std::streamsize>(payload.size()));
  const std::vector<std::byte> expected{std::byte{'h'}, std::byte{'e'},
                                        std::byte{'l'}, std::byte{'l'},
                                        std::byte{'o'}};
  EXPECT_EQ(stream.bytes(), expected);
}

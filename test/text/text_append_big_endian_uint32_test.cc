#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string> // std::string

TEST(zero) {
  std::string bytes;
  sourcemeta::core::append_big_endian_uint32(bytes, 0);
  EXPECT_EQ(bytes, std::string("\x00\x00\x00\x00", 4));
}

TEST(one) {
  std::string bytes;
  sourcemeta::core::append_big_endian_uint32(bytes, 1);
  EXPECT_EQ(bytes, std::string("\x00\x00\x00\x01", 4));
}

TEST(all_octets_distinct) {
  std::string bytes;
  sourcemeta::core::append_big_endian_uint32(bytes, 0x01234567u);
  EXPECT_EQ(bytes, std::string("\x01\x23\x45\x67", 4));
}

TEST(maximum) {
  std::string bytes;
  sourcemeta::core::append_big_endian_uint32(bytes, 0xffffffffu);
  EXPECT_EQ(bytes, std::string("\xff\xff\xff\xff", 4));
}

TEST(appends_after_existing_content) {
  std::string bytes{"foo"};
  sourcemeta::core::append_big_endian_uint32(bytes, 128);
  EXPECT_EQ(bytes, std::string("foo\x00\x00\x00\x80", 7));
}

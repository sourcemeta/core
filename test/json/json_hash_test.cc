#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

TEST(hash_empty) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{""};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_f) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"f"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000006600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_fo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x00000000006f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_foo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x000000006f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_fooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000006f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_foooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x00006f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_fooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x006f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_foooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_fooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x000000000000006f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_foooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x0000000000006f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_fooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x00000000006f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_foooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x000000006f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_fooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x0000006f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_foooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x00006f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_fooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x006f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_foooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000000000);
}

TEST(hash_fooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x000000000000006f);
}

TEST(hash_foooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000000000006f6f);
}

TEST(hash_fooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x00000000006f6f6f);
}

TEST(hash_foooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x000000006f6f6f6f);
}

TEST(hash_fooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x0000006f6f6f6f6f);
}

TEST(hash_foooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x00006f6f6f6f6f6f);
}

TEST(hash_fooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x006f6f6f6f6f6f6f);
}

TEST(hash_foooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000000000} << 64) |
                        0x6f6f6f6f6f6f6f6f);
}

TEST(hash_fooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x000000000000006f} << 64) |
                        0x6f6f6f6f6f6f6f6f);
}

TEST(hash_foooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000000000006f6f} << 64) |
                        0x6f6f6f6f6f6f6f6f);
}

TEST(hash_fooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x00000000006f6f6f} << 64) |
                        0x6f6f6f6f6f6f6f6f);
}

TEST(hash_foooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x000000006f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6f6f);
}

TEST(hash_fooooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x0000006f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6f6f);
}

TEST(hash_foooooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x00006f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6f6f);
}

TEST(hash_fooooooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x006f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6f6f);
}

TEST(hash_foooooooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6f6f);
}

TEST(hash_fooooooooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{
      "fooooooooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_FALSE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f66f6);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x6f6f6f6f6f6f6f6f} << 64) |
                        0x6f6f6f6f6f6f6f6f);
}

TEST(hash_no_collision) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{
      "zaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaz"};
  const auto hash{hasher(value)};
  EXPECT_FALSE(hasher.is_perfect(hash));
  EXPECT_EQ(hash.a, (sourcemeta::core::uint128_t{0x6161616161616161} << 64) |
                        0x6161616161617A03);
  EXPECT_EQ(hash.b, (sourcemeta::core::uint128_t{0x6161616161616161} << 64) |
                        0x6161616161616161);
}

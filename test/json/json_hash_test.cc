#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

TEST(JSON_key_hash, hash_empty) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{""};
  const auto hash{hasher(value)};
  EXPECT_FALSE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x0000000000000000);
  EXPECT_EQ(hash.b, 0x0000000000000000);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_f) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"f"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000006600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x0000000000006600);
  EXPECT_EQ(hash.b, 0x0000000000000000);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_fo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x0000000000000000} << 64) | 0x00000000006f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x00000000006f6600);
  EXPECT_EQ(hash.b, 0x0000000000000000);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_foo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x0000000000000000} << 64) | 0x000000006f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x000000006f6f6600);
  EXPECT_EQ(hash.b, 0x0000000000000000);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_fooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000006f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x0000006f6f6f6600);
  EXPECT_EQ(hash.b, 0x0000000000000000);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_foooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x0000000000000000} << 64) | 0x00006f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x00006f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x0000000000000000);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_fooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x0000000000000000} << 64) | 0x006f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x006f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x0000000000000000);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_foooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x0000000000000000} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x0000000000000000);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_fooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x000000000000006f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x000000000000006f);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_foooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x0000000000006f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x0000000000006f6f);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_fooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x00000000006f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x00000000006f6f6f);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_foooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x000000006f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x000000006f6f6f6f);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_fooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x0000006f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x0000006f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_foooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x00006f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x00006f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_fooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x006f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x006f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_foooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_fooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x000000000000006f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x000000000000006f);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_foooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000006f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x0000000000006f6f);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_fooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x00000000006f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x00000000006f6f6f);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_foooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x000000006f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x000000006f6f6f6f);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_fooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000006f6f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x0000006f6f6f6f6f);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_foooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x00006f6f6f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x00006f6f6f6f6f6f);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_fooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x006f6f6f6f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x006f6f6f6f6f6f6f);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_foooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x6f6f6f6f6f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_fooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x000000000000006f} << 64) | 0x6f6f6f6f6f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.d, 0x000000000000006f);
#endif
}

TEST(JSON_key_hash, hash_foooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000006f6f} << 64) | 0x6f6f6f6f6f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.d, 0x0000000000006f6f);
#endif
}

TEST(JSON_key_hash, hash_fooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x00000000006f6f6f} << 64) | 0x6f6f6f6f6f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.d, 0x00000000006f6f6f);
#endif
}

TEST(JSON_key_hash, hash_foooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x000000006f6f6f6f} << 64) | 0x6f6f6f6f6f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.d, 0x000000006f6f6f6f);
#endif
}

TEST(JSON_key_hash, hash_fooooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000006f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.d, 0x0000006f6f6f6f6f);
#endif
}

TEST(JSON_key_hash, hash_foooooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x00006f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.d, 0x00006f6f6f6f6f6f);
#endif
}

TEST(JSON_key_hash, hash_fooooooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"fooooooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x006f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.d, 0x006f6f6f6f6f6f6f);
#endif
}

TEST(JSON_key_hash, hash_foooooooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{"foooooooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_TRUE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x6f6f6f6f6f6f6f6f} << 64) | 0x6f6f6f6f6f6f6f6f);
#else
  EXPECT_EQ(hash.a, 0x6f6f6f6f6f6f6600);
  EXPECT_EQ(hash.b, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.c, 0x6f6f6f6f6f6f6f6f);
  EXPECT_EQ(hash.d, 0x6f6f6f6f6f6f6f6f);
#endif
}

TEST(JSON_key_hash, hash_fooooooooooooooooooooooooooooooo) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{
      "fooooooooooooooooooooooooooooooo"};
  const auto hash{hasher(value)};
  EXPECT_FALSE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x0000000000000000} << 64) | 0x00000000000000f6);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  // 0x20 (length) + 0x66 (f) + 0x6f (o)
  EXPECT_EQ(hash.a, 0x00000000000000f6);
  EXPECT_EQ(hash.b, 0x0000000000000000);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

TEST(JSON_key_hash, hash_no_collision) {
  const sourcemeta::core::PropertyHashJSON<sourcemeta::core::JSON::String>
      hasher;
  const sourcemeta::core::JSON::String value{
      "zaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaz"};
  const auto hash{hasher(value)};
  EXPECT_FALSE(hasher.is_perfect(hash));
#if defined(__SIZEOF_INT128__)
  EXPECT_EQ(hash.a,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000003);
  EXPECT_EQ(hash.b,
            (__uint128_t{0x0000000000000000} << 64) | 0x0000000000000000);
#else
  // 0x10C (length) + 0x7A (z) + 0x7A (z)
  EXPECT_EQ(hash.a, 0x0000000000000003);
  EXPECT_EQ(hash.b, 0x0000000000000000);
  EXPECT_EQ(hash.c, 0x0000000000000000);
  EXPECT_EQ(hash.d, 0x0000000000000000);
#endif
}

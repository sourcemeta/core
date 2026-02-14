#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>

TEST(Crypto_uuid_v4, regex) {
  const auto uuid{sourcemeta::core::uuidv4()};
#if defined(_MSC_VER)
  EXPECT_THAT(uuid,
              // Windows regular expressions don't support [, ], {, and }
              testing::MatchesRegex("^........-....-4...-....-............$"));
#else
  EXPECT_THAT(uuid,
              testing::MatchesRegex("^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-"
                                    "[89ab][0-9a-f]{3}-[0-9a-f]{12}$"));
#endif
}

TEST(Crypto_uuid_v4, version_and_variant) {
  for (auto count = 0; count < 100; ++count) {
    const auto uuid{sourcemeta::core::uuidv4()};
    EXPECT_EQ(uuid[14], '4');
    EXPECT_TRUE(uuid[19] == '8' || uuid[19] == '9' || uuid[19] == 'a' ||
                uuid[19] == 'b');
  }
}

TEST(Crypto_uuid_v4, idempotent) {
  const auto uuid_1{sourcemeta::core::uuidv4()};
  const auto uuid_2{sourcemeta::core::uuidv4()};
  const auto uuid_3{sourcemeta::core::uuidv4()};
  EXPECT_NE(uuid_1, uuid_2);
  EXPECT_NE(uuid_2, uuid_3);
  EXPECT_NE(uuid_3, uuid_1);
}

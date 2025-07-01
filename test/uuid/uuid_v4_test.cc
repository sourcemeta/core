#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sourcemeta/core/uuid.h>

TEST(UUID_v4, regex) {
  const auto uuid{sourcemeta::core::uuidv4()};
#if defined(_MSC_VER)
  EXPECT_THAT(uuid,
              // Windows regular expressions don't support [, ], {, and }
              testing::MatchesRegex("^........-....-....-....-............$"));
#else
  EXPECT_THAT(uuid,
              testing::MatchesRegex("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-"
                                    "9a-f]{4}-[0-9a-f]{12}$"));
#endif
}

TEST(UUID_v4, idempotent) {
  const auto uuid_1{sourcemeta::core::uuidv4()};
  const auto uuid_2{sourcemeta::core::uuidv4()};
  const auto uuid_3{sourcemeta::core::uuidv4()};
  EXPECT_NE(uuid_1, uuid_2);
  EXPECT_NE(uuid_2, uuid_3);
  EXPECT_NE(uuid_3, uuid_1);
}

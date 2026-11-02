#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

TEST(subject_type_name_public) {
  EXPECT_EQ(sourcemeta::core::oidc_subject_type_name(
                sourcemeta::core::OIDCSubjectType::Public),
            "public");
}

TEST(subject_type_name_pairwise) {
  EXPECT_EQ(sourcemeta::core::oidc_subject_type_name(
                sourcemeta::core::OIDCSubjectType::Pairwise),
            "pairwise");
}

TEST(to_subject_type_parses_known_values) {
  const auto public_type{sourcemeta::core::to_oidc_subject_type("public")};
  EXPECT_TRUE(public_type.has_value());
  EXPECT_EQ(public_type.value(), sourcemeta::core::OIDCSubjectType::Public);
  const auto pairwise_type{sourcemeta::core::to_oidc_subject_type("pairwise")};
  EXPECT_TRUE(pairwise_type.has_value());
  EXPECT_EQ(pairwise_type.value(), sourcemeta::core::OIDCSubjectType::Pairwise);
}

TEST(to_subject_type_rejects_unknown) {
  EXPECT_FALSE(sourcemeta::core::to_oidc_subject_type("private").has_value());
}

TEST(pairwise_subject_is_stable_per_sector_and_account) {
  const auto first{sourcemeta::core::oidc_pairwise_subject(
      "client.example", "user-1", "provider-secret")};
  const auto second{sourcemeta::core::oidc_pairwise_subject(
      "client.example", "user-1", "provider-secret")};
  EXPECT_FALSE(first.empty());
  EXPECT_EQ(first, second);
}

TEST(pairwise_subject_differs_per_sector) {
  const auto first{sourcemeta::core::oidc_pairwise_subject(
      "client-a.example", "user-1", "provider-secret")};
  const auto second{sourcemeta::core::oidc_pairwise_subject(
      "client-b.example", "user-1", "provider-secret")};
  EXPECT_FALSE(first == second);
}

TEST(pairwise_subject_differs_per_account) {
  const auto first{sourcemeta::core::oidc_pairwise_subject(
      "client.example", "user-1", "provider-secret")};
  const auto second{sourcemeta::core::oidc_pairwise_subject(
      "client.example", "user-2", "provider-secret")};
  EXPECT_FALSE(first == second);
}

TEST(pairwise_subject_differs_per_salt) {
  const auto first{sourcemeta::core::oidc_pairwise_subject(
      "client.example", "user-1", "secret-a")};
  const auto second{sourcemeta::core::oidc_pairwise_subject(
      "client.example", "user-1", "secret-b")};
  EXPECT_FALSE(first == second);
}

TEST(pairwise_subject_avoids_a_field_boundary_collision) {
  // Without length-framing, "client a" + "b" and "client" + "a b" would form
  // the same concatenation and collide
  const auto first{sourcemeta::core::oidc_pairwise_subject("client a", "b",
                                                           "provider-secret")};
  const auto second{sourcemeta::core::oidc_pairwise_subject("client", "a b",
                                                            "provider-secret")};
  EXPECT_FALSE(first == second);
}

TEST(pairwise_subject_stays_within_the_length_limit) {
  const auto subject{sourcemeta::core::oidc_pairwise_subject(
      "client.example", "user-1", "provider-secret")};
  EXPECT_TRUE(subject.size() <= 255);
}

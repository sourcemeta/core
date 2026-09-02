#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>

#include <cstdint>     // std::uint64_t
#include <limits>      // std::numeric_limits
#include <new>         // std::bad_array_new_length
#include <string>      // std::string
#include <string_view> // std::string_view

TEST(secure_zero_clears_a_string) {
  std::string secret{"password"};
  sourcemeta::core::secure_zero(secret);
  EXPECT_EQ(secret, std::string(8, '\x00'));
}

TEST(secure_zero_of_an_empty_string_is_a_no_op) {
  std::string secret;
  sourcemeta::core::secure_zero(secret);
  EXPECT_TRUE(secret.empty());
}

TEST(secure_zero_clears_a_raw_buffer) {
  std::string buffer{"abcd"};
  sourcemeta::core::secure_zero(buffer.data(), buffer.size());
  EXPECT_EQ(buffer, std::string(4, '\x00'));
}

TEST(secure_scope_clears_on_exit) {
  std::string secret{"password"};
  {
    const sourcemeta::core::SecureStringScope scope{secret};
  }

  EXPECT_EQ(secret, std::string(8, '\x00'));
}

TEST(secure_string_holds_its_content) {
  const sourcemeta::core::SecureString secret{"hunter2"};
  EXPECT_TRUE(secret == "hunter2");
  EXPECT_EQ(secret.size(), 7);
}

TEST(secure_string_exposes_its_bytes_through_data) {
  const sourcemeta::core::SecureString secret{"hunter2"};
  EXPECT_EQ(std::string_view(secret.data(), secret.size()), "hunter2");
}

TEST(secure_string_of_empty_content_views_as_empty) {
  const sourcemeta::core::SecureString secret{};
  const std::string_view view{secret};
  EXPECT_TRUE(view.empty());
  EXPECT_EQ(view, std::string_view{});
}

TEST(secure_string_appends_content) {
  sourcemeta::core::SecureString secret{"hunter"};
  secret.append("2");
  EXPECT_TRUE(secret == "hunter2");
}

TEST(secure_string_grows_across_a_reallocation) {
  sourcemeta::core::SecureString secret{"a"};
  secret.append(1000, 'b');
  EXPECT_EQ(secret.size(), 1001);
  EXPECT_EQ(secret.front(), 'a');
  EXPECT_EQ(secret.back(), 'b');
}

TEST(secure_string_capacity_of_an_empty_string) {
  // How much a fresh string reserves is an implementation choice, so the
  // assertion is the invariant the arena headroom arithmetic relies on rather
  // than a particular capacity
  const sourcemeta::core::SecureString secret;
  EXPECT_EQ(secret.size(), 0);
  EXPECT_TRUE(secret.capacity() >= secret.size());
}

TEST(secure_string_capacity_covers_the_held_bytes) {
  const sourcemeta::core::SecureString secret{"hunter2"};
  EXPECT_TRUE(secret.capacity() >= secret.size());
  EXPECT_EQ(secret.size(), 7);
}

TEST(secure_string_capacity_reflects_a_reservation) {
  sourcemeta::core::SecureString secret{"hunter2"};
  secret.reserve(64);
  EXPECT_TRUE(secret.capacity() >= 64);
  EXPECT_EQ(secret.size(), 7);
}

TEST(secure_string_capacity_survives_an_append_within_the_reservation) {
  sourcemeta::core::SecureString secret{"hunter2"};
  secret.reserve(64);
  const auto reserved{secret.capacity()};
  secret.append("!");
  EXPECT_EQ(secret.capacity(), reserved);
  EXPECT_EQ(secret.size(), 8);
}

TEST(secure_string_capacity_does_not_shrink_on_a_smaller_reservation) {
  sourcemeta::core::SecureString secret{"hunter2"};
  secret.reserve(64);
  const auto reserved{secret.capacity()};
  secret.reserve(2);
  EXPECT_EQ(secret.capacity(), reserved);
}

TEST(secure_string_appends_a_view_of_itself) {
  sourcemeta::core::SecureString secret{"hunter2"};
  secret.reserve(14);
  secret.append(std::string_view{secret});
  EXPECT_TRUE(secret == "hunter2hunter2");
}

TEST(secure_string_appends_a_view_of_itself_across_a_reallocation) {
  sourcemeta::core::SecureString secret{"abcdef"};
  secret.reserve(6);
  secret.append(std::string_view{secret});
  secret.append(std::string_view{secret});
  EXPECT_EQ(secret.size(), 24);
  EXPECT_EQ(secret.front(), 'a');
  EXPECT_EQ(secret.back(), 'f');
}

TEST(secure_string_appends_an_empty_view_of_an_empty_string) {
  sourcemeta::core::SecureString secret;
  secret.append(std::string_view{secret});
  EXPECT_TRUE(secret.empty());
}

TEST(secure_string_appends_an_empty_view) {
  sourcemeta::core::SecureString secret{"data"};
  secret.append(std::string_view{});
  EXPECT_TRUE(secret == "data");
}

TEST(secure_zero_of_a_null_buffer_is_a_no_op) {
  sourcemeta::core::secure_zero(nullptr, 4);
}

TEST(secure_string_from_a_pointer_and_a_length) {
  const std::string_view content{"hunter2"};
  const sourcemeta::core::SecureString secret{content.data(), content.size()};
  EXPECT_EQ(secret.size(), 7);
  EXPECT_EQ(secret, "hunter2");
  EXPECT_EQ(secret[0], 'h');
  EXPECT_EQ(secret[6], '2');
}

TEST(secure_string_of_a_repeated_byte) {
  const sourcemeta::core::SecureString secret{5, 'x'};
  EXPECT_EQ(secret.size(), 5);
  EXPECT_EQ(secret, "xxxxx");
  EXPECT_EQ(secret[0], 'x');
  EXPECT_EQ(secret[4], 'x');
}

TEST(secure_string_writes_a_byte_by_index) {
  sourcemeta::core::SecureString secret{"hunter2"};
  secret[0] = 'H';
  EXPECT_EQ(secret[0], 'H');
  EXPECT_EQ(secret.size(), 7);
  EXPECT_EQ(secret, "Hunter2");
}

TEST(secure_allocator_instances_are_interchangeable) {
  const sourcemeta::core::SecureAllocator<char> left;
  const sourcemeta::core::SecureAllocator<char> right;
  EXPECT_EQ(left, right);
  EXPECT_FALSE(left != right);
}

TEST(secure_allocator_rejects_an_overflowing_allocation) {
  sourcemeta::core::SecureAllocator<std::uint64_t> allocator;
  try {
    [[maybe_unused]] const auto *result{
        allocator.allocate(std::numeric_limits<std::size_t>::max())};
    FAIL();
  } catch (const std::bad_array_new_length &) {
    // Refusing the allocation is expected
  }
}

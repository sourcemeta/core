#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

#include <chrono> // std::chrono::milliseconds, std::chrono::nanoseconds, std::chrono::steady_clock
#include <cstddef> // std::size_t

// Burn enough processor time that the coarsest counter any supported platform
// keeps, which advances every ten milliseconds, cannot fail to have moved
static auto burn_processor_time() -> double {
  const auto deadline{std::chrono::steady_clock::now() +
                      std::chrono::milliseconds{200}};
  double total{0};
  std::size_t index{0};
  while (std::chrono::steady_clock::now() < deadline) {
    total += static_cast<double>(index);
    index += 1;
  }

  return total;
}

TEST(processor_time_is_reported) {
  const auto usage{sourcemeta::core::process_usage()};
  EXPECT_TRUE(usage.cpu_time.has_value());
  EXPECT_GE(usage.cpu_time.value(), std::chrono::nanoseconds::zero());
}

TEST(processor_time_advances_while_the_processor_is_busy) {
  const auto before{sourcemeta::core::process_usage()};
  const auto burned{burn_processor_time()};
  const auto after{sourcemeta::core::process_usage()};
  EXPECT_GT(burned, 0.0);
  EXPECT_GT(after.cpu_time.value(), before.cpu_time.value());
}

TEST(resident_memory_is_reported) {
  const auto usage{sourcemeta::core::process_usage()};
  EXPECT_TRUE(usage.resident_bytes.has_value());
  EXPECT_GT(usage.resident_bytes.value(), 0U);
}

// The two platforms that answer with the size of the mapped address space
#if defined(__linux__) || defined(__APPLE__)
TEST(virtual_memory_is_reported) {
  const auto usage{sourcemeta::core::process_usage()};
  EXPECT_TRUE(usage.virtual_bytes.has_value());
  EXPECT_GT(usage.virtual_bytes.value(), 0U);
}

TEST(virtual_memory_covers_at_least_the_resident_set) {
  const auto usage{sourcemeta::core::process_usage()};
  EXPECT_GE(usage.virtual_bytes.value(), usage.resident_bytes.value());
}
#endif

// Windows offers commit charge rather than the size of the mapped address
// space, which is a different quantity from what the others answer with
#if defined(_WIN32)
TEST(virtual_memory_is_absent_where_the_platform_measures_something_else) {
  const auto usage{sourcemeta::core::process_usage()};
  EXPECT_FALSE(usage.virtual_bytes.has_value());
}
#endif

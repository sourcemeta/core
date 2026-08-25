#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

#include <chrono>  // std::chrono::milliseconds, std::chrono::steady_clock
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
  EXPECT_TRUE(usage.cpu_seconds.has_value());
  EXPECT_GE(usage.cpu_seconds.value(), 0.0);
}

TEST(processor_time_advances_while_the_processor_is_busy) {
  const auto before{sourcemeta::core::process_usage()};
  const auto burned{burn_processor_time()};
  const auto after{sourcemeta::core::process_usage()};
  EXPECT_GT(burned, 0.0);
  EXPECT_GT(after.cpu_seconds.value(), before.cpu_seconds.value());
}

TEST(resident_memory_is_reported) {
  const auto usage{sourcemeta::core::process_usage()};
  EXPECT_TRUE(usage.resident_bytes.has_value());
  EXPECT_GT(usage.resident_bytes.value(), 0U);
}

TEST(virtual_memory_is_reported) {
  const auto usage{sourcemeta::core::process_usage()};
  EXPECT_TRUE(usage.virtual_bytes.has_value());
  EXPECT_GT(usage.virtual_bytes.value(), 0U);
}

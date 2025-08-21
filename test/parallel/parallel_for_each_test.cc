#include <gtest/gtest.h>

#include "sourcemeta/core/parallel_for_each.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <thread>
#include <vector>

TEST(Parallel_for_each, processes_all_elements_once) {
  std::vector<std::size_t> items;
  for (std::size_t index = 0; index < 20; index++) {
    items.push_back(index);
  }

  std::mutex mutex;
  std::vector<std::size_t> processed;

  sourcemeta::core::parallel_for_each(
      items.cbegin(), items.cend(),
      [&mutex, &processed](const auto value, const auto, const auto) {
        std::lock_guard<std::mutex> lock{mutex};
        processed.push_back(value);
      });

  EXPECT_EQ(processed.size(), items.size());
  std::sort(processed.begin(), processed.end());
  EXPECT_EQ(processed, items);
}

TEST(Parallel_for_each, processes_all_elements_once_concurrency_1) {
  std::vector<std::size_t> items;
  for (std::size_t index = 0; index < 20; index++) {
    items.push_back(index);
  }

  std::mutex mutex;
  std::vector<std::size_t> processed;
  std::set<std::size_t> parallelism;
  std::vector<std::size_t> cursors;

  sourcemeta::core::parallel_for_each(
      items.cbegin(), items.cend(),
      [&mutex, &processed, &parallelism,
       &cursors](const auto value, const auto concurrency, const auto cursor) {
        std::lock_guard<std::mutex> lock{mutex};
        processed.push_back(value);
        parallelism.emplace(concurrency);
        cursors.push_back(cursor - 1);
      },
      1);

  EXPECT_EQ(processed.size(), items.size());
  EXPECT_EQ(processed, items);
  EXPECT_EQ(parallelism.size(), 1);
  EXPECT_TRUE(parallelism.contains(1));
  EXPECT_EQ(cursors.size(), items.size());
  EXPECT_EQ(cursors, items);
}

TEST(Parallel_for_each, empty_range) {
  std::vector<std::size_t> items;
  std::atomic<std::size_t> touched{0};
  sourcemeta::core::parallel_for_each(
      items.begin(), items.end(),
      [&touched](const auto, const auto, const auto) {
        touched.fetch_add(1, std::memory_order_relaxed);
      });

  EXPECT_EQ(touched.load(), 0);
}

TEST(Parallel_for_each, work_callback_throw) {
  std::vector<std::size_t> items{1, 2, 3, 4, 5};

  EXPECT_THROW(sourcemeta::core::parallel_for_each(
                   items.begin(), items.end(),
                   [](const auto value, const auto, const auto) {
                     if (value == 3) {
                       throw std::runtime_error("error");
                     }
                   }),
               std::runtime_error);
}

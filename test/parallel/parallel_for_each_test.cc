#include <sourcemeta/core/parallel.h>
#include <sourcemeta/core/test.h>

#include <algorithm>
#include <atomic>
#include <barrier>
#include <chrono>
#include <mutex>
#include <numeric>
#include <set>
#include <stdexcept>
#include <thread>
#include <vector>

TEST(processes_all_elements_once) {
  std::vector<std::size_t> items;
  items.reserve(20);
  for (std::size_t index = 0; index < 20; index++) {
    items.push_back(index);
  }

  std::mutex mutex;
  std::vector<std::size_t> processed;

  sourcemeta::core::parallel_for_each(
      items.cbegin(), items.cend(),
      [&mutex, &processed](const auto value, const auto, const auto) {
        std::scoped_lock lock{mutex};
        processed.push_back(value);
      });

  EXPECT_EQ(processed.size(), items.size());
  std::sort(processed.begin(), processed.end());
  EXPECT_EQ(processed, items);
}

TEST(processes_all_elements_once_concurrency_1) {
  std::vector<std::size_t> items;
  items.reserve(20);
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
        std::scoped_lock lock{mutex};
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

TEST(single_element) {
  std::vector<std::size_t> items{42};

  std::vector<std::size_t> processed;
  std::size_t reported_parallelism{0};
  std::size_t reported_cursor{0};

  sourcemeta::core::parallel_for_each(
      items.cbegin(), items.cend(),
      [&processed, &reported_parallelism, &reported_cursor](
          const auto value, const auto concurrency, const auto cursor) {
        processed.push_back(value);
        reported_parallelism = concurrency;
        reported_cursor = cursor;
      });

  EXPECT_EQ(processed.size(), 1);
  EXPECT_EQ(processed[0], 42);
  EXPECT_EQ(reported_cursor, 1);
  EXPECT_EQ(reported_parallelism,
            std::max<std::size_t>(std::thread::hardware_concurrency(), 1));
}

TEST(empty_range) {
  std::vector<std::size_t> items;
  std::atomic<std::size_t> touched{0};
  sourcemeta::core::parallel_for_each(
      items.begin(), items.end(),
      [&touched](const auto, const auto, const auto) {
        touched.fetch_add(1, std::memory_order_relaxed);
      });

  EXPECT_EQ(touched.load(), 0);
}

TEST(work_callback_throw) {
  std::vector<std::size_t> items{1, 2, 3, 4, 5};

  try {
    sourcemeta::core::parallel_for_each(
        items.begin(), items.end(),
        [](const auto value, const auto, const auto) {
          if (value == 3) {
            throw std::runtime_error("error");
          }
        });
    FAIL();
  } catch (const std::runtime_error &error) {
    EXPECT_STREQ(error.what(), "error");
  }
}

TEST(custom_stack_size) {
  std::vector<std::size_t> items;
  items.reserve(20);
  for (std::size_t index = 0; index < 20; index++) {
    items.push_back(index);
  }

  std::mutex mutex;
  std::vector<std::size_t> processed;

  sourcemeta::core::parallel_for_each(
      items.cbegin(), items.cend(),
      [&mutex, &processed](const auto value, const auto, const auto) {
        std::scoped_lock lock{mutex};
        processed.push_back(value);
      },
      4, 1048576);

  EXPECT_EQ(processed.size(), items.size());
  std::sort(processed.begin(), processed.end());
  EXPECT_EQ(processed, items);
}

// No platform can back a stack this large, so the first worker the loop tries
// to start is the one that fails. Windows rejects the size before it reaches
// the thread API, which is a different message for the same path
#if defined(_WIN32)
TEST(thread_creation_failure_stack_size_too_large) {
  std::vector<std::size_t> items;
  items.reserve(20);
  for (std::size_t index = 0; index < 20; index++) {
    items.push_back(index);
  }

  std::atomic<std::size_t> processed{0};

  try {
    sourcemeta::core::parallel_for_each(
        items.cbegin(), items.cend(),
        [&processed](const auto, const auto, const auto) {
          processed.fetch_add(1);
        },
        4, std::size_t{1} << 46);
    FAIL();
  } catch (const std::runtime_error &error) {
    EXPECT_STREQ(error.what(),
                 "The requested stack size is too large for this platform");
  }

  EXPECT_LE(processed.load(), items.size());
}
#else
TEST(thread_creation_failure) {
  std::vector<std::size_t> items;
  items.reserve(20);
  for (std::size_t index = 0; index < 20; index++) {
    items.push_back(index);
  }

  std::atomic<std::size_t> processed{0};

  try {
    sourcemeta::core::parallel_for_each(
        items.cbegin(), items.cend(),
        [&processed](const auto, const auto, const auto) {
          processed.fetch_add(1);
        },
        4, std::size_t{1} << 46);
    FAIL();
  } catch (const std::runtime_error &error) {
    EXPECT_STREQ(error.what(), "Could not create thread");
  }

  // Every worker that did start is drained and joined before the failure
  // propagates, so none of them may still be reading this frame
  EXPECT_LE(processed.load(), items.size());
}
#endif

TEST(work_callback_throw_from_every_worker) {
  std::vector<std::size_t> items;
  items.reserve(8);
  for (std::size_t index = 0; index < 8; index++) {
    items.push_back(index);
  }

  // Every worker arrives before any of them throws, so more than one exception
  // reaches the handler and the first one is the one that propagates
  std::barrier barrier{4};
  std::atomic<std::size_t> thrown{0};

  try {
    sourcemeta::core::parallel_for_each(
        items.cbegin(), items.cend(),
        [&barrier, &thrown](const auto, const auto, const auto) {
          barrier.arrive_and_wait();
          thrown.fetch_add(1);
          throw std::runtime_error("worker failure");
        },
        4);
    FAIL();
  } catch (const std::runtime_error &error) {
    EXPECT_STREQ(error.what(), "worker failure");
  }

  EXPECT_EQ(thrown.load(), 4);
}

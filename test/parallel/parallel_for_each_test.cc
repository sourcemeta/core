#include <sourcemeta/core/parallel.h>
#include <sourcemeta/core/test.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <climits>
#include <cstdint>
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

// POSIX refuses a stack size below its own minimum, so one byte is a request
// no platform can honour and the loop fails on its first worker. Windows takes
// the size through a different API with its own bound, so it is driven there by
// a size that cannot fit the type that API takes
#if defined(_WIN32)
#if SIZE_MAX > UINT_MAX
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
        4, SIZE_MAX);
    FAIL();
  } catch (const std::runtime_error &error) {
    EXPECT_STREQ(error.what(),
                 "The requested stack size is too large for this platform");
  }

  EXPECT_EQ(processed.load(), 0);
}
#endif
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
        4, 1);
    FAIL();
  } catch (const std::runtime_error &error) {
    EXPECT_STREQ(error.what(),
                 "The requested stack size is not supported by this platform");
  }

  // The failure happens before the first worker starts, so the queue is
  // drained and no callback runs at all
  EXPECT_EQ(processed.load(), 0);
}

// A size above the minimum is recorded without being checked against what the
// machine has, so the refusal comes from the allocation at creation rather than
// from the request. The shift is only representable in a word wider than
// thirty-two bits
#if SIZE_MAX > UINT_MAX
TEST(thread_creation_failure_on_a_stack_too_large_to_allocate) {
  std::vector<std::size_t> items;
  items.reserve(20);
  for (std::size_t index = 0; index < 20; index++) {
    items.push_back(index);
  }

  std::atomic<std::size_t> processed{0};
  std::atomic<std::size_t> repeated{0};
  std::array<std::atomic<std::size_t>, 20> handled{};

  try {
    sourcemeta::core::parallel_for_each(
        items.cbegin(), items.cend(),
        [&processed, &repeated, &handled](const auto item, const auto,
                                          const auto) {
          processed.fetch_add(1);
          if (handled.at(item).fetch_add(1) != 0) {
            repeated.fetch_add(1);
          }
        },
        4, 1uz << 46);
    FAIL();
  } catch (const std::runtime_error &error) {
    EXPECT_STREQ(error.what(), "Could not create thread");
  }

  // Whichever workers were created before the refusal may have taken items off
  // the queue already, so the count is not pinned. What is pinned is that the
  // refusal is reported and that the queue handed no item out twice
  EXPECT_EQ(repeated.load(), 0);
  EXPECT_TRUE(processed.load() <= items.size());
}
#endif
#endif

// Every worker takes one item, throws on it, and exits, so each one that
// started reaches the exception handler exactly once no matter how the
// scheduler interleaves them. The second one to get there is what covers
// keeping the first exception rather than replacing it
TEST(work_callback_throw_from_every_worker) {
  std::vector<std::size_t> items;
  items.reserve(8);
  for (std::size_t index = 0; index < 8; index++) {
    items.push_back(index);
  }

  std::atomic<std::size_t> thrown{0};

  try {
    sourcemeta::core::parallel_for_each(
        items.cbegin(), items.cend(),
        [&thrown](const auto, const auto, const auto) {
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

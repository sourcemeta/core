#include <sourcemeta/core/parallel.h>
#include <sourcemeta/core/test.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <climits>
#include <cstdint>
#include <iterator>
#include <mutex>
#include <set>
#include <sstream>
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

static auto expect_single_pass(std::istringstream &input,
                               const std::vector<int> &expected) -> void {
  std::mutex mutex;
  std::vector<int> values;
  std::vector<std::size_t> cursors;
  std::set<std::size_t> parallelism;
  sourcemeta::core::parallel_for_each(
      std::istream_iterator<int>{input}, std::istream_iterator<int>{},
      [&](const auto value, const auto concurrency, const auto cursor) {
        const std::scoped_lock lock{mutex};
        values.push_back(value);
        cursors.push_back(cursor);
        parallelism.emplace(concurrency);
      },
      2);

  EXPECT_EQ(values, expected);
  EXPECT_EQ(cursors.size(), expected.size());
  for (std::size_t index = 0; index < cursors.size(); ++index) {
    EXPECT_EQ(cursors[index], index + 1);
  }
  if (expected.empty()) {
    EXPECT_TRUE(parallelism.empty());
  } else {
    EXPECT_EQ(parallelism.size(), 1);
    EXPECT_TRUE(parallelism.contains(1));
  }
}

TEST(single_pass_empty) {
  std::istringstream input;
  expect_single_pass(input, {});
}

TEST(single_pass_one_element) {
  std::istringstream input{"42"};
  expect_single_pass(input, {42});
}

TEST(single_pass_multiple_elements) {
  std::istringstream input{"1 2 3 4"};
  expect_single_pass(input, {1, 2, 3, 4});
}

TEST(forward_iterator_retains_parallel_workers) {
  const std::array<int, 4> items{{1, 2, 3, 4}};
  const auto caller{std::this_thread::get_id()};
  std::mutex mutex;
  std::vector<std::array<std::size_t, 3>> processed;
  bool used_caller{false};

  sourcemeta::core::parallel_for_each(
      items.cbegin(), items.cend(),
      [&](const auto value, const auto concurrency, const auto cursor) {
        const std::scoped_lock lock{mutex};
        processed.push_back(
            {{cursor, static_cast<std::size_t>(value), concurrency}});
        used_caller = used_caller || std::this_thread::get_id() == caller;
      },
      2);

  std::sort(processed.begin(), processed.end());
  EXPECT_EQ(processed.size(), items.size());
  for (std::size_t index = 0; index < processed.size(); ++index) {
    EXPECT_EQ(processed[index][0], index + 1);
    EXPECT_EQ(processed[index][1], static_cast<std::size_t>(items[index]));
    EXPECT_EQ(processed[index][2], 2);
  }
  EXPECT_FALSE(used_caller);
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
  } catch (const sourcemeta::core::ParallelThreadError &error) {
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
  } catch (const sourcemeta::core::ParallelThreadError &error) {
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
// thirty-two bits, and it names a size no address space can hold, so the
// allocation cannot succeed however generously the kernel overcommits
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
        4, 1uz << 62);
    FAIL();
  } catch (const sourcemeta::core::ParallelThreadError &error) {
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

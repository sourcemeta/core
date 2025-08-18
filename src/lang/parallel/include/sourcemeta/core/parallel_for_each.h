#ifndef SOURCEMETA_CORE_PARALLEL_FOR_EACH_H_
#define SOURCEMETA_CORE_PARALLEL_FOR_EACH_H_

#include <algorithm> // std::max
#include <concepts>  // std::copyable, std::invocable
#include <exception> // std::exception_ptr, std::current_exception, std::rethrow_exception
#include <functional> // std::function
#include <iterator>   // std::input_iterator, std::iter_reference_t
#include <mutex>      // std::mutex, std::lock_guard
#include <queue>      // std::queue
#include <thread>     // std::thread
#include <utility>    // std::forward
#include <vector>     // std::vector

#include <pthread.h>

namespace sourcemeta::core {

/// @ingroup parallel
///
/// Process a collection in parallel. If the parallelism is set to zero, the
/// function will run using the available number of cores. If the stack size is
/// set to zero, the platform default applies. For example:
///
/// ```c++
/// #include <sourcemeta/core/parallel.h>
/// #include <vector>
/// #include <mutex>
/// #include <iostream>
///
/// std::vector<std::size_t> input;
/// for (std::size_t index = 0; index < 20; index++) {
///   input.push_back(index);
/// }
///
/// std::mutex mutex;
/// std::vector<std::size_t> result;
///
/// sourcemeta::core::parallel_for_each(
///     input.cbegin(), input.cend(),
///     [&mutex, &result](const auto value,
///                       const auto parallelism,
///                       const auto cursor) {
///       std::lock_guard<std::mutex> lock{mutex};
///       result.push_back(value);
///       std::cerr << "Processing " << cursor
///                 << " with parallelism " << parallelism << "\n";
///     });
/// ```
template <typename Iterator, typename Callback>
  requires std::input_iterator<Iterator> && std::copyable<Iterator> &&
           std::invocable<Callback, std::iter_reference_t<Iterator>,
                          std::size_t, std::size_t>
auto parallel_for_each(
    Iterator first, Iterator last, Callback &&callback,
    const std::size_t parallelism = std::thread::hardware_concurrency(),
    const std::size_t stack_size_bytes = 0) -> void {
  std::queue<Iterator> tasks;
  for (auto iterator = first; iterator != last; ++iterator) {
    tasks.push(iterator);
  }

  std::mutex queue_mutex;
  std::mutex exception_mutex;

  auto effective_callback = std::forward<Callback>(callback);

  std::exception_ptr exception = nullptr;
  auto handle_exception = [&exception_mutex,
                           &exception](std::exception_ptr pointer) {
    std::lock_guard<std::mutex> lock{exception_mutex};
    if (!exception) {
      exception = pointer;
    }
  };

  const auto effective_parallelism{
      std::max(parallelism, static_cast<std::size_t>(1))};
  std::vector<std::thread> workers;
  workers.reserve(effective_parallelism);

  const auto total{tasks.size()};
  for (std::size_t index = 0; index < effective_parallelism; ++index) {
    // We can't use std::thread, as it doesn't let
    // us tweak the thread stack size
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (stack_size_bytes > 0) {
      pthread_attr_setstacksize(&attr, stack_size_bytes);
    }
    pthread_t handle;
    pthread_create(
        &handle, &attr,
        [](void *arg) -> void * {
          auto *fn = static_cast<std::function<void()> *>(arg);
          (*fn)();
          delete fn;
          return nullptr;
        },
        new std::function<void()>([&tasks, &queue_mutex, &effective_callback,
                                   &handle_exception, effective_parallelism,
                                   total] {
          try {
            while (true) {
              Iterator iterator;
              std::size_t cursor{0};
              {
                std::lock_guard<std::mutex> lock{queue_mutex};
                if (tasks.empty()) {
                  return;
                }
                iterator = tasks.front();
                cursor = total - tasks.size() + 1;
                tasks.pop();
              }
              effective_callback(*iterator, effective_parallelism, cursor);
            }
          } catch (...) {
            handle_exception(std::current_exception());
          }
        }));
    workers.emplace_back([handle] { pthread_join(handle, nullptr); });
    pthread_attr_destroy(&attr);
  }

  for (auto &thread : workers) {
    thread.join();
  }

  if (exception) {
    std::rethrow_exception(exception);
  }
}

} // namespace sourcemeta::core

#endif

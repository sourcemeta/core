#ifndef SOURCEMETA_CORE_SYSTEM_PARALLEL_H_
#define SOURCEMETA_CORE_SYSTEM_PARALLEL_H_

#ifndef SOURCEMETA_CORE_SYSTEM_EXPORT
#include <sourcemeta/core/system_export.h>
#endif

#include <algorithm> // std::max
#include <concepts>  // std::copyable, std::invocable
#include <cstddef>   // std::size_t
#include <exception> // std::exception_ptr, std::current_exception, std::rethrow_exception
#include <functional> // std::function
#include <iterator>   // std::input_iterator, std::iter_reference_t
#include <mutex>      // std::mutex, std::scoped_lock
#include <queue>      // std::queue
#include <thread>     // std::thread
#include <utility>    // std::forward
#include <vector>     // std::vector

namespace sourcemeta::core {

#ifndef DOXYGEN
// The platform threading APIs stay behind these, as the header that declares
// them is included by much of this project and the Windows one defines macros
// that collide with ordinary identifiers. The standard thread class cannot
// serve here, as it offers no way of choosing a stack size
SOURCEMETA_CORE_SYSTEM_EXPORT
auto parallel_thread_start(std::function<void()> callable,
                           const std::size_t stack_size_bytes) -> void *;
SOURCEMETA_CORE_SYSTEM_EXPORT
auto parallel_thread_join(void *thread) noexcept -> void;
#endif

/// @ingroup system
///
/// Process a collection in parallel. If the parallelism is set to zero, the
/// function will run using the available number of cores. If the stack size is
/// set to zero, the platform default applies, and a stack size the platform
/// cannot honour throws. For example:
///
/// ```c++
/// #include <sourcemeta/core/system.h>
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
///       std::scoped_lock lock{mutex};
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
  const auto effective_parallelism{(std::max)(parallelism, 1uz)};

  // Empty list
  if (first == last) {
    return;
  }

  // If it is a single element or there is no parallelism,
  // just do a normal loop without dealing with threads
  if (effective_parallelism == 1 || std::next(first) == last) {
    std::size_t cursor{1};
    for (auto iterator = first; iterator != last; ++iterator, ++cursor) {
      callback(*iterator, effective_parallelism, cursor);
    }

    return;
  }

  std::queue<Iterator> tasks;
  for (auto iterator = first; iterator != last; ++iterator) {
    tasks.push(iterator);
  }

  std::mutex queue_mutex;
  std::mutex exception_mutex;

  auto effective_callback = std::forward<Callback>(callback);

  std::exception_ptr exception = nullptr;
  auto handle_exception = [&exception_mutex,
                           &exception](const std::exception_ptr &pointer) {
    std::scoped_lock lock{exception_mutex};
    if (!exception) {
      exception = pointer;
    }
  };

  std::vector<void *> workers;
  workers.reserve(effective_parallelism);

  const auto total{tasks.size()};

  // Worker function that runs the actual per-item work and captures the
  // environment by reference. It will be heap-copied into the native thread
  // API.
  auto worker_callable = [&tasks, &queue_mutex, &effective_callback,
                          &handle_exception, effective_parallelism, total] {
    try {
      while (true) {
        Iterator iterator;
        std::size_t cursor{0};
        {
          std::scoped_lock lock{queue_mutex};
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
  };

  // A worker that fails to start leaves the ones already running referencing
  // the locals of this frame, so the failure is held rather than thrown here
  std::exception_ptr creation_failure{nullptr};

  // TODO: Replace std::function with std::move_only_function once
  // Apple Clang ships libc++ 19+ (__cpp_lib_move_only_function)
  for (std::size_t index = 0; index < effective_parallelism; ++index) {
    try {
      workers.push_back(
          parallel_thread_start(worker_callable, stack_size_bytes));
    } catch (...) {
      creation_failure = std::current_exception();
      break;
    }
  }

  // Draining the remaining tasks stops the running workers from pulling new
  // work, so that every one of them can be joined before this frame goes away
  if (creation_failure) {
    std::scoped_lock lock{queue_mutex};
    std::queue<Iterator> empty;
    tasks.swap(empty);
  }

  for (auto *worker_thread : workers) {
    parallel_thread_join(worker_thread);
  }

  if (creation_failure) {
    std::rethrow_exception(creation_failure);
  }

  if (exception) {
    std::rethrow_exception(exception);
  }
}

} // namespace sourcemeta::core

#endif

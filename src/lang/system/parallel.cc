#include <sourcemeta/core/system_parallel.h>

#include <cstddef>    // std::size_t
#include <functional> // std::function
#include <stdexcept>  // std::runtime_error
#include <utility>    // std::move

#if defined(_WIN32)
#include <climits> // UINT_MAX

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <process.h> // _beginthreadex
#include <windows.h> // HANDLE, WaitForSingleObject, CloseHandle, INFINITE
#else
#include <pthread.h> // pthread_t, pthread_attr_t, pthread_create, pthread_join, pthread_attr_init, pthread_attr_destroy, pthread_attr_setstacksize
#endif

namespace {

#if defined(_WIN32)
// See
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/beginthread-beginthreadex?view=msvc-170
auto __stdcall windows_thread_start(void *argument) -> unsigned {
  auto *callable{static_cast<std::function<void()> *>(argument)};
  (*callable)();
  delete callable;
  return 0;
}
#else
auto posix_thread_start(void *argument) -> void * {
  auto *callable{static_cast<std::function<void()> *>(argument)};
  (*callable)();
  delete callable;
  return nullptr;
}
#endif

} // namespace

namespace sourcemeta::core {

auto parallel_thread_start(std::function<void()> callable,
                           const std::size_t stack_size_bytes) -> void * {
#if defined(_WIN32)
  if (stack_size_bytes > static_cast<std::size_t>(UINT_MAX)) {
    throw std::runtime_error{
        "The requested stack size is too large for this platform"};
  }

  auto *heap_callable{new std::function<void()>(std::move(callable))};
  const auto thread{
      _beginthreadex(nullptr, static_cast<unsigned>(stack_size_bytes),
                     &windows_thread_start, heap_callable, 0, nullptr)};
  if (thread == 0) {
    delete heap_callable;
    throw std::runtime_error{"Could not create thread"};
  }

  return reinterpret_cast<void *>(thread);
#else
  pthread_attr_t attributes;
  pthread_attr_init(&attributes);
  // A stack size the platform cannot honour, such as one below its minimum
  // or one that is not a multiple of its page size, must be reported rather
  // than dropped in favour of the default, as the caller asked for it to
  // bound how deep the work it hands over may recurse
  if (stack_size_bytes > 0 &&
      pthread_attr_setstacksize(&attributes, stack_size_bytes) != 0) {
    pthread_attr_destroy(&attributes);
    throw std::runtime_error{
        "The requested stack size is not supported by this platform"};
  }

  auto *heap_callable{new std::function<void()>(std::move(callable))};
  // Allocated rather than held in the returned pointer itself, as this type is
  // not guaranteed to be a pointer, or to fit in one, on every platform
  auto *thread{new pthread_t{}};
  if (pthread_create(thread, &attributes, &posix_thread_start, heap_callable) !=
      0) {
    pthread_attr_destroy(&attributes);
    delete heap_callable;
    delete thread;
    throw std::runtime_error{"Could not create thread"};
  }

  pthread_attr_destroy(&attributes);
  return static_cast<void *>(thread);
#endif
}

auto parallel_thread_join(void *thread) noexcept -> void {
#if defined(_WIN32)
  auto *handle{static_cast<HANDLE>(thread)};
  WaitForSingleObject(handle, INFINITE);
  CloseHandle(handle);
#else
  auto *handle{static_cast<pthread_t *>(thread)};
  pthread_join(*handle, nullptr);
  delete handle;
#endif
}

} // namespace sourcemeta::core

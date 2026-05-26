#ifndef SOURCEMETA_CORE_STACKTRACE_WINDOWS_H_
#define SOURCEMETA_CORE_STACKTRACE_WINDOWS_H_

#include <sourcemeta/core/stacktrace.h>

// clang-format off
#include <windows.h>
#include <dbghelp.h>
// clang-format on

#include <atomic>    // std::atomic
#include <cstdio>    // std::snprintf
#include <cstring>   // std::strlen
#include <io.h>      // _write
#include <process.h> // _getpid

#pragma comment(lib, "dbghelp.lib")

namespace {

constexpr USHORT maximum_frames{128};
constexpr const char *separator{"========================================"
                                "========================================\n"};

std::atomic<bool> crash_handler_installed{false};
std::atomic<bool> symbols_initialized{false};

auto ensure_symbols_initialized() -> void {
  bool expected{false};
  if (symbols_initialized.compare_exchange_strong(expected, true)) {
    ::SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    ::SymInitialize(::GetCurrentProcess(), nullptr, TRUE);
  }
}

auto write_text(int file_descriptor, const char *text) -> void {
  ::_write(file_descriptor, text, static_cast<unsigned int>(std::strlen(text)));
}

__declspec(noinline) auto write_frames(int file_descriptor,
                                       USHORT frames_to_skip) -> void {
  void *frames[maximum_frames];
  const USHORT captured{
      ::CaptureStackBackTrace(frames_to_skip, maximum_frames, frames, nullptr)};
  ensure_symbols_initialized();
  const HANDLE process{::GetCurrentProcess()};

  alignas(SYMBOL_INFO) char symbol_buffer[sizeof(SYMBOL_INFO) + 512]{};
  auto *symbol{reinterpret_cast<SYMBOL_INFO *>(symbol_buffer)};
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  symbol->MaxNameLen = 511;

  for (USHORT index{0}; index < captured; ++index) {
    DWORD64 displacement{0};
    const char *symbol_name{"<unknown>"};
    if (::SymFromAddr(process, reinterpret_cast<DWORD64>(frames[index]),
                      &displacement, symbol)) {
      symbol_name = symbol->Name;
    }
    HMODULE module_handle{nullptr};
    char module_path[MAX_PATH]{"<unknown-module>"};
    if (::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                 GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                             static_cast<LPCSTR>(frames[index]),
                             &module_handle) != 0) {
      ::GetModuleFileNameA(module_handle, module_path, MAX_PATH);
    }
    char line[1024];
    const int length{std::snprintf(
        line, sizeof(line), "#%u 0x%p %s +0x%llx\n  in %s\n",
        static_cast<unsigned>(index), frames[index], symbol_name,
        static_cast<unsigned long long>(displacement), module_path)};
    if (length > 0) {
      ::_write(file_descriptor, line, static_cast<unsigned int>(length));
    }
  }
}

} // namespace

extern "C" auto WINAPI crash_handler(EXCEPTION_POINTERS *information) -> LONG {
  const int file_descriptor{2};
  write_text(file_descriptor, "\n");
  write_text(file_descriptor, separator);
  char header[256];
  const int length{std::snprintf(
      header, sizeof(header),
      "signal:  0x%08lx (SEH)\n"
      "pid:     %d\n\n",
      static_cast<unsigned long>(information->ExceptionRecord->ExceptionCode),
      _getpid())};
  if (length > 0) {
    ::_write(file_descriptor, header, static_cast<unsigned int>(length));
  }
  write_frames(file_descriptor, /*frames_to_skip=*/1);
  write_text(file_descriptor, separator);
  return EXCEPTION_CONTINUE_SEARCH;
}

namespace sourcemeta::core {

// NOLINTNEXTLINE(misc-definitions-in-headers)
auto stacktrace_on_crash() -> void {
  bool expected{false};
  if (!crash_handler_installed.compare_exchange_strong(expected, true)) {
    return;
  }
  ::SetUnhandledExceptionFilter(&crash_handler);
}

// NOLINTNEXTLINE(misc-definitions-in-headers)
__declspec(noinline) auto stacktrace() -> void {
  const int file_descriptor{2};
  write_text(file_descriptor, separator);
  char header[256];
  const int length{
      std::snprintf(header, sizeof(header), "pid:     %d\n\n", _getpid())};
  if (length > 0) {
    ::_write(file_descriptor, header, static_cast<unsigned int>(length));
  }
  write_frames(file_descriptor, /*frames_to_skip=*/1);
  write_text(file_descriptor, separator);
}

} // namespace sourcemeta::core

#endif

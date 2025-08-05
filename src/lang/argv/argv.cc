#include <sourcemeta/core/argv.h>

#ifdef _WIN32
#include <windows.h> // GetModuleFileNameA
#elif defined(__APPLE__)
#include <climits>       // PATH_MAX
#include <cstdlib>       // realpath, free
#include <mach-o/dyld.h> // _NSGetExecutablePath
#else
#include <limits.h> // PATH_MAX
#include <unistd.h> // readlink
#endif

#include <cassert> // assert
#include <string>  // std::string

namespace sourcemeta::core {

auto program_path(
#ifdef NDEBUG
    const int,
#else
    const int argc,
#endif
    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    const char *const argv[]) -> const std::filesystem::path & {
  assert(argc > 0);
  static std::filesystem::path path;
  if (path.empty()) {
    const std::filesystem::path candidate{argv[0]};
    if (candidate.is_absolute()) {
      path = std::filesystem::canonical(candidate);
      return path;
    }

    std::string result;

#ifdef _WIN32
    char buffer[MAX_PATH];
    DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (length > 0 && length < MAX_PATH) {
      result.assign(buffer, length);
    }

#elif defined(__APPLE__)
    std::uint32_t buffer_size{0};
    _NSGetExecutablePath(nullptr, &buffer_size);
    result.resize(buffer_size, '\0');
    if (_NSGetExecutablePath(result.data(), &buffer_size) == 0) {
      if (char *resolved = realpath(result.data(), nullptr)) {
        result.assign(resolved);
        std::free(resolved);
      } else {
        result.clear();
      }
    } else {
      result.clear();
    }

#else
    char buffer[PATH_MAX];
    const ssize_t length =
        readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (length != -1) {
      buffer[length] = '\0';
      result.assign(buffer, static_cast<std::size_t>(length));
    }
#endif

    if (result.empty()) {
      path = std::filesystem::canonical(argv[0]);
    } else {
      path = std::filesystem::path{result};
      assert(path.is_absolute());
    }
  }

  return path;
}

} // namespace sourcemeta::core

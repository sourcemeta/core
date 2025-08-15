#ifndef SOURCEMETA_CORE_BUILD_TEST_UTILSH_
#define SOURCEMETA_CORE_BUILD_TEST_UTILSH_

#include <cassert>
#include <chrono>
#include <fstream>
#include <string>
#include <thread>

inline auto READ_FILE(const std::filesystem::path &path) -> std::string {
  std::ifstream stream{path};
  std::ostringstream buffer;
  buffer << stream.rdbuf();
  return buffer.str();
}

inline auto WRITE_FILE(const std::filesystem::path &path,
                       const std::string &contents) -> void {
  std::filesystem::create_directories(path.parent_path());
  std::ofstream stream{path};
  assert(!stream.fail());
  stream << contents;
  stream.flush();
  stream.close();

  // TODO: Try to fix this for real. It seems like the GCC standard library has
  // an implementation of `last_write_time` that tends to report stale results
  // for a while
#ifdef __GLIBCXX__
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#endif
}

#endif

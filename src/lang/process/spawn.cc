#include <sourcemeta/core/process.h>

#include <cassert>          // assert
#include <filesystem>       // std::filesystem
#include <initializer_list> // std::initializer_list
#include <span>             // std::span
#include <vector>           // std::vector

#include <spawn.h> // posix_spawnp, posix_spawnattr_t, posix_spawnattr_init, posix_spawnattr_destroy, posix_spawn_file_actions_t, posix_spawn_file_actions_init, posix_spawn_file_actions_destroy, pid_t
#include <sys/wait.h> // waitpid, WIFEXITED, WEXITSTATUS

#if defined(__MSYS__) || defined(__CYGWIN__) || defined(__MINGW32__) ||        \
    defined(__MINGW64__)
#include <unistd.h> // chdir
#endif

extern char **environ;

namespace sourcemeta::core {

auto spawn(const std::string &program,
           std::span<const std::string_view> arguments,
           const std::filesystem::path &directory) -> int {
  assert(directory.is_absolute());
  assert(std::filesystem::exists(directory));
  assert(std::filesystem::is_directory(directory));

  std::vector<const char *> argv;
  argv.reserve(arguments.size() + 2);
  argv.push_back(program.c_str());

  for (const auto &argument : arguments) {
    argv.push_back(argument.data());
  }

  argv.push_back(nullptr);

  posix_spawnattr_t attributes;
  posix_spawnattr_init(&attributes);

  posix_spawn_file_actions_t file_actions;
  posix_spawn_file_actions_init(&file_actions);

#if defined(__MSYS__) || defined(__CYGWIN__) || defined(__MINGW32__) ||        \
    defined(__MINGW64__)
  const std::filesystem::path original_directory{
      std::filesystem::current_path()};
  std::filesystem::current_path(directory);
#else
  posix_spawn_file_actions_addchdir_np(&file_actions, directory.c_str());
#endif

  pid_t process_id;
  const int spawn_result{
      posix_spawnp(&process_id, program.c_str(), &file_actions, &attributes,
                   const_cast<char *const *>(argv.data()), environ)};

  posix_spawn_file_actions_destroy(&file_actions);
  posix_spawnattr_destroy(&attributes);

#if defined(__MSYS__) || defined(__CYGWIN__) || defined(__MINGW32__) ||        \
    defined(__MINGW64__)
  std::filesystem::current_path(original_directory);
#endif

  if (spawn_result != 0) {
    throw ProcessProgramNotNotFoundError{program};
  }

  int status;
  waitpid(process_id, &status, 0);

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }

  throw ProcessSpawnError{program, arguments};
}

auto spawn(const std::string &program,
           std::initializer_list<std::string_view> arguments,
           const std::filesystem::path &directory) -> int {
  return spawn(
      program,
      std::span<const std::string_view>{arguments.begin(), arguments.size()},
      directory);
}

} // namespace sourcemeta::core

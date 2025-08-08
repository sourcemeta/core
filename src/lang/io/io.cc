#include <sourcemeta/core/io.h>

namespace sourcemeta::core {

auto canonical(const std::filesystem::path &path) -> std::filesystem::path {
  // On Linux, FIFO files (like /dev/fd/XX due to process substitution)
  // cannot be made canonical
  // See https://github.com/sourcemeta/jsonschema/issues/252
  return std::filesystem::is_fifo(path) ? path
                                        : std::filesystem::canonical(path);
}

auto weakly_canonical(const std::filesystem::path &path)
    -> std::filesystem::path {
  // On Linux, FIFO files (like /dev/fd/XX due to process substitution)
  // cannot be made canonical
  // See https://github.com/sourcemeta/jsonschema/issues/252
  return std::filesystem::is_fifo(path)
             ? path
             : std::filesystem::weakly_canonical(path);
}

} // namespace sourcemeta::core

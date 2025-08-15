#include <sourcemeta/core/build_adapter_filesystem_json.h>

#include <sourcemeta/core/io.h>

#include <cassert> // assert
#include <fstream> // std::ofstream

namespace sourcemeta::core {

BuildAdapterFilesystemJSON::BuildAdapterFilesystemJSON(
    std::string dependency_extension)
    : extension{std::move(dependency_extension)} {
  assert(!this->extension.empty());
  assert(this->extension.starts_with("."));
}

auto BuildAdapterFilesystemJSON::dependencies_path(const node_type &path) const
    -> node_type {
  assert(path.is_absolute());
  return path.string() + this->extension;
}

auto BuildAdapterFilesystemJSON::read_dependencies(const node_type &path) const
    -> std::optional<internal_dependencies_type> {
  assert(path.is_absolute());
  const auto deps_path{this->dependencies_path(path)};
  if (std::filesystem::exists(deps_path)) {
    const auto deps{read_json(deps_path)};
    assert(deps.is_array());
    if (deps.empty()) {
      return std::nullopt;
    } else {
      return deps.as_array();
    }
  } else {
    return std::nullopt;
  }
}

auto BuildAdapterFilesystemJSON::write_dependencies(
    const node_type &path, const BuildDependencies<node_type> &dependencies)
    -> void {
  assert(path.is_absolute());
  assert(std::filesystem::exists(path));
  // Try to make sure as much as we can that any write operation made to disk
  flush(path);
  this->refresh(path);
  const auto deps_path{this->dependencies_path(path)};
  std::filesystem::create_directories(deps_path.parent_path());
  std::ofstream deps_stream{deps_path};
  assert(!deps_stream.fail());
  // Because dependency lists as array of strings, stringifying vs prettifying
  // doesn't yield that much of a return
  prettify(to_json(dependencies), deps_stream);
}

auto BuildAdapterFilesystemJSON::refresh(const node_type &path) -> void {
  // We prefer our own computed marks so that we don't have to rely
  // too much on file-system specific non-sense
  const auto value{std::filesystem::file_time_type::clock::now()};
  // Because builds are typically ran in parallel
  std::lock_guard<std::mutex> lock{this->mutex};
  this->marks.insert_or_assign(path, value);
}

auto BuildAdapterFilesystemJSON::mark(const node_type &path) const
    -> std::optional<mark_type> {
  assert(path.is_absolute());
  if (std::filesystem::exists(path)) {
    const auto match{this->marks.find(path)};
    if (match == this->marks.cend()) {
      // Keep in mind that depending on the OS, filesystem, and even standard
      // library implementation, this value might not be very reliable. In fact,
      // in many cases it can be outdated. Therefore, we never cache this value
      return std::filesystem::last_write_time(path);
    } else {
      return match->second;
    }
  } else {
    return std::nullopt;
  }
}

auto BuildAdapterFilesystemJSON::mark(
    const internal_dependency_type &node) const -> std::optional<mark_type> {
  assert(node.is_string());
  return this->mark(node.to_string());
}

auto BuildAdapterFilesystemJSON::is_newer_than(const mark_type left,
                                               const mark_type right) const
    -> bool {
  return left > right;
}

} // namespace sourcemeta::core

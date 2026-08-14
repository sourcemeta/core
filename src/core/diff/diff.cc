#include <sourcemeta/core/diff.h>
#include <sourcemeta/core/text.h>

#include "myers.h"
#include "stringify.h"

#include <cassert>       // assert
#include <cstddef>       // std::size_t, std::ptrdiff_t
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

namespace {

// A terminator at the very end closes the last line rather than opening an
// empty one, and whether it was there is what the incomplete-line marker
// reports
auto tokenise_lines(const std::string_view input,
                    std::vector<std::string_view> &lines) -> bool {
  if (input.empty()) {
    return true;
  }

  lines = sourcemeta::core::split(input, '\n');
  if (lines.back().empty()) {
    lines.pop_back();
    return true;
  }

  return false;
}

// A line that closes an input without a terminator can never equal the same
// text followed by one, so it draws its identity from a separate table
auto to_identities(
    const std::vector<std::string_view> &lines, const bool ends_with_newline,
    std::unordered_map<std::string_view, std::size_t> &complete,
    std::unordered_map<std::string_view, std::size_t> &incomplete,
    std::size_t &counter) -> std::vector<std::size_t> {
  std::vector<std::size_t> identities;
  identities.reserve(lines.size());

  for (std::size_t index{0}; index < lines.size(); ++index) {
    const auto is_incomplete{!ends_with_newline && index + 1 == lines.size()};
    auto &table{is_incomplete ? incomplete : complete};
    const auto match{table.emplace(lines[index], counter)};
    if (match.second) {
      counter += 1;
    }

    identities.push_back(match.first->second);
  }

  return identities;
}

// Slide every run of changed lines as far towards the end of the input as the
// surrounding lines allow, which is what makes paired changes line up
auto compact_changes(const std::vector<std::size_t> &identities,
                     std::vector<bool> &changed) -> void {
  const auto size{identities.size()};
  std::size_t index{0};

  while (index < size) {
    if (!changed[index]) {
      index += 1;
      continue;
    }

    auto group_start{index};
    auto group_end{index};
    while (group_end < size && changed[group_end]) {
      group_end += 1;
    }

    while (group_end < size && !changed[group_end] &&
           identities[group_start] == identities[group_end]) {
      changed[group_start] = false;
      changed[group_end] = true;
      group_start += 1;
      group_end += 1;
    }

    index = group_end;
  }
}

auto to_operations(const std::vector<bool> &original_changed,
                   const std::vector<bool> &modified_changed)
    -> std::vector<sourcemeta::core::DiffOperation> {
  using sourcemeta::core::DiffOperationType;
  std::vector<sourcemeta::core::DiffOperation> operations;
  const auto original_size{original_changed.size()};
  const auto modified_size{modified_changed.size()};
  std::size_t original_index{0};
  std::size_t modified_index{0};

  while (original_index < original_size || modified_index < modified_size) {
    if (original_index < original_size && modified_index < modified_size &&
        !original_changed[original_index] &&
        !modified_changed[modified_index]) {
      const auto original_begin{original_index};
      const auto modified_begin{modified_index};
      while (original_index < original_size && modified_index < modified_size &&
             !original_changed[original_index] &&
             !modified_changed[modified_index]) {
        original_index += 1;
        modified_index += 1;
      }

      operations.push_back({.type = DiffOperationType::Equal,
                            .original_start = original_begin,
                            .original_end = original_index,
                            .modified_start = modified_begin,
                            .modified_end = modified_index});
      continue;
    }

    const auto original_begin{original_index};
    while (original_index < original_size && original_changed[original_index]) {
      original_index += 1;
    }

    const auto modified_begin{modified_index};
    while (modified_index < modified_size && modified_changed[modified_index]) {
      modified_index += 1;
    }

    // Every unchanged line of one input pairs with an unchanged line of the
    // other, so neither cursor can stall here
    assert(original_index > original_begin || modified_index > modified_begin);

    if (original_index > original_begin) {
      operations.push_back({.type = DiffOperationType::Delete,
                            .original_start = original_begin,
                            .original_end = original_index,
                            .modified_start = modified_begin,
                            .modified_end = modified_begin});
    }

    if (modified_index > modified_begin) {
      operations.push_back({.type = DiffOperationType::Insert,
                            .original_start = original_index,
                            .original_end = original_index,
                            .modified_start = modified_begin,
                            .modified_end = modified_index});
    }
  }

  return operations;
}

} // namespace

namespace sourcemeta::core {

auto diff(const std::string_view original, const std::string_view modified,
          const DiffMode mode, const DiffAlgorithm algorithm) -> Diff {
  Diff result;

  switch (mode) {
    case DiffMode::Line:
      result.original_ends_with_newline =
          tokenise_lines(original, result.original);
      result.modified_ends_with_newline =
          tokenise_lines(modified, result.modified);
      break;
  }

  std::unordered_map<std::string_view, std::size_t> complete;
  std::unordered_map<std::string_view, std::size_t> incomplete;
  std::size_t counter{0};
  const auto original_identities{
      to_identities(result.original, result.original_ends_with_newline,
                    complete, incomplete, counter)};
  const auto modified_identities{
      to_identities(result.modified, result.modified_ends_with_newline,
                    complete, incomplete, counter)};

  std::vector<bool> original_changed(original_identities.size(), false);
  std::vector<bool> modified_changed(modified_identities.size(), false);

  switch (algorithm) {
    case DiffAlgorithm::Myers: {
      const auto span{original_identities.size() + modified_identities.size()};
      internal::MyersWorkspace workspace{
          .forward = std::vector<std::ptrdiff_t>((2 * span) + 3, 0),
          .backward = std::vector<std::ptrdiff_t>((2 * span) + 3, 0),
          .offset = static_cast<std::ptrdiff_t>(span + 1)};
      internal::myers_compare(original_identities, 0,
                              original_identities.size(), modified_identities,
                              0, modified_identities.size(), original_changed,
                              modified_changed, workspace);
      break;
    }
  }

  compact_changes(original_identities, original_changed);
  compact_changes(modified_identities, modified_changed);

  result.operations = to_operations(original_changed, modified_changed);
  return result;
}

auto stringify(const Diff &document, std::ostream &stream,
               const DiffFormat format, const DiffFormatOptions &options)
    -> void {
  switch (format) {
    case DiffFormat::Unified:
      internal::stringify_diff_unified(document, stream, options);
      break;
  }
}

} // namespace sourcemeta::core

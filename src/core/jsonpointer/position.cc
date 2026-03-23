#include <sourcemeta/core/json_value.h>
#include <sourcemeta/core/jsonpointer.h>

#include <algorithm> // std::count_if
#include <cassert>   // assert
#include <cstddef>   // std::size_t
#include <cstdint>   // std::uint64_t
#include <optional>  // std::optional

namespace sourcemeta::core {

auto PointerPositionTracker::operator()(
    const JSON::ParsePhase phase, const JSON::Type, const std::uint64_t line,
    const std::uint64_t column, const JSON::ParseContext context,
    const std::size_t index, const JSON::String &property) -> void {
  this->events.push_back({.phase = phase,
                          .context = context,
                          .index = index,
                          .property = context == JSON::ParseContext::Property
                                          ? &property
                                          : nullptr,
                          .line = line,
                          .column = column});
}

auto PointerPositionTracker::get(const Pointer &pointer) const
    -> std::optional<Position> {
  const auto target_size{pointer.size()};
  std::size_t nesting{0};
  std::size_t path_depth{0};
  std::size_t matched{0};
  std::uint64_t start_line{0};
  std::uint64_t start_column{0};

  for (const auto &event : this->events) {
    switch (event.phase) {
      case JSON::ParsePhase::Pre:
        if (nesting > 0) {
          if (matched == path_depth && path_depth < target_size) {
            const auto &token{pointer.at(path_depth)};
            if ((event.context == JSON::ParseContext::Property &&
                 token.is_property() && event.property != nullptr &&
                 token.to_property() == *event.property) ||
                (event.context == JSON::ParseContext::Index &&
                 !token.is_property() && token.to_index() == event.index)) {
              matched += 1;
              if (matched == target_size) {
                start_line = event.line;
                start_column = event.column;
              }
            }
          }
          path_depth += 1;
        } else if (target_size == 0) {
          start_line = event.line;
          start_column = event.column;
        }

        nesting += 1;
        break;
      case JSON::ParsePhase::Post:
        nesting -= 1;

        if (nesting > 0) {
          path_depth -= 1;
          if (matched == target_size && path_depth + 1 == matched) {
            return Position{start_line, start_column, event.line, event.column};
          }
          if (matched > path_depth) {
            matched = path_depth;
          }
        } else if (target_size == 0) {
          return Position{start_line, start_column, event.line, event.column};
        }

        break;
      default:
        assert(false);
        break;
    }
  }

  return std::nullopt;
}

auto PointerPositionTracker::size() const -> std::size_t {
  return static_cast<std::size_t>(std::count_if(
      this->events.cbegin(), this->events.cend(), [](const Event &event) {
        return event.phase == JSON::ParsePhase::Post;
      }));
}

auto PointerPositionTracker::to_json() const -> JSON {
  auto result{JSON::make_object()};
  Pointer current;
  std::vector<std::pair<std::uint64_t, std::uint64_t>> start_stack;

  for (const auto &event : this->events) {
    switch (event.phase) {
      case JSON::ParsePhase::Pre:
        start_stack.emplace_back(event.line, event.column);
        switch (event.context) {
          case JSON::ParseContext::Property:
            assert(event.property != nullptr);
            current.push_back(*event.property);
            break;
          case JSON::ParseContext::Index:
            current.push_back(event.index);
            break;
          default:
            break;
        }

        break;
      case JSON::ParsePhase::Post:
        assert(!start_stack.empty());
        result.assign_assume_new(
            to_string(current),
            sourcemeta::core::to_json(Position{start_stack.back().first,
                                               start_stack.back().second,
                                               event.line, event.column}));
        start_stack.pop_back();
        if (!current.empty()) {
          current.pop_back();
        }

        break;
      default:
        assert(false);
        break;
    }
  }

  assert(current.empty());
  assert(start_stack.empty());
  return result;
}

} // namespace sourcemeta::core

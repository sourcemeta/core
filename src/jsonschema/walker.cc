#include <sourcemeta/jsontoolkit/jsonpointer.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>
#include <sourcemeta/jsontoolkit/jsonschema_walker.h>

namespace {
enum class SchemaWalkerype_t { Deep, Flat };

auto walk(sourcemeta::jsontoolkit::Pointer &pointer,
          std::vector<sourcemeta::jsontoolkit::Pointer> &pointers,
          const sourcemeta::jsontoolkit::JSON &subschema,
          const sourcemeta::jsontoolkit::SchemaWalker &walker,
          const sourcemeta::jsontoolkit::SchemaResolver &resolver,
          const std::string &dialect, const SchemaWalkerype_t type,
          const std::size_t level) -> void {
  if (!is_schema(subschema)) {
    return;
  }

  if (type == SchemaWalkerype_t::Deep || level > 0) {
    pointers.push_back(pointer);
  }

  // We can't recurse any further
  if (!subschema.is_object() ||
      (type == SchemaWalkerype_t::Flat && level > 0)) {
    return;
  }

  // Recalculate the dialect and its vocabularies at every step.
  // This is needed for correctly traversing through schemas that
  // contains pointers that use different dialect/vocabularies.
  // This is often the case for bundled schemas.
  const std::optional<std::string> current_dialect{
      sourcemeta::jsontoolkit::dialect(subschema, dialect)};
  const std::string &new_dialect{current_dialect.value()};
  const std::map<std::string, bool> vocabularies{
      sourcemeta::jsontoolkit::vocabularies(subschema, resolver, new_dialect)
          .get()};

  for (auto &pair : subschema.as_object()) {
    switch (walker(pair.first, vocabularies)) {
      case sourcemeta::jsontoolkit::SchemaWalkerStrategy::Value: {
        sourcemeta::jsontoolkit::Pointer new_pointer{pointer};
        new_pointer.emplace_back(pair.first);
        walk(new_pointer, pointers, pair.second, walker, resolver, new_dialect,
             type, level + 1);
      } break;
      case sourcemeta::jsontoolkit::SchemaWalkerStrategy::Elements:
        if (pair.second.is_array()) {
          for (std::size_t index = 0; index < pair.second.size(); index++) {
            sourcemeta::jsontoolkit::Pointer new_pointer{pointer};
            new_pointer.emplace_back(pair.first);
            new_pointer.emplace_back(index);
            walk(new_pointer, pointers, pair.second.at(index), walker, resolver,
                 new_dialect, type, level + 1);
          }
        }

        break;
      case sourcemeta::jsontoolkit::SchemaWalkerStrategy::Members:
        if (pair.second.is_object()) {
          for (auto &subpair : pair.second.as_object()) {
            sourcemeta::jsontoolkit::Pointer new_pointer{pointer};
            new_pointer.emplace_back(pair.first);
            new_pointer.emplace_back(subpair.first);
            walk(new_pointer, pointers, subpair.second, walker, resolver,
                 new_dialect, type, level + 1);
          }
        }

        break;
      case sourcemeta::jsontoolkit::SchemaWalkerStrategy::ValueOrElements:
        if (pair.second.is_array()) {
          for (std::size_t index = 0; index < pair.second.size(); index++) {
            sourcemeta::jsontoolkit::Pointer new_pointer{pointer};
            new_pointer.emplace_back(pair.first);
            new_pointer.emplace_back(index);
            walk(new_pointer, pointers, pair.second.at(index), walker, resolver,
                 new_dialect, type, level + 1);
          }
        } else {
          sourcemeta::jsontoolkit::Pointer new_pointer{pointer};
          new_pointer.emplace_back(pair.first);
          walk(new_pointer, pointers, pair.second, walker, resolver,
               new_dialect, type, level + 1);
        }

        break;
      case sourcemeta::jsontoolkit::SchemaWalkerStrategy::ElementsOrMembers:
        if (pair.second.is_array()) {
          for (std::size_t index = 0; index < pair.second.size(); index++) {
            sourcemeta::jsontoolkit::Pointer new_pointer{pointer};
            new_pointer.emplace_back(pair.first);
            new_pointer.emplace_back(index);
            walk(new_pointer, pointers, pair.second.at(index), walker, resolver,
                 new_dialect, type, level + 1);
          }
        } else if (pair.second.is_object()) {
          for (auto &subpair : pair.second.as_object()) {
            sourcemeta::jsontoolkit::Pointer new_pointer{pointer};
            new_pointer.emplace_back(pair.first);
            new_pointer.emplace_back(subpair.first);
            walk(new_pointer, pointers, subpair.second, walker, resolver,
                 new_dialect, type, level + 1);
          }
        }

        break;
      case sourcemeta::jsontoolkit::SchemaWalkerStrategy::None:
        break;
    }
  }
}
} // namespace

// TODO: These iterators are not very efficient. They traverse once on
// construction and then the client traverses again.

sourcemeta::jsontoolkit::SchemaIterator::SchemaIterator(
    const sourcemeta::jsontoolkit::JSON &schema,
    const sourcemeta::jsontoolkit::SchemaWalker &walker,
    const sourcemeta::jsontoolkit::SchemaResolver &resolver,
    const std::optional<std::string> &default_dialect) {
  const std::optional<std::string> dialect{
      sourcemeta::jsontoolkit::dialect(schema, default_dialect)};

  // If the given schema declares no dialect and the user didn't
  // not pass a default, then there is nothing we can do. We know
  // the current schema is a subschema, but cannot walk any further.
  if (!dialect.has_value()) {
    this->pointers.push_back(sourcemeta::jsontoolkit::Pointer{});
  } else {
    sourcemeta::jsontoolkit::Pointer pointer;
    walk(pointer, this->pointers, schema, walker, resolver, dialect.value(),
         SchemaWalkerype_t::Deep, 0);
  }
}

sourcemeta::jsontoolkit::SchemaIteratorFlat::SchemaIteratorFlat(
    const sourcemeta::jsontoolkit::JSON &schema,
    const sourcemeta::jsontoolkit::SchemaWalker &walker,
    const sourcemeta::jsontoolkit::SchemaResolver &resolver,
    const std::optional<std::string> &default_dialect) {
  const std::optional<std::string> dialect{
      sourcemeta::jsontoolkit::dialect(schema, default_dialect)};
  if (dialect.has_value()) {
    sourcemeta::jsontoolkit::Pointer pointer;
    walk(pointer, this->pointers, schema, walker, resolver, dialect.value(),
         SchemaWalkerype_t::Flat, 0);
  }
}

auto sourcemeta::jsontoolkit::SchemaIterator::begin() const -> const_iterator {
  return this->pointers.begin();
}
auto sourcemeta::jsontoolkit::SchemaIterator::end() const -> const_iterator {
  return this->pointers.end();
}
auto sourcemeta::jsontoolkit::SchemaIterator::cbegin() const -> const_iterator {
  return this->pointers.cbegin();
}
auto sourcemeta::jsontoolkit::SchemaIterator::cend() const -> const_iterator {
  return this->pointers.cend();
}

auto sourcemeta::jsontoolkit::SchemaIteratorFlat::begin() const
    -> const_iterator {
  return this->pointers.begin();
}
auto sourcemeta::jsontoolkit::SchemaIteratorFlat::end() const
    -> const_iterator {
  return this->pointers.end();
}
auto sourcemeta::jsontoolkit::SchemaIteratorFlat::cbegin() const
    -> const_iterator {
  return this->pointers.cbegin();
}
auto sourcemeta::jsontoolkit::SchemaIteratorFlat::cend() const
    -> const_iterator {
  return this->pointers.cend();
}

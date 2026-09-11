#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/uri.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace {

// Every key a fixture may declare. Anything else is a mistake that would
// otherwise go unnoticed, as the runner would simply not read it
// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
const std::vector<std::string> KNOWN_KEYS{
    "schema",      "resolver",     "defaultDialect", "defaultId",
    "defaultBase", "paths",        "root",           "identifierMode",
    "pointers",    "reachability", "standalone"};
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

// Every type of location and every mode that the frame reports, so that a
// fixture blessing an unknown one does not go unnoticed
// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
const std::vector<std::string> KNOWN_TYPES{"resource", "anchor", "pointer",
                                           "subschema"};
const std::vector<std::string> KNOWN_MODES{"root", "locations", "references",
                                           "pointers"};
// The keywords that framing writes a reference down for. A reference is
// recorded on the schema that makes it, with the keyword as the last token,
// so anything else means a reference came from a place nothing defines
const std::vector<std::string> REFERENCE_KEYWORDS{
    "$schema", "$ref", "$dynamicRef", "$recursiveRef"};
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

auto last_token(const sourcemeta::core::JSON::String &pointer)
    -> sourcemeta::core::JSON::String {
  const auto position{pointer.rfind('/')};
  return position == sourcemeta::core::JSON::String::npos
             ? pointer
             : pointer.substr(position + 1);
}

auto location_keys(const sourcemeta::core::JSON &frame)
    -> std::set<sourcemeta::core::JSON::String> {
  std::set<sourcemeta::core::JSON::String> result;
  for (const auto &kind : {"static", "dynamic"}) {
    for (const auto &location : frame.at("locations").at(kind).as_object()) {
      result.insert(location.first);
    }
  }

  return result;
}

// What the frame has to satisfy whatever the schema, so that a fixture cannot
// bless an expectation that contradicts the way locations and references are
// meant to relate to each other
auto check_frame_invariants(const sourcemeta::core::JSON &frame,
                            const bool standalone) -> void {
  EXPECT_TRUE(std::ranges::find(KNOWN_MODES, frame.at("mode").to_string()) !=
              KNOWN_MODES.cend());

  const auto keys{location_keys(frame)};
  std::set<sourcemeta::core::JSON::String> pointers;
  for (const auto &kind : {"static", "dynamic"}) {
    for (const auto &location : frame.at("locations").at(kind).as_object()) {
      pointers.insert(location.second.at("pointer").to_string());
    }
  }

  for (const auto &kind : {"static", "dynamic"}) {
    for (const auto &location : frame.at("locations").at(kind).as_object()) {
      const auto type{location.second.at("type").to_string()};
      EXPECT_TRUE(std::ranges::find(KNOWN_TYPES, type) != KNOWN_TYPES.cend());

      // Every location reports the identifier of the schema that was framed,
      // which is a property of the frame rather than of the location
      EXPECT_EQ(location.second.at("root"), frame.at("locations")
                                                .at("static")
                                                .as_object()
                                                .cbegin()
                                                ->second.at("root"));

      // A location is addressed by a base and the pointer that leads there
      // from it, so the pointer it reports relative to that base is the tail
      // of the pointer it reports from the top of the document
      const auto &pointer{location.second.at("pointer").to_string()};
      const auto &relative{location.second.at("relativePointer").to_string()};
      EXPECT_TRUE(pointer.ends_with(relative));

      // The key spells that same pointer as a URI fragment, and a schema that
      // declares no identifier has no base for it to hang off, so recovering
      // the fragment has to lead back to a tail of the pointer. It is a tail
      // rather than the whole of it because a location that sits under an
      // identifier is addressed from there as well as from the document
      const sourcemeta::core::URI uri{location.first};
      const auto fragment{uri.fragment()};
      if (type == "anchor") {
        // An anchor is addressed by name rather than by where it sits, and
        // the keyword that declares a recursive anchor names the empty one,
        // which leaves the key with no fragment to speak of
        if (fragment.has_value()) {
          EXPECT_FALSE(fragment.value().starts_with('/'));
        }
      } else if (fragment.has_value() && !fragment.value().empty()) {
        const auto recovered{sourcemeta::core::fragment_to_pointer(uri)};
        EXPECT_TRUE(recovered.has_value());
        if (recovered.has_value()) {
          EXPECT_TRUE(pointer.ends_with(
              sourcemeta::core::to_string(recovered.value())));
        }
      }

      // Where a location sits within the schema is either the top of it or
      // somewhere a location accounts for
      const auto &parent{location.second.at("parent")};
      if (parent.is_string()) {
        EXPECT_TRUE(pointers.contains(parent.to_string()));
      }

      // A reference that lands on a location necessarily passes through it
      if (location.second.at("hasReferencesTo").to_boolean()) {
        EXPECT_TRUE(location.second.at("hasReferencesThrough").to_boolean());
      }
    }
  }

  bool every_reference_resolves{true};
  for (const auto &reference : frame.at("references").as_array()) {
    const auto &origin{reference.at("origin").to_string()};
    EXPECT_FALSE(origin.empty());
    EXPECT_TRUE(std::ranges::find(REFERENCE_KEYWORDS, last_token(origin)) !=
                REFERENCE_KEYWORDS.cend());

    // A reference is recorded on the schema that makes it, which is a place
    // the frame accounts for whenever it accounts for pointers at all
    if (frame.at("mode").to_string() == "pointers") {
      EXPECT_TRUE(pointers.contains(
          origin.substr(0, origin.size() - last_token(origin).size() - 1)));
    }

    // Where a reference lands is spelled out whole and broken down, and the
    // two have to agree, a destination that is nothing but a fragment being
    // the one with no base to report
    const auto &destination{reference.at("destination").to_string()};
    const auto &base{reference.at("base")};
    const auto &fragment{reference.at("fragment")};
    sourcemeta::core::JSON::String recomposed{
        base.is_null() ? sourcemeta::core::JSON::String{} : base.to_string()};
    if (fragment.is_string()) {
      recomposed.push_back('#');
      recomposed.append(fragment.to_string());
    }

    EXPECT_EQ(recomposed, destination);

    // The dialect of a schema is not something the schema has to carry with
    // it, so a reference to one that is not there does not make the frame any
    // less standalone
    if (last_token(origin) != "$schema" && !keys.contains(destination)) {
      every_reference_resolves = false;
    }
  }

  // Standing alone is a question about references, so the modes that resolve
  // none of them report nothing rather than report that a schema with no
  // reference to speak of stands alone
  const auto mode{frame.at("mode").to_string()};
  if (mode == "root" || mode == "locations") {
    EXPECT_FALSE(standalone);
  } else {
    EXPECT_EQ(standalone, every_reference_resolves);
  }
}

auto make_resolver(const sourcemeta::core::JSON &test)
    -> sourcemeta::core::SchemaResolver {
  if (!test.defines("resolver")) {
    return sourcemeta::core::schema_resolver;
  }

  const auto &registry{test.at("resolver")};
  return [registry](const std::string_view identifier)
             -> sourcemeta::core::SchemaResolverResult {
    const auto *match{
        registry.try_at(sourcemeta::core::JSON::String{identifier})};
    if (match != nullptr) {
      return *match;
    }

    return sourcemeta::core::schema_resolver(identifier);
  };
}

// A frame keeps views into the default dialect, the default identifier, the
// default base, and the paths it was given, so all four have to outlive it.
// The caller owns this, as anything built inside `analyse` would dangle on
// return and only misbehave later, when the frame is read back
struct Inputs {
  sourcemeta::core::JSON::String default_dialect;
  sourcemeta::core::JSON::String default_id;
  sourcemeta::core::JSON::String default_base;
  std::vector<sourcemeta::core::Pointer> paths;
};

auto make_inputs(const sourcemeta::core::JSON &test) -> Inputs {
  Inputs inputs;
  const auto *raw_dialect{test.try_at("defaultDialect")};
  if (raw_dialect != nullptr && !raw_dialect->is_null()) {
    inputs.default_dialect = raw_dialect->to_string();
  }

  const auto *raw_id{test.try_at("defaultId")};
  if (raw_id != nullptr && !raw_id->is_null()) {
    inputs.default_id = raw_id->to_string();
  }

  const auto *raw_base{test.try_at("defaultBase")};
  if (raw_base != nullptr && !raw_base->is_null()) {
    inputs.default_base = raw_base->to_string();
  }

  if (test.defines("paths")) {
    for (const auto &path : test.at("paths").as_array()) {
      inputs.paths.push_back(sourcemeta::core::to_pointer(path.to_string()));
    }
  }

  return inputs;
}

auto make_paths(const sourcemeta::core::JSON &test, const Inputs &inputs)
    -> sourcemeta::core::SchemaFrame::Paths {
  sourcemeta::core::SchemaFrame::Paths paths;
  if (test.defines("paths")) {
    for (const auto &path : inputs.paths) {
      paths.push_back(sourcemeta::core::to_weak_pointer(path));
    }
  } else {
    paths.push_back(sourcemeta::core::EMPTY_WEAK_POINTER);
  }

  return paths;
}

auto make_identifier_mode(const sourcemeta::core::JSON &test)
    -> sourcemeta::core::SchemaFrame::IdentifierMode {
  const auto *identifier_mode{test.try_at("identifierMode")};
  return (identifier_mode != nullptr &&
          identifier_mode->to_string() == "fallback")
             ? sourcemeta::core::SchemaFrame::IdentifierMode::Fallback
             : sourcemeta::core::SchemaFrame::IdentifierMode::Additional;
}

// Whether the target location can be evaluated when validation starts from the
// origin location
auto check_reachability(const sourcemeta::core::SchemaFrame &frame,
                        const sourcemeta::core::SchemaResolver &resolver,
                        const sourcemeta::core::JSON &check) -> void {
  const auto type{check.defines("type") &&
                          check.at("type").to_string() == "dynamic"
                      ? sourcemeta::core::SchemaReferenceType::Dynamic
                      : sourcemeta::core::SchemaReferenceType::Static};
  const auto target{frame.location(type, check.at("to").to_string())};
  EXPECT_TRUE(target.has_value());
  const auto origin{frame.traverse(check.at("from").to_string())};
  EXPECT_TRUE(origin.has_value());
  EXPECT_EQ(frame.is_reachable(origin.value().get(), target.value().get(),
                               sourcemeta::core::schema_walker, resolver),
            check.at("reachable").to_boolean());
}

// Every mode below sourcemeta::core::SchemaFrame::Mode::Pointers reports a
// pointer location only when a reference names it, so the expectations for
// those follow from the one fixture we store rather than from near-duplicates
// of it
auto drop_pointer_locations(
    sourcemeta::core::JSON &document,
    const std::set<sourcemeta::core::JSON::String> &keep) -> void {
  for (const auto &kind : {"static", "dynamic"}) {
    auto &group{document.at("locations").at(kind)};
    std::vector<sourcemeta::core::JSON::String> doomed;
    for (const auto &location : group.as_object()) {
      if (location.second.at("type").to_string() == "pointer" &&
          !keep.contains(location.first)) {
        doomed.push_back(location.first);
      }
    }

    for (const auto &uri : doomed) {
      group.erase(uri);
    }
  }
}

auto reference_destinations(const sourcemeta::core::JSON &document)
    -> std::set<sourcemeta::core::JSON::String> {
  std::set<sourcemeta::core::JSON::String> result;
  for (const auto &reference : document.at("references").as_array()) {
    result.insert(reference.at("destination").to_string());
  }

  return result;
}

auto run_frame_test(const sourcemeta::core::JSON &test) -> void {
  for (const auto &entry : test.as_object()) {
    EXPECT_TRUE(std::ranges::find(KNOWN_KEYS, entry.first) !=
                KNOWN_KEYS.cend());
  }

  EXPECT_TRUE(test.defines("schema"));
  EXPECT_TRUE(test.defines("root"));
  EXPECT_TRUE(test.defines("pointers"));
  EXPECT_TRUE(test.defines("standalone"));

  const auto resolver{make_resolver(test)};
  const auto inputs{make_inputs(test)};
  const auto paths{make_paths(test, inputs)};
  const auto identifier_mode{make_identifier_mode(test)};

  const sourcemeta::core::SchemaFrame root{
      sourcemeta::core::SchemaFrame::Mode::Root,
      test.at("schema"),
      sourcemeta::core::schema_walker,
      resolver,
      inputs.default_dialect,
      inputs.default_id,
      identifier_mode,
      paths,
      inputs.default_base};
  const auto root_json{root.to_json(resolver)};
  EXPECT_EQ(root_json, test.at("root"));
  check_frame_invariants(root_json, root.standalone());

  const sourcemeta::core::SchemaFrame pointers{
      sourcemeta::core::SchemaFrame::Mode::Pointers,
      test.at("schema"),
      sourcemeta::core::schema_walker,
      resolver,
      inputs.default_dialect,
      inputs.default_id,
      identifier_mode,
      paths,
      inputs.default_base};
  const auto pointers_json{pointers.to_json(resolver)};
  EXPECT_EQ(pointers_json, test.at("pointers"));
  check_frame_invariants(pointers_json, pointers.standalone());

  // Wherever the mode that reports on a single schema gives it an address,
  // that address is one the mode that locates everything reports too, as each
  // mode is a superset of the one before it. The empty key is the way that
  // mode says the schema it analysed goes by no name of its own, which is not
  // an address for anything to agree with
  //
  // A caller-provided identifier is where the two part ways, as the mode that
  // reports on a single schema takes it for whichever schema it was pointed
  // at, while every other mode only ever takes it for the top of the
  // document. So the two disagree wherever a path leads below that
  const auto default_id_below_the_top{!inputs.default_id.empty() &&
                                      paths.size() == 1 &&
                                      !paths.front().empty()};
  const auto every_pointers_key{location_keys(pointers_json)};
  if (!default_id_below_the_top) {
    for (const auto &key : location_keys(root_json)) {
      EXPECT_TRUE(key.empty() || every_pointers_key.contains(key));
    }
  }

  // References mode locates every schema, but of the remaining pointers only
  // the ones that a reference actually names
  auto expected_references{test.at("pointers")};
  expected_references.assign("mode", sourcemeta::core::JSON{"references"});
  drop_pointer_locations(expected_references,
                         reference_destinations(expected_references));
  const sourcemeta::core::SchemaFrame references{
      sourcemeta::core::SchemaFrame::Mode::References,
      test.at("schema"),
      sourcemeta::core::schema_walker,
      resolver,
      inputs.default_dialect,
      inputs.default_id,
      identifier_mode,
      paths,
      inputs.default_base};
  const auto references_json{references.to_json(resolver)};
  EXPECT_EQ(references_json, expected_references);

  EXPECT_EQ(references.standalone(), test.at("standalone").to_boolean());
  check_frame_invariants(references_json, references.standalone());

  if (test.defines("reachability")) {
    for (const auto &check : test.at("reachability").as_array()) {
      check_reachability(references, resolver, check);
    }
  }

  // Locations mode resolves no reference at all, so it keeps no pointer that
  // only a reference would have justified either
  auto expected_locations{test.at("pointers")};
  expected_locations.assign("mode", sourcemeta::core::JSON{"locations"});
  expected_locations.assign("references", sourcemeta::core::JSON::make_array());
  drop_pointer_locations(expected_locations, {});
  for (const auto &kind : {"static", "dynamic"}) {
    auto &group{expected_locations.at("locations").at(kind)};
    std::vector<sourcemeta::core::JSON::String> uris;
    for (const auto &location : group.as_object()) {
      uris.push_back(location.first);
    }

    for (const auto &uri : uris) {
      group.at(uri).assign("hasReferencesTo", sourcemeta::core::JSON{false});
      group.at(uri).assign("hasReferencesThrough",
                           sourcemeta::core::JSON{false});
    }
  }
  const sourcemeta::core::SchemaFrame locations{
      sourcemeta::core::SchemaFrame::Mode::Locations,
      test.at("schema"),
      sourcemeta::core::schema_walker,
      resolver,
      inputs.default_dialect,
      inputs.default_id,
      identifier_mode,
      paths,
      inputs.default_base};
  const auto locations_json{locations.to_json(resolver)};
  EXPECT_EQ(locations_json, expected_locations);
  check_frame_invariants(locations_json, locations.standalone());
}

auto register_tests(const std::filesystem::path &directory) -> std::size_t {
  std::size_t count{0};
  for (const std::filesystem::directory_entry &entry :
       std::filesystem::recursive_directory_iterator{directory}) {
    if (!entry.is_regular_file() || entry.path().extension() != ".json") {
      continue;
    }

    const auto suite{entry.path().parent_path().filename().string()};
    std::ostringstream name;
    for (const auto character : entry.path().stem().string()) {
      name << (character == '-' ? '_' : character);
    }

    const auto test{sourcemeta::core::read_json(entry.path())};
    sourcemeta::core::test_register("FrameSuite_" + suite, name.str(), __FILE__,
                                    __LINE__,
                                    [test]() -> void { run_frame_test(test); });
    count += 1;
  }

  return count;
}

} // namespace

auto main(int argc, char **argv) -> int {
  const auto count{register_tests(std::filesystem::path{FRAME_SUITE_PATH})};
  // A fixture in the wrong place, or with the wrong extension, would otherwise
  // never run and nobody would notice
  if (count == 0) {
    std::cerr << "No frame fixtures found at " << FRAME_SUITE_PATH << "\n";
    return 1;
  }

  return sourcemeta::core::test_run(argc, argv);
}

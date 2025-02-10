#ifndef SOURCEMETA_CORE_JSONSCHEMA_TEST_UTILS_H_
#define SOURCEMETA_CORE_JSONSCHEMA_TEST_UTILS_H_

#include <cassert>
#include <sstream>
#include <tuple>
#include <vector>

#define EXPECT_POINTER_TEMPLATE(pointer, expected)                             \
  {                                                                            \
    std::ostringstream stream;                                                 \
    sourcemeta::core::stringify((pointer), stream);                            \
    EXPECT_EQ(stream.str(), (expected));                                       \
  }

#define EXPECT_POINTER_TEMPLATES(input_list, expected)                         \
  {                                                                            \
    const std::vector<std::string> expected_list(expected);                    \
    EXPECT_EQ(input_list.size(), expected_list.size());                        \
    for (std::size_t index = 0; index < (input_list).size(); index++) {        \
      std::ostringstream stream;                                               \
      sourcemeta::core::stringify((input_list)[index], stream);                \
      EXPECT_EQ(stream.str(), expected_list[index]);                           \
    }                                                                          \
  }

#define POINTER_TEMPLATES(...) std::vector<std::string>({__VA_ARGS__})

#define TO_POINTER(pointer_string)                                             \
  sourcemeta::core::to_pointer((pointer_string))

#define EXPECT_FRAME(frame, expected_type, reference, root_id,                 \
                     expected_pointer, expected_dialect,                       \
                     expected_base_dialect, expected_base,                     \
                     expected_relative_pointer, expected_instance_locations,   \
                     expected_destination_of_size)                             \
  EXPECT_TRUE((frame).locations().contains({(expected_type), (reference)}));   \
  EXPECT_TRUE((frame)                                                          \
                  .locations()                                                 \
                  .at({(expected_type), (reference)})                          \
                  .root.has_value());                                          \
  EXPECT_EQ(                                                                   \
      (frame).locations().at({(expected_type), (reference)}).root.value(),     \
      (root_id));                                                              \
  EXPECT_EQ((frame).locations().at({(expected_type), (reference)}).pointer,    \
            TO_POINTER(expected_pointer));                                     \
  EXPECT_EQ((frame).locations().at({(expected_type), (reference)}).dialect,    \
            (expected_dialect));                                               \
  EXPECT_EQ((frame).locations().at({(expected_type), (reference)}).base,       \
            (expected_base));                                                  \
  EXPECT_EQ(                                                                   \
      (frame).locations().at({(expected_type), (reference)}).base_dialect,     \
      (expected_base_dialect));                                                \
  EXPECT_EQ(                                                                   \
      (frame).locations().at({(expected_type), (reference)}).relative_pointer, \
      TO_POINTER(expected_relative_pointer));                                  \
  EXPECT_POINTER_TEMPLATES((frame)                                             \
                               .locations()                                    \
                               .at({(expected_type), (reference)})             \
                               .instance_locations,                            \
                           expected_instance_locations);                       \
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({(expected_type), (reference)})                            \
                .destination_of.size(),                                        \
            expected_destination_of_size);

#define EXPECT_FRAME_STATIC(                                                   \
    frame, reference, root_id, expected_pointer, expected_dialect,             \
    expected_base_dialect, expected_base, expected_relative_pointer,           \
    expected_instance_locations, expected_destination_of_size)                 \
  EXPECT_FRAME(frame, sourcemeta::core::SchemaReferenceType::Static,           \
               reference, root_id, expected_pointer, expected_dialect,         \
               expected_base_dialect, expected_base,                           \
               expected_relative_pointer, expected_instance_locations,         \
               expected_destination_of_size)

#define EXPECT_FRAME_STATIC_RESOURCE(                                          \
    frame, reference, root_id, expected_pointer, expected_dialect,             \
    expected_base_dialect, expected_base, expected_relative_pointer,           \
    expected_instance_locations, expected_destination_of_size)                 \
  EXPECT_FRAME_STATIC(frame, reference, root_id, expected_pointer,             \
                      expected_dialect, expected_base_dialect, expected_base,  \
                      expected_relative_pointer, expected_instance_locations,  \
                      expected_destination_of_size)                            \
  EXPECT_EQ(                                                                   \
      (frame)                                                                  \
          .locations()                                                         \
          .at({sourcemeta::core::SchemaReferenceType::Static, (reference)})    \
          .type,                                                               \
      sourcemeta::core::SchemaFrame::LocationType::Resource);

#define EXPECT_FRAME_STATIC_POINTER(                                           \
    frame, reference, root_id, expected_pointer, expected_dialect,             \
    expected_base_dialect, expected_base, expected_relative_pointer,           \
    expected_instance_locations, expected_destination_of_size)                 \
  EXPECT_FRAME_STATIC(frame, reference, root_id, expected_pointer,             \
                      expected_dialect, expected_base_dialect, expected_base,  \
                      expected_relative_pointer, expected_instance_locations,  \
                      expected_destination_of_size)                            \
  EXPECT_EQ(                                                                   \
      (frame)                                                                  \
          .locations()                                                         \
          .at({sourcemeta::core::SchemaReferenceType::Static, (reference)})    \
          .type,                                                               \
      sourcemeta::core::SchemaFrame::LocationType::Pointer);

#define EXPECT_FRAME_STATIC_SUBSCHEMA(                                         \
    frame, reference, root_id, expected_pointer, expected_dialect,             \
    expected_base_dialect, expected_base, expected_relative_pointer,           \
    expected_instance_locations, expected_destination_of_size)                 \
  EXPECT_FRAME_STATIC(frame, reference, root_id, expected_pointer,             \
                      expected_dialect, expected_base_dialect, expected_base,  \
                      expected_relative_pointer, expected_instance_locations,  \
                      expected_destination_of_size)                            \
  EXPECT_EQ(                                                                   \
      (frame)                                                                  \
          .locations()                                                         \
          .at({sourcemeta::core::SchemaReferenceType::Static, (reference)})    \
          .type,                                                               \
      sourcemeta::core::SchemaFrame::LocationType::Subschema);

#define EXPECT_FRAME_STATIC_ANCHOR(                                            \
    frame, reference, root_id, expected_pointer, expected_dialect,             \
    expected_base_dialect, expected_base, expected_relative_pointer,           \
    expected_instance_locations, expected_destination_of_size)                 \
  EXPECT_FRAME_STATIC(frame, reference, root_id, expected_pointer,             \
                      expected_dialect, expected_base_dialect, expected_base,  \
                      expected_relative_pointer, expected_instance_locations,  \
                      expected_destination_of_size)                            \
  EXPECT_EQ(                                                                   \
      (frame)                                                                  \
          .locations()                                                         \
          .at({sourcemeta::core::SchemaReferenceType::Static, (reference)})    \
          .type,                                                               \
      sourcemeta::core::SchemaFrame::LocationType::Anchor);

#define EXPECT_FRAME_DYNAMIC_ANCHOR(                                           \
    frame, reference, root_id, expected_pointer, expected_dialect,             \
    expected_base_dialect, expected_base, expected_relative_pointer,           \
    expected_instance_locations, expected_destination_of_size)                 \
  EXPECT_FRAME_DYNAMIC(frame, reference, root_id, expected_pointer,            \
                       expected_dialect, expected_base_dialect, expected_base, \
                       expected_relative_pointer, expected_instance_locations, \
                       expected_destination_of_size)                           \
  EXPECT_EQ(                                                                   \
      (frame)                                                                  \
          .locations()                                                         \
          .at({sourcemeta::core::SchemaReferenceType::Dynamic, (reference)})   \
          .type,                                                               \
      sourcemeta::core::SchemaFrame::LocationType::Anchor);

#define EXPECT_FRAME_DYNAMIC(                                                  \
    frame, reference, root_id, expected_pointer, expected_dialect,             \
    expected_base_dialect, expected_base, expected_relative_pointer,           \
    expected_instance_locations, expected_destination_of_size)                 \
  EXPECT_FRAME(frame, sourcemeta::core::SchemaReferenceType::Dynamic,          \
               reference, root_id, expected_pointer, expected_dialect,         \
               expected_base_dialect, expected_base,                           \
               expected_relative_pointer, expected_instance_locations,         \
               expected_destination_of_size)

#define __EXPECT_ANONYMOUS_FRAME(                                              \
    frame, expected_type, reference, expected_pointer, expected_dialect,       \
    expected_base_dialect, expected_instance_locations,                        \
    expected_destination_of_size)                                              \
  EXPECT_TRUE((frame).locations().contains({(expected_type), (reference)}));   \
  EXPECT_FALSE((frame)                                                         \
                   .locations()                                                \
                   .at({(expected_type), (reference)})                         \
                   .root.has_value());                                         \
  EXPECT_EQ((frame).locations().at({(expected_type), (reference)}).pointer,    \
            TO_POINTER(expected_pointer));                                     \
  EXPECT_EQ((frame).locations().at({(expected_type), (reference)}).dialect,    \
            (expected_dialect));                                               \
  EXPECT_EQ(                                                                   \
      (frame).locations().at({(expected_type), (reference)}).base_dialect,     \
      (expected_base_dialect));                                                \
  EXPECT_POINTER_TEMPLATES((frame)                                             \
                               .locations()                                    \
                               .at({(expected_type), (reference)})             \
                               .instance_locations,                            \
                           expected_instance_locations);                       \
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({(expected_type), (reference)})                            \
                .destination_of.size(),                                        \
            (expected_destination_of_size));

#define EXPECT_ANONYMOUS_FRAME_STATIC(frame, reference, expected_pointer,      \
                                      expected_dialect, expected_base_dialect, \
                                      expected_instance_locations,             \
                                      expected_destination_of_size)            \
  __EXPECT_ANONYMOUS_FRAME(                                                    \
      frame, sourcemeta::core::SchemaReferenceType::Static, reference,         \
      expected_pointer, expected_dialect, expected_base_dialect,               \
      expected_instance_locations, expected_destination_of_size)

#define EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(                                \
    frame, reference, expected_pointer, expected_dialect,                      \
    expected_base_dialect, expected_instance_locations,                        \
    expected_destination_of_size)                                              \
  __EXPECT_ANONYMOUS_FRAME(                                                    \
      frame, sourcemeta::core::SchemaReferenceType::Static, reference,         \
      expected_pointer, expected_dialect, expected_base_dialect,               \
      expected_instance_locations, expected_destination_of_size)               \
  EXPECT_EQ(                                                                   \
      (frame)                                                                  \
          .locations()                                                         \
          .at({sourcemeta::core::SchemaReferenceType::Static, (reference)})    \
          .type,                                                               \
      sourcemeta::core::SchemaFrame::LocationType::Resource);

#define EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(                                 \
    frame, reference, expected_pointer, expected_dialect,                      \
    expected_base_dialect, expected_instance_locations,                        \
    expected_destination_of_size)                                              \
  __EXPECT_ANONYMOUS_FRAME(                                                    \
      frame, sourcemeta::core::SchemaReferenceType::Static, reference,         \
      expected_pointer, expected_dialect, expected_base_dialect,               \
      expected_instance_locations, expected_destination_of_size)               \
  EXPECT_EQ(                                                                   \
      (frame)                                                                  \
          .locations()                                                         \
          .at({sourcemeta::core::SchemaReferenceType::Static, (reference)})    \
          .type,                                                               \
      sourcemeta::core::SchemaFrame::LocationType::Pointer);

#define EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(                               \
    frame, reference, expected_pointer, expected_dialect,                      \
    expected_base_dialect, expected_instance_locations,                        \
    expected_destination_of_size)                                              \
  __EXPECT_ANONYMOUS_FRAME(                                                    \
      frame, sourcemeta::core::SchemaReferenceType::Static, reference,         \
      expected_pointer, expected_dialect, expected_base_dialect,               \
      expected_instance_locations, expected_destination_of_size)               \
  EXPECT_EQ(                                                                   \
      (frame)                                                                  \
          .locations()                                                         \
          .at({sourcemeta::core::SchemaReferenceType::Static, (reference)})    \
          .type,                                                               \
      sourcemeta::core::SchemaFrame::LocationType::Subschema);

#define EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(                                  \
    frame, reference, expected_pointer, expected_dialect,                      \
    expected_base_dialect, expected_instance_locations,                        \
    expected_destination_of_size)                                              \
  __EXPECT_ANONYMOUS_FRAME(                                                    \
      frame, sourcemeta::core::SchemaReferenceType::Static, reference,         \
      expected_pointer, expected_dialect, expected_base_dialect,               \
      expected_instance_locations, expected_destination_of_size)               \
  EXPECT_EQ(                                                                   \
      (frame)                                                                  \
          .locations()                                                         \
          .at({sourcemeta::core::SchemaReferenceType::Static, (reference)})    \
          .type,                                                               \
      sourcemeta::core::SchemaFrame::LocationType::Anchor);

#define EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(                                 \
    frame, reference, expected_pointer, expected_dialect,                      \
    expected_base_dialect, expected_instance_locations,                        \
    expected_destination_of_size)                                              \
  __EXPECT_ANONYMOUS_FRAME(                                                    \
      frame, sourcemeta::core::SchemaReferenceType::Dynamic, reference,        \
      expected_pointer, expected_dialect, expected_base_dialect,               \
      expected_instance_locations, expected_destination_of_size)               \
  EXPECT_EQ(                                                                   \
      (frame)                                                                  \
          .locations()                                                         \
          .at({sourcemeta::core::SchemaReferenceType::Dynamic, (reference)})   \
          .type,                                                               \
      sourcemeta::core::SchemaFrame::LocationType::Anchor);

#define EXPECT_REFERENCE(frame, expected_type, expected_pointer, expected_uri, \
                         expected_base, expected_fragment)                     \
  EXPECT_TRUE((frame).references().contains(                                   \
      {expected_type, TO_POINTER(expected_pointer)}));                         \
  EXPECT_EQ((frame)                                                            \
                .references()                                                  \
                .at({expected_type, TO_POINTER(expected_pointer)})             \
                .destination,                                                  \
            (expected_uri));                                                   \
  EXPECT_EQ((frame)                                                            \
                .references()                                                  \
                .at({expected_type, TO_POINTER(expected_pointer)})             \
                .base,                                                         \
            (expected_base));                                                  \
  EXPECT_EQ((frame)                                                            \
                .references()                                                  \
                .at({expected_type, TO_POINTER(expected_pointer)})             \
                .fragment,                                                     \
            (expected_fragment));

#define EXPECT_STATIC_REFERENCE(frame, expected_pointer, expected_uri,         \
                                expected_base, expected_fragment)              \
  EXPECT_REFERENCE(frame, sourcemeta::core::SchemaReferenceType::Static,       \
                   expected_pointer, expected_uri, expected_base,              \
                   expected_fragment)

#define EXPECT_DYNAMIC_REFERENCE(frame, expected_pointer, expected_uri,        \
                                 expected_base, expected_fragment)             \
  EXPECT_REFERENCE(frame, sourcemeta::core::SchemaReferenceType::Dynamic,      \
                   expected_pointer, expected_uri, expected_base,              \
                   expected_fragment)

#define EXPECT_FRAME_DESTINATION_OF(frame, expected_type, expected_uri,        \
                                    expected_index, expected_size,             \
                                    expected_origin)                           \
  EXPECT_EQ(frame.locations()                                                  \
                .at({sourcemeta::core::SchemaReferenceType::expected_type,     \
                     expected_uri})                                            \
                .destination_of.size(),                                        \
            expected_size);                                                    \
  EXPECT_EQ(frame.locations()                                                  \
                .at({sourcemeta::core::SchemaReferenceType::expected_type,     \
                     expected_uri})                                            \
                .destination_of.at(expected_index)                             \
                .get()                                                         \
                .second,                                                       \
            expected_origin);

#define EXPECT_UNEVALUATED_STATIC(keywords, expected_pointer,                  \
                                  expected_dependencies_size)                  \
  EXPECT_EQ(keywords.at(expected_pointer).static_dependencies.size(),          \
            expected_dependencies_size);

#define EXPECT_UNEVALUATED_DYNAMIC(keywords, expected_pointer,                 \
                                   expected_dependencies_size)                 \
  EXPECT_EQ(keywords.at(expected_pointer).dynamic_dependencies.size(),         \
            expected_dependencies_size);

#define EXPECT_UNEVALUATED_UNRESOLVED(keywords, expected_pointer)              \
  EXPECT_TRUE(keywords.at(expected_pointer).unresolved);

#define EXPECT_UNEVALUATED_RESOLVED(keywords, expected_pointer)                \
  EXPECT_FALSE(keywords.at(expected_pointer).unresolved);

#define EXPECT_UNEVALUATED_STATIC_DEPENDENCY(keywords, expected_pointer,       \
                                             expected_destination)             \
  EXPECT_TRUE(                                                                 \
      keywords.at(expected_pointer)                                            \
          .static_dependencies.contains(TO_POINTER(expected_destination)));

#define EXPECT_UNEVALUATED_DYNAMIC_DEPENDENCY(keywords, expected_pointer,      \
                                              expected_destination)            \
  EXPECT_TRUE(                                                                 \
      keywords.at(expected_pointer)                                            \
          .dynamic_dependencies.contains(TO_POINTER(expected_destination)));

#define __EXPECT_OFFICIAL_WALKER_ENTRY(                                        \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_dialect, expected_base_dialect, expected_instance_location,       \
    expected_relative_instance_location)                                       \
  EXPECT_EQ(sourcemeta::core::to_string(entries.at(index).pointer),            \
            expected_schema_location);                                         \
  {                                                                            \
    const std::optional<std::string> expected_parent_location_optional{        \
        expected_parent_location};                                             \
    if (expected_parent_location_optional.has_value()) {                       \
      EXPECT_TRUE(entries.at(index).parent.has_value());                       \
      EXPECT_EQ(sourcemeta::core::to_string(entries.at(index).parent.value()), \
                expected_parent_location_optional.value());                    \
    } else {                                                                   \
      EXPECT_FALSE(entries.at(index).parent.has_value());                      \
    }                                                                          \
  }                                                                            \
  EXPECT_TRUE(entries.at(index).dialect.has_value());                          \
  EXPECT_TRUE(entries.at(index).base_dialect.has_value());                     \
  EXPECT_EQ(entries.at(index).dialect.value(), expected_dialect);              \
  EXPECT_EQ(entries.at(index).base_dialect.value(), expected_base_dialect);    \
  EXPECT_FALSE(entries.at(index).vocabularies.empty());                        \
  EXPECT_POINTER_TEMPLATE(entries.at(index).instance_location,                 \
                          expected_instance_location);                         \
  EXPECT_POINTER_TEMPLATE(entries.at(index).relative_instance_location,        \
                          expected_relative_instance_location);

#define EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(                                  \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "https://json-schema.org/draft/2020-12/schema",                          \
      "https://json-schema.org/draft/2020-12/schema",                          \
      expected_instance_location, expected_relative_instance_location);        \
  EXPECT_FALSE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_2020_12_ORPHAN(                           \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "https://json-schema.org/draft/2020-12/schema",                          \
      "https://json-schema.org/draft/2020-12/schema",                          \
      expected_instance_location, expected_relative_instance_location);        \
  EXPECT_TRUE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_2019_09(                                  \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "https://json-schema.org/draft/2019-09/schema",                          \
      "https://json-schema.org/draft/2019-09/schema",                          \
      expected_instance_location, expected_relative_instance_location);        \
  EXPECT_FALSE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_2019_09_ORPHAN(                           \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "https://json-schema.org/draft/2019-09/schema",                          \
      "https://json-schema.org/draft/2019-09/schema",                          \
      expected_instance_location, expected_relative_instance_location);        \
  EXPECT_TRUE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(                                   \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-07/schema#",                               \
      "http://json-schema.org/draft-07/schema#", expected_instance_location,   \
      expected_relative_instance_location);                                    \
  EXPECT_FALSE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7_ORPHAN(                            \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-07/schema#",                               \
      "http://json-schema.org/draft-07/schema#", expected_instance_location,   \
      expected_relative_instance_location);                                    \
  EXPECT_TRUE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(                                   \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-06/schema#",                               \
      "http://json-schema.org/draft-06/schema#", expected_instance_location,   \
      expected_relative_instance_location);                                    \
  EXPECT_FALSE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6_ORPHAN(                            \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-06/schema#",                               \
      "http://json-schema.org/draft-06/schema#", expected_instance_location,   \
      expected_relative_instance_location);                                    \
  EXPECT_TRUE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT4(                                   \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-04/schema#",                               \
      "http://json-schema.org/draft-04/schema#", expected_instance_location,   \
      expected_relative_instance_location);                                    \
  EXPECT_FALSE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT4_ORPHAN(                            \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-04/schema#",                               \
      "http://json-schema.org/draft-04/schema#", expected_instance_location,   \
      expected_relative_instance_location);                                    \
  EXPECT_TRUE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(                                   \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-03/schema#",                               \
      "http://json-schema.org/draft-03/schema#", expected_instance_location,   \
      expected_relative_instance_location);                                    \
  EXPECT_FALSE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3_ORPHAN(                            \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-03/schema#",                               \
      "http://json-schema.org/draft-03/schema#", expected_instance_location,   \
      expected_relative_instance_location);                                    \
  EXPECT_TRUE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(                                   \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-02/schema#",                               \
      "http://json-schema.org/draft-02/hyper-schema#",                         \
      expected_instance_location, expected_relative_instance_location);        \
  EXPECT_FALSE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2_ORPHAN(                            \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-02/schema#",                               \
      "http://json-schema.org/draft-02/hyper-schema#",                         \
      expected_instance_location, expected_relative_instance_location);        \
  EXPECT_TRUE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT1(                                   \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-01/schema#",                               \
      "http://json-schema.org/draft-01/hyper-schema#",                         \
      expected_instance_location, expected_relative_instance_location);        \
  EXPECT_FALSE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT1_ORPHAN(                            \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-01/schema#",                               \
      "http://json-schema.org/draft-01/hyper-schema#",                         \
      expected_instance_location, expected_relative_instance_location);        \
  EXPECT_TRUE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT0(                                   \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-00/schema#",                               \
      "http://json-schema.org/draft-00/hyper-schema#",                         \
      expected_instance_location, expected_relative_instance_location);        \
  EXPECT_FALSE(entries.at(index).orphan);

#define EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT0_ORPHAN(                            \
    entries, index, expected_schema_location, expected_parent_location,        \
    expected_instance_location, expected_relative_instance_location)           \
  __EXPECT_OFFICIAL_WALKER_ENTRY(                                              \
      entries, index, expected_schema_location, expected_parent_location,      \
      "http://json-schema.org/draft-00/schema#",                               \
      "http://json-schema.org/draft-00/hyper-schema#",                         \
      expected_instance_location, expected_relative_instance_location);        \
  EXPECT_TRUE(entries.at(index).orphan);

#endif

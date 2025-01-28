#ifndef SOURCEMETA_CORE_JSONSCHEMA_TEST_UTILS_H_
#define SOURCEMETA_CORE_JSONSCHEMA_TEST_UTILS_H_

#include <cassert>
#include <tuple>
#include <vector>

#define TO_POINTER(pointer_string)                                             \
  sourcemeta::core::to_pointer((pointer_string))

#define EXPECT_FRAME(frame, expected_type, reference, root_id,                 \
                     expected_pointer, expected_dialect,                       \
                     expected_base_dialect, expected_base,                     \
                     expected_relative_pointer, expected_destination_of_size)  \
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
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({(expected_type), (reference)})                            \
                .destination_of.size(),                                        \
            expected_destination_of_size);

#define EXPECT_FRAME_STATIC(frame, reference, root_id, expected_pointer,       \
                            expected_dialect, expected_base_dialect,           \
                            expected_base, expected_relative_pointer,          \
                            expected_destination_of_size)                      \
  EXPECT_FRAME(frame, sourcemeta::core::ReferenceType::Static, reference,      \
               root_id, expected_pointer, expected_dialect,                    \
               expected_base_dialect, expected_base,                           \
               expected_relative_pointer, expected_destination_of_size)

#define EXPECT_FRAME_STATIC_RESOURCE(                                          \
    frame, reference, root_id, expected_pointer, expected_dialect,             \
    expected_base_dialect, expected_base, expected_relative_pointer,           \
    expected_destination_of_size)                                              \
  EXPECT_FRAME_STATIC(frame, reference, root_id, expected_pointer,             \
                      expected_dialect, expected_base_dialect, expected_base,  \
                      expected_relative_pointer, expected_destination_of_size) \
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({sourcemeta::core::ReferenceType::Static, (reference)})    \
                .type,                                                         \
            sourcemeta::core::Frame::LocationType::Resource);

#define EXPECT_FRAME_STATIC_POINTER(                                           \
    frame, reference, root_id, expected_pointer, expected_dialect,             \
    expected_base_dialect, expected_base, expected_relative_pointer,           \
    expected_destination_of_size)                                              \
  EXPECT_FRAME_STATIC(frame, reference, root_id, expected_pointer,             \
                      expected_dialect, expected_base_dialect, expected_base,  \
                      expected_relative_pointer, expected_destination_of_size) \
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({sourcemeta::core::ReferenceType::Static, (reference)})    \
                .type,                                                         \
            sourcemeta::core::Frame::LocationType::Pointer);

#define EXPECT_FRAME_STATIC_SUBSCHEMA(                                         \
    frame, reference, root_id, expected_pointer, expected_dialect,             \
    expected_base_dialect, expected_base, expected_relative_pointer,           \
    expected_destination_of_size)                                              \
  EXPECT_FRAME_STATIC(frame, reference, root_id, expected_pointer,             \
                      expected_dialect, expected_base_dialect, expected_base,  \
                      expected_relative_pointer, expected_destination_of_size) \
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({sourcemeta::core::ReferenceType::Static, (reference)})    \
                .type,                                                         \
            sourcemeta::core::Frame::LocationType::Subschema);

#define EXPECT_FRAME_STATIC_ANCHOR(                                            \
    frame, reference, root_id, expected_pointer, expected_dialect,             \
    expected_base_dialect, expected_base, expected_relative_pointer,           \
    expected_destination_of_size)                                              \
  EXPECT_FRAME_STATIC(frame, reference, root_id, expected_pointer,             \
                      expected_dialect, expected_base_dialect, expected_base,  \
                      expected_relative_pointer, expected_destination_of_size) \
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({sourcemeta::core::ReferenceType::Static, (reference)})    \
                .type,                                                         \
            sourcemeta::core::Frame::LocationType::Anchor);

#define EXPECT_FRAME_DYNAMIC_ANCHOR(                                           \
    frame, reference, root_id, expected_pointer, expected_dialect,             \
    expected_base_dialect, expected_base, expected_relative_pointer,           \
    expected_destination_of_size)                                              \
  EXPECT_FRAME_DYNAMIC(frame, reference, root_id, expected_pointer,            \
                       expected_dialect, expected_base_dialect, expected_base, \
                       expected_relative_pointer,                              \
                       expected_destination_of_size)                           \
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({sourcemeta::core::ReferenceType::Dynamic, (reference)})   \
                .type,                                                         \
            sourcemeta::core::Frame::LocationType::Anchor);

#define EXPECT_FRAME_DYNAMIC(frame, reference, root_id, expected_pointer,      \
                             expected_dialect, expected_base_dialect,          \
                             expected_base, expected_relative_pointer,         \
                             expected_destination_of_size)                     \
  EXPECT_FRAME(frame, sourcemeta::core::ReferenceType::Dynamic, reference,     \
               root_id, expected_pointer, expected_dialect,                    \
               expected_base_dialect, expected_base,                           \
               expected_relative_pointer, expected_destination_of_size)

#define __EXPECT_ANONYMOUS_FRAME(                                              \
    frame, expected_type, reference, expected_pointer, expected_dialect,       \
    expected_base_dialect, expected_destination_of_size)                       \
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
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({(expected_type), (reference)})                            \
                .destination_of.size(),                                        \
            (expected_destination_of_size));

#define EXPECT_ANONYMOUS_FRAME_STATIC(frame, reference, expected_pointer,      \
                                      expected_dialect, expected_base_dialect, \
                                      expected_destination_of_size)            \
  __EXPECT_ANONYMOUS_FRAME(frame, sourcemeta::core::ReferenceType::Static,     \
                           reference, expected_pointer, expected_dialect,      \
                           expected_base_dialect,                              \
                           expected_destination_of_size)

#define EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(                                \
    frame, reference, expected_pointer, expected_dialect,                      \
    expected_base_dialect, expected_destination_of_size)                       \
  __EXPECT_ANONYMOUS_FRAME(frame, sourcemeta::core::ReferenceType::Static,     \
                           reference, expected_pointer, expected_dialect,      \
                           expected_base_dialect,                              \
                           expected_destination_of_size)                       \
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({sourcemeta::core::ReferenceType::Static, (reference)})    \
                .type,                                                         \
            sourcemeta::core::Frame::LocationType::Resource);

#define EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(                                 \
    frame, reference, expected_pointer, expected_dialect,                      \
    expected_base_dialect, expected_destination_of_size)                       \
  __EXPECT_ANONYMOUS_FRAME(frame, sourcemeta::core::ReferenceType::Static,     \
                           reference, expected_pointer, expected_dialect,      \
                           expected_base_dialect,                              \
                           expected_destination_of_size)                       \
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({sourcemeta::core::ReferenceType::Static, (reference)})    \
                .type,                                                         \
            sourcemeta::core::Frame::LocationType::Pointer);

#define EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(                               \
    frame, reference, expected_pointer, expected_dialect,                      \
    expected_base_dialect, expected_destination_of_size)                       \
  __EXPECT_ANONYMOUS_FRAME(frame, sourcemeta::core::ReferenceType::Static,     \
                           reference, expected_pointer, expected_dialect,      \
                           expected_base_dialect,                              \
                           expected_destination_of_size)                       \
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({sourcemeta::core::ReferenceType::Static, (reference)})    \
                .type,                                                         \
            sourcemeta::core::Frame::LocationType::Subschema);

#define EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(                                  \
    frame, reference, expected_pointer, expected_dialect,                      \
    expected_base_dialect, expected_destination_of_size)                       \
  __EXPECT_ANONYMOUS_FRAME(frame, sourcemeta::core::ReferenceType::Static,     \
                           reference, expected_pointer, expected_dialect,      \
                           expected_base_dialect,                              \
                           expected_destination_of_size)                       \
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({sourcemeta::core::ReferenceType::Static, (reference)})    \
                .type,                                                         \
            sourcemeta::core::Frame::LocationType::Anchor);

#define EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(                                 \
    frame, reference, expected_pointer, expected_dialect,                      \
    expected_base_dialect, expected_destination_of_size)                       \
  __EXPECT_ANONYMOUS_FRAME(frame, sourcemeta::core::ReferenceType::Dynamic,    \
                           reference, expected_pointer, expected_dialect,      \
                           expected_base_dialect,                              \
                           expected_destination_of_size)                       \
  EXPECT_EQ((frame)                                                            \
                .locations()                                                   \
                .at({sourcemeta::core::ReferenceType::Dynamic, (reference)})   \
                .type,                                                         \
            sourcemeta::core::Frame::LocationType::Anchor);

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
  EXPECT_REFERENCE(frame, sourcemeta::core::ReferenceType::Static,             \
                   expected_pointer, expected_uri, expected_base,              \
                   expected_fragment)

#define EXPECT_DYNAMIC_REFERENCE(frame, expected_pointer, expected_uri,        \
                                 expected_base, expected_fragment)             \
  EXPECT_REFERENCE(frame, sourcemeta::core::ReferenceType::Dynamic,            \
                   expected_pointer, expected_uri, expected_base,              \
                   expected_fragment)

#define EXPECT_FRAME_DESTINATION_OF(frame, expected_type, expected_uri,        \
                                    expected_index, expected_origin)           \
  EXPECT_EQ(                                                                   \
      frame.locations()                                                        \
          .at({sourcemeta::core::ReferenceType::expected_type, expected_uri})  \
          .destination_of.at(expected_index)                                   \
          .get()                                                               \
          .first,                                                              \
      sourcemeta::core::ReferenceType::Static);                                \
  EXPECT_EQ(                                                                   \
      frame.locations()                                                        \
          .at({sourcemeta::core::ReferenceType::expected_type, expected_uri})  \
          .destination_of.at(expected_index)                                   \
          .get()                                                               \
          .second,                                                             \
      TO_POINTER(expected_origin));

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

#endif

#ifndef SOURCEMETA_CORE_URLPATTERN_TEST_UTILS_H_
#define SOURCEMETA_CORE_URLPATTERN_TEST_UTILS_H_

#define EXPECT_COMPONENT_PARSE_ERROR(ComponentType, input, expected_column)    \
  try {                                                                        \
    sourcemeta::core::ComponentType result{(input)};                           \
    FAIL();                                                                    \
  } catch (const sourcemeta::core::URLPatternParseError &error) {              \
    EXPECT_EQ(error.column(), (expected_column));                              \
    SUCCEED();                                                                 \
  } catch (...) {                                                              \
    FAIL();                                                                    \
  }

#define EXPECT_PART_WITHOUT_VALUE(result, index, expected_type)                \
  EXPECT_TRUE(std::holds_alternative<sourcemeta::core::expected_type>(         \
      (result).value.at(index)));

#define EXPECT_PART_WITH_VALUE(result, index, expected_type, expected_value)   \
  EXPECT_PART_WITHOUT_VALUE(result, index, expected_type);                     \
  EXPECT_EQ(                                                                   \
      std::get<sourcemeta::core::expected_type>((result).value.at(index))      \
          .value,                                                              \
      (expected_value));

#define EXPECT_PART_WITH_REGEX(result, index, expected_type, expected_pattern) \
  EXPECT_PART_WITHOUT_VALUE(result, index, expected_type);                     \
  EXPECT_EQ(                                                                   \
      std::get<sourcemeta::core::expected_type>((result).value.at(index))      \
          .original_pattern,                                                   \
      (expected_pattern));

#define EXPECT_PART_WITH_VALUE_AND_REGEX(result, index, expected_type,         \
                                         expected_value, expected_pattern)     \
  EXPECT_PART_WITHOUT_VALUE(result, index, expected_type);                     \
  EXPECT_EQ(                                                                   \
      std::get<sourcemeta::core::expected_type>((result).value.at(index))      \
          .value,                                                              \
      (expected_value));                                                       \
  EXPECT_EQ(                                                                   \
      std::get<sourcemeta::core::expected_type>((result).value.at(index))      \
          .original_pattern,                                                   \
      (expected_pattern));

#define EXPECT_PART_SIZE(result, index, expected_type, expected_size)          \
  EXPECT_PART_WITHOUT_VALUE(result, index, expected_type)                      \
  EXPECT_EQ(                                                                   \
      std::get<sourcemeta::core::expected_type>((result).value.at(index))      \
          .value.size(),                                                       \
      (expected_size));

#define EXPECT_PART_GROUP(result, index, expected_type, expected_size,         \
                          expected_has_inner_segment_prefix,                   \
                          expected_has_inner_segment_suffix)                   \
  EXPECT_PART_SIZE(result, index, expected_type, expected_size)                \
  EXPECT_EQ(                                                                   \
      std::get<sourcemeta::core::expected_type>((result).value.at(index))      \
          .has_inner_segment_prefix,                                           \
      (expected_has_inner_segment_prefix));                                    \
  EXPECT_EQ(                                                                   \
      std::get<sourcemeta::core::expected_type>((result).value.at(index))      \
          .has_inner_segment_suffix,                                           \
      (expected_has_inner_segment_suffix));

#define EXPECT_PART_NESTED_WITHOUT_VALUE(result, index, expected_type,         \
                                         subindex, expected_subtype)           \
  EXPECT_PART_WITHOUT_VALUE(result, index, expected_type);                     \
  EXPECT_TRUE(std::holds_alternative<sourcemeta::core::expected_subtype>(      \
      std::get<sourcemeta::core::expected_type>((result).value.at(index))      \
          .value.at(subindex)));

#define EXPECT_PART_NESTED_WITH_VALUE(result, index, expected_type, subindex,  \
                                      expected_subtype, expected_value)        \
  EXPECT_PART_NESTED_WITHOUT_VALUE(result, index, expected_type, subindex,     \
                                   expected_subtype);                          \
  EXPECT_EQ(                                                                   \
      std::get<sourcemeta::core::expected_subtype>(                            \
          std::get<sourcemeta::core::expected_type>((result).value.at(index))  \
              .value.at(subindex))                                             \
          .value,                                                              \
      (expected_value));

#define EXPECT_PART_COMPLEX_SIZE(result, index, expected_size)                 \
  EXPECT_PART_WITHOUT_VALUE(result, index, URLPatternPartComplexSegment)       \
  EXPECT_EQ(std::get<sourcemeta::core::URLPatternPartComplexSegment>(          \
                (result).value.at(index))                                      \
                .value.size(),                                                 \
            (expected_size));

#define EXPECT_PART_COMPLEX_WITHOUT_VALUE(result, index, subindex,             \
                                          expected_subtype)                    \
  EXPECT_PART_WITHOUT_VALUE(result, index, URLPatternPartComplexSegment);      \
  EXPECT_TRUE(std::holds_alternative<sourcemeta::core::expected_subtype>(      \
      std::get<sourcemeta::core::URLPatternPartComplexSegment>(                \
          (result).value.at(index))                                            \
          .value.at(subindex)));

#define EXPECT_PART_COMPLEX_WITH_VALUE(result, index, subindex,                \
                                       expected_subtype, expected_value)       \
  EXPECT_PART_COMPLEX_WITHOUT_VALUE(result, index, subindex,                   \
                                    expected_subtype);                         \
  EXPECT_EQ(std::get<sourcemeta::core::expected_subtype>(                      \
                std::get<sourcemeta::core::URLPatternPartComplexSegment>(      \
                    (result).value.at(index))                                  \
                    .value.at(subindex))                                       \
                .value,                                                        \
            (expected_value));

#define EXPECT_PART_COMPLEX_NESTED_SIZE(result, index, subindex,               \
                                        expected_subtype, expected_size)       \
  EXPECT_PART_COMPLEX_WITHOUT_VALUE(result, index, subindex, expected_subtype) \
  EXPECT_EQ(std::get<sourcemeta::core::expected_subtype>(                      \
                std::get<sourcemeta::core::URLPatternPartComplexSegment>(      \
                    (result).value.at(index))                                  \
                    .value.at(subindex))                                       \
                .value.size(),                                                 \
            (expected_size));

#define EXPECT_PART_COMPLEX_GROUP(                                             \
    result, index, subindex, expected_subtype, expected_size,                  \
    expected_has_inner_segment_prefix, expected_has_inner_segment_suffix)      \
  EXPECT_PART_COMPLEX_NESTED_SIZE(result, index, subindex, expected_subtype,   \
                                  expected_size)                               \
  EXPECT_EQ(std::get<sourcemeta::core::expected_subtype>(                      \
                std::get<sourcemeta::core::URLPatternPartComplexSegment>(      \
                    (result).value.at(index))                                  \
                    .value.at(subindex))                                       \
                .has_inner_segment_prefix,                                     \
            (expected_has_inner_segment_prefix));                              \
  EXPECT_EQ(std::get<sourcemeta::core::expected_subtype>(                      \
                std::get<sourcemeta::core::URLPatternPartComplexSegment>(      \
                    (result).value.at(index))                                  \
                    .value.at(subindex))                                       \
                .has_inner_segment_suffix,                                     \
            (expected_has_inner_segment_suffix));

#define EXPECT_PART_COMPLEX_NESTED_WITHOUT_VALUE(                              \
    result, index, subindex, expected_subtype, subsubindex,                    \
    expected_subsubtype)                                                       \
  EXPECT_PART_COMPLEX_WITHOUT_VALUE(result, index, subindex,                   \
                                    expected_subtype);                         \
  EXPECT_TRUE(std::holds_alternative<sourcemeta::core::expected_subsubtype>(   \
      std::get<sourcemeta::core::expected_subtype>(                            \
          std::get<sourcemeta::core::URLPatternPartComplexSegment>(            \
              (result).value.at(index))                                        \
              .value.at(subindex))                                             \
          .value.at(subsubindex)));

#define EXPECT_PART_COMPLEX_NESTED_WITH_VALUE(                                 \
    result, index, subindex, expected_subtype, subsubindex,                    \
    expected_subsubtype, expected_value)                                       \
  EXPECT_PART_COMPLEX_NESTED_WITHOUT_VALUE(result, index, subindex,            \
                                           expected_subtype, subsubindex,      \
                                           expected_subsubtype);               \
  EXPECT_EQ(std::get<sourcemeta::core::expected_subsubtype>(                   \
                std::get<sourcemeta::core::expected_subtype>(                  \
                    std::get<sourcemeta::core::URLPatternPartComplexSegment>(  \
                        (result).value.at(index))                              \
                        .value.at(subindex))                                   \
                    .value.at(subsubindex))                                    \
                .value,                                                        \
            (expected_value));

#define EXPECT_SINGLE_PART_WITHOUT_VALUE(component, expected_type)             \
  EXPECT_TRUE(std::holds_alternative<sourcemeta::core::expected_type>(         \
      (component).value));

#define EXPECT_SINGLE_PART_WITH_VALUE(component, expected_type,                \
                                      expected_value)                          \
  EXPECT_SINGLE_PART_WITHOUT_VALUE(component, expected_type);                  \
  EXPECT_EQ(                                                                   \
      std::get<sourcemeta::core::expected_type>((component).value).value,      \
      (expected_value));

#define EXPECT_SINGLE_PART_WITH_REGEX(component, expected_type,                \
                                      expected_pattern)                        \
  EXPECT_SINGLE_PART_WITHOUT_VALUE(component, expected_type);                  \
  EXPECT_EQ(std::get<sourcemeta::core::expected_type>((component).value)       \
                .original_pattern,                                             \
            (expected_pattern));

#define EXPECT_SINGLE_PART_WITH_VALUE_AND_REGEX(                               \
    component, expected_type, expected_value, expected_pattern)                \
  EXPECT_SINGLE_PART_WITHOUT_VALUE(component, expected_type);                  \
  EXPECT_EQ(                                                                   \
      std::get<sourcemeta::core::expected_type>((component).value).value,      \
      (expected_value));                                                       \
  EXPECT_EQ(std::get<sourcemeta::core::expected_type>((component).value)       \
                .original_pattern,                                             \
            (expected_pattern));

#define EXPECT_PART_VECTOR_SIZE(component, expected_size)                      \
  EXPECT_EQ((component).value.size(), (expected_size));

#endif

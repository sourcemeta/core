#ifndef SOURCEMETA_CORE_ALTERSCHEMA_TEST_UTILS_H_
#define SOURCEMETA_CORE_ALTERSCHEMA_TEST_UTILS_H_

#include <sourcemeta/core/alterschema.h>
#include <sourcemeta/core/jsonschema.h>

#include <tuple>
#include <vector>

#define LINT_WITHOUT_FIX_FOR_READABILITY(document, result, traces)             \
  std::vector<std::tuple<sourcemeta::core::Pointer, std::string, std::string,  \
                         sourcemeta::core::SchemaTransformRule::Result>>       \
      traces;                                                                  \
  sourcemeta::core::SchemaTransformer bundle;                                  \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaMode::Readability);       \
  const auto result =                                                          \
      bundle.check(document, sourcemeta::core::schema_official_walker,         \
                   sourcemeta::core::schema_official_resolver,                 \
                   [&traces](const auto &pointer, const auto &name,            \
                             const auto &message, const auto &outcome) {       \
                     traces.emplace_back(pointer, name, message, outcome);     \
                   });

#define EXPECT_LINT_TRACE(traces, index, pointer, name, message)               \
  EXPECT_EQ(sourcemeta::core::to_string(std::get<0>((traces).at(index))),      \
            (pointer));                                                        \
  EXPECT_EQ(std::get<1>((traces).at(index)), (name));                          \
  EXPECT_EQ(std::get<2>((traces).at(index)), (message));

#define LINT_AND_FIX_FOR_READABILITY(traces)                                   \
  sourcemeta::core::SchemaTransformer bundle;                                  \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaMode::Readability);       \
  bundle.apply(document, sourcemeta::core::schema_official_walker,             \
               sourcemeta::core::schema_official_resolver,                     \
               [](const auto &, const auto &, const auto &, const auto &) {});

#define LINT_AND_FIX_FOR_READABILITY_STRICT(traces)                            \
  sourcemeta::core::SchemaTransformer bundle;                                  \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaMode::ReadabilityStrict); \
  bundle.apply(document, sourcemeta::core::schema_official_walker,             \
               sourcemeta::core::schema_official_resolver,                     \
               [](const auto &, const auto &, const auto &, const auto &) {});

#define LINT_AND_FIX_FOR_STATIC_ANALYSIS(document)                             \
  sourcemeta::core::SchemaTransformer bundle;                                  \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaMode::StaticAnalysis);    \
  bundle.apply(document, sourcemeta::core::schema_official_walker,             \
               sourcemeta::core::schema_official_resolver,                     \
               [](const auto &, const auto &, const auto &, const auto &) {});

#endif

#ifndef SOURCEMETA_CORE_ALTERSCHEMA_TEST_UTILS_H_
#define SOURCEMETA_CORE_ALTERSCHEMA_TEST_UTILS_H_

#include <sourcemeta/core/alterschema.h>
#include <sourcemeta/core/jsonschema.h>

#define LINT_AND_FIX_FOR_READABILITY(document)                                 \
  sourcemeta::core::SchemaTransformer bundle;                                  \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaCategory::AntiPattern);   \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaCategory::Simplify);      \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaCategory::Superfluous);   \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaCategory::Redundant);     \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaCategory::SyntaxSugar);   \
  bundle.apply(document, sourcemeta::core::schema_official_walker,             \
               sourcemeta::core::schema_official_resolver,                     \
               [](const auto &, const auto &, const auto &, const auto &) {});

#define LINT_AND_FIX_FOR_ANALYSIS(document)                                    \
  sourcemeta::core::SchemaTransformer bundle;                                  \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaCategory::AntiPattern);   \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaCategory::Simplify);      \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaCategory::Superfluous);   \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaCategory::Implicit);      \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaCategory::Desugar);       \
  bundle.apply(document, sourcemeta::core::schema_official_walker,             \
               sourcemeta::core::schema_official_resolver,                     \
               [](const auto &, const auto &, const auto &, const auto &) {});

#endif

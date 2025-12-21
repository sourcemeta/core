#ifndef SOURCEMETA_CORE_ALTERSCHEMA_TEST_UTILS_H_
#define SOURCEMETA_CORE_ALTERSCHEMA_TEST_UTILS_H_

#include <sourcemeta/core/alterschema.h>
#include <sourcemeta/core/jsonschema.h>

#include <tuple>
#include <vector>

static auto alterschema_test_resolver(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier ==
      "https://sourcemeta.com/2020-12-custom-vocabulary-optional") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/2020-12-custom-vocabulary-optional",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$vocabulary": {
        "https://json-schema.org/draft/2020-12/vocab/core": true,
        "https://sourcemeta.com/2020-12-custom-vocabulary-optional": false
      }
    })JSON");
  } else {
    return sourcemeta::core::schema_resolver(identifier);
  }
}

#define LINT_WITHOUT_FIX(document, result, traces)                             \
  std::vector<std::tuple<sourcemeta::core::Pointer, std::string, std::string,  \
                         sourcemeta::core::SchemaTransformRule::Result>>       \
      traces;                                                                  \
  sourcemeta::core::SchemaTransformer bundle;                                  \
  sourcemeta::core::add(bundle, sourcemeta::core::AlterSchemaMode::Linter);    \
  const auto result = bundle.check(                                            \
      document, sourcemeta::core::schema_walker, alterschema_test_resolver,    \
      [&traces](const auto &pointer, const auto &name, const auto &message,    \
                const auto &outcome) {                                         \
        traces.emplace_back(pointer, name, message, outcome);                  \
      });

#define EXPECT_LINT_TRACE(traces, index, pointer, name, message)               \
  EXPECT_EQ(sourcemeta::core::to_string(std::get<0>((traces).at(index))),      \
            (pointer));                                                        \
  EXPECT_EQ(std::get<1>((traces).at(index)), (name));                          \
  EXPECT_EQ(std::get<2>((traces).at(index)), (message));

#define LINT_AND_FIX(traces)                                                   \
  sourcemeta::core::SchemaTransformer bundle;                                  \
  sourcemeta::core::add(bundle, sourcemeta::core::AlterSchemaMode::Linter);    \
  bundle.apply(document, sourcemeta::core::schema_walker,                      \
               alterschema_test_resolver,                                      \
               [](const auto &, const auto &, const auto &, const auto &) {});

#define CANONICALIZE(document)                                                 \
  sourcemeta::core::SchemaTransformer bundle;                                  \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaMode::Canonicalizer);     \
  bundle.apply(document, sourcemeta::core::schema_walker,                      \
               alterschema_test_resolver,                                      \
               [](const auto &, const auto &, const auto &, const auto &) {});

#endif

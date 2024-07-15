#ifndef SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_DEFAULT_COMPILER_2019_09_H_
#define SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_DEFAULT_COMPILER_2019_09_H_

#include <sourcemeta/jsontoolkit/jsonschema.h>
#include <sourcemeta/jsontoolkit/jsonschema_compile.h>

#include "compile_helpers.h"

namespace internal {
using namespace sourcemeta::jsontoolkit;

auto compiler_2019_09_validation_dependentrequired(
    const SchemaCompilerContext &,
    const SchemaCompilerSchemaContext &schema_context,
    const SchemaCompilerDynamicContext &dynamic_context)
    -> SchemaCompilerTemplate {
  if (!schema_context.schema.at(dynamic_context.keyword).is_object()) {
    return {};
  }

  SchemaCompilerTemplate children;
  for (const auto &entry :
       schema_context.schema.at(dynamic_context.keyword).as_object()) {
    if (!entry.second.is_array()) {
      continue;
    }

    std::set<JSON::String> properties;
    for (const auto &property : entry.second.as_array()) {
      assert(property.is_string());
      properties.emplace(property.to_string());
    }

    children.push_back(make<SchemaCompilerInternalDefinesAll>(
        schema_context, relative_dynamic_context, std::move(properties),
        {make<SchemaCompilerAssertionDefines>(
            schema_context, relative_dynamic_context, entry.first, {},
            SchemaCompilerTargetType::Instance)},
        SchemaCompilerTargetType::Instance));
  }

  return {make<SchemaCompilerLogicalAnd>(
      schema_context, dynamic_context, SchemaCompilerValueNone{},
      std::move(children), type_condition(schema_context, JSON::Type::Object))};
}

} // namespace internal
#endif

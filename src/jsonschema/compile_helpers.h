#ifndef SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_COMPILE_HELPERS_H_
#define SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_COMPILE_HELPERS_H_

#include <sourcemeta/jsontoolkit/jsonschema_compile.h>

#include <utility> // std::declval, std::move

namespace sourcemeta::jsontoolkit {

static const SchemaCompilerDynamicContext relative_dynamic_context{
    "", empty_pointer, empty_pointer};

inline auto keyword_location(const SchemaCompilerSchemaContext &schema_context)
    -> std::string {
  return to_uri(schema_context.relative_pointer, schema_context.base)
      .recompose();
}

inline auto relative_schema_location(
    const SchemaCompilerDynamicContext &context) -> Pointer {
  return context.keyword.empty()
             ? context.base_schema_location
             : context.base_schema_location.concat({context.keyword});
}

// Instantiate a value-oriented step
template <typename Step>
auto make(const bool report, const SchemaCompilerContext &context,
          const SchemaCompilerSchemaContext &schema_context,
          const SchemaCompilerDynamicContext &dynamic_context,
          // Take the value type from the "type" property of the step struct
          decltype(std::declval<Step>().value) &&value,
          SchemaCompilerTemplate &&condition) -> Step {
  return {relative_schema_location(dynamic_context),
          dynamic_context.base_instance_location,
          keyword_location(schema_context),
          schema_context.base.recompose(),
          context.uses_dynamic_scopes,
          report,
          std::move(value),
          std::move(condition)};
}

// Instantiate an applicator step
template <typename Step>
auto make(const bool report, const SchemaCompilerContext &context,
          const SchemaCompilerSchemaContext &schema_context,
          const SchemaCompilerDynamicContext &dynamic_context,
          // Take the value type from the "value" property of the step struct
          decltype(std::declval<Step>().value) &&value,
          SchemaCompilerTemplate &&children,
          SchemaCompilerTemplate &&condition) -> Step {
  return {relative_schema_location(dynamic_context),
          dynamic_context.base_instance_location,
          keyword_location(schema_context),
          schema_context.base.recompose(),
          context.uses_dynamic_scopes,
          report,
          std::move(value),
          std::move(children),
          std::move(condition)};
}

// Instantiate a control step
template <typename Step>
auto make(const bool report, const SchemaCompilerContext &context,
          const SchemaCompilerSchemaContext &schema_context,
          const SchemaCompilerDynamicContext &dynamic_context,
          // Take the value type from the "id" property of the step struct
          decltype(std::declval<Step>().id) &&id,
          SchemaCompilerTemplate &&children) -> Step {
  return {relative_schema_location(dynamic_context),
          dynamic_context.base_instance_location,
          keyword_location(schema_context),
          schema_context.base.recompose(),
          context.uses_dynamic_scopes,
          report,
          std::move(id),
          std::move(children)};
}

// TODO: Completely get rid of this. We should never have
// explicit type conditions
inline auto type_condition(const SchemaCompilerContext &context,
                           const SchemaCompilerSchemaContext &schema_context,
                           const JSON::Type type) -> SchemaCompilerTemplate {
  // As an optimization
  if (schema_context.schema.is_object() &&
      schema_context.schema.defines("type") &&
      schema_context.schema.at("type").is_string()) {
    const auto &type_string{schema_context.schema.at("type").to_string()};
    if (type == JSON::Type::Null && type_string == "null") {
      return {};
    } else if (type == JSON::Type::Boolean && type_string == "boolean") {
      return {};
    } else if (type == JSON::Type::Integer && type_string == "integer") {
      return {};
    } else if (type == JSON::Type::String && type_string == "string") {
      return {};
    } else if (type == JSON::Type::Array && type_string == "array") {
      return {};
    } else if (type == JSON::Type::Object && type_string == "object") {
      return {};
    }
  }

  return {make<SchemaCompilerAssertionTypeStrict>(
      true, context, schema_context, relative_dynamic_context,
      SchemaCompilerValueType{type}, {})};
}

} // namespace sourcemeta::jsontoolkit

#endif

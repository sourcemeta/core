#ifndef SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_COMPILE_H_
#define SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_COMPILE_H_

#include "jsonschema_export.h"

#include <sourcemeta/jsontoolkit/jsonschema_reference.h>
#include <sourcemeta/jsontoolkit/jsonschema_resolver.h>
#include <sourcemeta/jsontoolkit/jsonschema_walker.h>

#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonpointer.h>
#include <sourcemeta/jsontoolkit/uri.h>

#include <functional> // std::function
#include <map>        // std::map
#include <optional>   // std::optional, std::nullopt
#include <regex>      // std::regex
#include <set>        // std::set
#include <string>     // std::string
#include <tuple>      // std::tuple
#include <utility>    // std::move, std::pair
#include <variant>    // std::variant
#include <vector>     // std::vector

namespace sourcemeta::jsontoolkit {

/// @ingroup jsonschema
/// Represents a compiler step empty value
struct SchemaCompilerValueNone {};

/// @ingroup jsonschema
/// Represents a compiler step JSON value
using SchemaCompilerValueJSON = JSON;

/// @ingroup jsonschema
/// Represents a set of JSON values
using SchemaCompilerValueArray = std::set<JSON>;

/// @ingroup jsonschema
/// Represents a compiler step string value
using SchemaCompilerValueString = JSON::String;

/// @ingroup jsonschema
/// Represents a compiler step string values
using SchemaCompilerValueStrings = std::set<JSON::String>;

/// @ingroup jsonschema
/// Represents a compiler step JSON type value
using SchemaCompilerValueType = JSON::Type;

/// @ingroup jsonschema
/// Represents a compiler step JSON types value
using SchemaCompilerValueTypes = std::set<JSON::Type>;

/// @ingroup jsonschema
/// Represents a compiler step ECMA regular expression value. We store both the
/// original string and the regular expression as standard regular expressions
/// do not keep a copy of their original value (which we need for serialization
/// purposes)
using SchemaCompilerValueRegex = std::pair<std::regex, std::string>;

/// @ingroup jsonschema
/// Represents a compiler step JSON unsigned integer value
using SchemaCompilerValueUnsignedInteger = std::size_t;

/// @ingroup jsonschema
/// Represents a compiler step range value. The boolean option
/// modifies whether the range is considered exhaustively or
/// if the evaluator is allowed to break early
using SchemaCompilerValueRange =
    std::tuple<std::size_t, std::optional<std::size_t>, bool>;

/// @ingroup jsonschema
/// Represents a compiler step boolean value
using SchemaCompilerValueBoolean = bool;

/// @ingroup jsonschema
/// Represents a compiler step string to index map
using SchemaCompilerValueNamedIndexes =
    std::map<SchemaCompilerValueString, SchemaCompilerValueUnsignedInteger>;

/// @ingroup jsonschema
/// Represents a compiler step string logical type
enum class SchemaCompilerValueStringType { URI };

/// @ingroup jsonschema
/// Represents an array loop compiler step annotation keywords
struct SchemaCompilerValueItemsAnnotationKeywords {
  const SchemaCompilerValueString index;
  const SchemaCompilerValueStrings filter;
  const SchemaCompilerValueStrings mask;
};

/// @ingroup jsonschema
/// Represents an compiler step that maps strings to strings
using SchemaCompilerValueStringMap =
    std::map<SchemaCompilerValueString, SchemaCompilerValueStrings>;

/// @ingroup jsonschema
/// Represents a compiler step JSON value accompanied with an index
using SchemaCompilerValueIndexedJSON =
    std::pair<SchemaCompilerValueUnsignedInteger, JSON>;

/// @ingroup jsonschema
/// Represents a compiler assertion step that always fails
struct SchemaCompilerAssertionFail;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks if an object defines a
/// given property
struct SchemaCompilerAssertionDefines;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks if an object defines a
/// set of properties
struct SchemaCompilerAssertionDefinesAll;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks if an object defines a
/// set of properties if it defines other set of properties
struct SchemaCompilerAssertionPropertyDependencies;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks if a document is of the
/// given type
struct SchemaCompilerAssertionType;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks if a document is of any of
/// the given types
struct SchemaCompilerAssertionTypeAny;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks if a document is of the
/// given type (strict version)
struct SchemaCompilerAssertionTypeStrict;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks if a document is of any of
/// the given types (strict version)
struct SchemaCompilerAssertionTypeStrictAny;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a string against an ECMA
/// regular expression
struct SchemaCompilerAssertionRegex;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a given string has less
/// than a certain number of characters
struct SchemaCompilerAssertionStringSizeLess;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a given string has greater
/// than a certain number of characters
struct SchemaCompilerAssertionStringSizeGreater;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a given array has less
/// than a certain number of items
struct SchemaCompilerAssertionArraySizeLess;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a given array has greater
/// than a certain number of items
struct SchemaCompilerAssertionArraySizeGreater;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a given object has less
/// than a certain number of properties
struct SchemaCompilerAssertionObjectSizeLess;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a given object has greater
/// than a certain number of properties
struct SchemaCompilerAssertionObjectSizeGreater;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks the instance equals a given
/// JSON document
struct SchemaCompilerAssertionEqual;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks that a JSON document is
/// equal to at least one of the given elements
struct SchemaCompilerAssertionEqualsAny;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a JSON document is greater
/// than or equal to another JSON document
struct SchemaCompilerAssertionGreaterEqual;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a JSON document is less
/// than or equal to another JSON document
struct SchemaCompilerAssertionLessEqual;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a JSON document is greater
/// than another JSON document
struct SchemaCompilerAssertionGreater;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a JSON document is less
/// than another JSON document
struct SchemaCompilerAssertionLess;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a given JSON array does not
/// contain duplicate items
struct SchemaCompilerAssertionUnique;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks a number is divisible by
/// another number
struct SchemaCompilerAssertionDivisible;

/// @ingroup jsonschema
/// Represents a compiler assertion step that checks that a string is of a
/// certain type
struct SchemaCompilerAssertionStringType;

/// @ingroup jsonschema
/// Represents a compiler step that emits an annotation
struct SchemaCompilerAnnotationEmit;

/// @ingroup jsonschema
/// Represents a compiler step that emits an annotation when the size of the
/// array instance is equal to the given size
struct SchemaCompilerAnnotationWhenArraySizeEqual;

/// @ingroup jsonschema
/// Represents a compiler step that emits an annotation when the size of the
/// array instance is greater than the given size
struct SchemaCompilerAnnotationWhenArraySizeGreater;

/// @ingroup jsonschema
/// Represents a compiler step that emits an annotation to the parent
struct SchemaCompilerAnnotationToParent;

/// @ingroup jsonschema
/// Represents a compiler step that emits the current basename as an annotation
/// to the parent
struct SchemaCompilerAnnotationBasenameToParent;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents a disjunction
struct SchemaCompilerLogicalOr;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents a conjunction
struct SchemaCompilerLogicalAnd;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents an exclusive disjunction
struct SchemaCompilerLogicalXor;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents a conjunction that always
/// reports success and marks its outcome for other steps
struct SchemaCompilerLogicalTryMark;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents a negation
struct SchemaCompilerLogicalNot;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents a conjunction
/// when the instance is of a given type
struct SchemaCompilerLogicalWhenType;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents a conjunction
/// when the instance is an object and defines a given property
struct SchemaCompilerLogicalWhenDefines;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents a conjunction
/// when the instance and desired evaluation path was not marked
struct SchemaCompilerLogicalWhenAdjacentUnmarked;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents a conjunction
/// when the instance and desired evaluation path was marked
struct SchemaCompilerLogicalWhenAdjacentMarked;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents a conjunction
/// when the array instance size is greater than the given number
struct SchemaCompilerLogicalWhenArraySizeGreater;

/// @ingroup jsonschema
/// Represents a compiler logical step that represents a conjunction
/// when the array instance size is equal to the given number
struct SchemaCompilerLogicalWhenArraySizeEqual;

/// @ingroup jsonschema
/// Represents a compiler step that matches steps to object properties
struct SchemaCompilerLoopPropertiesMatch;

/// @ingroup jsonschema
/// Represents a compiler step that loops over object properties
struct SchemaCompilerLoopProperties;

/// @ingroup jsonschema
/// Represents a compiler step that loops over object properties that match a
/// given ECMA regular expression
struct SchemaCompilerLoopPropertiesRegex;

/// @ingroup jsonschema
/// Represents a compiler step that loops over object properties that were
/// not collected as adjacent annotations
struct SchemaCompilerLoopPropertiesNoAdjacentAnnotation;

/// @ingroup jsonschema
/// Represents a compiler step that loops over object properties that were
/// not collected as annotations
struct SchemaCompilerLoopPropertiesNoAnnotation;

/// @ingroup jsonschema
/// Represents a compiler step that loops over object property keys
struct SchemaCompilerLoopKeys;

/// @ingroup jsonschema
/// Represents a compiler step that loops over array items starting from a given
/// index
struct SchemaCompilerLoopItems;

/// @ingroup jsonschema
/// Represents a compiler step that loops over array items when the array is
/// considered unmarked
struct SchemaCompilerLoopItemsUnmarked;

/// @ingroup jsonschema
/// Represents a compiler step that loops over unevaluated array items
struct SchemaCompilerLoopItemsUnevaluated;

/// @ingroup jsonschema
/// Represents a compiler step that checks array items match a given criteria
struct SchemaCompilerLoopContains;

/// @ingroup jsonschema
/// Represents a compiler step that consists of a mark to jump to while
/// executing children instructions
struct SchemaCompilerControlLabel;

/// @ingroup jsonschema
/// Represents a compiler step that consists of a mark to jump to, but without
/// executing children instructions
struct SchemaCompilerControlMark;

/// @ingroup jsonschema
/// Represents a compiler step that consists of jumping into a pre-registered
/// label
struct SchemaCompilerControlJump;

/// @ingroup jsonschema
/// Represents a compiler step that consists of jump to a dynamic anchor
struct SchemaCompilerControlDynamicAnchorJump;

/// @ingroup jsonschema
/// Represents a schema compilation step that can be evaluated
using SchemaCompilerTemplate = std::vector<std::variant<
    SchemaCompilerAssertionFail, SchemaCompilerAssertionDefines,
    SchemaCompilerAssertionDefinesAll,
    SchemaCompilerAssertionPropertyDependencies, SchemaCompilerAssertionType,
    SchemaCompilerAssertionTypeAny, SchemaCompilerAssertionTypeStrict,
    SchemaCompilerAssertionTypeStrictAny, SchemaCompilerAssertionRegex,
    SchemaCompilerAssertionStringSizeLess,
    SchemaCompilerAssertionStringSizeGreater,
    SchemaCompilerAssertionArraySizeLess,
    SchemaCompilerAssertionArraySizeGreater,
    SchemaCompilerAssertionObjectSizeLess,
    SchemaCompilerAssertionObjectSizeGreater, SchemaCompilerAssertionEqual,
    SchemaCompilerAssertionEqualsAny, SchemaCompilerAssertionGreaterEqual,
    SchemaCompilerAssertionLessEqual, SchemaCompilerAssertionGreater,
    SchemaCompilerAssertionLess, SchemaCompilerAssertionUnique,
    SchemaCompilerAssertionDivisible, SchemaCompilerAssertionStringType,
    SchemaCompilerAnnotationEmit, SchemaCompilerAnnotationWhenArraySizeEqual,
    SchemaCompilerAnnotationWhenArraySizeGreater,
    SchemaCompilerAnnotationToParent, SchemaCompilerAnnotationBasenameToParent,
    SchemaCompilerLogicalOr, SchemaCompilerLogicalAnd, SchemaCompilerLogicalXor,
    SchemaCompilerLogicalTryMark, SchemaCompilerLogicalNot,
    SchemaCompilerLogicalWhenType, SchemaCompilerLogicalWhenDefines,
    SchemaCompilerLogicalWhenAdjacentUnmarked,
    SchemaCompilerLogicalWhenAdjacentMarked,
    SchemaCompilerLogicalWhenArraySizeGreater,
    SchemaCompilerLogicalWhenArraySizeEqual, SchemaCompilerLoopPropertiesMatch,
    SchemaCompilerLoopProperties, SchemaCompilerLoopPropertiesRegex,
    SchemaCompilerLoopPropertiesNoAdjacentAnnotation,
    SchemaCompilerLoopPropertiesNoAnnotation, SchemaCompilerLoopKeys,
    SchemaCompilerLoopItems, SchemaCompilerLoopItemsUnmarked,
    SchemaCompilerLoopItemsUnevaluated, SchemaCompilerLoopContains,
    SchemaCompilerControlLabel, SchemaCompilerControlMark,
    SchemaCompilerControlJump, SchemaCompilerControlDynamicAnchorJump>>;

#if !defined(DOXYGEN)
#define DEFINE_STEP_WITH_VALUE(category, name, type)                           \
  struct SchemaCompiler##category##name {                                      \
    const Pointer relative_schema_location;                                    \
    const Pointer relative_instance_location;                                  \
    const std::string keyword_location;                                        \
    const std::string schema_resource;                                         \
    const bool dynamic;                                                        \
    const bool report;                                                         \
    const type value;                                                          \
  };

#define DEFINE_STEP_APPLICATOR(category, name, type)                           \
  struct SchemaCompiler##category##name {                                      \
    const Pointer relative_schema_location;                                    \
    const Pointer relative_instance_location;                                  \
    const std::string keyword_location;                                        \
    const std::string schema_resource;                                         \
    const bool dynamic;                                                        \
    const bool report;                                                         \
    const type value;                                                          \
    const SchemaCompilerTemplate children;                                     \
  };

DEFINE_STEP_WITH_VALUE(Assertion, Fail, SchemaCompilerValueNone)
DEFINE_STEP_WITH_VALUE(Assertion, Defines, SchemaCompilerValueString)
DEFINE_STEP_WITH_VALUE(Assertion, DefinesAll, SchemaCompilerValueStrings)
DEFINE_STEP_WITH_VALUE(Assertion, PropertyDependencies,
                       SchemaCompilerValueStringMap)
DEFINE_STEP_WITH_VALUE(Assertion, Type, SchemaCompilerValueType)
DEFINE_STEP_WITH_VALUE(Assertion, TypeAny, SchemaCompilerValueTypes)
DEFINE_STEP_WITH_VALUE(Assertion, TypeStrict, SchemaCompilerValueType)
DEFINE_STEP_WITH_VALUE(Assertion, TypeStrictAny, SchemaCompilerValueTypes)
DEFINE_STEP_WITH_VALUE(Assertion, Regex, SchemaCompilerValueRegex)
DEFINE_STEP_WITH_VALUE(Assertion, StringSizeLess,
                       SchemaCompilerValueUnsignedInteger)
DEFINE_STEP_WITH_VALUE(Assertion, StringSizeGreater,
                       SchemaCompilerValueUnsignedInteger)
DEFINE_STEP_WITH_VALUE(Assertion, ArraySizeLess,
                       SchemaCompilerValueUnsignedInteger)
DEFINE_STEP_WITH_VALUE(Assertion, ArraySizeGreater,
                       SchemaCompilerValueUnsignedInteger)
DEFINE_STEP_WITH_VALUE(Assertion, ObjectSizeLess,
                       SchemaCompilerValueUnsignedInteger)
DEFINE_STEP_WITH_VALUE(Assertion, ObjectSizeGreater,
                       SchemaCompilerValueUnsignedInteger)
DEFINE_STEP_WITH_VALUE(Assertion, Equal, SchemaCompilerValueJSON)
DEFINE_STEP_WITH_VALUE(Assertion, EqualsAny, SchemaCompilerValueArray)
DEFINE_STEP_WITH_VALUE(Assertion, GreaterEqual, SchemaCompilerValueJSON)
DEFINE_STEP_WITH_VALUE(Assertion, LessEqual, SchemaCompilerValueJSON)
DEFINE_STEP_WITH_VALUE(Assertion, Greater, SchemaCompilerValueJSON)
DEFINE_STEP_WITH_VALUE(Assertion, Less, SchemaCompilerValueJSON)
DEFINE_STEP_WITH_VALUE(Assertion, Unique, SchemaCompilerValueNone)
DEFINE_STEP_WITH_VALUE(Assertion, Divisible, SchemaCompilerValueJSON)
DEFINE_STEP_WITH_VALUE(Assertion, StringType, SchemaCompilerValueStringType)
DEFINE_STEP_WITH_VALUE(Annotation, Emit, SchemaCompilerValueJSON)
DEFINE_STEP_WITH_VALUE(Annotation, WhenArraySizeEqual,
                       SchemaCompilerValueIndexedJSON)
DEFINE_STEP_WITH_VALUE(Annotation, WhenArraySizeGreater,
                       SchemaCompilerValueIndexedJSON)
DEFINE_STEP_WITH_VALUE(Annotation, ToParent, SchemaCompilerValueJSON)
DEFINE_STEP_WITH_VALUE(Annotation, BasenameToParent, SchemaCompilerValueNone)
DEFINE_STEP_APPLICATOR(Logical, Or, SchemaCompilerValueBoolean)
DEFINE_STEP_APPLICATOR(Logical, And, SchemaCompilerValueNone)
DEFINE_STEP_APPLICATOR(Logical, Xor, SchemaCompilerValueNone)
DEFINE_STEP_APPLICATOR(Logical, TryMark, SchemaCompilerValueNone)
DEFINE_STEP_APPLICATOR(Logical, Not, SchemaCompilerValueNone)
DEFINE_STEP_APPLICATOR(Logical, WhenType, SchemaCompilerValueType)
DEFINE_STEP_APPLICATOR(Logical, WhenDefines, SchemaCompilerValueString)
DEFINE_STEP_APPLICATOR(Logical, WhenAdjacentUnmarked, SchemaCompilerValueString)
DEFINE_STEP_APPLICATOR(Logical, WhenAdjacentMarked, SchemaCompilerValueString)
DEFINE_STEP_APPLICATOR(Logical, WhenArraySizeGreater,
                       SchemaCompilerValueUnsignedInteger)
DEFINE_STEP_APPLICATOR(Logical, WhenArraySizeEqual,
                       SchemaCompilerValueUnsignedInteger)
DEFINE_STEP_APPLICATOR(Loop, PropertiesMatch, SchemaCompilerValueNamedIndexes)
DEFINE_STEP_APPLICATOR(Loop, Properties, SchemaCompilerValueNone)
DEFINE_STEP_APPLICATOR(Loop, PropertiesRegex, SchemaCompilerValueRegex)
DEFINE_STEP_APPLICATOR(Loop, PropertiesNoAdjacentAnnotation,
                       SchemaCompilerValueStrings)
DEFINE_STEP_APPLICATOR(Loop, PropertiesNoAnnotation, SchemaCompilerValueStrings)
DEFINE_STEP_APPLICATOR(Loop, Keys, SchemaCompilerValueNone)
DEFINE_STEP_APPLICATOR(Loop, Items, SchemaCompilerValueUnsignedInteger)
DEFINE_STEP_APPLICATOR(Loop, ItemsUnmarked, SchemaCompilerValueStrings)
DEFINE_STEP_APPLICATOR(Loop, ItemsUnevaluated,
                       SchemaCompilerValueItemsAnnotationKeywords)
DEFINE_STEP_APPLICATOR(Loop, Contains, SchemaCompilerValueRange)
DEFINE_STEP_APPLICATOR(Control, Label, SchemaCompilerValueUnsignedInteger)
DEFINE_STEP_APPLICATOR(Control, Mark, SchemaCompilerValueUnsignedInteger)
DEFINE_STEP_APPLICATOR(Control, Jump, SchemaCompilerValueUnsignedInteger)
DEFINE_STEP_APPLICATOR(Control, DynamicAnchorJump, SchemaCompilerValueString)

#undef DEFINE_STEP_WITH_VALUE
#undef DEFINE_STEP_APPLICATOR
#endif

/// @ingroup jsonschema
/// The schema compiler context is the current subschema information you have at
/// your disposal to implement a keyword
struct SchemaCompilerSchemaContext {
  /// The schema location relative to the base URI
  const Pointer &relative_pointer;
  /// The current subschema
  const JSON &schema;
  /// The schema vocabularies in use
  const std::map<std::string, bool> &vocabularies;
  /// The schema base URI
  const URI &base;
  /// The set of labels registered so far
  std::set<std::size_t> labels;
};

/// @ingroup jsonschema
/// The dynamic compiler context is the read-write information you have at your
/// disposal to implement a keyword
struct SchemaCompilerDynamicContext {
  /// The schema keyword
  const std::string &keyword;
  /// The schema base keyword path
  const Pointer &base_schema_location;
  /// The base instance location that the keyword must be evaluated to
  const Pointer &base_instance_location;
};

#if !defined(DOXYGEN)
struct SchemaCompilerContext;
#endif

/// @ingroup jsonschema
/// Represents the mode of compilation
enum class SchemaCompilerCompilationMode {
  /// Produce a compile template optimized for speed, ignoring everything
  /// that is not strictly required for that case
  Optimized,
  /// Produce a compile template optimized for full coverage
  Full
};

/// @ingroup jsonschema
/// A compiler is represented as a function that maps a keyword compiler
/// contexts into a compiler template. You can provide your own to implement
/// your own keywords
using SchemaCompiler = std::function<SchemaCompilerTemplate(
    const SchemaCompilerContext &, const SchemaCompilerSchemaContext &,
    const SchemaCompilerDynamicContext &)>;

/// @ingroup jsonschema
/// The static compiler context is the information you have at your
/// disposal to implement a keyword that will never change throughout
/// the compilation process
struct SchemaCompilerContext {
  /// The selected mode of compilation
  const SchemaCompilerCompilationMode mode;
  /// The root schema resource
  const JSON &root;
  /// The reference frame of the entire schema
  const ReferenceFrame &frame;
  /// The references of the entire schema
  const ReferenceMap &references;
  /// The schema walker in use
  const SchemaWalker &walker;
  /// The schema resolver in use
  const SchemaResolver &resolver;
  /// The schema compiler in use
  const SchemaCompiler &compiler;
  /// Whether the schema makes use of dynamic scoping
  const bool uses_dynamic_scopes;
};

/// @ingroup jsonschema
/// Represents the mode of evalution
enum class SchemaCompilerEvaluationMode {
  /// Attempt to get to a boolean result as fast as possible
  Fast,
  /// Perform a full schema evaluation
  Exhaustive
};

/// @ingroup jsonschema
/// Represents the state of a step evaluation
enum class SchemaCompilerEvaluationType { Pre, Post };

/// @ingroup jsonschema
/// A callback of this type is invoked after evaluating any keyword. The
/// arguments go as follows:
///
/// - The stage at which the step in question is
/// - Whether the evaluation was successful or not (always true before
/// evaluation)
/// - The step that was just evaluated
/// - The evaluation path
/// - The instance location
/// - The annotation result, if any (otherwise null)
///
/// You can use this callback mechanism to implement arbitrary output formats.
using SchemaCompilerEvaluationCallback =
    std::function<void(const SchemaCompilerEvaluationType, bool,
                       const SchemaCompilerTemplate::value_type &,
                       const Pointer &, const Pointer &, const JSON &)>;

// TODO: Support standard output formats too

/// @ingroup jsonschema
///
/// A simple evaluation callback that reports a stack trace in the case of
/// validation error that you can report as you with. For example:
///
/// ```cpp
/// #include <sourcemeta/jsontoolkit/json.h>
/// #include <sourcemeta/jsontoolkit/jsonschema.h>
/// #include <cassert>
/// #include <functional>
///
/// const sourcemeta::jsontoolkit::JSON schema =
///     sourcemeta::jsontoolkit::parse(R"JSON({
///   "$schema": "https://json-schema.org/draft/2020-12/schema",
///   "type": "string"
/// })JSON");
///
/// const auto schema_template{sourcemeta::jsontoolkit::compile(
///     schema, sourcemeta::jsontoolkit::default_schema_walker,
///     sourcemeta::jsontoolkit::official_resolver,
///     sourcemeta::jsontoolkit::default_schema_compiler)};
///
/// const sourcemeta::jsontoolkit::JSON instance{5};
///
/// sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput output;
/// const auto result{sourcemeta::jsontoolkit::evaluate(
///   schema_template, instance,
///   sourcemeta::jsontoolkit::SchemaCompilerEvaluationMode::Fast,
///   std::ref(output))};
///
/// if (!result) {
///   for (const auto &trace : output) {
///     std::cerr << trace.message << "\n";
///     sourcemeta::jsontoolkit::stringify(trace.instance_location, std::cerr);
///     std::cerr << "\n";
///     sourcemeta::jsontoolkit::stringify(trace.evaluate_path, std::cerr);
///     std::cerr << "\n";
///   }
/// }
/// ```
class SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT SchemaCompilerErrorTraceOutput {
public:
  SchemaCompilerErrorTraceOutput(const JSON &instance,
                                 const Pointer &base = empty_pointer);

  // Prevent accidental copies
  SchemaCompilerErrorTraceOutput(const SchemaCompilerErrorTraceOutput &) =
      delete;
  auto operator=(const SchemaCompilerErrorTraceOutput &)
      -> SchemaCompilerErrorTraceOutput & = delete;

  struct Entry {
    const std::string message;
    const Pointer instance_location;
    const Pointer evaluate_path;
  };

  auto operator()(const SchemaCompilerEvaluationType type, const bool result,
                  const SchemaCompilerTemplate::value_type &step,
                  const Pointer &evaluate_path,
                  const Pointer &instance_location,
                  const JSON &annotation) -> void;

  using container_type = typename std::vector<Entry>;
  using const_iterator = typename container_type::const_iterator;
  auto begin() const -> const_iterator;
  auto end() const -> const_iterator;
  auto cbegin() const -> const_iterator;
  auto cend() const -> const_iterator;

private:
// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(disable : 4251)
#endif
  const JSON &instance_;
  const Pointer base_;
  container_type output;
  std::set<Pointer> mask;
#if defined(_MSC_VER)
#pragma warning(default : 4251)
#endif
};

/// @ingroup jsonschema
///
/// This function translates a step execution into a human-readable string.
/// Useful as the building block for producing user-friendly evaluation results.
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
describe(const bool valid, const SchemaCompilerTemplate::value_type &step,
         const Pointer &evaluate_path, const Pointer &instance_location,
         const JSON &instance, const JSON &annotation) -> std::string;

/// @ingroup jsonschema
///
/// This function evaluates a schema compiler template in validation mode,
/// returning a boolean without error information. For example:
///
/// ```cpp
/// #include <sourcemeta/jsontoolkit/json.h>
/// #include <sourcemeta/jsontoolkit/jsonschema.h>
/// #include <cassert>
///
/// const sourcemeta::jsontoolkit::JSON schema =
///     sourcemeta::jsontoolkit::parse(R"JSON({
///   "$schema": "https://json-schema.org/draft/2020-12/schema",
///   "type": "string"
/// })JSON");
///
/// const auto schema_template{sourcemeta::jsontoolkit::compile(
///     schema, sourcemeta::jsontoolkit::default_schema_walker,
///     sourcemeta::jsontoolkit::official_resolver,
///     sourcemeta::jsontoolkit::default_schema_compiler)};
///
/// const sourcemeta::jsontoolkit::JSON instance{"foo bar"};
/// const auto result{sourcemeta::jsontoolkit::evaluate(
///   schema_template, instance)};
/// assert(result);
/// ```
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
evaluate(const SchemaCompilerTemplate &steps, const JSON &instance) -> bool;

/// @ingroup jsonschema
///
/// This function evaluates a schema compiler template, executing the given
/// callback at every step of the way. For example:
///
/// ```cpp
/// #include <sourcemeta/jsontoolkit/json.h>
/// #include <sourcemeta/jsontoolkit/jsonschema.h>
/// #include <cassert>
/// #include <iostream>
///
/// const sourcemeta::jsontoolkit::JSON schema =
///     sourcemeta::jsontoolkit::parse(R"JSON({
///   "$schema": "https://json-schema.org/draft/2020-12/schema",
///   "type": "string"
/// })JSON");
///
/// const auto schema_template{sourcemeta::jsontoolkit::compile(
///     schema, sourcemeta::jsontoolkit::default_schema_walker,
///     sourcemeta::jsontoolkit::official_resolver,
///     sourcemeta::jsontoolkit::default_schema_compiler)};
///
/// static auto callback(
///     bool result,
///     const sourcemeta::jsontoolkit::SchemaCompilerTemplate::value_type &step,
///     const sourcemeta::jsontoolkit::Pointer &evaluate_path,
///     const sourcemeta::jsontoolkit::Pointer &instance_location,
///     const sourcemeta::jsontoolkit::JSON &document,
///     const sourcemeta::jsontoolkit::JSON &annotation) -> void {
///   std::cout << "TYPE: " << (result ? "Success" : "Failure") << "\n";
///   std::cout << "STEP:\n";
///   sourcemeta::jsontoolkit::prettify(sourcemeta::jsontoolkit::to_json({step}),
///                                     std::cout);
///   std::cout << "\nEVALUATE PATH:";
///   sourcemeta::jsontoolkit::stringify(evaluate_path, std::cout);
///   std::cout << "\nINSTANCE LOCATION:";
///   sourcemeta::jsontoolkit::stringify(instance_location, std::cout);
///   std::cout << "\nANNOTATION:\n";
///   sourcemeta::jsontoolkit::prettify(annotation, std::cout);
///   std::cout << "\n";
/// }
///
/// const sourcemeta::jsontoolkit::JSON instance{"foo bar"};
/// const auto result{sourcemeta::jsontoolkit::evaluate(
///   schema_template, instance,
///   sourcemeta::jsontoolkit::SchemaCompilerEvaluationMode::Fast,
///   callback)};
///
/// assert(result);
/// ```
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
evaluate(const SchemaCompilerTemplate &steps, const JSON &instance,
         const SchemaCompilerEvaluationMode mode,
         const SchemaCompilerEvaluationCallback &callback) -> bool;

/// @ingroup jsonschema
/// A default compiler that aims to implement every keyword for official JSON
/// Schema dialects.
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT default_schema_compiler(
    const SchemaCompilerContext &, const SchemaCompilerSchemaContext &,
    const SchemaCompilerDynamicContext &) -> SchemaCompilerTemplate;

/// @ingroup jsonschema
///
/// This function compiles an input JSON Schema into a template that can be
/// later evaluated. For example:
///
/// ```cpp
/// #include <sourcemeta/jsontoolkit/json.h>
/// #include <sourcemeta/jsontoolkit/jsonschema.h>
///
/// const sourcemeta::jsontoolkit::JSON schema =
///     sourcemeta::jsontoolkit::parse(R"JSON({
///   "$schema": "https://json-schema.org/draft/2020-12/schema",
///   "type": "string"
/// })JSON");
///
/// const auto schema_template{sourcemeta::jsontoolkit::compile(
///     schema, sourcemeta::jsontoolkit::default_schema_walker,
///     sourcemeta::jsontoolkit::official_resolver,
///     sourcemeta::jsontoolkit::default_schema_compiler)};
///
/// // Evaluate or encode
/// ```
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
compile(const JSON &schema, const SchemaWalker &walker,
        const SchemaResolver &resolver, const SchemaCompiler &compiler,
        const SchemaCompilerCompilationMode mode =
            SchemaCompilerCompilationMode::Optimized,
        const std::optional<std::string> &default_dialect = std::nullopt)
    -> SchemaCompilerTemplate;

/// @ingroup jsonschema
///
/// This function compiles a single subschema into a compiler template as
/// determined by the given pointer. If a URI is given, the compiler will
/// attempt to jump to that corresponding frame entry. Otherwise, it will
/// navigate within the current keyword. This function is not meant to be used
/// directly, but instead as a building block for supporting applicators on
/// compiler functions.
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
compile(const SchemaCompilerContext &context,
        const SchemaCompilerSchemaContext &schema_context,
        const SchemaCompilerDynamicContext &dynamic_context,
        const Pointer &schema_suffix,
        const Pointer &instance_suffix = empty_pointer,
        const std::optional<std::string> &uri = std::nullopt)
    -> SchemaCompilerTemplate;

/// @ingroup jsonschema
///
/// This function converts a compiler template into JSON. Convenient for storing
/// it or sending it over the wire. For example:
///
/// ```cpp
/// #include <sourcemeta/jsontoolkit/json.h>
/// #include <sourcemeta/jsontoolkit/jsonschema.h>
/// #include <iostream>
///
/// const sourcemeta::jsontoolkit::JSON schema =
///     sourcemeta::jsontoolkit::parse(R"JSON({
///   "$schema": "https://json-schema.org/draft/2020-12/schema",
///   "type": "string"
/// })JSON");
///
/// const auto schema_template{sourcemeta::jsontoolkit::compile(
///     schema, sourcemeta::jsontoolkit::default_schema_walker,
///     sourcemeta::jsontoolkit::official_resolver,
///     sourcemeta::jsontoolkit::default_schema_compiler)};
///
/// const sourcemeta::jsontoolkit::JSON result{
///     sourcemeta::jsontoolkit::to_json(schema_template)};
///
/// sourcemeta::jsontoolkit::prettify(result, std::cout);
/// std::cout << "\n";
/// ```
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
to_json(const SchemaCompilerTemplate &steps) -> JSON;

/// @ingroup jsonschema
///
/// An opinionated key comparison for printing JSON Schema compiler templates
/// with sourcemeta::jsontoolkit::prettify or
/// sourcemeta::jsontoolkit::stringify. For example:
///
/// ```cpp
/// #include <sourcemeta/jsontoolkit/json.h>
/// #include <sourcemeta/jsontoolkit/jsonschema.h>
/// #include <iostream>
///
/// const sourcemeta::jsontoolkit::JSON schema =
///     sourcemeta::jsontoolkit::parse(R"JSON({
///   "$schema": "https://json-schema.org/draft/2020-12/schema",
///   "type": "string"
/// })JSON");
///
/// const auto schema_template{sourcemeta::jsontoolkit::compile(
///     schema, sourcemeta::jsontoolkit::default_schema_walker,
///     sourcemeta::jsontoolkit::official_resolver,
///     sourcemeta::jsontoolkit::default_schema_compiler)};
///
/// const sourcemeta::jsontoolkit::JSON result{
///     sourcemeta::jsontoolkit::to_json(schema_template)};
///
/// sourcemeta::jsontoolkit::prettify(result, std::cout,
/// compiler_template_format_compare); std::cout << "\n";
/// ```
auto SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT compiler_template_format_compare(
    const JSON::String &left, const JSON::String &right) -> bool;

} // namespace sourcemeta::jsontoolkit

#endif

#ifndef SOURCEMETA_CORE_JSONSCHEMA_TEST_TRANSFORM_RULES_H_
#define SOURCEMETA_CORE_JSONSCHEMA_TEST_TRANSFORM_RULES_H_

#include <sourcemeta/core/jsonschema.h>

class ExampleRule1 final : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRule1()
      : sourcemeta::core::SchemaTransformRule(
            "example_rule_1", "Keyword foo is not permitted") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> bool override {
    return schema.defines("foo");
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.erase("foo");
  }
};

class ExampleRule2 final : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRule2()
      : sourcemeta::core::SchemaTransformRule(
            "example_rule_2", "Keyword bar is not permitted") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> bool override {
    return schema.defines("bar");
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.erase("bar");
  }
};

class ExampleRule3 final : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRule3()
      : sourcemeta::core::SchemaTransformRule("example_rule_3",
                                              "Example rule 3") {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &location,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const -> bool override {
    return !schema.defines("top") && location.pointer.empty();
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.assign("top", sourcemeta::core::JSON{true});
  }
};

class ExampleRule4 final : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRule4()
      : sourcemeta::core::SchemaTransformRule("example_rule_4",
                                              "Example rule 4") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> bool override {
    return !schema.defines("here");
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.assign("here", sourcemeta::core::JSON{true});
  }
};

class ExampleRule5 final : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRule5()
      : sourcemeta::core::SchemaTransformRule("example_rule_5",
                                              "Example rule 5") {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &location,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const -> bool override {
    return !schema.defines("baz") &&
           location.pointer == sourcemeta::core::Pointer{"properties", "baz"};
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.assign("baz", sourcemeta::core::JSON{true});
  }
};

class ExampleRule6 final : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRule6()
      : sourcemeta::core::SchemaTransformRule("example_rule_6",
                                              "Example rule 6") {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &location,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const -> bool override {
    return !schema.defines("draft") &&
           location.dialect == "http://json-schema.org/draft-03/schema#";
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.assign("draft", sourcemeta::core::JSON{3});
  }
};

class ExampleRule7 final : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRule7()
      : sourcemeta::core::SchemaTransformRule("example_rule_1",
                                              "My custom message") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> bool override {
    return schema.defines("foo");
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.erase("foo");
  }
};

class ExampleRuleConflictsWith1 final
    : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRuleConflictsWith1()
      : sourcemeta::core::SchemaTransformRule(
            "example_rule_conflicts_with_1",
            "Example rule that conflicts with rule 1") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> bool override {
    return schema.defines("$schema") && schema.size() == 1;
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.assign("foo", sourcemeta::core::JSON{true});
  }
};

#endif

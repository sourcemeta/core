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
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return schema.defines("foo");
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.erase("foo");
  }
};

class ExampleRule1WithDescription final
    : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRule1WithDescription()
      : sourcemeta::core::SchemaTransformRule(
            "example_rule_1", "Keyword foo is not permitted") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (schema.defines("foo")) {
      return "This is a message from the rule";
    } else {
      return false;
    }
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
      -> sourcemeta::core::SchemaTransformRule::Result override {
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
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
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
      -> sourcemeta::core::SchemaTransformRule::Result override {
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
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
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
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
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
      -> sourcemeta::core::SchemaTransformRule::Result override {
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
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return schema.defines("$schema") && schema.size() == 1;
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.assign("foo", sourcemeta::core::JSON{true});
  }
};

class ExampleRuleUnfixable1 final
    : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRuleUnfixable1()
      : sourcemeta::core::SchemaTransformRule(
            "example_rule_unfixable_1",
            "An example rule that cannot be fixed") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return schema.defines("foo");
  }
};

class ExampleRuleUnfixableWithDescription1 final
    : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRuleUnfixableWithDescription1()
      : sourcemeta::core::SchemaTransformRule(
            "example_rule_unfixable_with_description_1",
            "An example rule that cannot be fixed") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (schema.defines("foo")) {
      return "The subschema cannot define foo";
    } else {
      return false;
    }
  }
};

class ExampleRuleDefinitionsToDefsNoRereference final
    : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRuleDefinitionsToDefsNoRereference()
      : sourcemeta::core::SchemaTransformRule(
            "example_rule_definitions_to_defs_no_rereference",
            "Rename `definitions` to `$defs`") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return schema.defines("definitions") && !schema.defines("$defs");
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.rename("definitions", "$defs");
  }
};

class ExampleRuleDefinitionsToDefsWithRereference final
    : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRuleDefinitionsToDefsWithRereference()
      : sourcemeta::core::SchemaTransformRule(
            "example_rule_definitions_to_defs_with_rereference",
            "Rename `definitions` to `$defs`") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return schema.defines("definitions") && !schema.defines("$defs");
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.rename("definitions", "$defs");
  }

  [[nodiscard]] auto rereference(const std::string &,
                                 const sourcemeta::core::Pointer &,
                                 const sourcemeta::core::Pointer &target,
                                 const sourcemeta::core::Pointer &current) const
      -> sourcemeta::core::Pointer override {
    return target.rebase(current.concat({"definitions"}),
                         current.concat({"$defs"}));
  }
};

class ExampleRuleRemoveIdentifiers final
    : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRuleRemoveIdentifiers()
      : sourcemeta::core::SchemaTransformRule("example_rule_remove_identifiers",
                                              "Remove all identifiers") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return schema.is_object() &&
           (schema.defines("$id") || schema.defines("$anchor"));
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.erase_keys({"$id", "$anchor"});
  }
};

class ExampleRuleRemoveFooArrayFront final
    : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRuleRemoveFooArrayFront()
      : sourcemeta::core::SchemaTransformRule(
            "example_rule_remove_foo_array_front",
            "Remove the first item of the foo array") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return schema.is_object() && schema.defines("foo") &&
           schema.at("foo").is_array() && !schema.at("foo").empty();
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.at("foo").erase(schema.at("foo").as_array().begin());
  }
};

#endif

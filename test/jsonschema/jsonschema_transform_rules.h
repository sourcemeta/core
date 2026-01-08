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

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
    schema.erase("foo");
  }
};

class ExampleRule1WithPointer final
    : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRule1WithPointer()
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
      return sourcemeta::core::Pointer{"foo"};
    } else {
      return false;
    }
  }

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
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
      std::ostringstream message;
      message << "This is a message from the rule";
      return {{sourcemeta::core::Pointer{"foo"}}, std::move(message).str()};
    } else {
      return false;
    }
  }

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
    schema.erase("foo");
  }
};

class ExampleRuleWithManyPointers final
    : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRuleWithManyPointers()
      : sourcemeta::core::SchemaTransformRule("example_rule_with_many_pointers",
                                              "Foo Bar") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (schema.defines("foo") && schema.defines("bar")) {
      std::vector<sourcemeta::core::Pointer> locations;
      locations.emplace_back(sourcemeta::core::Pointer{"foo"});
      locations.emplace_back(sourcemeta::core::Pointer{"bar"});
      return locations;
    } else {
      return false;
    }
  }

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
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

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
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

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
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

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
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

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
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

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
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

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
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

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
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

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
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

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
    schema.rename("definitions", "$defs");
  }

  [[nodiscard]] auto rereference(const std::string_view,
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

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
    schema.erase_keys({"$id", "$anchor"});
  }
};

class ExampleRuleDraftTag final : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRuleDraftTag()
      : sourcemeta::core::SchemaTransformRule("example_rule_draft_tag", "") {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (!schema.is_object() || schema.defines("x-dialect-type")) {
      return false;
    }

    return vocabularies.contains(
               sourcemeta::core::Vocabularies::Known::JSON_Schema_Draft_7) ||
           vocabularies.contains(
               sourcemeta::core::Vocabularies::Known::JSON_Schema_Draft_6) ||
           vocabularies.contains(
               sourcemeta::core::Vocabularies::Known::JSON_Schema_Draft_4);
  }

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
    schema.assign("x-dialect-type", sourcemeta::core::JSON{"draft"});
  }
};

class ExampleRuleModernTag final
    : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRuleModernTag()
      : sourcemeta::core::SchemaTransformRule("example_rule_modern_tag", "") {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (!schema.is_object() || schema.defines("x-dialect-type")) {
      return false;
    }

    return vocabularies.contains(sourcemeta::core::Vocabularies::Known::
                                     JSON_Schema_2020_12_Core) ||
           vocabularies.contains(
               sourcemeta::core::Vocabularies::Known::JSON_Schema_2019_09_Core);
  }

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
    schema.assign("x-dialect-type", sourcemeta::core::JSON{"modern"});
  }
};

// This rule is designed to trigger a use-after-free bug in the transformer.
// It checks location.dialect in condition(), then completely replaces the
// schema object in transform(), which frees the memory that location.dialect
// points to. When the transformer calls condition() again after transform()
// to verify the rule was applied, it will access freed memory.
//
// The key trick: we check that dialect does NOT equal some unrelated string.
// After transform() frees the memory, location.dialect contains garbage that
// almost certainly won't equal "xxxxxxx", so the condition still returns true,
// causing the transformer to think the rule didn't apply and throw an error
// or loop infinitely.
class ExampleRuleDialectUseAfterFree final
    : public sourcemeta::core::SchemaTransformRule {
public:
  ExampleRuleDialectUseAfterFree()
      : sourcemeta::core::SchemaTransformRule(
            "example_rule_dialect_use_after_free", "") {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &location,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    // Check location.dialect - this string_view points into the schema.
    // We check NOT equal to garbage, so after memory is freed and contains
    // random data, the condition will still be true (garbage != "xxxxxxx").
    return !schema.defines("x-transformed") && location.dialect != "xxxxxxx";
  }

  auto transform(sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaTransformRule::Result &) const
      -> void override {
    // Completely clear and rebuild the schema object to ensure the old
    // $schema string memory is freed. This will cause location.dialect
    // to become a dangling pointer.
    schema.clear();
    schema.into_object();
    // Restore a valid $schema but as a NEW string allocation
    schema.assign(
        "$schema",
        sourcemeta::core::JSON{"https://json-schema.org/draft/2020-12/schema"});
    // Add many keys to try to overwrite the old freed memory with garbage
    schema.assign("x-transformed", sourcemeta::core::JSON{true});
    schema.assign("x-padding-1",
                  sourcemeta::core::JSON{"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"});
    schema.assign("x-padding-2",
                  sourcemeta::core::JSON{"YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY"});
    schema.assign("x-padding-3",
                  sourcemeta::core::JSON{"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"});
  }
};

#endif

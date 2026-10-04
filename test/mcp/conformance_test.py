"""Regressions for the development-only official-schema runner."""
import unittest

from conformance import RevisionValidators


class DefinitionValidatorTest(unittest.TestCase):
    def test_declared_draft_four_dialect(self):
        schema = {
            "$schema": "http://json-schema.org/draft-04/schema#",
            "definitions": {"Number": {"type": "number", "minimum": 1,
                                      "exclusiveMinimum": True}},
        }
        validator = RevisionValidators("draft-four", schema).definition("Number")
        self.assertFalse(validator.is_valid(1))
        self.assertTrue(validator.is_valid(2))

    def test_absolute_self_reference(self):
        schema = {
            "$schema": "https://json-schema.org/draft/2020-12/schema",
            "$id": "https://example.com/original",
            "$defs": {
                "Text": {"type": "string"},
                "Entry": {"$ref": "https://example.com/original#/$defs/Text"},
            },
        }
        validator = RevisionValidators("absolute", schema).definition("Entry")
        self.assertTrue(validator.is_valid("text"))
        self.assertFalse(validator.is_valid(5))

    def test_cached_definitions_are_isolated_by_revision(self):
        text = RevisionValidators("text", {
            "$schema": "https://json-schema.org/draft/2020-12/schema",
            "$defs": {"Entry": {"type": "string"}},
        })
        number = RevisionValidators("number", {
            "$schema": "https://json-schema.org/draft/2020-12/schema",
            "$defs": {"Entry": {"type": "integer"}},
        })
        self.assertIs(text.definition("Entry"), text.definition("Entry"))
        self.assertIsNot(text.definition("Entry"), number.definition("Entry"))
        self.assertTrue(text.definition("Entry").is_valid("text"))
        self.assertFalse(text.definition("Entry").is_valid(5))
        self.assertTrue(number.definition("Entry").is_valid(5))
        self.assertFalse(number.definition("Entry").is_valid("text"))
        with self.assertRaisesRegex(RuntimeError, "text:.*Missing"):
            text.definition("Missing")


if __name__ == "__main__":
    unittest.main()

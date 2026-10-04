"""Regressions for the development-only official-schema runner."""
import unittest

from conformance import definition_validator


class DefinitionValidatorTest(unittest.TestCase):
    def test_declared_draft_four_dialect(self):
        schema = {
            "$schema": "http://json-schema.org/draft-04/schema#",
            "definitions": {"Number": {"type": "number", "minimum": 1,
                                      "exclusiveMinimum": True}},
        }
        validator = definition_validator(schema, "Number")
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
        validator = definition_validator(schema, "Entry")
        self.assertTrue(validator.is_valid("text"))
        self.assertFalse(validator.is_valid(5))


if __name__ == "__main__":
    unittest.main()

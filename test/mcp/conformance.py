#!/usr/bin/env python3
"""Validate actual builder output against pinned, official MCP schemas.

Development-only: pip install jsonschema==4.26.0
Run: python3 test/mcp/conformance.py build-debug/test/mcp/sourcemeta_core_mcp_unit
Use --schema-directory for an offline schema/<revision>/schema.json checkout.
"""

import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile
import urllib.request

import jsonschema
from referencing import Registry, Resource
from referencing.jsonschema import specification_with

PIN = "75db1e987cbbba6d170315dc99d0dfc440754aef"
VERSIONS = ("2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28")


class RevisionValidators:
    """Check one revision's root once, then cache its definition validators."""

    def __init__(self, revision, schema):
        self.revision = revision
        self.schema = schema
        self.validator = jsonschema.validators.validator_for(schema)
        self.validator.check_schema(schema)
        self.definitions = "$defs" if "$defs" in schema else "definitions"
        dialect = schema.get("$schema", self.validator.META_SCHEMA.get(
            "$id", self.validator.META_SCHEMA.get("id")))
        resource = Resource.from_contents(
            schema, default_specification=specification_with(dialect))
        identity = schema.get("$id", schema.get(
            "id", f"urn:sourcemeta:mcp:conformance:{revision}"))
        self.registry = Registry().with_resource(identity, resource)
        self.selected = {}

    def definition(self, name):
        """Preserve the declared dialect and original resource identity."""
        if name not in self.schema[self.definitions]:
            raise RuntimeError(f"{self.revision}: unknown official definition: {name}")
        if name not in self.selected:
            root = {self.definitions: self.schema[self.definitions],
                    "$ref": f"#/{self.definitions}/{name}"}
            for key in ("$schema", "$id", "id"):
                if key in self.schema:
                    root[key] = self.schema[key]
            self.selected[name] = self.validator(root, registry=self.registry)
        return self.selected[name]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("binary", type=Path)
    parser.add_argument("--schema-directory", type=Path)
    args = parser.parse_args()
    hashes = json.loads(Path(__file__).with_name("schema-sha256.json").read_text())
    schemas = {}
    for version in VERSIONS:
        if args.schema_directory:
            raw = (args.schema_directory / version / "schema.json").read_bytes()
        else:
            url = f"https://raw.githubusercontent.com/modelcontextprotocol/modelcontextprotocol/{PIN}/schema/{version}/schema.json"
            with urllib.request.urlopen(url, timeout=30) as response:
                raw = response.read()
        if hashlib.sha256(raw).hexdigest() != hashes[version]:
            raise RuntimeError(f"Pinned schema checksum mismatch: {version}")
        schema = json.loads(raw)
        schemas[version] = RevisionValidators(version, schema)
    with tempfile.TemporaryDirectory(prefix="mcp-conformance-") as directory:
        corpus = Path(directory) / "corpus.jsonl"
        environment = dict(os.environ, SOURCEMETA_MCP_CORPUS=str(corpus))
        subprocess.run([str(args.binary.resolve()), "--filter", "mcp_conformance."],
                       env=environment, check=True)
        count = 0
        for line in corpus.read_text().splitlines():
            entry = json.loads(line)
            name = entry["definition"]
            selected = schemas[entry["version"]].definition(name)
            for error in selected.iter_errors(entry["value"]):
                raise RuntimeError(f"{entry['version']} {name} at {list(error.path)}: {error.message}")
            count += 1
        if count < 90:
            raise RuntimeError(f"Unexpectedly small conformance corpus: {count}")
        print(f"Validated {count} emitted payloads against four official MCP revisions (pin {PIN}).")


if __name__ == "__main__":
    main()

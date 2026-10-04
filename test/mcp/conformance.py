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

PIN = "75db1e987cbbba6d170315dc99d0dfc440754aef"
VERSIONS = ("2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28")


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
        validator = jsonschema.validators.validator_for(schema)
        validator.check_schema(schema)
        schemas[version] = schema, validator
    with tempfile.TemporaryDirectory(prefix="mcp-conformance-") as directory:
        corpus = Path(directory) / "corpus.jsonl"
        environment = dict(os.environ, SOURCEMETA_MCP_CORPUS=str(corpus))
        subprocess.run([str(args.binary.resolve()), "--filter", "mcp_conformance."],
                       env=environment, check=True)
        count = 0
        for line in corpus.read_text().splitlines():
            entry = json.loads(line)
            schema, validator = schemas[entry["version"]]
            definitions = "$defs" if "$defs" in schema else "definitions"
            name = entry["definition"]
            if name not in schema[definitions]:
                raise RuntimeError(f"Unknown official definition: {entry['version']} {name}")
            root = {definitions: schema[definitions], "$ref": f"#/{definitions}/{name}"}
            for error in validator(root).iter_errors(entry["value"]):
                raise RuntimeError(f"{entry['version']} {name} at {list(error.path)}: {error.message}")
            count += 1
        if count < 90:
            raise RuntimeError(f"Unexpectedly small conformance corpus: {count}")
        print(f"Validated {count} emitted payloads against four official MCP revisions (pin {PIN}).")


if __name__ == "__main__":
    main()

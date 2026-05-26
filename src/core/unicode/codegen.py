#!/usr/bin/env python3

import re
import sys

LINE = re.compile(r"^([0-9A-Fa-f]+)(?:\.\.([0-9A-Fa-f]+))?\s*;\s*(\d+)\b")

TOTAL_CODEPOINTS = 0x110000
PAGE_SHIFT = 10
PAGE_SIZE = 1 << PAGE_SHIFT
NUM_PAGES = TOTAL_CODEPOINTS // PAGE_SIZE


def parse_ranges(path):
    ranges = []
    with open(path) as source:
        for line_number, line in enumerate(source, start=1):
            stripped = line.strip()
            if not stripped or stripped.startswith("#"):
                continue
            match = LINE.match(stripped)
            if not match:
                raise ValueError(
                    f"{path}:{line_number}: unparseable line: {stripped!r}"
                )
            first = int(match.group(1), 16)
            last = int(match.group(2), 16) if match.group(2) else first
            value = int(match.group(3))
            ranges.append((first, last, value))
    return ranges


def build_dense_values(ranges, default):
    values = [default] * TOTAL_CODEPOINTS
    for first, last, value in ranges:
        for codepoint in range(first, last + 1):
            values[codepoint] = value
    return values


def build_pages(values):
    page_to_id = {}
    unique_pages = []
    stage1 = []
    for page_index in range(NUM_PAGES):
        start = page_index * PAGE_SIZE
        page = tuple(values[start : start + PAGE_SIZE])
        if page not in page_to_id:
            page_to_id[page] = len(unique_pages)
            unique_pages.append(page)
        stage1.append(page_to_id[page])
    return stage1, unique_pages


def emit_row(output, items, indent="    "):
    for offset in range(0, len(items), 16):
        chunk = items[offset : offset + 16]
        output.write(indent + ", ".join(str(value) for value in chunk) + ",\n")


def emit(stage1, unique_pages, output_path):
    with open(output_path, "w") as output:
        output.write("#include <cstdint>\n\n")
        output.write("namespace {\n\n")

        output.write(
            f"constexpr std::uint16_t COMBINING_CLASS_STAGE1[{len(stage1)}] = {{\n"
        )
        emit_row(output, stage1)
        output.write("};\n\n")

        stage2_size = len(unique_pages) * PAGE_SIZE
        output.write(
            f"constexpr std::uint8_t COMBINING_CLASS_STAGE2[{stage2_size}] = {{\n"
        )
        for page in unique_pages:
            emit_row(output, list(page))
        output.write("};\n\n")

        output.write("} // namespace\n")


def main():
    if len(sys.argv) != 3:
        print(
            f"Usage: {sys.argv[0]} <input.txt> <output.h>",
            file=sys.stderr,
        )
        sys.exit(1)
    ranges = parse_ranges(sys.argv[1])
    values = build_dense_values(ranges, default=0)
    stage1, unique_pages = build_pages(values)
    emit(stage1, unique_pages, sys.argv[2])


if __name__ == "__main__":
    main()

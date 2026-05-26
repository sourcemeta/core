#!/usr/bin/env python3

import re
import sys

LINE = re.compile(r"^([0-9A-Fa-f]+)(?:\.\.([0-9A-Fa-f]+))?\s*;\s*(\S+)")

TOTAL_CODEPOINTS = 0x110000
PAGE_SHIFT = 10
PAGE_SIZE = 1 << PAGE_SHIFT
NUM_PAGES = TOTAL_CODEPOINTS // PAGE_SIZE

JOINING_TYPE_VALUES = {
    "U": 0,
    "T": 1,
    "L": 2,
    "R": 3,
    "D": 4,
    "C": 5,
}


def parse_ranges(path, value_parser):
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
            ranges.append((first, last, value_parser(match.group(3))))
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


def emit_row(output, items):
    for offset in range(0, len(items), 16):
        chunk = items[offset : offset + 16]
        output.write("    " + ", ".join(str(value) for value in chunk) + ",\n")


def emit_property(output, prefix, stage1, unique_pages):
    output.write(
        f"constexpr std::uint16_t {prefix}_STAGE1[{len(stage1)}] = {{\n"
    )
    emit_row(output, stage1)
    output.write("};\n\n")

    stage2_size = len(unique_pages) * PAGE_SIZE
    output.write(
        f"constexpr std::uint8_t {prefix}_STAGE2[{stage2_size}] = {{\n"
    )
    for page in unique_pages:
        emit_row(output, list(page))
    output.write("};\n\n")


def generate_property(input_path, value_parser, default):
    ranges = parse_ranges(input_path, value_parser)
    values = build_dense_values(ranges, default)
    return build_pages(values)


def main():
    if len(sys.argv) != 4:
        print(
            f"Usage: {sys.argv[0]} "
            "<output.h> "
            "<DerivedCombiningClass.txt> "
            "<DerivedJoiningType.txt>",
            file=sys.stderr,
        )
        sys.exit(1)

    output_path = sys.argv[1]
    combining_class_input = sys.argv[2]
    joining_type_input = sys.argv[3]

    combining_class_stage1, combining_class_pages = generate_property(
        combining_class_input, int, default=0
    )
    joining_type_stage1, joining_type_pages = generate_property(
        joining_type_input,
        lambda raw: JOINING_TYPE_VALUES[raw],
        default=JOINING_TYPE_VALUES["U"],
    )

    with open(output_path, "w") as output:
        output.write("#include <cstdint>\n\n")
        output.write("namespace {\n\n")
        emit_property(
            output, "COMBINING_CLASS", combining_class_stage1, combining_class_pages
        )
        emit_property(
            output, "JOINING_TYPE", joining_type_stage1, joining_type_pages
        )
        output.write("} // namespace\n")


if __name__ == "__main__":
    main()

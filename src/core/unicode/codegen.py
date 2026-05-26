#!/usr/bin/env python3

import re
import sys

LINE = re.compile(r"^([0-9A-Fa-f]+)(?:\.\.([0-9A-Fa-f]+))?\s*;\s*(\S+)")
MISSING = re.compile(
    r"^#\s*@missing:\s*([0-9A-Fa-f]+)\.\.([0-9A-Fa-f]+)\s*;\s*(\S+)"
)

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

JOINING_TYPE_FULL_NAMES = {
    "Non_Joining": 0,
    "Transparent": 1,
    "Left_Joining": 2,
    "Right_Joining": 3,
    "Dual_Joining": 4,
    "Join_Causing": 5,
}

BIDI_CLASS_VALUES = {
    "L": 0,
    "R": 1,
    "AL": 2,
    "EN": 3,
    "ES": 4,
    "ET": 5,
    "AN": 6,
    "CS": 7,
    "NSM": 8,
    "BN": 9,
    "B": 10,
    "S": 11,
    "WS": 12,
    "ON": 13,
    "LRE": 14,
    "LRO": 15,
    "RLE": 16,
    "RLO": 17,
    "PDF": 18,
    "LRI": 19,
    "RLI": 20,
    "FSI": 21,
    "PDI": 22,
}

BIDI_CLASS_FULL_NAMES = {
    "Left_To_Right": 0,
    "Right_To_Left": 1,
    "Arabic_Letter": 2,
    "European_Number": 3,
    "European_Separator": 4,
    "European_Terminator": 5,
    "Arabic_Number": 6,
    "Common_Separator": 7,
    "Nonspacing_Mark": 8,
    "Boundary_Neutral": 9,
    "Paragraph_Separator": 10,
    "Segment_Separator": 11,
    "White_Space": 12,
    "Other_Neutral": 13,
    "Left_To_Right_Embedding": 14,
    "Left_To_Right_Override": 15,
    "Right_To_Left_Embedding": 16,
    "Right_To_Left_Override": 17,
    "Pop_Directional_Format": 18,
    "Left_To_Right_Isolate": 19,
    "Right_To_Left_Isolate": 20,
    "First_Strong_Isolate": 21,
    "Pop_Directional_Isolate": 22,
}


def parse_value(path, line_number, raw_value, value_parser, label):
    try:
        return value_parser(raw_value)
    except (KeyError, ValueError) as error:
        raise ValueError(
            f"{path}:{line_number}: invalid {label} value "
            f"{raw_value!r}: {error}"
        ) from error


def parse_missing(path, name_parser):
    missing = []
    with open(path) as source:
        for line_number, line in enumerate(source, start=1):
            stripped = line.strip()
            match = MISSING.match(stripped)
            if not match:
                continue
            first = int(match.group(1), 16)
            last = int(match.group(2), 16)
            value = parse_value(
                path, line_number, match.group(3), name_parser, "@missing"
            )
            missing.append((first, last, value))
    return missing


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
            value = parse_value(
                path, line_number, match.group(3), value_parser, "data"
            )
            ranges.append((first, last, value))
    return ranges


def build_dense_values(missing_ranges, ranges):
    values = [0] * TOTAL_CODEPOINTS
    for first, last, value in missing_ranges:
        for codepoint in range(first, last + 1):
            values[codepoint] = value
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


def generate_property(input_path, data_parser, missing_parser):
    missing = parse_missing(input_path, missing_parser)
    ranges = parse_ranges(input_path, data_parser)
    values = build_dense_values(missing, ranges)
    return build_pages(values)


def main():
    if len(sys.argv) != 5:
        print(
            f"Usage: {sys.argv[0]} "
            "<output.h> "
            "<DerivedCombiningClass.txt> "
            "<DerivedJoiningType.txt> "
            "<DerivedBidiClass.txt>",
            file=sys.stderr,
        )
        sys.exit(1)

    output_path = sys.argv[1]
    combining_class_input = sys.argv[2]
    joining_type_input = sys.argv[3]
    bidi_class_input = sys.argv[4]

    combining_class_stage1, combining_class_pages = generate_property(
        combining_class_input,
        data_parser=int,
        missing_parser=lambda raw: 0 if raw == "Not_Reordered" else int(raw),
    )
    joining_type_stage1, joining_type_pages = generate_property(
        joining_type_input,
        data_parser=lambda raw: JOINING_TYPE_VALUES[raw],
        missing_parser=lambda raw: JOINING_TYPE_FULL_NAMES[raw],
    )
    bidi_class_stage1, bidi_class_pages = generate_property(
        bidi_class_input,
        data_parser=lambda raw: BIDI_CLASS_VALUES[raw],
        missing_parser=lambda raw: BIDI_CLASS_FULL_NAMES[raw],
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
        emit_property(
            output, "BIDI_CLASS", bidi_class_stage1, bidi_class_pages
        )
        output.write("} // namespace\n")


if __name__ == "__main__":
    main()

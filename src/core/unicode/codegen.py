#!/usr/bin/env python3

import re
import sys

LINE = re.compile(r"^([0-9A-Fa-f]+)(?:\.\.([0-9A-Fa-f]+))?\s*;\s*(\d+)\b")


def parse_ranges(path):
    ranges = []
    with open(path) as source:
        for line in source:
            stripped = line.strip()
            if not stripped or stripped.startswith("#"):
                continue
            match = LINE.match(stripped)
            if not match:
                continue
            first = int(match.group(1), 16)
            last = int(match.group(2), 16) if match.group(2) else first
            value = int(match.group(3))
            if value == 0:
                continue
            ranges.append((first, last, value))
    ranges.sort()
    return ranges


def coalesce(ranges):
    if not ranges:
        return ranges
    result = [ranges[0]]
    for first, last, value in ranges[1:]:
        previous_first, previous_last, previous_value = result[-1]
        if value == previous_value and first == previous_last + 1:
            result[-1] = (previous_first, last, value)
        else:
            result.append((first, last, value))
    return result


def emit(ranges, output_path):
    with open(output_path, "w") as output:
        output.write("namespace {\n\n")
        output.write("constexpr char32_t COMBINING_CLASS_FIRSTS[] = {\n")
        for first, _, _ in ranges:
            output.write(f"    0x{first:04X}U,\n")
        output.write("};\n\n")
        output.write("constexpr char32_t COMBINING_CLASS_LASTS[] = {\n")
        for _, last, _ in ranges:
            output.write(f"    0x{last:04X}U,\n")
        output.write("};\n\n")
        output.write("constexpr std::uint8_t COMBINING_CLASS_VALUES[] = {\n")
        for _, _, value in ranges:
            output.write(f"    {value}U,\n")
        output.write("};\n\n")
        output.write("} // namespace\n")


def main():
    if len(sys.argv) != 3:
        print(
            f"Usage: {sys.argv[0]} <DerivedCombiningClass.txt> <output.h>",
            file=sys.stderr,
        )
        sys.exit(1)
    ranges = coalesce(parse_ranges(sys.argv[1]))
    emit(ranges, sys.argv[2])


if __name__ == "__main__":
    main()

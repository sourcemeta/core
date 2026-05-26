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

UNICODE_SCRIPT_VALUES = {
    "Adlam": 0,
    "Ahom": 1,
    "Anatolian_Hieroglyphs": 2,
    "Arabic": 3,
    "Armenian": 4,
    "Avestan": 5,
    "Balinese": 6,
    "Bamum": 7,
    "Bassa_Vah": 8,
    "Batak": 9,
    "Bengali": 10,
    "Beria_Erfe": 11,
    "Bhaiksuki": 12,
    "Bopomofo": 13,
    "Brahmi": 14,
    "Braille": 15,
    "Buginese": 16,
    "Buhid": 17,
    "Canadian_Aboriginal": 18,
    "Carian": 19,
    "Caucasian_Albanian": 20,
    "Chakma": 21,
    "Cham": 22,
    "Cherokee": 23,
    "Chorasmian": 24,
    "Common": 25,
    "Coptic": 26,
    "Cuneiform": 27,
    "Cypriot": 28,
    "Cypro_Minoan": 29,
    "Cyrillic": 30,
    "Deseret": 31,
    "Devanagari": 32,
    "Dives_Akuru": 33,
    "Dogra": 34,
    "Duployan": 35,
    "Egyptian_Hieroglyphs": 36,
    "Elbasan": 37,
    "Elymaic": 38,
    "Ethiopic": 39,
    "Garay": 40,
    "Georgian": 41,
    "Glagolitic": 42,
    "Gothic": 43,
    "Grantha": 44,
    "Greek": 45,
    "Gujarati": 46,
    "Gunjala_Gondi": 47,
    "Gurmukhi": 48,
    "Gurung_Khema": 49,
    "Han": 50,
    "Hangul": 51,
    "Hanifi_Rohingya": 52,
    "Hanunoo": 53,
    "Hatran": 54,
    "Hebrew": 55,
    "Hiragana": 56,
    "Imperial_Aramaic": 57,
    "Inherited": 58,
    "Inscriptional_Pahlavi": 59,
    "Inscriptional_Parthian": 60,
    "Javanese": 61,
    "Kaithi": 62,
    "Kannada": 63,
    "Katakana": 64,
    "Kawi": 65,
    "Kayah_Li": 66,
    "Kharoshthi": 67,
    "Khitan_Small_Script": 68,
    "Khmer": 69,
    "Khojki": 70,
    "Khudawadi": 71,
    "Kirat_Rai": 72,
    "Lao": 73,
    "Latin": 74,
    "Lepcha": 75,
    "Limbu": 76,
    "Linear_A": 77,
    "Linear_B": 78,
    "Lisu": 79,
    "Lycian": 80,
    "Lydian": 81,
    "Mahajani": 82,
    "Makasar": 83,
    "Malayalam": 84,
    "Mandaic": 85,
    "Manichaean": 86,
    "Marchen": 87,
    "Masaram_Gondi": 88,
    "Medefaidrin": 89,
    "Meetei_Mayek": 90,
    "Mende_Kikakui": 91,
    "Meroitic_Cursive": 92,
    "Meroitic_Hieroglyphs": 93,
    "Miao": 94,
    "Modi": 95,
    "Mongolian": 96,
    "Mro": 97,
    "Multani": 98,
    "Myanmar": 99,
    "Nabataean": 100,
    "Nag_Mundari": 101,
    "Nandinagari": 102,
    "New_Tai_Lue": 103,
    "Newa": 104,
    "Nko": 105,
    "Nushu": 106,
    "Nyiakeng_Puachue_Hmong": 107,
    "Ogham": 108,
    "Ol_Chiki": 109,
    "Ol_Onal": 110,
    "Old_Hungarian": 111,
    "Old_Italic": 112,
    "Old_North_Arabian": 113,
    "Old_Permic": 114,
    "Old_Persian": 115,
    "Old_Sogdian": 116,
    "Old_South_Arabian": 117,
    "Old_Turkic": 118,
    "Old_Uyghur": 119,
    "Oriya": 120,
    "Osage": 121,
    "Osmanya": 122,
    "Pahawh_Hmong": 123,
    "Palmyrene": 124,
    "Pau_Cin_Hau": 125,
    "Phags_Pa": 126,
    "Phoenician": 127,
    "Psalter_Pahlavi": 128,
    "Rejang": 129,
    "Runic": 130,
    "Samaritan": 131,
    "Saurashtra": 132,
    "Sharada": 133,
    "Shavian": 134,
    "Siddham": 135,
    "Sidetic": 136,
    "SignWriting": 137,
    "Sinhala": 138,
    "Sogdian": 139,
    "Sora_Sompeng": 140,
    "Soyombo": 141,
    "Sundanese": 142,
    "Sunuwar": 143,
    "Syloti_Nagri": 144,
    "Syriac": 145,
    "Tagalog": 146,
    "Tagbanwa": 147,
    "Tai_Le": 148,
    "Tai_Tham": 149,
    "Tai_Viet": 150,
    "Tai_Yo": 151,
    "Takri": 152,
    "Tamil": 153,
    "Tangsa": 154,
    "Tangut": 155,
    "Telugu": 156,
    "Thaana": 157,
    "Thai": 158,
    "Tibetan": 159,
    "Tifinagh": 160,
    "Tirhuta": 161,
    "Todhri": 162,
    "Tolong_Siki": 163,
    "Toto": 164,
    "Tulu_Tigalari": 165,
    "Ugaritic": 166,
    "Unknown": 167,
    "Vai": 168,
    "Vithkuqi": 169,
    "Wancho": 170,
    "Warang_Citi": 171,
    "Yezidi": 172,
    "Yi": 173,
    "Zanabazar_Square": 174,
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
    if len(sys.argv) != 6:
        print(
            f"Usage: {sys.argv[0]} "
            "<output.h> "
            "<DerivedCombiningClass.txt> "
            "<DerivedJoiningType.txt> "
            "<DerivedBidiClass.txt> "
            "<Scripts.txt>",
            file=sys.stderr,
        )
        sys.exit(1)

    output_path = sys.argv[1]
    combining_class_input = sys.argv[2]
    joining_type_input = sys.argv[3]
    bidi_class_input = sys.argv[4]
    script_input = sys.argv[5]

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
    script_stage1, script_pages = generate_property(
        script_input,
        data_parser=lambda raw: UNICODE_SCRIPT_VALUES[raw],
        missing_parser=lambda raw: UNICODE_SCRIPT_VALUES[raw],
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
        emit_property(
            output, "UNICODE_SCRIPT", script_stage1, script_pages
        )
        output.write("} // namespace\n")


if __name__ == "__main__":
    main()

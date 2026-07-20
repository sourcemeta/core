#include <sourcemeta/core/langtag.h>
#include <sourcemeta/core/test.h>

// RFC 5646 Appendix A: simple language subtag
TEST(rfc_simple_language_subtag) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("de"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("fr"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("ja"));
}

// RFC 5646 Appendix A: language subtag plus script subtag
TEST(rfc_language_plus_script) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-Hant"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-Hans"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("sr-Cyrl"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("sr-Latn"));
}

// RFC 5646 Appendix A: extended language subtags
TEST(rfc_extended_language_subtags) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-cmn-Hans-CN"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("cmn-Hans-CN"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-yue-HK"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("yue-HK"));
}

// RFC 5646 Appendix A: language-script-region
TEST(rfc_language_script_region) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-Hans-CN"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("sr-Latn-RS"));
}

// RFC 5646 Appendix A: language-variant
TEST(rfc_language_variant) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("sl-rozaj"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("sl-rozaj-biske"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("sl-nedis"));
}

// RFC 5646 Appendix A: language-region-variant
TEST(rfc_language_region_variant) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("de-CH-1901"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("sl-IT-nedis"));
}

// RFC 5646 Appendix A: language-script-region-variant
TEST(rfc_language_script_region_variant) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("hy-Latn-IT-arevela"));
}

// RFC 5646 Appendix A: language-region
TEST(rfc_language_region) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("de-DE"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-US"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("es-419"));
}

// RFC 5646 Appendix A: private use subtags
TEST(rfc_private_use_subtags) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("de-CH-x-phonebk"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("az-Arab-x-AYB"));
}

// RFC 5646 Appendix A: tags using the private use registry values
TEST(rfc_private_use_registry_values) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("qaa-Qaaa-QM-x-southern"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("de-Qaaa"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("sr-Latn-QM"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("sr-Qaaa-RS"));
}

// RFC 5646 Appendix A: tags using extensions
TEST(rfc_extensions) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-US-u-islamcal"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-CN-a-myext-x-private"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-a-myext-b-another"));
}

// RFC 5646 Appendix A: private use only
TEST(rfc_private_use_only) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("x-whatever"));
}

// RFC 5646 Appendix A: invalid, two region subtags
TEST(rfc_invalid_two_region_subtags) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("de-419-DE"));
}

// RFC 5646 Appendix A: invalid, single-character primary language
TEST(rfc_invalid_single_character_primary) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("a-DE"));
}

// RFC 5646 Appendix A: invalid, two extensions with the same singleton
TEST(rfc_invalid_duplicate_singleton) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("ar-a-aaa-b-bbb-a-ccc"));
}

// RFC 5646 Appendix A: invalid, singleton with no following subtag
TEST(rfc_invalid_extension_without_subtag) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("tlh-a-b-foo"));
}

// RFC 5646 Section 2.2.1: primary language of two letters
TEST(language_two_letters) { EXPECT_TRUE(sourcemeta::core::is_langtag("en")); }

// RFC 5646 Section 2.2.1: primary language of three letters
TEST(language_three_letters) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("mas"));
}

// RFC 5646 Section 2.2.1: four-letter reserved primary language
TEST(language_four_letters_reserved) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("aaaa"));
}

// RFC 5646 Section 2.2.1: a four-letter primary language takes no extended
// language but still admits a script and region
TEST(language_four_letters_with_script_region) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("aaaa-Latn-US"));
}

// RFC 5646 Section 2.2.1: registered primary language of five to eight letters
TEST(language_five_to_eight_letters) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("abcde"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("abcdefgh"));
}

// RFC 5646 Section 2.2.1: a long primary language still admits a region
TEST(language_five_to_eight_letters_with_region) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("abcde-US"));
}

// RFC 5646 Section 2.2.1: a single-letter primary language is not allowed
TEST(language_one_letter_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("a"));
}

// RFC 5646 Section 2.2.1: a primary language longer than eight letters is not
// allowed
TEST(language_nine_letters_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("abcdefghi"));
}

// RFC 5646 Section 2.2.1: the primary language is letters only
TEST(language_with_digit_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("e2"));
  EXPECT_FALSE(sourcemeta::core::is_langtag("12"));
}

// RFC 5646 Section 2.2.2: a single extended language subtag
TEST(single_extlang) { EXPECT_TRUE(sourcemeta::core::is_langtag("zh-yue")); }

// RFC 5646 Section 2.2.2: up to three extended language subtags
TEST(three_extlangs) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-aaa-bbb-ccc"));
}

// RFC 5646 Section 2.2.2: more than three extended language subtags is not
// allowed
TEST(four_extlangs_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("zh-aaa-bbb-ccc-ddd"));
}

// RFC 5646 Section 2.2.2: extended language subtags only follow a two or three
// letter primary language
TEST(extlang_only_after_short_primary) {
  // A four-letter or longer primary language cannot take an extended language
  EXPECT_FALSE(sourcemeta::core::is_langtag("aaaa-bbb"));
  EXPECT_FALSE(sourcemeta::core::is_langtag("abcde-fgh"));
}

// RFC 5646 Section 2.2.3: a script subtag after an extended language
TEST(script_after_extlang) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-yue-Hant-CN"));
}

// RFC 5646 Section 2.1: extlang, script, region, variant, extension and
// private use all present in a single tag
TEST(full_langtag) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-yue-Hant-CN-1994-a-bbb-x-foo"));
}

// RFC 5646 Section 2.2.3: at most one script subtag
TEST(two_scripts_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("zh-Hant-Latn"));
}

// RFC 5646 Section 2.2.4: an alphabetic region subtag
TEST(region_alpha) { EXPECT_TRUE(sourcemeta::core::is_langtag("en-GB")); }

// RFC 5646 Section 2.2.4: a numeric region subtag
TEST(region_numeric) { EXPECT_TRUE(sourcemeta::core::is_langtag("es-005")); }

// RFC 5646 Section 2.2.4: a numeric region subtag has exactly three digits
TEST(two_digit_region_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("en-12"));
}

// RFC 5646 Section 2.2.4: at most one region subtag
TEST(two_regions_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("en-US-GB"));
}

// RFC 5646 Section 2.2.5: a variant of five to eight characters
TEST(variant_five_letters) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("de-nedis"));
}

// RFC 5646 Section 2.2.5: a variant of eight characters
TEST(variant_eight_characters) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("de-abcdefgh"));
}

// RFC 5646 Section 2.2.5: a four-character variant beginning with a digit
TEST(digit_led_four_character_variant) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("de-1994"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("de-1234"));
}

// RFC 5646 Section 2.2.5: a digit-led four-character variant may mix letters
TEST(digit_led_four_character_variant_with_letters) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("de-1abc"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("de-1a2b"));
}

// RFC 5646 Section 2.2.5: a four-character subtag must begin with a digit
TEST(four_character_non_digit_led_rejected) {
  // A four-character subtag is only a variant when it begins with a digit
  EXPECT_FALSE(sourcemeta::core::is_langtag("de-ab12"));
}

// RFC 5646 Section 2.2.5: several variant subtags in a row
TEST(multiple_variants) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("sl-rozaj-biske-1994"));
}

// RFC 5646 Section 2.2.5: the same variant must not appear twice
TEST(duplicate_variant_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("de-1994-1994"));
  EXPECT_FALSE(sourcemeta::core::is_langtag("sl-rozaj-rozaj"));
}

// RFC 5646 Section 2.2.5: variant duplication is detected case insensitively
TEST(duplicate_variant_case_insensitive_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("sl-rozaj-ROZAJ"));
}

// RFC 5646 Section 2.2.6: a digit singleton introduces an extension
TEST(digit_singleton_extension) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-0-bbb"));
}

// RFC 5646 Section 2.2.6: an extension may carry several subtags
TEST(extension_multiple_subtags) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-a-bbb-ccc-ddd"));
}

// RFC 5646 Section 2.2.6: several extensions with different singletons
TEST(multiple_extensions) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-a-bbb-c-ddd"));
}

// RFC 5646 Section 2.2.6: every letter except "x" is a valid singleton,
// including the letters bordering the reserved "x"
TEST(extension_singleton_letter_boundaries) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-w-aa"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-y-aa"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-z-aa"));
}

// RFC 5646 Section 2.2.6: a singleton must be followed by a subtag
TEST(bare_extension_singleton_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("en-a"));
}

// RFC 5646 Section 2.2.6: a singleton cannot be followed by another singleton
TEST(adjacent_singletons_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("en-a-b"));
}

// RFC 5646 Section 2.2.6: an extension subtag is at most eight characters
TEST(extension_subtag_too_long_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("en-a-123456789"));
}

// RFC 5646 Section 2.2.6: an eight-character extension subtag is allowed
TEST(extension_subtag_eight_characters) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-a-abcd1234"));
}

// RFC 5646 Section 2.2.6: an extension subtag is at least two characters
TEST(extension_subtag_single_character_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("en-a-b-ccc"));
}

// RFC 5646 Section 2.2.6: a singleton must not appear more than once
TEST(duplicate_singleton_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("en-a-bbb-a-ccc"));
}

// RFC 5646 Section 2.2.6: singleton duplication is detected case insensitively
TEST(duplicate_singleton_case_insensitive_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("en-a-bbb-A-ccc"));
}

// RFC 5646 Section 2.2.6: a digit singleton must not appear more than once
TEST(duplicate_digit_singleton_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("en-0-aaa-0-bbb"));
}

// RFC 5646 Section 2.2.7: a private use tag with a single subtag
TEST(private_use_only_single_subtag) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("x-foo"));
}

// RFC 5646 Section 2.2.7: a private use tag with several subtags
TEST(private_use_only_multiple_subtags) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("x-a-b-c"));
}

// RFC 5646 Section 2.2.7: a private use subtag of a single character
TEST(private_use_single_character_subtag) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("x-a"));
}

// RFC 5646 Section 2.2.7: a single-character private use subtag is allowed
// after a language, where an extension subtag could never be that short
TEST(private_use_single_character_subtag_after_language) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-x-c"));
}

// RFC 5646 Section 2.2.7: the private use singleton is case insensitive
TEST(private_use_uppercase_singleton) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("X-foo"));
}

// RFC 5646 Section 2.2.7: an uppercase "X" introduces private use after a
// language and is never treated as an extension singleton
TEST(private_use_uppercase_singleton_after_language) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-X-foo"));
}

// RFC 5646 Section 2.2.7: a private use sequence after an extension
TEST(private_use_after_extension) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-a-bb-x-cc"));
}

// RFC 5646 Section 2.2.7: a private use singleton must be followed by a subtag
TEST(bare_private_use_singleton_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("x"));
  EXPECT_FALSE(sourcemeta::core::is_langtag("en-x"));
}

// RFC 5646 Section 2.2.7: a private use subtag is at most eight characters
TEST(private_use_subtag_too_long_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("x-123456789"));
}

// RFC 5646 Section 2.2.7: an eight-character private use subtag is allowed
TEST(private_use_subtag_eight_characters) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("x-abcd1234"));
}

// RFC 5646 Section 2.2.8: the irregular grandfathered tags
TEST(grandfathered_irregular) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("en-GB-oed"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-ami"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-bnn"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-default"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-enochian"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-hak"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-klingon"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-lux"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-mingo"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-navajo"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-pwn"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-tao"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-tay"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("i-tsu"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("sgn-BE-FR"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("sgn-BE-NL"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("sgn-CH-DE"));
}

// RFC 5646 Section 2.2.8: irregular grandfathered tags are case insensitive
TEST(grandfathered_irregular_case_insensitive) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("I-AMI"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("EN-gb-OED"));
}

// RFC 5646 Section 2.2.8: the regular grandfathered tags match the langtag
// grammar
TEST(grandfathered_regular_via_grammar) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("art-lojban"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("cel-gaulish"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("no-bok"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("no-nyn"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-guoyu"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-hakka"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-min"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-min-nan"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-xiang"));
}

// RFC 5646 Section 2.1.1: language tags are not case sensitive
TEST(case_insensitive) {
  EXPECT_TRUE(sourcemeta::core::is_langtag("EN-us"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("zh-hant-hk"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("DE-ch-1901"));
}

// RFC 5646 Section 2.2.9: well-formedness does not require registered subtags
TEST(well_formed_but_not_valid) {
  // Unregistered subtags are still well-formed
  EXPECT_TRUE(sourcemeta::core::is_langtag("zz-Zzzz"));
  EXPECT_TRUE(sourcemeta::core::is_langtag("qaa"));
}

// RFC 5646 Section 2.1: the empty string is not a language tag
TEST(empty) { EXPECT_FALSE(sourcemeta::core::is_langtag("")); }

// RFC 5646 Section 2.1: a lone separator is not a language tag
TEST(only_hyphen) { EXPECT_FALSE(sourcemeta::core::is_langtag("-")); }

// RFC 5646 Section 2.1: a tag must not begin with a separator
TEST(leading_hyphen) { EXPECT_FALSE(sourcemeta::core::is_langtag("-en")); }

// RFC 5646 Section 2.1: a tag must not end with a separator
TEST(trailing_hyphen) { EXPECT_FALSE(sourcemeta::core::is_langtag("en-")); }

// RFC 5646 Section 2.1: subtags must not be empty
TEST(double_hyphen) { EXPECT_FALSE(sourcemeta::core::is_langtag("en--US")); }

// RFC 5646 Section 2.1: subtags are alphanumeric only
TEST(non_alphanumeric_character) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("en_US"));
  EXPECT_FALSE(sourcemeta::core::is_langtag("en-US!"));
  EXPECT_FALSE(sourcemeta::core::is_langtag("en.US"));
}

// RFC 5646 Section 2.1: subtags are limited to ASCII letters and digits
TEST(non_ascii_character) {
  EXPECT_FALSE(sourcemeta::core::is_langtag("én"));
  EXPECT_FALSE(sourcemeta::core::is_langtag("en-café"));
}

// RFC 5646 Section 2.1: whitespace is not part of the grammar
TEST(whitespace) {
  EXPECT_FALSE(sourcemeta::core::is_langtag(" en"));
  EXPECT_FALSE(sourcemeta::core::is_langtag("en "));
  EXPECT_FALSE(sourcemeta::core::is_langtag("en US"));
}

// RFC 5646 Section 2.1.1: a lowercase primary language subtag is canonical
TEST(canonical_simple_language) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("en"));
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("mas"));
}

// RFC 5646 Section 2.1.1: the primary language subtag is lowercase
TEST(canonical_uppercase_language_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("EN"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("En"));
}

// RFC 5646 Section 2.1.1: a four-letter subtag at the start of the tag is a
// primary language, not a script, so it stays lowercase
TEST(canonical_four_letter_language_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("aaaa"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("Aaaa"));
}

// RFC 5646 Section 2.1.1: region subtags are uppercase
TEST(canonical_region_uppercase) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("en-GB"));
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("de-DE"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("en-gb"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("en-Gb"));
}

// RFC 5646 Section 2.1.1: numeric region subtags carry no casing
TEST(canonical_numeric_region) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("es-419"));
}

// RFC 5646 Section 2.1.1: script subtags are titlecase
TEST(canonical_script_titlecase) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("zh-Hant"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("zh-hant"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("zh-HANT"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("zh-hANT"));
}

// RFC 5646 Section 2.1.1: titlecase script and uppercase region together
TEST(canonical_script_and_region) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("zh-Hant-HK"));
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("sr-Latn-RS"));
}

// RFC 5646 Section 2.1.1: extended language subtags are lowercase
TEST(canonical_extlang_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("zh-yue-Hant-CN"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("zh-YUE"));
}

// RFC 5646 Section 2.1.1: variant subtags are lowercase
TEST(canonical_variant_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("sl-rozaj-biske"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("sl-ROZAJ"));
}

// RFC 5646 Section 2.1.1: a digit-led variant keeps its letters lowercase
TEST(canonical_digit_led_variant) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("de-CH-1901"));
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("de-1abc"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("de-1ABC"));
}

// RFC 5646 Section 2.1.1: extension singletons are lowercase
TEST(canonical_singleton_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("en-a-bbb"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("en-A-bbb"));
}

// RFC 5646 Section 2.1.1: extension subtags are lowercase
TEST(canonical_extension_subtags_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("en-US-u-islamcal"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("en-a-BBB"));
}

// RFC 5646 Section 2.1.1: two-letter and four-letter subtags that occur after
// a singleton stay lowercase, as in the examples of that section
TEST(canonical_after_singleton_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("en-CA-x-ca"));
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("az-Latn-x-latn"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("en-CA-x-CA"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("az-Latn-x-Latn"));
}

// RFC 5646 Section 2.1.1: private use tags are lowercase throughout
TEST(canonical_private_use_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("x-foo"));
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("x-ab"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("X-foo"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("x-FOO"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("x-AB"));
}

// RFC 5646 Appendix A: a well-formed private use sequence with uppercase
// subtags is not canonical
TEST(canonical_rfc_private_use_examples) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("de-CH-x-phonebk"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("az-Arab-x-AYB"));
}

// RFC 5646 Section 2.2.8: the registry deprecates every irregular
// grandfathered tag in favour of a canonical replacement
TEST(canonical_irregular_grandfathered_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("en-GB-oed"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("i-klingon"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("sgn-BE-FR"));
}

// RFC 5646 Section 2.2.8: the regular grandfathered tags match the grammar
// and are already lowercase
TEST(canonical_regular_grandfathered) {
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("zh-min-nan"));
  EXPECT_TRUE(sourcemeta::core::is_canonical_langtag("art-lojban"));
}

// RFC 5646 Section 2.1: a canonical tag is first of all a language tag
TEST(canonical_malformed_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag(""));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("en-"));
  EXPECT_FALSE(sourcemeta::core::is_canonical_langtag("de-419-DE"));
}

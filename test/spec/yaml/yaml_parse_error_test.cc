#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>
#include <sourcemeta/core/yaml.h>

#include <cstddef>    // std::size_t
#include <filesystem> // std::filesystem
#include <string>     // std::string

// A run of one character that takes four bytes to write, for showing that what
// a copy occupies is counted in bytes rather than in the characters they spell
static auto wide_characters(const std::size_t count) -> std::string {
  const auto character{sourcemeta::core::codepoint_to_utf8(U'\U0001F600')};
  std::string result;
  result.reserve(count * character.size());
  for (std::size_t index = 0; index < count; index++) {
    result += character;
  }

  return result;
}

TEST(deeply_nested_flow_is_rejected) {
  const std::string input{std::string(2000, '[') + std::string(2000, ']')};
  try {
    const auto result{sourcemeta::core::parse_yaml(input)};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Maximum nesting depth exceeded");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 101);
  }
}

TEST(empty) {
  const std::string input{""};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
    EXPECT_STREQ(error.what(), "Empty YAML document");
  } catch (...) {
    FAIL();
  }
}

TEST(blank) {
  const std::string input{"    "};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
    EXPECT_STREQ(error.what(), "Empty YAML document");
  } catch (...) {
    FAIL();
  }
}

TEST(invalid_1) {
  const std::string input{"{ xx"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
    EXPECT_STREQ(error.what(), "Unexpected end of input in flow mapping");
  } catch (...) {
    FAIL();
  }
}

TEST(undefined_anchor) {
  const std::string input{"alias: *unknown"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLUnknownAnchorError &error) {
    EXPECT_EQ(error.anchor(), "unknown");
    EXPECT_STREQ(error.what(), "YAML alias references undefined anchor");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 8);
  } catch (...) {
    FAIL();
  }
}

TEST(read_yaml_invalid_carries_path) {
  try {
    sourcemeta::core::read_yaml(std::filesystem::path{STUBS_PATH} /
                                "invalid.yaml");
    FAIL();
  } catch (const sourcemeta::core::YAMLFileParseError &error) {
    EXPECT_EQ(error.path(), std::filesystem::path{STUBS_PATH} / "invalid.yaml");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_or_json_invalid_yaml_throws_yaml_error) {
  try {
    sourcemeta::core::read_yaml_or_json(std::filesystem::path{STUBS_PATH} /
                                        "invalid.yaml");
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
    EXPECT_STREQ(error.what(), "Unexpected end of input in flow mapping");
  } catch (...) {
    FAIL();
  }
}

TEST(invalid_hex_escape) {
  const std::string input{R"("\xGG")"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_STREQ(error.what(), "Invalid hex escape sequence");
    EXPECT_EQ(error.column(), 6);
  } catch (...) {
    FAIL();
  }
}

TEST(invalid_unicode_escape_4) {
  const std::string input{R"("\uZZZZ")"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_STREQ(error.what(), "Invalid hex escape sequence");
    EXPECT_EQ(error.column(), 8);
  } catch (...) {
    FAIL();
  }
}

TEST(invalid_unicode_escape_8) {
  const std::string input{R"("\UZZZZZZZZ")"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_STREQ(error.what(), "Invalid hex escape sequence");
    EXPECT_EQ(error.column(), 12);
  } catch (...) {
    FAIL();
  }
}

TEST(surrogate_unicode_escape_is_rejected) {
  // YAML 1.2.2 Section 5.7: a lone surrogate is not a Unicode scalar value
  const std::string input{R"("\uD800")"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_STREQ(error.what(), "Invalid Unicode escape sequence");
    EXPECT_EQ(error.column(), 8);
  } catch (...) {
    FAIL();
  }
}

TEST(out_of_range_unicode_escape_is_rejected) {
  const std::string input{R"("\UFFFFFFFF")"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_STREQ(error.what(), "Invalid Unicode escape sequence");
    EXPECT_EQ(error.column(), 12);
  } catch (...) {
    FAIL();
  }
}

TEST(exponential_alias_expansion_is_bounded) {
  const std::string input{"a: &a [ x, x, x, x, x, x, x, x, x, x ]\n"
                          "b: &b [ *a, *a, *a, *a, *a, *a, *a, *a, *a, *a ]\n"
                          "c: &c [ *b, *b, *b, *b, *b, *b, *b, *b, *b, *b ]\n"
                          "d: &d [ *c, *c, *c, *c, *c, *c, *c, *c, *c, *c ]\n"
                          "e: &e [ *d, *d, *d, *d, *d, *d, *d, *d, *d, *d ]\n"
                          "f: &f [ *e, *e, *e, *e, *e, *e, *e, *e, *e, *e ]\n"
                          "g: &g [ *f, *f, *f, *f, *f, *f, *f, *f, *f, *f ]\n"
                          "h: &h [ *g, *g, *g, *g, *g, *g, *g, *g, *g, *g ]\n"
                          "i: &i [ *h, *h, *h, *h, *h, *h, *h, *h, *h, *h ]\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 5);
    EXPECT_EQ(error.column(), 9);
    EXPECT_STREQ(error.what(), "Maximum YAML alias expansion exceeded");
  } catch (...) {
    FAIL();
  }
}

// The allowance is a ratio to the text that has been read rather than a fixed
// count, so a chain of aliases that a longer document expands in full is
// turned away in a document that carries nothing but the chain
TEST(alias_expansion_beyond_the_allowance_is_rejected) {
  const std::string aliases{"a: &a [ x, x, x, x, x, x, x, x, x, x ]\n"
                            "b: &b [ *a, *a, *a, *a, *a, *a, *a, *a, *a, *a ]\n"
                            "c: &c [ *b, *b, *b, *b, *b, *b, *b, *b, *b, *b ]\n"
                            "d: &d [ *c, *c, *c, *c, *c, *c, *c, *c, *c, *c ]\n"
                            "e: [ *d, *d, *d, *d, *d, *d, *d, *d, *d, *d ]\n"};

  try {
    sourcemeta::core::parse_yaml(aliases);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 5);
    EXPECT_EQ(error.column(), 6);
    EXPECT_STREQ(error.what(), "Maximum YAML alias expansion exceeded");
  } catch (...) {
    FAIL();
  }
}

// The allowance an alias draws on is fixed by the text ahead of it, so a
// comment sitting behind it buys nothing even though reading the token that
// follows the alias skips over that comment first
TEST(alias_expansion_allowance_ignores_a_comment_behind_the_alias) {
  const std::string input{"a: &a [ x, x, x, x, x, x, x, x, x, x ]\n"
                          "b: &b [ *a, *a, *a, *a, *a, *a, *a, *a, *a, *a ]\n"
                          "c: &c [ *b, *b, *b, *b, *b, *b, *b, *b, *b, *b ]\n"
                          "d: &d [ *c, *c, *c, *c, *c, *c, *c, *c, *c, *c ]\n"
                          "e: *d #" +
                          std::string(2000, 'x') + "\n"};

  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 5);
    EXPECT_EQ(error.column(), 4);
    EXPECT_STREQ(error.what(), "Maximum YAML alias expansion exceeded");
  } catch (...) {
    FAIL();
  }
}

// What an expansion costs is what materialising it takes, so a place holding a
// long run of text cannot be copied for the price of an empty one. Charging
// each place alone let this one spend about one part in a thousand of its
// allowance while materialising two megabytes from two kilobytes of input.
TEST(alias_expansion_allowance_counts_the_text_a_place_carries) {
  const std::string input{"a: &a " + std::string(2000, 'x') + "\n" +
                          "b: &b [ *a, *a, *a, *a, *a, *a, *a, *a, *a, *a ]\n"
                          "c: &c [ *b, *b, *b, *b, *b, *b, *b, *b, *b, *b ]\n"
                          "d: [ *c, *c, *c, *c, *c, *c, *c, *c, *c, *c ]\n"};

  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 4);
    EXPECT_STREQ(error.what(), "Maximum YAML alias expansion exceeded");
    EXPECT_EQ(error.column(), 10);
  } catch (...) {
    FAIL();
  }
}

// Text the parser has not reached cannot pay for an expansion it takes no part
// in, so trailing content leaves the allowance where it stood. This is what
// keeps a short document from buying room out of the documents that follow it
// in a stream
TEST(alias_expansion_allowance_ignores_text_after_the_expansion) {
  const std::string input{"a: &a [ x, x, x, x, x, x, x, x, x, x ]\n"
                          "b: &b [ *a, *a, *a, *a, *a, *a, *a, *a, *a, *a ]\n"
                          "c: &c [ *b, *b, *b, *b, *b, *b, *b, *b, *b, *b ]\n"
                          "d: &d [ *c, *c, *c, *c, *c, *c, *c, *c, *c, *c ]\n"
                          "e: [ *d, *d, *d, *d, *d, *d, *d, *d, *d, *d ]\n"
                          "filler: " +
                          std::string(2000, 'x') + "\n"};

  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 5);
    EXPECT_EQ(error.column(), 6);
    EXPECT_STREQ(error.what(), "Maximum YAML alias expansion exceeded");
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.8.1: the version parameter is a major and minor decimal
// number, so a comment sign without a preceding separation space is invalid.
TEST(yaml_directive_comment_without_separation_is_rejected) {
  const std::string input{"%YAML 1.1#foo\n---\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid version in %YAML directive");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_directive_non_numeric_version_is_rejected) {
  const std::string input{"%YAML abc\n---\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid version in %YAML directive");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 8.2.1: a nested block sequence that is compact on the same
// line as its parent entry is indented by spaces, so a tab is invalid.
TEST(tab_before_compact_nested_sequence_is_rejected) {
  const std::string input{"-\t-\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

TEST(tab_after_space_before_compact_nested_sequence_is_rejected) {
  const std::string input{"- \t-\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
    EXPECT_EQ(error.column(), 4);
  } catch (...) {
    FAIL();
  }
}

TEST(tab_before_compact_nested_sequence_under_explicit_key_is_rejected) {
  const std::string input{"?\t-\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.1: a block mapping key establishes its indentation,
// which is made of spaces, so a tab in the leading whitespace of the key is
// invalid.
TEST(tab_in_block_mapping_key_indentation_is_rejected) {
  const std::string input{"foo:\n  a: 1\n \tb: 2\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 3);
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
    EXPECT_EQ(error.column(), 4);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 8.2.1: a compact mapping under an explicit key indicator
// is indented by spaces, so a tab in that separation is invalid.
TEST(tab_before_compact_mapping_under_explicit_key_is_rejected) {
  const std::string input{"?\tkey:\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
    EXPECT_EQ(error.column(), 6);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.1: a folded double-quoted line that must reach a block
// indentation cannot use a tab for that indentation.
TEST(tab_indented_double_quoted_continuation_is_rejected) {
  const std::string input{"foo: \"bar\n\tbaz\"\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 2);
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.1: the white space of an empty block scalar line is
// indentation, so a tab before any space, leaving the line blank, is invalid.
TEST(tab_on_empty_block_scalar_line_is_rejected) {
  const std::string input{"foo: |\n\t\nbar: 1\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 2);
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 8.2: a flow collection nested in a block context continues
// on following lines only when they are indented past that block context.
TEST(flow_mapping_content_at_block_indent_is_rejected) {
  const std::string input{"k: {\nk\n:\nv\n}\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 2);
    EXPECT_STREQ(error.what(), "Insufficient indentation in flow collection");
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_sequence_content_at_block_indent_is_rejected) {
  const std::string input{"flow: [a,\nb,\nc]\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 2);
    EXPECT_STREQ(error.what(), "Insufficient indentation in flow collection");
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.1: only spaces count as indentation, so a flow
// continuation line whose leading white space is a tab is not indented and is
// rejected in a block context.
TEST(tab_indented_flow_continuation_line_is_rejected) {
  const std::string input{"- [\n\tfoo,\n foo\n]\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 2);
    EXPECT_STREQ(error.what(), "Insufficient indentation in flow collection");
    EXPECT_EQ(error.column(), 2);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 8.2.2: a node property that decorates a block collection
// is indented past the mapping the collection belongs to, unlike the block
// sequence itself, which may sit at the mapping's own indentation
TEST(anchored_indentless_sequence_at_the_mapping_indentation_is_rejected) {
  const std::string input{"a:\n  b:\n  &x\n  - 1\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 3);
    EXPECT_STREQ(error.what(), "Node property at wrong indentation level");
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

TEST(tagged_indentless_sequence_at_the_mapping_indentation_is_rejected) {
  const std::string input{"a:\n  b:\n  !!seq\n  - 1\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 3);
    EXPECT_STREQ(error.what(), "Node property at wrong indentation level");
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.8.1: the %YAML directive requires a version parameter.
TEST(yaml_directive_without_version_is_rejected) {
  const std::string input{"%YAML\n---\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid version in %YAML directive");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// A version has exactly one dot separating the major and minor numbers.
TEST(yaml_directive_multiple_dot_version_is_rejected) {
  const std::string input{"%YAML 1.2.3\n--- x\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid version in %YAML directive");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.1: a tab does not count as indentation, so a flow scalar
// continuation whose spaces only reach the block indentation, followed by a
// tab, is under-indented and rejected.
TEST(tab_after_spaces_flow_scalar_continuation_is_rejected) {
  const std::string input{"a:\n b: \"x\n \ty\"\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &) {
    // The continuation reaching only the block indentation with spaces is
    // expected
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.1: a tab at the start of a line is invalid indentation
// regardless of a preceding blank indented line. The per-line indentation state
// must reset on a line break so the blank line's spaces do not carry over.
TEST(tab_line_start_after_blank_indented_line_is_rejected) {
  const std::string input{"a:\n   \n\tb\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 3);
    EXPECT_EQ(error.column(), 2);
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 5.1: the printable character set excludes the control
// block below the space, so a raw control character is rejected.
TEST(raw_control_character_is_rejected) {
  const std::string input{std::string{"key: va"} + '\x01' + "lue"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Control character not allowed in YAML stream");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 8);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.8.1: a document naming a higher major version is
// rejected.
TEST(yaml_directive_higher_major_version_is_rejected) {
  const std::string input{"%YAML 2.0\n---\nfoo: bar\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unsupported major version in %YAML directive");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.8.2: a handle may carry at most one tag directive in a
// document.
TEST(duplicate_tag_directive_same_handle_is_rejected) {
  const std::string input{"%TAG !e! tag:example.com,2000:app/\n"
                          "%TAG !e! tag:example.com,2000:other/\n"
                          "--- value\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Duplicate %TAG directive for the same handle");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.8.2.1: a named tag handle must be associated with a
// prefix by a tag directive.
TEST(undefined_tag_handle_is_rejected) {
  const std::string input{"!e!foo bar\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Undefined tag handle");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 7);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 5.1: the nb-json production excludes the C0 control block
// other than tab, so a C0 control is rejected even inside a quoted scalar.
TEST(c0_control_inside_a_quoted_scalar_is_rejected) {
  const std::string input{std::string{"key: \"a"} + '\x01' + "b\""};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Control character not allowed in YAML stream");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 8);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 5.2: a stream is a sequence of characters in a supported
// encoding, so a byte sequence that is not well-formed UTF-8 is not a stream.
TEST(invalid_utf8_sequence_is_rejected) {
  const std::string input{std::string{"key: va"} + '\xFF' + "lue"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid UTF-8 sequence in YAML stream");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 8);
  } catch (...) {
    FAIL();
  }
}

// RFC 8259 Section 6: numeric values that cannot be represented in the JSON
// grammar, such as Infinity and NaN, are not permitted.
TEST(positive_infinity_is_rejected) {
  const std::string input{".inf"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Infinity and NaN are not permitted");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  } catch (...) {
    FAIL();
  }
}

TEST(negative_infinity_is_rejected) {
  const std::string input{"-.inf"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Infinity and NaN are not permitted");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  } catch (...) {
    FAIL();
  }
}

TEST(not_a_number_is_rejected) {
  const std::string input{".nan"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Infinity and NaN are not permitted");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  } catch (...) {
    FAIL();
  }
}

TEST(infinity_as_mapping_value_is_rejected) {
  const std::string input{"key: .inf"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Infinity and NaN are not permitted");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  } catch (...) {
    FAIL();
  }
}

// RFC 8259 Section 4: an object member name is a string, so an explicit block
// mapping key that is a flow sequence cannot be represented and is rejected,
// matching PyYAML and js-yaml.
TEST(block_explicit_flow_sequence_key_is_rejected) {
  const std::string input{"? [a, b]\n: value"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

TEST(block_explicit_flow_mapping_key_is_rejected) {
  const std::string input{"? {a: b}\n: value"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

TEST(block_plain_key_then_flow_sequence_explicit_key_is_rejected) {
  const std::string input{"a: 1\n? [b]\n: 2"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

TEST(block_plain_key_then_flow_mapping_explicit_key_is_rejected) {
  const std::string input{"a: 1\n? {b: c}\n: 2"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

TEST(block_plain_key_then_block_sequence_explicit_key_is_rejected) {
  const std::string input{"a: 1\n?\n  - b\n: 2"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
  } catch (...) {
    FAIL();
  }
}

TEST(block_explicit_block_sequence_key_is_rejected) {
  const std::string input{"?\n  - a\n  - b\n: value"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

// RFC 8259 Section 4: an alias that resolves to a collection cannot be used as
// an object member name.
TEST(alias_collection_key_is_rejected) {
  const std::string input{"first: &a [1, 2]\n*a : value"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_sequence_explicit_collection_key_is_rejected) {
  const std::string input{"[ ? [a, b] : v ]"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 5);
  } catch (...) {
    FAIL();
  }
}

// A scalar key is resolved to its typed value before becoming a member name,
// so a hexadecimal key and its decimal equivalent collide.
TEST(flow_mapping_resolved_key_duplicate_is_rejected) {
  const std::string input{"{0x1: a, 1: b}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "1");
    EXPECT_STREQ(error.what(), "Duplicate key in YAML mapping");
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_resolved_key_duplicate_is_rejected) {
  const std::string input{"0x1: a\n1: b"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "1");
    EXPECT_STREQ(error.what(), "Duplicate key in YAML mapping");
  } catch (...) {
    FAIL();
  }
}

// The position reported is that of the offending second key, rather than of the
// mapping or of the first occurrence
TEST(block_mapping_duplicate_reports_the_second_key_position) {
  const std::string input{"foo: 1\nbar: 2\nfoo: 3"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "foo");
    EXPECT_EQ(error.line(), 3);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_mapping_duplicate_reports_the_second_key_position) {
  const std::string input{"{foo: 1, foo: 2}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "foo");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 10);
  } catch (...) {
    FAIL();
  }
}

// RFC 8259 Section 6: Infinity and NaN are not permitted, and an explicit float
// tag does not exempt them in conversion mode.
TEST(float_tag_infinity_is_rejected) {
  const std::string input{"!!float .inf"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Infinity and NaN are not permitted");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  } catch (...) {
    FAIL();
  }
}

TEST(float_tag_not_a_number_is_rejected) {
  const std::string input{"!!float .nan"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Infinity and NaN are not permitted");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  } catch (...) {
    FAIL();
  }
}

TEST(escape_incomplete_hex) {
  const std::string input{R"("\xZ")"};
  try {
    const auto result{sourcemeta::core::parse_yaml(input)};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid hex escape sequence");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 6);
  }
}

TEST(unterminated_single_quote) {
  const std::string input{"'abc"};
  try {
    const auto result{sourcemeta::core::parse_yaml(input)};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing closing quote in single-quoted scalar");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  }
}

TEST(flow_mapping_bare_key_at_eof_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("{a")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected end of input in flow mapping");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  }
}

TEST(flow_mapping_colon_at_eof_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("{a:")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Expected value after ':'");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  }
}

TEST(flow_mapping_anchor_key_at_eof_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("{&a")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected end of input in flow mapping");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  }
}

TEST(flow_sequence_dash_at_eof_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("[-")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid plain scalar start in flow context");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 2);
  }
}

TEST(alias_key_referencing_unknown_anchor_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("*a: b")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "YAML alias references undefined anchor");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  }
}

TEST(flow_explicit_key_at_eof_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("[?")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected end after explicit key in flow");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  }
}

TEST(flow_collection_indented_at_parent_block_level_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("a: [\n1]")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Insufficient indentation in flow collection");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 1);
  }
}

TEST(explicit_key_block_mapping_duplicate_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("? a\n: 1\n? a\n: 2")};
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "a");
    EXPECT_STREQ(error.what(), "Duplicate key in YAML mapping");
  } catch (...) {
    FAIL();
  }
}

TEST(mapping_key_that_is_a_collection_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("[? {a: 1}: b]")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 4);
  }
}

TEST(tab_trailing_after_quoted_scalar_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("'x'\tbad")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid trailing content");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 4);
  }
}

// YAML 1.2.2 Section 7.1: an alias in key position stands for the value of its
// anchor, so it collides with an identical key that is already present
TEST(explicit_key_alias_duplicate_is_rejected) {
  const std::string input{"? &anchor foo\n: 1\n? *anchor\n: 2"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "foo");
    EXPECT_STREQ(error.what(), "Duplicate key in YAML mapping");
    EXPECT_EQ(error.line(), 3);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

TEST(explicit_key_alias_duplicate_after_plain_key_is_rejected) {
  const std::string input{"x: 1\n? &anchor foo\n: 2\n? *anchor\n: 3"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "foo");
  } catch (...) {
    FAIL();
  }
}

TEST(explicit_key_alias_resolves_when_mapping_starts_with_it) {
  const std::string input{"? &a foo\n: 1\n? *a\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "foo");
  } catch (...) {
    FAIL();
  }
}

TEST(explicit_key_alias_to_unknown_anchor_is_rejected) {
  const std::string input{"x: 1\n? *missing\n: 2"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLUnknownAnchorError &error) {
    EXPECT_EQ(error.anchor(), "missing");
  } catch (...) {
    FAIL();
  }
}

// An anchor names the resolved node, so aliasing a key whose text differs from
// the member name it resolves to still collides with that member name
TEST(explicit_key_alias_duplicate_through_resolved_key_is_rejected) {
  const std::string input{"? &a 0x1\n: 1\n? *a\n: 2"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "1");
  } catch (...) {
    FAIL();
  }
}

TEST(explicit_key_alias_duplicate_through_null_key_is_rejected) {
  const std::string input{"? &a ~\n: 1\n? *a\n: 2"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "");
  } catch (...) {
    FAIL();
  }
}

TEST(explicit_key_alias_duplicate_through_resolved_key_after_plain_key) {
  const std::string input{"x: 1\n? &a 0x1\n: 2\n? *a\n: 3"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "1");
  } catch (...) {
    FAIL();
  }
}

// An anchor on an implicit key names the resolved key node too, so aliasing it
// collides with the member name that key produced rather than with its raw text
TEST(implicit_key_alias_duplicate_through_resolved_key_is_rejected) {
  const std::string input{"&a 0x1: first\n*a : second"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "1");
  } catch (...) {
    FAIL();
  }
}

TEST(implicit_key_alias_duplicate_is_rejected) {
  const std::string input{"&a foo: 1\nbar: 2\n*a : 3"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "foo");
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 7.4.2: the entries of a flow mapping are separated by a
// comma, and the value of an entry is a single node, so a second pair cannot
// stand in the place of the first entry's value
TEST(flow_mapping_entries_without_a_comma) {
  const std::string input{"{a: 1 b: 2}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing comma between flow mapping entries");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 8);
  }
}

TEST(flow_sequence_single_pair_value_cannot_be_another_pair) {
  const std::string input{"[a: b: c]"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing comma in flow sequence");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 6);
  }
}

// Section 9.1 has each document be "completely independent from the rest", so
// an anchor declared in one is out of reach of an alias in the next. The
// anchors survived the boundary, which let the second document borrow them.
TEST(anchor_does_not_reach_across_a_directives_end_marker) {
  try {
    sourcemeta::core::read_yaml(std::filesystem::path{STUBS_PATH} /
                                "multi_document_anchor_reuse.yaml");
    FAIL();
  } catch (const sourcemeta::core::YAMLFileParseError &error) {
    EXPECT_STREQ(error.what(), "YAML alias references undefined anchor");
  } catch (...) {
    FAIL();
  }
}

TEST(anchor_does_not_reach_across_a_document_end_marker) {
  try {
    sourcemeta::core::read_yaml(std::filesystem::path{STUBS_PATH} /
                                "multi_document_anchor_reuse_after_end.yaml");
    FAIL();
  } catch (const sourcemeta::core::YAMLFileParseError &error) {
    EXPECT_STREQ(error.what(), "YAML alias references undefined anchor");
  } catch (...) {
    FAIL();
  }
}

// An empty key is a key like any other, so naming it twice names the same key
// twice. Nothing recorded it, which left the mapping quietly keeping only the
// last of them.
TEST(leading_colon_value_indicator_duplicate_is_rejected) {
  const std::string input{": v\n~: w\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_STREQ(error.what(), "Duplicate key in YAML mapping");
  } catch (...) {
    FAIL();
  }
}

// The entry a value indicator opens is committed on more than one path, and an
// empty key naming the same place twice has to be refused on each of them. The
// path that ends the document committed without ever weighing the key, which
// quietly kept the last of them.
TEST(leading_colon_value_indicator_duplicate_at_the_end_of_a_document) {
  const std::string input{": [1]\n:\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_STREQ(error.what(), "Duplicate key in YAML mapping");
  } catch (...) {
    FAIL();
  }
}

TEST(leading_colon_value_indicator_before_a_second_entry_with_no_key) {
  const std::string input{":\n? a\n: b\n: c\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "");
    EXPECT_EQ(error.line(), 4);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// What a copy occupies is counted in bytes, where the logical length counts
// the characters those bytes spell. A character written in four of them was
// charged as one, so this chain stayed within an allowance it outgrows four
// times over.
TEST(alias_expansion_allowance_counts_bytes_not_characters) {
  const std::string input{"a: &a " + wide_characters(100) + "\n" +
                          "b: &b [ *a, *a, *a, *a, *a, *a, *a, *a, *a, *a, *a, "
                          "*a, *a, *a, *a, *a, *a, *a, *a, *a ]\n"
                          "c: [ *b, *b, *b, *b, *b, *b, *b, *b, *b, *b, *b, "
                          "*b, *b, *b, *b, *b, *b, *b, *b, *b ]\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 3);
    EXPECT_STREQ(error.what(), "Maximum YAML alias expansion exceeded");
    EXPECT_EQ(error.column(), 54);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.9.2 forbids only "[", "]", "{", "}" and "," in an
// anchor name, so a colon belongs to the name and the alias here reaches to the
// end of the word rather than leaving a mapping indicator behind
TEST(undefined_anchor_with_an_undefined_anchor_as_the_first_key) {
  const std::string input{"*missing: 2"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLUnknownAnchorError &error) {
    EXPECT_EQ(error.anchor(), "missing:");
    EXPECT_STREQ(error.what(), "YAML alias references undefined anchor");
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 7.1 resolves an alias against an anchor already seen, and
// a key is a position no suite case aliases from
TEST(undefined_anchor_with_an_undefined_anchor_as_an_explicit_key) {
  const std::string input{"? *missing\n: 2"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLUnknownAnchorError &error) {
    EXPECT_EQ(error.anchor(), "missing");
    EXPECT_STREQ(error.what(), "YAML alias references undefined anchor");
  } catch (...) {
    FAIL();
  }
}

// The same reading of the anchor name on a line that is not the document's
// first, which the parser reaches through a separate path
TEST(undefined_anchor_with_an_undefined_anchor_as_a_later_key) {
  const std::string input{"a: 1\n*missing: 2"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLUnknownAnchorError &error) {
    EXPECT_EQ(error.anchor(), "missing:");
    EXPECT_STREQ(error.what(), "YAML alias references undefined anchor");
  } catch (...) {
    FAIL();
  }
}

// With the indicator separated the name ends before it, which is what shows the
// reading above to be the grammar at work rather than the colon being lost
TEST(undefined_anchor_as_a_key_separated_from_its_indicator) {
  const std::string input{"*missing : 2"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLUnknownAnchorError &error) {
    EXPECT_EQ(error.anchor(), "missing");
    EXPECT_STREQ(error.what(), "YAML alias references undefined anchor");
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 8.2.1 indents the compact content of a block entry with
// spaces, so a tab in that separation is invalid where the content is itself an
// indicator. The sequence sibling of this check is already exercised, the
// explicit key one is not
TEST(a_tab_before_an_explicit_key_indicator_is_rejected) {
  const std::string input{"-\t? a\n  : 1"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

// Section 5.7 gives the escape as a fixed number of hex digits, so an input
// that ends inside one never completes it
TEST(a_hex_escape_cut_short_by_the_end_of_input_is_rejected) {
  const std::string input{"\"\\x4"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Truncated hex escape sequence");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 5);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.8.1 joins a major and a minor decimal number with a
// dot, so neither side may be missing
TEST(yaml_directive_version_with_no_major_is_rejected) {
  const std::string input{"%YAML .1\n---\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid version in %YAML directive");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_directive_version_with_no_minor_is_rejected) {
  const std::string input{"%YAML 1.\n---\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid version in %YAML directive");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_directive_version_with_a_non_digit_major_is_rejected) {
  const std::string input{"%YAML a.1\n---\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid version in %YAML directive");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.8.1 rejects a document naming a higher major version
// than the processor supports, however many digits that version carries
TEST(yaml_directive_version_of_a_higher_major_is_rejected) {
  const std::string input{"%YAML 2.0\n---\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unsupported major version in %YAML directive");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_directive_version_of_a_two_digit_major_is_rejected) {
  const std::string input{"%YAML 11.0\n---\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unsupported major version in %YAML directive");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.8.2 spells the directive as "TAG" followed by a handle
// and then a prefix, so neither argument may be left out
TEST(yaml_tag_directive_with_no_handle_is_rejected) {
  const std::string input{"%TAG\n--- value\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Incomplete %TAG directive");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_tag_directive_with_no_prefix_is_rejected) {
  const std::string input{"%TAG !\n--- value\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Incomplete %TAG directive");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.9.1 writes a verbatim tag between angle brackets, and
// the opening bracket is not a tag character, so one left unclosed cannot be
// read as a shorthand tag either
TEST(yaml_verbatim_tag_without_its_closing_bracket_is_rejected) {
  const std::string input{"!<tag:yaml.org,2002:str value\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unterminated verbatim tag");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// A verbatim tag is built from URI characters, which exclude a line break
TEST(yaml_verbatim_tag_broken_by_a_line_break_is_rejected) {
  const std::string input{"!<tag:yaml.org\n2002:str> value\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unterminated verbatim tag");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.9.1 takes one or more tag characters after the handle,
// and Example 6.27 turns down a shorthand for want of them
TEST(yaml_secondary_handle_with_an_empty_suffix_is_rejected) {
  const std::string input{"!! value\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Tag shorthand with no suffix");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_named_handle_with_an_empty_suffix_is_rejected) {
  const std::string input{"!e! value\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Tag shorthand with no suffix");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 4);
  } catch (...) {
    FAIL();
  }
}

// A document end marker with nothing after it closes a document that never
// began, which is nothing to read
TEST(yaml_a_document_end_marker_with_nothing_after_it) {
  const std::string input{"...\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Empty YAML document");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 5.2 only strips the byte order mark of UTF-8, which takes
// all three of its bytes, so a prefix of it is content
TEST(first_byte_of_the_byte_order_mark_on_its_own) {
  const std::string input{"\xEF: 1"};
  try {
    const auto result{sourcemeta::core::parse_yaml(input)};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid UTF-8 sequence in YAML stream");
  }
}

TEST(first_two_bytes_of_the_byte_order_mark_on_their_own) {
  const std::string input{"\xEF\xBB: 1"};
  try {
    const auto result{sourcemeta::core::parse_yaml(input)};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid UTF-8 sequence in YAML stream");
  }
}

TEST(comment_as_the_whole_stream) {
  const std::string input{"# note"};
  try {
    const auto result{sourcemeta::core::parse_yaml(input)};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Empty YAML document");
  }
}

TEST(verbatim_tag_that_never_closes) {
  const std::string input{"foo: !<tag"};
  try {
    const auto result{sourcemeta::core::parse_yaml(input)};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unterminated verbatim tag");
  }
}

TEST(only_spaces_in_the_stream) {
  const std::string input{"   "};
  try {
    const auto result{sourcemeta::core::parse_yaml(input)};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Empty YAML document");
  }
}

// A verbatim tag that the document ends inside closes no more than one that a
// line break ends inside
TEST(yaml_verbatim_tag_that_the_document_ends_inside_is_rejected) {
  const std::string input{"a: !<tag"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unterminated verbatim tag");
  } catch (...) {
    FAIL();
  }
}

// An escape needs a character to escape, and the end of the document is not
// one, so the quote it opened is never closed
TEST(yaml_double_quoted_scalar_ending_on_an_escape_is_rejected) {
  const std::string input{"a: \"x\\"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing closing quote in double-quoted scalar");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_flow_sequence_that_the_document_ends_inside_is_rejected) {
  const std::string input{"a: [b"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing comma in flow sequence");
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 productions 192 to 194 have a block mapping entry's value follow a
// literal colon, so a plain scalar alone at the mapping's own indentation forms
// no entry. A caller holding the whole input cannot ask for a second document,
// so content the first one does not account for is content nothing could ever
// reach, and parsing a string holds it to that just as reading a file does
TEST(yaml_string_with_a_trailing_scalar_that_forms_no_entry) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"key: 1\nv\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected content after document");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_string_with_a_trailing_scalar_after_an_empty_value) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"key:\nv\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected content after document");
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 production 142 has a flow mapping entry separate its key from its
// value with a colon, so a key that another collection follows names no entry
TEST(flow_mapping_key_followed_by_a_flow_sequence) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"{a [1]}"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Expected ':' after mapping key");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_empty_key_entry_twice_is_a_duplicate) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: 1\n: b\n: c\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_STREQ(error.what(), "Duplicate key in YAML mapping");
  } catch (...) {
    FAIL();
  }
}

// The escape takes a fixed count of characters without asking whether each is
// a hexadecimal digit, so one that starts as a number and stops being one is
// turned down by the conversion rather than by the scan
TEST(yaml_hex_escape_ending_in_a_character_that_is_no_digit) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: \"\\x4G\"\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid hex escape sequence");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_four_digit_escape_ending_in_a_character_that_is_no_digit) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: \"\\u00G0\"\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid hex escape sequence");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_eight_digit_escape_ending_in_a_character_that_is_no_digit) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: \"\\U0000004G\"\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid hex escape sequence");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_tab_indenting_a_block_mapping_on_the_first_line) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"\ta: 1\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_tab_indenting_a_block_mapping_on_a_later_line) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a:\n\tb: 1\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
  } catch (...) {
    FAIL();
  }
}

// Section 8.1.1.1: "It is an error for any of the leading empty lines to
// contain more spaces than the first non-empty line". A scalar at the root has
// no node holding it, so a first non-empty line of no indentation at all is
// content of it and the leading empty line stands further in than that
TEST(yaml_root_block_scalar_leading_empty_line_deeper_than_its_content) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"|\n  \nx\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(),
                 "Leading empty line has more spaces than content indentation");
  } catch (...) {
    FAIL();
  }
}

// Production 162 ends the header with `s-b-comment`, whose comment is reached
// through `s-separate-in-line`, so a comment with nothing white before it is
// no comment at all
TEST(yaml_block_scalar_header_comment_without_separating_whitespace) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: |#c\n  x\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid content in block scalar header");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_block_scalar_header_with_an_invalid_character) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: |x\n  y\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid content in block scalar header");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_block_scalar_tab_only_line_at_the_end_of_input) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"|\n\t"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_block_scalar_tab_only_line_ending_in_a_carriage_return) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"|\n\t\r\nx\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_single_quoted_scalar_unterminated_after_a_line_break) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: 'x\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing closing quote in single-quoted scalar");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_single_quoted_scalar_with_a_directives_end_marker_on_a_later_line) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: 'x\n---\n'"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Document marker inside flow scalar");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_single_quoted_scalar_with_a_document_end_marker_on_a_later_line) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: 'x\n...\n'"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Document marker inside flow scalar");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_single_quoted_scalar_followed_by_a_comment_without_whitespace) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: 'x'#c\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid trailing content");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_single_quoted_scalar_followed_by_a_plain_scalar_in_a_flow_sequence) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"[ 'x' y ]\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid trailing content");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_double_quoted_scalar_with_an_unknown_escape) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: \"\\q\"\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(),
                 "Invalid escape sequence in double-quoted scalar");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_double_quoted_scalar_with_an_escaped_break_before_the_end_of_input) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: \"x\\\n  "})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing closing quote in double-quoted scalar");
  } catch (...) {
    FAIL();
  }
}

// Production 126 writes `ns-plain-first(c)` as `( ns-char - c-indicator ) | (
// ( "?" | ":" | "-" ) /* Followed by an ns-plain-safe(c) */ )`, and production
// 129 writes `ns-plain-safe-in ::= ns-char - c-flow-indicator`
TEST(yaml_dash_alone_inside_a_flow_sequence) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"[ - ]"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid plain scalar start in flow context");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_question_mark_before_a_flow_indicator_inside_a_flow_sequence) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"[ ?, x ]"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid plain scalar start in flow context");
  } catch (...) {
    FAIL();
  }
}

// Section 6.6: "Comments must be separated from other tokens by white space
// characters"
TEST(yaml_comment_right_after_a_flow_sequence_indicator) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"[#c]"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected '#' character");
  } catch (...) {
    FAIL();
  }
}

// Production 130 leaves a colon that white space follows out of
// `ns-plain-char(c)`, so a line carrying one cannot be a line of a plain
// scalar and the scalar ends before it
TEST(yaml_plain_scalar_value_followed_by_a_deeper_mapping_entry) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: x\n   b: 1\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected content after document");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_plain_scalar_value_followed_by_a_deeper_key_holding_a_colon) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: x\n   b:c: 1\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected content after document");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_plain_scalar_value_followed_by_a_deeper_colon_at_the_end_of_input) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: x\n   b:"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected content after document");
  } catch (...) {
    FAIL();
  }
}

// A line carrying a mapping entry is no line of a plain scalar, so the scalar
// ends before it, and Section 6.7 restricts an implicit key to a single line,
// so the entry has no key to take either
TEST(yaml_root_plain_scalar_followed_by_a_line_opening_with_a_colon) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a\n: b\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected content after document");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_tag_followed_by_a_comma_in_block_context) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: !!str, b\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid character after tag in block context");
  } catch (...) {
    FAIL();
  }
}

// Section 6.9.1 closes a verbatim tag with an angle bracket, which a line
// break can never stand in for
TEST(yaml_verbatim_tag_cut_off_by_a_lone_carriage_return) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: !<tag\r"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unterminated verbatim tag");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_block_scalar_line_of_a_tabulation_and_a_space) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"|\n\t \nx\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Tab characters cannot be used for indentation");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_flow_sequence_entry_separator_at_the_end_of_input) {
  sourcemeta::core::YAMLRoundTrip roundtrip;
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"[a,"}, roundtrip)};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected token");
  } catch (...) {
    FAIL();
  }
}

// Production 129 takes the flow indicators out of `ns-plain-safe-in`, so a
// plain scalar ends where one stands, leaving the collection with two entries
// and no separator between them
TEST(yaml_plain_scalar_followed_by_a_flow_mapping_inside_a_flow_sequence) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"[a{}]"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing comma in flow sequence");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_directive_at_the_end_of_input) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"%YAML 1.2"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Empty YAML document");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_plain_scalar_followed_by_a_flow_sequence_inside_a_flow_sequence) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"[a[]]"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing comma in flow sequence");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_unterminated_flow_mapping_after_a_value) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"{a: b"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing comma between flow mapping entries");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_unterminated_flow_sequence_after_an_entry) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"[a"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing comma in flow sequence");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_unterminated_flow_mapping_after_an_explicit_key) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"{? a"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected end of input in flow mapping");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_unterminated_flow_sequence_after_an_explicit_key) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"[? a"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected token");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_version_directive_followed_by_content_that_is_no_comment) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"%YAML 1.2 x\n--- a\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid content in %YAML directive");
  } catch (...) {
    FAIL();
  }
}

// Section 6.8.1: "It is an error to specify more than one YAML directive for
// the same document"
TEST(yaml_version_directive_given_twice_for_one_document) {
  try {
    [[maybe_unused]] const auto result{sourcemeta::core::parse_yaml(
        std::string{"%YAML 1.2\n%YAML 1.2\n--- a\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Duplicate %YAML directive");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_flow_sequence_opening_with_a_comma) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"[,a]"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Leading comma in flow sequence");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_flow_sequence_with_two_commas_in_a_row) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"[a,,b]"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Empty entry in flow sequence");
  } catch (...) {
    FAIL();
  }
}

// Section 8.2 indents a flow collection nested in a block context past that
// context, which an entry standing at the block's own level does not
TEST(yaml_flow_sequence_entry_at_the_parent_block_level) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: [\n1 ]\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Insufficient indentation in flow collection");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_flow_sequence_left_unterminated_at_the_end_of_a_line) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: [\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(
        error.what(),
        "Flow content indented less than or equal to parent block level");
  } catch (...) {
    FAIL();
  }
}

// Section 6.7: "Implicit keys are restricted to a single line"
TEST(yaml_multiline_quoted_key_of_a_pair_inside_a_flow_sequence) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"['a\n b': 1]"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Multi-line implicit mapping key");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_flow_pair_value_indicator_on_the_line_after_its_key) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"['a'\n: 1]"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Implicit key and value indicator on "
                               "different lines in flow context");
  } catch (...) {
    FAIL();
  }
}

// Section 8.2.2 indents the content of a block mapping entry past the entry's
// own key, so a nested key sharing that key's line has nowhere to stand
TEST(yaml_nested_implicit_key_on_the_line_of_its_parent_key) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: b: 1\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Implicit mapping key in block value on "
                               "same line as parent key");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_node_property_before_an_implicit_key_on_the_document_start_line) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"--- &x a: 1\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Node properties before implicit mapping key on "
                               "document start line");
  } catch (...) {
    FAIL();
  }
}

// Section 3.2.2.2 gives a node at most one anchor
TEST(yaml_two_anchors_on_one_scalar) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: &x &y 1\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Multiple anchors on a scalar node");
  } catch (...) {
    FAIL();
  }
}

// Section 7.1 makes an alias stand for a node already anchored elsewhere, so
// it is no node of its own to anchor
TEST(yaml_anchor_on_an_alias_node) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: &x *y\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Cannot anchor an alias node");
  } catch (...) {
    FAIL();
  }
}

// Production 187 writes `l+block-sequence(n) ::= ( s-indent(n+m)
// c-l-block-seq-entry(n+m) )+`, so every entry of one sequence stands at the
// same indentation
TEST(yaml_sequence_entry_indented_one_space_past_its_siblings) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"- 'a'\n - b\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Wrong indentation for sequence entry");
  } catch (...) {
    FAIL();
  }
}

// Section 8.2.2 reaches the block collection of an entry through
// `s-l-comments`, which ends on a line break, so a nested mapping opens on a
// line of its own
TEST(yaml_explicit_key_on_the_line_of_the_value_indicator_before_it) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: ? b\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(
        error.what(),
        "Explicit mapping key in block value on same line as parent key");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_explicit_key_indented_past_an_entry_that_already_has_a_value) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: 1\n  ? b\n  : c\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected content after document");
  } catch (...) {
    FAIL();
  }
}

// Production 193 has the value of an implicit entry follow a literal colon, so
// an alias with none after it forms no entry any more than a plain scalar with
// none after it does, and what forms no entry is content the document cannot
// account for
TEST(yaml_alias_as_a_mapping_key_without_a_value_is_rejected) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: &k key\n*k\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected content after document");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_alias_naming_an_anchor_whose_name_takes_in_a_colon) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: &k key\n*k: 1\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLUnknownAnchorError &error) {
    EXPECT_STREQ(error.what(), "YAML alias references undefined anchor");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_version_directive_after_a_mapping_that_no_marker_closed) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: 1\n%YAML 1.2\n--- b\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Directive not allowed without preceding "
                               "document end marker");
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_tag_directive_after_a_sequence_that_no_marker_closed) {
  try {
    [[maybe_unused]] const auto result{sourcemeta::core::parse_yaml(
        std::string{"- a\n%TAG !e! tag:x\n--- b\n"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Directive not allowed without preceding "
                               "document end marker");
  } catch (...) {
    FAIL();
  }
}

// That section has an alias stand for the value of its anchor wherever it is
// written, so one standing as a plain key collides with the member name it
// resolves to just as an explicit one does
TEST(plain_key_alias_duplicate_is_rejected) {
  const std::string input{"x: &a dup\ndup: 1\n*a : 2\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "dup");
  } catch (...) {
    FAIL();
  }
}

TEST(read_yaml_reporting_a_file_that_does_not_parse) {
  bool more_documents{false};
  try {
    const auto result{sourcemeta::core::read_yaml(
        std::filesystem::path{STUBS_PATH} / "invalid.yaml", more_documents)};
    FAIL();
  } catch (const sourcemeta::core::YAMLFileParseError &error) {
    EXPECT_EQ(error.path(), std::filesystem::path{STUBS_PATH} / "invalid.yaml");
  }
}

TEST(block_explicit_key_indicator_alone_twice) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"?\n?"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "");
  } catch (...) {
    FAIL();
  }
}

TEST(block_plain_key_then_explicit_key_indicator_alone_twice) {
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::parse_yaml(std::string{"a: 1\n?\n?"})};
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "");
  } catch (...) {
    FAIL();
  }
}

// Production 142 reads the indicator as one only where no non-space character
// follows it
TEST(flow_mapping_explicit_key_indicator_followed_by_content) {
  const std::string input{"{?}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid plain scalar start in flow context");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 2);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_mapping_duplicate_explicit_key) {
  const std::string input{"{? a : 1, a: 2}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "a");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 11);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_mapping_alias_key_referencing_no_anchor) {
  const std::string input{"{*x : b}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLUnknownAnchorError &error) {
    EXPECT_EQ(error.anchor(), "x");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 2);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_mapping_explicit_collection_key) {
  const std::string input{"{? [a] : b}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 4);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_mapping_sequence_key) {
  const std::string input{"{[a]: b}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 2);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_mapping_mapping_key) {
  const std::string input{"{{a: 1}:b}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 2);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_sequence_pair_sequence_key) {
  const std::string input{"[ [a]: b ]"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

// Example 7.21 writes the value of a JSON-like key next to the indicator, which
// production 155 lets be a flow collection
TEST(flow_sequence_pair_mapping_key_with_an_adjacent_value) {
  const std::string input{"[ {JSON: like}:adjacent ]"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

// Production 154 holds the key of a pair carrying no indicator to a plain
// scalar or an alias, so a pair cannot itself be one
TEST(flow_sequence_pair_key_cannot_be_another_pair) {
  const std::string input{"[a: b: c]"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing comma in flow sequence");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 6);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_sequence_entry_without_a_separator) {
  const std::string input{"[a [b]]"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing comma in flow sequence");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 4);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_tagged_empty_key_twice) {
  const std::string input{"-\n  !!null : a\n  !!null : b"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "");
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 production 191 writes the value of an explicit entry with the
// indicator standing at the indentation of the mapping, which follows a line
// break, so an indicator beside the key leaves that key reading as the compact
// mapping of production 195
TEST(block_mapping_explicit_compact_mapping_key) {
  const std::string input{"? earth: blue\n: moon"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_explicit_compact_mapping_key_without_a_value) {
  const std::string input{"? a : b"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_explicit_compact_mapping_key_after_another_entry) {
  const std::string input{"a: 1\n? b: c\n: d"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 production 201 reads the block sequence that is the value of a
// mapping entry one indentation level further out than the mapping, so a
// sequence standing at the indentation of the mapping is the key it opens
TEST(block_mapping_explicit_sequence_key_at_the_indentation) {
  const std::string input{"?\n- a\n- b\n:\n- c\n- d"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_explicit_sequence_key_indented) {
  const std::string input{"? - a\n: b"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_explicit_sequence_key_after_another_entry) {
  const std::string input{"a: 1\n? - x\n: y"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

// A flow collection standing before a value indicator is the implicit key of a
// block mapping entry, which RFC 8259 Section 4 cannot hold as a member name
TEST(block_mapping_flow_sequence_key) {
  const std::string input{"[flow]: block"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 7);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_flow_mapping_key) {
  const std::string input{"{a: 1}: block"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 7);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_flow_sequence_key_in_a_sequence_entry) {
  const std::string input{"- [flow]: block"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 9);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_flow_sequence_key_as_a_value) {
  const std::string input{"k:\n  [flow]: block"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 9);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_two_entries_with_no_key) {
  const std::string input{": a\n: b"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_compact_mapping_as_an_explicit_key_in_a_sequence_entry) {
  const std::string input{"- sun: yellow\n- ? earth: blue\n  : moon: white"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 5);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_anchored_flow_sequence_key) {
  const std::string input{"---\n&mapping\n&key [ &item a, b, c ]: value"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 3);
    EXPECT_EQ(error.column(), 23);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_mapping_anchored_collection_key) {
  const std::string input{"{ &a [a, &b b]: *b, *a : [c, *b, d]}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 6);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_mapping_collection_key_beside_a_scalar_entry) {
  const std::string input{"{a: [b, c], [d, e]: f}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 13);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_sequence_nested_collection_keys) {
  const std::string input{"---\n[\n  [ a, [ [[b,c]]: d, e]]: 23\n]"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
    EXPECT_EQ(error.line(), 3);
    EXPECT_EQ(error.column(), 10);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_mapping_input_ending_after_the_explicit_key_indicator) {
  const std::string input{"{? "};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected end of input in flow mapping");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_mapping_explicit_key_indicator_twice) {
  const std::string input{"{? ? a : b}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Expected scalar key in mapping");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 4);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_sequence_input_ending_after_an_explicit_key_property) {
  const std::string input{"[? &x"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected end after explicit key in flow");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 0);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_value_indicator_alone_twice) {
  const std::string input{":\n: a"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLDuplicateKeyError &error) {
    EXPECT_EQ(error.key(), "");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 production 197 reads the flow node that is the value of a mapping
// entry indented further than the mapping, so a scalar standing at that
// indentation is not that value, whether node properties precede it or not
TEST(block_mapping_dedented_scalar_after_an_anchored_empty_value) {
  const std::string input{"a: &x\nb"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected content after document");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_dedented_scalar_after_a_tagged_empty_value) {
  const std::string input{"a: !!str\nb"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected content after document");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_dedented_scalar_after_an_anchored_empty_nested_value) {
  const std::string input{"x:\n  a: &v\n  b"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected content after document");
    EXPECT_EQ(error.line(), 3);
    EXPECT_EQ(error.column(), 3);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_dedented_scalar_after_an_empty_value) {
  const std::string input{"a:\nb"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected content after document");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(block_sequence_after_a_tag_on_the_same_line) {
  const std::string input{"!!str - a"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(),
                 "Block sequence after node property must start on a new line");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 7);
  } catch (...) {
    FAIL();
  }
}

TEST(block_sequence_after_an_anchor_on_the_same_line) {
  const std::string input{"&x - a"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(),
                 "Block sequence after node property must start on a new line");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 4);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 6.7 restricts an implicit key to a single line, and the
// document start marker holds the first line, so node properties there leave
// no room for the key they would decorate
TEST(block_mapping_tagged_implicit_key_on_the_document_start_line) {
  const std::string input{"--- !!str a: 1"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Node properties before implicit mapping key on "
                               "document start line");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 11);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_continuation_after_the_document_start_line) {
  const std::string input{"--- a: 1\nb: 2"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(),
                 "Block mapping continuation after document start line");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 1);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 production 159 writes an alias node on its own, as what it stands
// for carries its own properties, so an anchor standing before one names
// nothing. The four walks that read a key have to answer for that alike
TEST(flow_mapping_anchored_alias_key) {
  const std::string input{"{a: &v 1, &x *v : b}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Cannot anchor an alias node");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 14);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_mapping_anchored_explicit_alias_key) {
  const std::string input{"{a: &v 1, ? &x *v : b}"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Cannot anchor an alias node");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 16);
  } catch (...) {
    FAIL();
  }
}

TEST(flow_sequence_anchored_explicit_alias_pair_key) {
  const std::string input{"[&v q, ? &x *v : b]"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Cannot anchor an alias node");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 13);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_anchored_alias_key) {
  const std::string input{"? &v q\n: 1\n&x *v : b"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Cannot anchor an alias node");
    EXPECT_EQ(error.line(), 3);
    EXPECT_EQ(error.column(), 4);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_anchored_explicit_alias_key_after_another_entry) {
  const std::string input{"a: &v 1\n? &x *v\n: b"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Cannot anchor an alias node");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 6);
  } catch (...) {
    FAIL();
  }
}

TEST(block_mapping_anchored_alias_key_after_another_entry) {
  const std::string input{"a: &v 1\n&x *v : b"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Cannot anchor an alias node");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 4);
  } catch (...) {
    FAIL();
  }
}

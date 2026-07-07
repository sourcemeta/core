#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/yaml.h>

#include <sstream> // std::istringstream
#include <string>  // std::string

TEST(deeply_nested_flow_is_rejected) {
  const std::string input{std::string(2000, '[') + std::string(2000, ']')};
  try {
    const auto result{sourcemeta::core::parse_yaml(input)};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Maximum nesting depth exceeded");
  }
}

TEST(scalar_1) {
  const std::string input{"1"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{1};
  EXPECT_EQ(result, expected);
}

TEST(scalar_2) {
  const std::string input{"   1    "};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{1};
  EXPECT_EQ(result, expected);
}

TEST(object_1) {
  const std::string input{"hello: world\nfoo: 1\nbar: true"};

  const auto result{sourcemeta::core::parse_yaml(input)};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "hello": "world",
    "foo": 1,
    "bar": true
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(object_2) {
  const std::string input{"foo: >\n  bar\n  baz"};

  const auto result{sourcemeta::core::parse_yaml(input)};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar baz"
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(object_3) {
  const std::string input{"version: \"1.29.2\""};

  const auto result{sourcemeta::core::parse_yaml(input)};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "version": "1.29.2"
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(object_4) {
  const std::string input{"version: \'1.29.2\'"};

  const auto result{sourcemeta::core::parse_yaml(input)};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "version": "1.29.2"
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(object_5) {
  const std::string input{"version: 1.29.2"};

  const auto result{sourcemeta::core::parse_yaml(input)};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "version": "1.29.2"
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(object_6) {
  const std::string input{"version: v1.29.2"};

  const auto result{sourcemeta::core::parse_yaml(input)};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "version": "v1.29.2"
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(object_7) {
  const std::string input{"version: 1.29v"};

  const auto result{sourcemeta::core::parse_yaml(input)};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "version": "1.29v"
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(array_1) {
  const std::string input{"- foo\n- true"};

  const auto result{sourcemeta::core::parse_yaml(input)};

  const sourcemeta::core::JSON expected =
      sourcemeta::core::parse_json(R"JSON([ "foo", true ])JSON");

  EXPECT_EQ(result, expected);
}

TEST(empty) {
  const std::string input{""};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
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
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 5);
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

TEST(stub_test_1) {
  const auto result{sourcemeta::core::read_yaml(
      std::filesystem::path{STUBS_PATH} / "test_1.yaml")};
  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "baz": 2
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(yaml_or_json_stub_test_1) {
  const auto result{sourcemeta::core::read_yaml_or_json(
      std::filesystem::path{STUBS_PATH} / "test_1.yaml")};
  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "baz": 2
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(yaml_or_json_stub_test_2) {
  const auto result{sourcemeta::core::read_yaml_or_json(
      std::filesystem::path{STUBS_PATH} / "test_1.json")};
  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "baz": 2
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(file_not_exists) {
  try {
    sourcemeta::core::read_yaml(std::filesystem::path{STUBS_PATH} /
                                "not_exists.yaml");
    FAIL();
  } catch (const sourcemeta::core::IOFileNotFoundError &error) {
    EXPECT_EQ(error.path(),
              std::filesystem::path{STUBS_PATH} / "not_exists.yaml");
  }
}

TEST(yaml_or_json_file_not_exists) {
  try {
    sourcemeta::core::read_yaml_or_json(std::filesystem::path{STUBS_PATH} /
                                        "not_exists.yaml");
    FAIL();
  } catch (const sourcemeta::core::IOFileNotFoundError &error) {
    EXPECT_EQ(error.path(),
              std::filesystem::path{STUBS_PATH} / "not_exists.yaml");
  }
}

TEST(read_yaml_invalid_carries_path) {
  try {
    sourcemeta::core::read_yaml(std::filesystem::path{STUBS_PATH} /
                                "invalid.yaml");
    FAIL();
  } catch (const sourcemeta::core::YAMLFileParseError &error) {
    EXPECT_EQ(error.path(), std::filesystem::path{STUBS_PATH} / "invalid.yaml");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 15);
  } catch (...) {
    FAIL();
  }
}

TEST(istringstream) {
  std::istringstream stream{"hello: world\nfoo: 1\nbar: true"};
  const auto result{sourcemeta::core::parse_yaml(stream)};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "hello": "world",
    "foo": 1,
    "bar": true
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(multi_document_unix_line_endings) {
  std::istringstream stream{"---\nfoo\n---\nbar\n---\nbaz"};

  const auto doc1{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(doc1, sourcemeta::core::JSON{"foo"});

  const auto doc2{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(doc2, sourcemeta::core::JSON{"bar"});

  const auto doc3{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(doc3, sourcemeta::core::JSON{"baz"});

  EXPECT_EQ(stream.peek(), EOF);
}

TEST(multi_document_windows_line_endings) {
  std::istringstream stream{"---\r\nfoo\r\n---\r\nbar\r\n---\r\nbaz"};

  const auto doc1{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(doc1, sourcemeta::core::JSON{"foo"});

  const auto doc2{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(doc2, sourcemeta::core::JSON{"bar"});

  const auto doc3{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(doc3, sourcemeta::core::JSON{"baz"});

  EXPECT_EQ(stream.peek(), EOF);
}

TEST(decimal_large_integer) {
  const std::string input{"123456789012345678901234567890"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::Decimal{"123456789012345678901234567890"}};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "123456789012345678901234567890");
}

TEST(decimal_high_precision_real) {
  const std::string input{"3.141592653589793238462643383279"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_decimal());
}

TEST(decimal_exponential_notation) {
  const std::string input{"1.234567890123456789012345678901e50"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_decimal());
}

TEST(decimal_in_object) {
  const std::string input{"large: 999999999999999999999999999999\n"
                          "precise: 2.718281828459045235360287471352"};
  const auto result{sourcemeta::core::parse_yaml(input)};

  EXPECT_TRUE(result.is_object());
  EXPECT_TRUE(result.defines("large"));
  EXPECT_TRUE(result.defines("precise"));
  EXPECT_TRUE(result.at("large").is_decimal());
  EXPECT_TRUE(result.at("precise").is_decimal());
  EXPECT_EQ(result.at("large").to_decimal().to_string(),
            "999999999999999999999999999999");
}

TEST(decimal_in_array) {
  const std::string input{"- 123456789012345678901234567890\n"
                          "- 9.87654321098765432109876543210e100"};
  const auto result{sourcemeta::core::parse_yaml(input)};

  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_decimal());
  EXPECT_TRUE(result.at(1).is_decimal());
}

TEST(integer_with_leading_plus) {
  const std::string input{"+42"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{42};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_integer());
}

TEST(real_with_leading_plus) {
  const std::string input{"+1.5"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{1.5};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_real());
}

TEST(exponent_with_leading_plus) {
  const std::string input{"+2E0"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{sourcemeta::core::Decimal{"2E0"}};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_decimal());
}

TEST(incomplete_exponent_is_a_string) {
  // YAML 1.2.2 core schema requires a digit after the exponent, so "1e" is a
  // plain string rather than a malformed number
  const std::string input{"1e"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{"1e"};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_string());
}

TEST(exponent_sign_without_digit_is_a_string) {
  const std::string input{"1e+"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{"1e+"};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_string());
}

TEST(real_long_small_decimal) {
  const std::string input{
      "0.00000000000000000000000000000000000000000000000000000000000000000000"
      "1"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_real());
  EXPECT_EQ(result.to_real(), 1e-69);
}

TEST(real_subnormal_decimal) {
  const std::string input{"0." + std::string(309, '0') + "5"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_real());
  EXPECT_EQ(result.to_real(), 5e-310);
}

TEST(decimal_underflowing_decimal) {
  const std::string input{"0." + std::string(400, '0') + "1"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal(), sourcemeta::core::Decimal{input});
}

TEST(scientific_constant_planck) {
  const std::string input{"6.62607E-34"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal(), sourcemeta::core::Decimal{"6.62607E-34"});
}

TEST(scientific_constant_elementary_charge) {
  const std::string input{"1.60218E-19"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal(), sourcemeta::core::Decimal{"1.60218E-19"});
}

TEST(scientific_constant_boltzmann) {
  const std::string input{"1.38065E-23"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal(), sourcemeta::core::Decimal{"1.38065E-23"});
}

TEST(yaml_or_json_custom_extension_yaml_content) {
  const auto result{sourcemeta::core::read_yaml_or_json(
      std::filesystem::path{STUBS_PATH} / "test_2.custom")};
  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "baz": 2
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(yaml_or_json_custom_extension_json_content) {
  const auto result{sourcemeta::core::read_yaml_or_json(
      std::filesystem::path{STUBS_PATH} / "test_3.custom")};
  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "baz": 2
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(yaml_or_json_no_extension_yaml_content) {
  const auto result{sourcemeta::core::read_yaml_or_json(
      std::filesystem::path{STUBS_PATH} / "test_no_extension_yaml")};
  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "baz": 2
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(yaml_or_json_no_extension_json_content) {
  const auto result{sourcemeta::core::read_yaml_or_json(
      std::filesystem::path{STUBS_PATH} / "test_no_extension_json")};
  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "baz": 2
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(yaml_or_json_invalid_json_throws_json_error) {
  try {
    sourcemeta::core::read_yaml_or_json(std::filesystem::path{STUBS_PATH} /
                                        "invalid.json");
    FAIL();
  } catch (const sourcemeta::core::JSONParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 3);
  }
}

TEST(yaml_or_json_invalid_yaml_throws_yaml_error) {
  try {
    sourcemeta::core::read_yaml_or_json(std::filesystem::path{STUBS_PATH} /
                                        "invalid.yaml");
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 15);
  } catch (...) {
    FAIL();
  }
}

TEST(verbatim_tag_bool) {
  const std::string input{"!<tag:yaml.org,2002:bool> true"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_boolean());
  EXPECT_EQ(result, sourcemeta::core::JSON{true});
}

TEST(verbatim_tag_int) {
  const std::string input{"!<tag:yaml.org,2002:int> 42"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_integer());
  EXPECT_EQ(result, sourcemeta::core::JSON{42});
}

TEST(verbatim_tag_null) {
  const std::string input{"!<tag:yaml.org,2002:null> ~"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_null());
}

TEST(verbatim_tag_str) {
  const std::string input{"!<tag:yaml.org,2002:str> true"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result, sourcemeta::core::JSON{"true"});
}

TEST(verbatim_tag_float) {
  const std::string input{"!<tag:yaml.org,2002:float> 3.14"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_real());
  EXPECT_DOUBLE_EQ(result.to_real(), 3.14);
}

TEST(plain_scalar_triple_dash_value) {
  const std::string input{"key: ---"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("key"), sourcemeta::core::JSON{"---"});
}

TEST(plain_scalar_triple_dot_value) {
  const std::string input{"key: ..."};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("key"), sourcemeta::core::JSON{"..."});
}

TEST(invalid_hex_escape) {
  const std::string input{"\"\\xGG\""};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(invalid_unicode_escape_4) {
  const std::string input{"\"\\uZZZZ\""};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(invalid_unicode_escape_8) {
  const std::string input{"\"\\UZZZZZZZZ\""};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(valid_bmp_unicode_escape) {
  const std::string input{"\"\\u00e9\""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{"\xC3\xA9"};
  EXPECT_EQ(result, expected);
}

TEST(valid_astral_unicode_escape) {
  const std::string input{"\"\\U0001F600\""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{"\xF0\x9F\x98\x80"};
  EXPECT_EQ(result, expected);
}

TEST(surrogate_unicode_escape_is_rejected) {
  // YAML 1.2.2 Section 5.7: a lone surrogate is not a Unicode scalar value
  const std::string input{"\"\\uD800\""};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(out_of_range_unicode_escape_is_rejected) {
  const std::string input{"\"\\UFFFFFFFF\""};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
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
    EXPECT_EQ(error.line(), 7);
    EXPECT_EQ(error.column(), 37);
  } catch (...) {
    FAIL();
  }
}

// A !!float tag whose value is outside the 64-bit integer range must not
// invoke undefined behavior by casting an out-of-range double to an integer.
TEST(float_tag_out_of_integer_range) {
  const std::string input{"!!float 1e300"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_real());
  EXPECT_EQ(result.to_real(), 1e300);
}

TEST(float_tag_just_above_integer_range) {
  const std::string input{"!!float 1e19"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_real());
  EXPECT_EQ(result.to_real(), 1e19);
}

// YAML 1.2.2 Section 8.2.1: an empty block sequence entry is a null node rather
// than being dropped from the sequence.
TEST(bare_block_sequence_entry_is_null) {
  const std::string input{"-"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_null());
}

TEST(bare_block_sequence_entry_before_document_end_is_null) {
  const std::string input{"-\n...\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_null());
}

// YAML 1.2.2 Section 6.8: an unknown directive name is reserved and ignored, so
// a document introduced by one is parsed normally.
TEST(unknown_directive_is_ignored) {
  const std::string input{"%YAMLL 1.1\n---\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_null());
}

TEST(directive_name_prefix_of_yaml_is_reserved) {
  const std::string input{"%YAMLX 9.9\n--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"value"});
}

// YAML 1.2.2 Section 6.8.1: the version parameter is a major and minor decimal
// number, so a comment sign without a preceding separation space is invalid.
TEST(yaml_directive_comment_without_separation_is_rejected) {
  const std::string input{"%YAML 1.1#foo\n---\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
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
    EXPECT_EQ(error.line(), 1);
  } catch (...) {
    FAIL();
  }
}

TEST(yaml_directive_with_comment_is_accepted) {
  const std::string input{"%YAML 1.2 # a comment\n--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"value"});
}

// YAML 1.2.2 Section 6.1: a tab that follows a space is separation rather than
// indentation, so it does not make the line invalid.
TEST(tab_after_space_separation_is_not_indentation) {
  const std::string input{" \tvalue"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"value"});
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
  } catch (...) {
    FAIL();
  }
}

// A tab after a sequence entry indicator is valid separation when the entry
// value is a scalar rather than a nested block collection.
TEST(tab_before_scalar_sequence_entry_is_accepted) {
  const std::string input{"-\tvalue\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0), sourcemeta::core::JSON{"value"});
}

TEST(tab_before_negative_number_sequence_entry_is_accepted) {
  const std::string input{"-\t-1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0), sourcemeta::core::JSON{-1});
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
  } catch (...) {
    FAIL();
  }
}

TEST(tab_before_scalar_mapping_value_is_accepted) {
  const std::string input{"foo:\n \tbar\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("foo"), sourcemeta::core::JSON{"bar"});
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
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 7.3.1: an escaped tab in a double-quoted scalar is
// content, so it survives the folding that strips literal trailing white space.
TEST(escaped_tab_survives_double_quoted_folding) {
  const std::string input{"\"1 trailing\\t\n  \ttab\""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"1 trailing\t tab"});
}

TEST(escaped_space_survives_double_quoted_folding) {
  const std::string input{"\"a\\ \n  b\""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a  b"});
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
  } catch (...) {
    FAIL();
  }
}

// A tab that begins a folded line of a top-level quoted scalar is folding white
// space rather than indentation, so it is valid.
TEST(tab_led_fold_in_root_double_quoted_scalar_is_accepted) {
  const std::string input{
      "\" 1st non-empty\n\n 2nd non-empty \n\t3rd non-empty \""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{
                        " 1st non-empty\n2nd non-empty 3rd non-empty "});
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
  } catch (...) {
    FAIL();
  }
}

// A tab that follows a space on an empty block scalar line is separation rather
// than indentation, so it is valid.
TEST(tab_after_space_on_empty_block_scalar_line_is_accepted) {
  const std::string input{"foo: |\n \t\nbar: 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("bar"), sourcemeta::core::JSON{1});
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
  } catch (...) {
    FAIL();
  }
}

TEST(flow_mapping_content_indented_past_block_is_accepted) {
  const std::string input{"k: {\n k\n :\n v\n }\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "k": { "k": "v" } })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(flow_sequence_content_at_block_indent_is_rejected) {
  const std::string input{"flow: [a,\nb,\nc]\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 2);
  } catch (...) {
    FAIL();
  }
}

TEST(top_level_flow_sequence_at_column_one_is_accepted) {
  const std::string input{"[\nfoo: bar\n]\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON([ { "foo": "bar" } ])JSON")};
  EXPECT_EQ(result, expected);
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
  } catch (...) {
    FAIL();
  }
}

// A tab after enough spaces to clear the flow content indentation is separation
// rather than indentation, so it is valid.
TEST(tab_after_flow_indentation_spaces_is_accepted) {
  const std::string input{"root:\n Flow: [\n  a,\n  \tb ]\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{sourcemeta::core::parse_json(
      R"JSON({ "root": { "Flow": [ "a", "b" ] } })JSON")};
  EXPECT_EQ(result, expected);
}

// A tab on a wholly blank line inside a flow collection is not indentation and
// does not invalidate the following properly indented content.
TEST(tab_on_blank_flow_line_is_accepted) {
  const std::string input{"- [\n \t\n foo\n ]\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON([ [ "foo" ] ])JSON")};
  EXPECT_EQ(result, expected);
}

// YAML 1.2.2 Section 8.1.1.1: in a literal block scalar, the spaces of a
// trailing line that are beyond the content indentation are content, even when
// that line is the last one and ends the input without a line break.
TEST(literal_block_scalar_trailing_more_indented_line) {
  const std::string input{"foo: |\n  x\n   "};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("foo"), sourcemeta::core::JSON{"x\n \n"});
}

// The %TAG directive is recognized by name, not by a prefix match, so a valid
// tag directive is still processed after tightening the directive-name parsing.
TEST(tag_directive_is_accepted) {
  const std::string input{"%TAG !e! tag:example.com,2000:app/\n--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"value"});
}

TEST(directive_name_prefix_of_tag_is_reserved) {
  const std::string input{"%TAGX whatever\n--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"value"});
}

// YAML 1.2.2 Section 6.8.1: the %YAML directive requires a version parameter.
TEST(yaml_directive_without_version_is_rejected) {
  const std::string input{"%YAML\n---\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
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
    EXPECT_EQ(error.line(), 1);
  } catch (...) {
    FAIL();
  }
}

// A lower minor version is accepted and processed with the 1.2 machinery.
TEST(yaml_directive_older_minor_version_is_accepted) {
  const std::string input{"%YAML 1.1\n--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"value"});
}

// The escape-protected span also covers the hex and Unicode escape forms, so a
// tab written as \x09 at a line end survives folding just like \t does.
TEST(hex_escape_whitespace_survives_double_quoted_folding) {
  const std::string input{"\"a\\x09\n  b\""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a\t b"});
}

TEST(unicode_escape_whitespace_survives_double_quoted_folding) {
  const std::string input{"\"a\\u0009\n  b\""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a\t b"});
}

// The flow continuation-indent check applies through nesting, so a properly
// indented nested flow collection is not falsely rejected.
TEST(nested_flow_collection_indented_past_block_is_accepted) {
  const std::string input{"key: [ [1],\n       [2] ]\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "key": [ [1], [2] ] })JSON")};
  EXPECT_EQ(result, expected);
}

// An empty block sequence entry that sits immediately before a document-end or
// document-start marker is a null entry, in the loop path as well as the first.
TEST(empty_sequence_entry_before_document_start_is_null) {
  std::istringstream stream{"- a\n-\n---\nb\n"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON([ "a", null ])JSON")};
  EXPECT_EQ(first, expected);
  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"b"});
}

// YAML 1.2.2 Section 8.1.1.2: keep chomping of an empty folded block scalar
// header preserves the header-ending line break, just as the literal style
// does.
TEST(folded_block_scalar_keep_empty_preserves_newline) {
  const std::string input{"- >+\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0), sourcemeta::core::JSON{"\n"});
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
    // Expected: the continuation reaches only the block indentation with spaces
  } catch (...) {
    FAIL();
  }
}

// The same continuation indented one space past the block is accepted, with the
// tab treated as separation.
TEST(tab_after_sufficient_spaces_flow_scalar_continuation_is_accepted) {
  const std::string input{"a:\n b: \"x\n  \ty\"\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("a").at("b"), sourcemeta::core::JSON{"x y"});
}

// YAML 1.2.2 Section 6.1: a tab at the start of a line is invalid indentation
// regardless of a preceding blank indented line. The per-line indentation state
// must reset on a line break so the blank line's spaces do not carry over.
TEST(tab_line_start_after_blank_indented_line_is_rejected) {
  const std::string input{"a:\n   \n\tb\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &) {
    // Expected: the tab on the final line is line-start indentation
  } catch (...) {
    FAIL();
  }
}

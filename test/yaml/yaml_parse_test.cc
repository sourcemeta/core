#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/yaml.h>

#include <sstream>

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

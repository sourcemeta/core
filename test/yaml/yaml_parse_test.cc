#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/yaml.h>

#include <fstream>  // std::ifstream
#include <ios>      // std::ios::binary
#include <iostream> // std::cerr
#include <sstream>  // std::istringstream
#include <string>   // std::string

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

// YAML 1.2.2 Section 6.8.2: a tag directive applies to the document that
// follows it, so a handle declared before a later document resolves within that
// document rather than being discarded at its document start marker.
TEST(read_yaml_resolves_a_tag_directive_before_a_later_document) {
  const auto result{sourcemeta::core::read_yaml(
      std::filesystem::path{STUBS_PATH} / "multi_document_tag_directive.yaml")};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "first": "document" })JSON")};
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

TEST(file_stream_single_document_exhausts_the_stream) {
  auto stream{sourcemeta::core::read_file(std::filesystem::path{STUBS_PATH} /
                                          "test_1.yaml")};

  const auto result{sourcemeta::core::parse_yaml(stream)};
  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "baz": 2
  })JSON");

  EXPECT_EQ(result, expected);
  EXPECT_TRUE(stream.good());
  EXPECT_EQ(stream.peek(), EOF);
}

TEST(file_stream_multi_document_unix_line_endings) {
  auto stream{sourcemeta::core::read_file(std::filesystem::path{STUBS_PATH} /
                                          "multi_document_lf.yaml")};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first, sourcemeta::core::JSON{"foo"});
  EXPECT_TRUE(stream.good());

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"bar"});
  EXPECT_TRUE(stream.good());

  const auto third{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(third, sourcemeta::core::JSON{"baz"});
  EXPECT_TRUE(stream.good());

  EXPECT_EQ(stream.peek(), EOF);
}

TEST(file_stream_multi_document_windows_line_endings) {
  auto stream{sourcemeta::core::read_file(std::filesystem::path{STUBS_PATH} /
                                          "multi_document_crlf.yaml")};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first, sourcemeta::core::JSON{"foo"});
  EXPECT_TRUE(stream.good());

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"bar"});
  EXPECT_TRUE(stream.good());

  const auto third{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(third, sourcemeta::core::JSON{"baz"});
  EXPECT_TRUE(stream.good());

  EXPECT_EQ(stream.peek(), EOF);
}

TEST(file_stream_multi_document_byte_order_mark) {
  auto stream{sourcemeta::core::read_file(std::filesystem::path{STUBS_PATH} /
                                          "multi_document_bom.yaml")};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first, sourcemeta::core::JSON{"foo"});
  EXPECT_TRUE(stream.good());

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"bar"});
  EXPECT_TRUE(stream.good());

  EXPECT_EQ(stream.peek(), EOF);
}

TEST(file_stream_multi_document_objects) {
  auto stream{sourcemeta::core::read_file(std::filesystem::path{STUBS_PATH} /
                                          "multi_document_objects.yaml")};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first, sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON"));

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::parse_json(R"JSON({ "bar": 2 })JSON"));

  const auto third{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(third, sourcemeta::core::parse_json(R"JSON({ "baz": 3 })JSON"));

  EXPECT_EQ(stream.peek(), EOF);
}

TEST(file_stream_multi_document_blank_lines) {
  auto stream{sourcemeta::core::read_file(std::filesystem::path{STUBS_PATH} /
                                          "multi_document_blank_lines.yaml")};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first, sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON"));

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::parse_json(R"JSON({ "bar": 2 })JSON"));

  EXPECT_EQ(stream.peek(), EOF);
}

TEST(file_stream_multi_document_tag_directive) {
  auto stream{sourcemeta::core::read_file(std::filesystem::path{STUBS_PATH} /
                                          "multi_document_tag_directive.yaml")};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first,
            sourcemeta::core::parse_json(R"JSON({ "first": "document" })JSON"));

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"bar"});

  EXPECT_EQ(stream.peek(), EOF);
}

TEST(file_stream_leaves_the_remaining_documents_unread) {
  auto stream{sourcemeta::core::read_file(std::filesystem::path{STUBS_PATH} /
                                          "multi_document_lf.yaml")};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first, sourcemeta::core::JSON{"foo"});
  EXPECT_EQ(stream.tellg(), std::streampos{8});
  EXPECT_EQ(sourcemeta::core::read_to_string(stream), "---\nbar\n---\nbaz\n");
}

TEST(binary_file_stream_multi_document_windows_line_endings) {
  std::ifstream stream{std::filesystem::path{STUBS_PATH} /
                           "multi_document_crlf.yaml",
                       std::ios::binary};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first, sourcemeta::core::JSON{"foo"});
  EXPECT_TRUE(stream.good());

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"bar"});
  EXPECT_TRUE(stream.good());

  const auto third{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(third, sourcemeta::core::JSON{"baz"});
  EXPECT_TRUE(stream.good());

  EXPECT_EQ(stream.peek(), EOF);
}

TEST(binary_file_stream_multi_document_unix_line_endings) {
  std::ifstream stream{std::filesystem::path{STUBS_PATH} /
                           "multi_document_lf.yaml",
                       std::ios::binary};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first, sourcemeta::core::JSON{"foo"});

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"bar"});

  const auto third{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(third, sourcemeta::core::JSON{"baz"});

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
  const std::string input{R"("\xGG")"};
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
  const std::string input{R"("\uZZZZ")"};
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
  const std::string input{R"("\UZZZZZZZZ")"};
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
  const std::string input{R"("\u00e9")"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{"\xC3\xA9"};
  EXPECT_EQ(result, expected);
}

TEST(valid_astral_unicode_escape) {
  const std::string input{R"("\U0001F600")"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{"\xF0\x9F\x98\x80"};
  EXPECT_EQ(result, expected);
}

TEST(surrogate_unicode_escape_is_rejected) {
  // YAML 1.2.2 Section 5.7: a lone surrogate is not a Unicode scalar value
  const std::string input{R"("\uD800")"};
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
  const std::string input{R"("\UFFFFFFFF")"};
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
    // The continuation reaching only the block indentation with spaces is
    // expected
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
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 3);
    EXPECT_EQ(error.column(), 2);
  } catch (...) {
    FAIL();
  }
}

// YAML 1.2.2 Section 10.3.2: the hexadecimal integer form carries no sign, so a
// signed value is a plain string rather than an integer.
TEST(signed_hexadecimal_is_a_string) {
  const std::string input{"key: -0x10"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "key": "-0x10" })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_string());
  EXPECT_FALSE(result.at("key").is_integer());
}

// YAML 1.2.2 Section 10.3.2: the octal integer form carries no sign, so a
// signed value is a plain string rather than an integer.
TEST(signed_octal_is_a_string) {
  const std::string input{"key: +0o7"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "key": "+0o7" })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_string());
  EXPECT_FALSE(result.at("key").is_integer());
}

// YAML 1.2.2 Section 10.3.2: the base-ten integer form does carry a sign, so a
// signed decimal remains an integer.
TEST(negative_decimal_integers_are_preserved) {
  const std::string input{"a: -16\nb: -12"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "a": -16, "b": -12 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("a").is_integer());
  EXPECT_FALSE(result.at("a").is_real());
  EXPECT_TRUE(result.at("b").is_integer());
  EXPECT_FALSE(result.at("b").is_real());
}

// YAML 1.2.2 Section 10.3.2: the octal indicator is lowercase, so an uppercase
// indicator makes the value a plain string.
TEST(uppercase_octal_indicator_is_a_string) {
  const std::string input{"key: 0O17"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "key": "0O17" })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_string());
  EXPECT_FALSE(result.at("key").is_integer());
}

// YAML 1.2.2 Section 10.3.2: the hexadecimal indicator is lowercase, so an
// uppercase indicator makes the value a plain string.
TEST(uppercase_hexadecimal_indicator_is_a_string) {
  const std::string input{"key: 0X1F"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "key": "0X1F" })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_string());
  EXPECT_FALSE(result.at("key").is_integer());
}

// YAML 1.2.2 Section 10.3.2: a lowercase octal indicator resolves to an
// integer.
TEST(lowercase_octal_integer_is_parsed) {
  const std::string input{"key: 0o17"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "key": 15 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_integer());
  EXPECT_FALSE(result.at("key").is_real());
}

// YAML 1.2.2 Section 10.3.2: a lowercase hexadecimal indicator resolves to an
// integer.
TEST(lowercase_hexadecimal_integer_is_parsed) {
  const std::string input{"key: 0x1f"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "key": 31 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_integer());
  EXPECT_FALSE(result.at("key").is_real());
}

// A digit outside the indicated base makes the whole scalar fail to resolve as
// an integer, so it stays a string rather than becoming the prefix that parsed
TEST(octal_integer_with_invalid_digit_stays_a_string) {
  const std::string input{"key: 0o18"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "key": "0o18" })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_string());
}

TEST(hexadecimal_integer_with_invalid_digit_stays_a_string) {
  const std::string input{"key: 0x1g"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "key": "0x1g" })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_string());
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
  } catch (...) {
    FAIL();
  }

  const std::string valid{"key: caf\xc3\xa9"};
  const auto result{sourcemeta::core::parse_yaml(valid)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("key", sourcemeta::core::JSON{"caf\xc3\xa9"});
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_string());
}

// YAML 1.2.2 Section 5.1: a non-ASCII character encoded as a multibyte sequence
// is printable, so its continuation bytes are not treated as control
// characters.
TEST(utf8_multibyte_scalar_is_accepted) {
  const std::string input{"key: caf\xc3\xa9"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("key", sourcemeta::core::JSON{"caf\xc3\xa9"});
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_string());
}

// YAML 1.2.2 Section 5.2: a stream may begin with a byte order mark, which is
// skipped rather than becoming part of the first token.
TEST(leading_byte_order_mark_is_stripped) {
  const std::string input{"\xef\xbb\xbf"
                          "foo: bar"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": "bar" })JSON")};
  EXPECT_EQ(result, expected);
}

// YAML 1.2.2 Section 6.8.1: a document naming a higher major version is
// rejected.
TEST(yaml_directive_higher_major_version_is_rejected) {
  const std::string input{"%YAML 2.0\n---\nfoo: bar\n"};
  try {
    sourcemeta::core::parse_yaml(input);
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_EQ(error.line(), 1);
  } catch (...) {
    FAIL();
  }

  const std::string valid{"%YAML 1.2\n--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(valid)};
  const sourcemeta::core::JSON expected{"value"};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_string());
}

// YAML 1.2.2 Section 6.8.1: a higher minor version of the same major version is
// accepted.
TEST(yaml_directive_higher_minor_version_is_accepted) {
  const std::string input{"%YAML 1.3\n--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{"value"};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_string());
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
  } catch (...) {
    FAIL();
  }

  const std::string valid{"%TAG !e! tag:example.com,2000:app/\n"
                          "%TAG !f! tag:example.com,2000:other/\n"
                          "--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(valid)};
  const sourcemeta::core::JSON expected{"value"};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_string());
}

// YAML 1.2.2 Section 6.8.2: distinct handles each carry their own tag
// directive.
TEST(distinct_tag_directive_handles_are_accepted) {
  const std::string input{"%TAG !e! tag:example.com,2000:app/\n"
                          "%TAG !f! tag:example.com,2000:other/\n"
                          "--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{"value"};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_string());
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
  } catch (...) {
    FAIL();
  }

  const std::string valid{"!!str foo\n"};
  const auto result{sourcemeta::core::parse_yaml(valid)};
  const sourcemeta::core::JSON expected{"foo"};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_string());
}

// YAML 1.2.2 Section 6.8.2.1: the secondary tag handle is associated with a
// prefix by default.
TEST(secondary_tag_handle_str_is_accepted) {
  const std::string input{"!!str 1"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{"1"};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_string());
  EXPECT_FALSE(result.is_integer());
}

// YAML 1.2.2 Section 6.8.2.1: the primary tag handle is associated with a
// prefix by default.
TEST(primary_tag_handle_plain_is_accepted) {
  const std::string input{"! plain"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{"plain"};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_string());
}

// YAML 1.2.2 Section 5.1: "To ensure JSON compatibility, YAML processors must
// allow all non-C0 characters inside quoted scalars" (the nb-json production
// "x09 | [x20-x10FFFF]"), so a delete character or a permanently unassigned
// code point inside a double-quoted scalar is content rather than an error.
TEST(non_c0_control_inside_a_quoted_scalar_is_accepted) {
  const std::string input{std::string{"key: \"a"} + "\x7F" + "b\""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("key",
                  sourcemeta::core::JSON{std::string{"a"} + "\x7F" + "b"});
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_string());

  const std::string noncharacter{std::string{"key: \"a"} + "\xEF\xBF\xBE" +
                                 "b\""};
  const auto noncharacter_result{sourcemeta::core::parse_yaml(noncharacter)};
  auto noncharacter_expected{sourcemeta::core::JSON::make_object()};
  noncharacter_expected.assign(
      "key", sourcemeta::core::JSON{std::string{"a"} + "\xEF\xBF\xBE" + "b"});
  EXPECT_EQ(noncharacter_result, noncharacter_expected);
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
  } catch (...) {
    FAIL();
  }

  const std::string valid{"key: caf\xc3\xa9"};
  const auto result{sourcemeta::core::parse_yaml(valid)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("key", sourcemeta::core::JSON{"caf\xc3\xa9"});
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_string());
}

// YAML 1.2.2 Section 5.2: a stream that begins with a byte order mark is read
// in the encoding it selects, so a stream reader resumes a later document at
// the correct byte rather than one offset by the mark.
TEST(byte_order_mark_stream_resumes_at_the_correct_document) {
  std::istringstream stream{std::string{"\xEF\xBB\xBF"} + "a: 1\n---\nb: 2\n"};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  const sourcemeta::core::JSON expected_first{
      sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  EXPECT_EQ(first, expected_first);

  const auto second{sourcemeta::core::parse_yaml(stream)};
  const sourcemeta::core::JSON expected_second{
      sourcemeta::core::parse_json(R"JSON({ "b": 2 })JSON")};
  EXPECT_EQ(second, expected_second);
}

// YAML 1.2.2 Section 10.3.2: a value with a fractional part is a float, so an
// integral-valued float resolves to a real rather than an integer.
TEST(integral_valued_float_is_a_real) {
  const std::string input{"key: 1.0"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "key": 1.0 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_real());
  EXPECT_FALSE(result.at("key").is_integer());
}

TEST(bare_integral_float_is_a_real) {
  const std::string input{"1.0"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{sourcemeta::core::parse_json("1.0")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_real());
  EXPECT_FALSE(result.is_integer());
}

// YAML 1.2.2 Section 10.3.2: an explicit float tag on an integer produces a
// float, so it resolves to a real rather than an integer.
TEST(float_tag_on_integer_is_a_real) {
  const std::string input{"!!float 1"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_real());
  EXPECT_FALSE(result.is_integer());
  EXPECT_EQ(result.to_real(), 1.0);
}

TEST(plain_integer_stays_an_integer) {
  const std::string input{"1"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{1};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.is_integer());
  EXPECT_FALSE(result.is_real());
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
  } catch (...) {
    FAIL();
  }
}

// RFC 8259 Section 6 forbids only the special numeric values, so a quoted
// infinity token is an ordinary string and a normal float still parses.
TEST(quoted_infinity_is_a_string) {
  const std::string input{"key: \".inf\""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "key": ".inf" })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_string());
}

TEST(normal_float_still_parses) {
  const std::string input{"key: 1.5"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "key": 1.5 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("key").is_real());
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
  } catch (...) {
    FAIL();
  }
}

// A scalar key that resolves to a non-string is still stringified into an
// object member name rather than rejected.
TEST(integer_key_is_stringified) {
  const std::string input{"1: v"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "1": "v" })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(boolean_key_is_stringified) {
  const std::string input{"true: v"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "true": "v" })JSON")};
  EXPECT_EQ(result, expected);
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

TEST(flow_mapping_null_key_resolves_to_empty_string) {
  const std::string input{"{~: v}"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "": "v" })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(flow_mapping_integer_key_resolves_to_string) {
  const std::string input{"{1: a}"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "1": "a" })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(flow_mapping_distinct_keys_are_accepted) {
  const std::string input{"{a: 1, b: 2}"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "a": 1, "b": 2 })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(block_explicit_scalar_key_is_resolved) {
  const std::string input{"? 0x1\n: v"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "1": "v" })JSON")};
  EXPECT_EQ(result, expected);
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
  } catch (...) {
    FAIL();
  }
}

// Round-trip mode preserves the original text, so an explicitly floated
// infinity is kept as a string rather than rejected.
TEST(float_tag_infinity_preserved_in_roundtrip) {
  const std::string input{"!!float .inf"};
  sourcemeta::core::YAMLRoundTrip roundtrip;
  const auto result{sourcemeta::core::parse_yaml(input, roundtrip)};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result, sourcemeta::core::JSON{".inf"});
}

// An explicit string tag on a key keeps the key's literal text as the member
// name rather than resolving it to a typed value.
TEST(str_tagged_key_keeps_literal_text) {
  const std::string input{"!!str null: value"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "null": "value" })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(str_tagged_numeric_key_keeps_literal_text) {
  const std::string input{"!!str 01: value"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "01": "value" })JSON")};
  EXPECT_EQ(result, expected);
}

// An alias key is stringified from its already-resolved anchor value, so a
// quoted keyword anchor keeps its literal text as the member name rather than
// being resolved a second time.
TEST(alias_quoted_keyword_key_is_preserved) {
  const std::string input{"- &a \"null\"\n- *a : value"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{sourcemeta::core::parse_json(
      R"JSON([ "null", { "null": "value" } ])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(escape_vertical_tab) {
  const std::string input{R"("\v")"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), std::string("\x0b", 1));
}

TEST(escape_form_feed) {
  const std::string input{R"("\f")"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), std::string("\x0c", 1));
}

TEST(escape_line_separator) {
  const std::string input{R"("\L")"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), std::string("\xe2\x80\xa8", 3));
}

TEST(escape_paragraph_separator) {
  const std::string input{R"("\P")"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), std::string("\xe2\x80\xa9", 3));
}

TEST(escape_incomplete_hex) {
  const std::string input{R"("\xZ")"};
  try {
    const auto result{sourcemeta::core::parse_yaml(input)};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid hex escape sequence");
  }
}

TEST(unterminated_single_quote) {
  const std::string input{"'abc"};
  try {
    const auto result{sourcemeta::core::parse_yaml(input)};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Missing closing quote in single-quoted scalar");
  }
}

TEST(single_quoted_cr_folds_to_space) {
  const std::string input{"'a\rb'"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.to_string(), "a b");
}

TEST(single_quoted_crlf_folds_to_space) {
  const std::string input{"'a\r\nb'"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.to_string(), "a b");
}

TEST(double_quoted_cr_folds_to_space) {
  const std::string input{"\"a\rb\""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.to_string(), "a b");
}

TEST(double_quoted_escaped_cr_line_continuation) {
  const std::string input{"\"a\\\rb\""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.to_string(), "ab");
}

TEST(double_quoted_escaped_crlf_line_continuation) {
  const std::string input{"\"a\\\r\nb\""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.to_string(), "ab");
}

TEST(block_scalar_with_cr_line_breaks) {
  const std::string input{"|\r  a\r  b\r"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.to_string(), "a\nb\n");
}

TEST(block_scalar_with_crlf_line_breaks) {
  const std::string input{"|\r\n  a\r\n  b\r\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.to_string(), "a\nb\n");
}

TEST(flow_mapping_bare_key_at_eof_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("{a")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Expected ':' after mapping key");
  }
}

TEST(flow_mapping_colon_at_eof_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("{a:")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected token");
  }
}

TEST(flow_mapping_anchor_key_at_eof_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("{&a")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Expected scalar key in mapping");
  }
}

TEST(explicit_bool_true_capitalized) {
  const auto result{sourcemeta::core::parse_yaml("!!bool True")};
  EXPECT_TRUE(result.is_boolean());
  EXPECT_TRUE(result.to_boolean());
}

TEST(explicit_bool_true_uppercase) {
  const auto result{sourcemeta::core::parse_yaml("!!bool TRUE")};
  EXPECT_TRUE(result.is_boolean());
  EXPECT_TRUE(result.to_boolean());
}

TEST(sign_only_scalar_is_a_string) {
  const auto result{sourcemeta::core::parse_yaml("+")};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "+");
}

TEST(dot_after_exponent_is_a_string) {
  const auto result{sourcemeta::core::parse_yaml("1e1.5")};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "1e1.5");
}

TEST(double_exponent_is_a_string) {
  const auto result{sourcemeta::core::parse_yaml("1e2e3")};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "1e2e3");
}

TEST(line_starting_with_double_dash_non_marker) {
  const auto result{sourcemeta::core::parse_yaml("--x")};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "--x");
}

TEST(flow_mapping_colon_before_flow_indicator) {
  const auto result{sourcemeta::core::parse_yaml("{a:}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_TRUE(result.defines("a"));
}

TEST(colon_as_final_byte_no_newline) {
  const auto result{sourcemeta::core::parse_yaml("a:")};
  EXPECT_TRUE(result.is_object());
  EXPECT_TRUE(result.defines("a"));
}

TEST(flow_sequence_dash_at_eof_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("[-")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid plain scalar start in flow context");
  }
}

TEST(anchor_with_empty_value_before_document_end) {
  const auto result{sourcemeta::core::parse_yaml("&a\n...")};
  EXPECT_TRUE(result.is_null());
}

TEST(tag_then_flow_mapping_end) {
  const auto result{sourcemeta::core::parse_yaml("{ a: !!str }")};
  EXPECT_TRUE(result.is_object());
  EXPECT_TRUE(result.defines("a"));
}

TEST(tag_then_flow_sequence_end) {
  const auto result{sourcemeta::core::parse_yaml("[ !!str ]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
}

TEST(alias_key_referencing_unknown_anchor_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("*a: b")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "YAML alias references undefined anchor");
  }
}

TEST(flow_explicit_key_at_eof_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("[?")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Unexpected token");
  }
}

TEST(flow_collection_indented_at_parent_block_level_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("a: [\n1]")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Insufficient indentation in flow collection");
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

TEST(explicit_key_value_at_document_end) {
  const auto result{sourcemeta::core::parse_yaml("? a\n:\n...")};
  EXPECT_TRUE(result.is_object());
  EXPECT_TRUE(result.defines("a"));
}

TEST(block_mapping_second_key_value_at_document_end) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\nb:\n...")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("b").is_null());
}

TEST(leading_colon_value_indicator_mapping) {
  const auto result{sourcemeta::core::parse_yaml(": v")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
}

TEST(anchor_empty_value_before_document_start) {
  const auto result{sourcemeta::core::parse_yaml("&a\n---\nb")};
  EXPECT_TRUE(result.is_null());
}

TEST(mapping_key_that_is_a_collection_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("[? {a: 1}: b]")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Mapping key cannot be a collection");
  }
}

TEST(tab_trailing_after_quoted_scalar_is_rejected) {
  try {
    const auto result{sourcemeta::core::parse_yaml("'x'\tbad")};
    FAIL();
  } catch (const sourcemeta::core::YAMLParseError &error) {
    EXPECT_STREQ(error.what(), "Invalid trailing content");
  }
}

TEST(cr_trailing_after_quoted_scalar) {
  const auto result{sourcemeta::core::parse_yaml("'x'\r")};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "x");
}

TEST(block_scalar_blank_lines_with_cr) {
  const auto result{sourcemeta::core::parse_yaml("|\r  a\r\r  b")};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "a\n\nb");
}

TEST(tab_after_escaped_line_break) {
  const auto result{sourcemeta::core::parse_yaml("\"a\\\n\tb\"")};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "ab");
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

TEST(explicit_key_alias_resolves_to_its_anchor_value) {
  const std::string input{"x: &a zzz\n? *a\n: 2"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "x": "zzz", "zzz": 2 })JSON")};
  EXPECT_EQ(result, expected);
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

// The same anchor used as a value yields the typed node, not the member name
TEST(explicit_key_anchor_aliased_as_a_value_keeps_its_type) {
  const std::string input{"? &a 0x1\n: 1\nb: *a"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "1": 1, "b": 1 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("b").is_integer());
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

TEST(implicit_key_anchor_aliased_as_a_value_keeps_its_type) {
  const std::string input{"&a 0x1: first\nb: *a"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "1": "first", "b": 1 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.at("b").is_integer());
}

TEST(implicit_key_anchor_on_a_plain_scalar_stays_a_string) {
  const std::string input{"&a foo: 1\nb: *a"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1, "b": "foo" })JSON")};
  EXPECT_EQ(result, expected);
}

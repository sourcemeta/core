#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/yaml.h>

#include <cstddef>    // std::size_t
#include <filesystem> // std::filesystem
#include <fstream>    // std::ifstream
#include <ios>        // std::ios::binary
#include <sstream>    // std::istringstream
#include <string>     // std::string
#include <utility>    // std::move

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

// YAML 1.2.2 Section 9.2: a document that is not terminated by a document end
// marker is followed by one that begins with a directives end marker, and
// production 211 admits an explicit document directly after any other, so a
// stream separated by directives end markers alone is well formed.
TEST(read_yaml_accepts_documents_separated_by_a_directives_end_marker) {
  const auto result{sourcemeta::core::read_yaml(
      std::filesystem::path{STUBS_PATH} / "multi_document_objects.yaml")};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(read_yaml_accepts_scalar_documents_separated_by_a_directives_end_marker) {
  const auto result{sourcemeta::core::read_yaml(
      std::filesystem::path{STUBS_PATH} / "multi_document_lf.yaml")};
  const sourcemeta::core::JSON expected{sourcemeta::core::JSON{"foo"}};
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

// A token handed back has not been read as far as the caller is concerned, so
// where the stream stands has to be where that token begins. Leaving it where
// reading the token ended made the next read resume past it and lose it.
TEST(multi_document_after_a_leading_document_end_marker) {
  std::istringstream stream{"...\nfoo\n---\nbar"};

  const auto doc1{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(doc1.is_null());

  const auto doc2{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(doc2, sourcemeta::core::JSON{"foo"});

  const auto doc3{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(doc3, sourcemeta::core::JSON{"bar"});
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

TEST(decimal_long_small_decimal) {
  const std::string input{
      "0.00000000000000000000000000000000000000000000000000000000000000000000"
      "1"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal(),
            sourcemeta::core::Decimal{
                "0.000000000000000000000000000000000000000000000000000000000000"
                "00000000"
                "1"});
}

TEST(decimal_subnormal_decimal) {
  const std::string input{"0." + std::string(309, '0') + "5"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal(),
            sourcemeta::core::Decimal{"0." + std::string(309, '0') + "5"});
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
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal(), sourcemeta::core::Decimal{"3.14"});
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

// Reusing an anchor so that the expanded document outgrows the text describing
// it is ordinary YAML, and stays well within the expansion allowance
TEST(repeated_alias_expansion_beyond_the_input_length_is_accepted) {
  const std::string input{"a: &a [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]\n"
                          "b: [ *a, *a, *a, *a, *a, *a, *a, *a, *a, *a ]\n"};

  const auto result{sourcemeta::core::parse_yaml(input)};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "a": [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ],
    "b": [
      [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ],
      [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ],
      [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ],
      [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ],
      [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ],
      [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ],
      [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ],
      [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ],
      [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ],
      [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
    ]
  })JSON");

  EXPECT_EQ(result, expected);
}

// A !!float tag whose value is outside the 64-bit integer range must not
// invoke undefined behavior by casting an out-of-range double to an integer.
TEST(float_tag_out_of_integer_range) {
  const std::string input{"!!float 1e300"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal(), sourcemeta::core::Decimal{"1e300"});
}

TEST(float_tag_just_above_integer_range) {
  const std::string input{"!!float 1e19"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal(), sourcemeta::core::Decimal{"1e19"});
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

TEST(tab_before_scalar_mapping_value_is_accepted) {
  const std::string input{"foo:\n \tbar\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("foo"), sourcemeta::core::JSON{"bar"});
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

// A tab that begins a folded line of a top-level quoted scalar is folding white
// space rather than indentation, so it is valid.
TEST(tab_led_fold_in_root_double_quoted_scalar_is_accepted) {
  const std::string input{
      "\" 1st non-empty\n\n 2nd non-empty \n\t3rd non-empty \""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{
                        " 1st non-empty\n2nd non-empty 3rd non-empty "});
}

// A tab that follows a space on an empty block scalar line is separation rather
// than indentation, so it is valid.
TEST(tab_after_space_on_empty_block_scalar_line_is_accepted) {
  const std::string input{"foo: |\n \t\nbar: 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("bar"), sourcemeta::core::JSON{1});
}

TEST(flow_mapping_content_indented_past_block_is_accepted) {
  const std::string input{"k: {\n k\n :\n v\n }\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "k": { "k": "v" } })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(top_level_flow_sequence_at_column_one_is_accepted) {
  const std::string input{"[\nfoo: bar\n]\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON([ { "foo": "bar" } ])JSON")};
  EXPECT_EQ(result, expected);
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
// YAML 1.2.2 Section 8.1.1.1: the content indentation level of a block scalar
// is the indentation level of the node itself plus the indicator, and the root
// node of a document sits one level further out than the leftmost column
TEST(root_literal_block_scalar_with_an_indentation_indicator) {
  const std::string input{"|1\n  foo\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"  foo\n"});
}

TEST(root_literal_block_scalar_with_a_two_indentation_indicator) {
  const std::string input{"|2\n  foo\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{" foo\n"});
}

TEST(root_literal_block_scalar_with_a_three_indentation_indicator) {
  const std::string input{"|3\n  foo\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo\n"});
}

TEST(root_folded_block_scalar_with_an_indentation_indicator) {
  const std::string input{">1\n  foo\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"  foo\n"});
}

TEST(explicit_root_literal_block_scalar_with_an_indentation_indicator) {
  const std::string input{"--- |1\n  foo\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"  foo\n"});
}

TEST(nested_sequence_block_scalar_with_an_indentation_indicator) {
  const std::string input{"a:\n  - |1\n     foo\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.at("a").at(0), sourcemeta::core::JSON{"  foo\n"});
}

TEST(nested_sequence_block_scalar_with_a_two_indentation_indicator) {
  const std::string input{"a:\n  - |2\n     foo\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.at("a").at(0), sourcemeta::core::JSON{" foo\n"});
}

// YAML 1.2.2 Section 8.2.1: a block sequence entry that is no more indented
// than its own sequence starts a sibling entry, so a node property before it
// decorates an empty node rather than a nested sequence
TEST(tag_on_an_empty_sequence_item_before_a_sibling) {
  const std::string input{"- !Custom\n- foo\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0), sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at(1), sourcemeta::core::JSON{"foo"});
}

TEST(string_tag_on_an_empty_sequence_item_before_a_sibling) {
  const std::string input{"- !!str\n- foo\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0), sourcemeta::core::JSON{""});
  EXPECT_EQ(result.at(1), sourcemeta::core::JSON{"foo"});
}

TEST(string_tag_on_an_empty_mapping_value_before_a_sibling) {
  const std::string input{"foo: !!str\nbar: baz\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("foo"), sourcemeta::core::JSON{""});
  EXPECT_EQ(result.at("bar"), sourcemeta::core::JSON{"baz"});
}

// YAML 1.2.2 Section 5.4: a lone carriage return is a line break of its own
TEST(carriage_return_only_line_breaks) {
  const std::string input{"a: 1\rb: 2\r"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a"), sourcemeta::core::JSON{1});
  EXPECT_EQ(result.at("b"), sourcemeta::core::JSON{2});
}

TEST(carriage_return_only_document_markers) {
  std::istringstream stream{"a: 1\r---\rb: 2\r"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first.at("a"), sourcemeta::core::JSON{1});
  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second.at("b"), sourcemeta::core::JSON{2});
}

// YAML 1.2.2 Section 8.2.2: a key whose value is empty is followed by the next
// key of whichever mapping that key's indentation belongs to, so a less
// indented key that follows closes the mapping rather than becoming the value
TEST(empty_mapping_value_before_a_less_indented_key) {
  const std::string input{"a:\n  b:\nc: 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").at("b"), sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("c"), sourcemeta::core::JSON{1});
}

TEST(empty_mapping_values_before_a_less_indented_key) {
  const std::string input{"a:\n  b:\n  d:\nc: 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").size(), 2);
  EXPECT_EQ(result.at("a").at("b"), sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("a").at("d"), sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("c"), sourcemeta::core::JSON{1});
}

TEST(empty_mapping_value_two_levels_before_a_less_indented_key) {
  const std::string input{"a:\n  b:\n    c:\nd: 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").at("b").at("c"), sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("d"), sourcemeta::core::JSON{1});
}

TEST(empty_mapping_value_before_a_more_indented_key) {
  const std::string input{"a:\n  b:\n      c: 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("a").at("b").at("c"), sourcemeta::core::JSON{1});
}

TEST(empty_mapping_values_across_a_workflow_shape) {
  const std::string input{"on:\n  push:\n  pull_request:\njobs:\n  x: 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("on").at("push"), sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("on").at("pull_request"),
            sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("jobs").at("x"), sourcemeta::core::JSON{1});
}

TEST(empty_alias_mapping_value_before_a_less_indented_key) {
  const std::string input{
      "anchor: &a name\nmap:\n  first: 1\n  *a :\nnext: 2\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("map").size(), 2);
  EXPECT_EQ(result.at("map").at("first"), sourcemeta::core::JSON{1});
  EXPECT_EQ(result.at("map").at("name"), sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("next"), sourcemeta::core::JSON{2});
}

TEST(alias_mapping_value_on_a_more_indented_line) {
  const std::string input{
      "anchor: &a name\nmap:\n  first: 1\n  *a :\n    deep: 1\nnext: 2\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("map").at("name").at("deep"), sourcemeta::core::JSON{1});
  EXPECT_EQ(result.at("next"), sourcemeta::core::JSON{2});
}

TEST(alias_mapping_value_on_the_same_line) {
  const std::string input{
      "anchor: &a name\nmap:\n  first: 1\n  *a : v\nnext: 2\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("map").at("name"), sourcemeta::core::JSON{"v"});
  EXPECT_EQ(result.at("next"), sourcemeta::core::JSON{2});
}

TEST(anchored_key_after_an_empty_mapping_value) {
  const std::string input{"a:\n  b:\n&x c: 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").at("b"), sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("c"), sourcemeta::core::JSON{1});
}

TEST(tagged_key_after_an_empty_mapping_value) {
  const std::string input{"a:\n  b:\n!!str c: 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").at("b"), sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("c"), sourcemeta::core::JSON{1});
}

TEST(anchored_key_after_an_empty_subsequent_mapping_value) {
  const std::string input{"a:\n  first: 1\n  b:\n&x c: 2\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").at("b"), sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("c"), sourcemeta::core::JSON{2});
}

TEST(anchored_sibling_key_after_an_empty_mapping_value) {
  const std::string input{"a:\n  b:\n  &x c: 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("a").size(), 2);
  EXPECT_EQ(result.at("a").at("b"), sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("a").at("c"), sourcemeta::core::JSON{1});
}

TEST(anchored_key_after_a_mapping_value) {
  const std::string input{"a:\n  b: v\n&x c: 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").at("b"), sourcemeta::core::JSON{"v"});
  EXPECT_EQ(result.at("c"), sourcemeta::core::JSON{1});
}

TEST(anchored_key_after_an_empty_alias_mapping_value) {
  const std::string input{
      "anchor: &a name\nmap:\n  first: 1\n  *a :\n&x next: 2\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("map").at("name"), sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("next"), sourcemeta::core::JSON{2});
}

TEST(unindented_sequence_still_starts_a_mapping_value) {
  const std::string input{"a:\n- 1\n- 2\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.at("a").size(), 2);
  EXPECT_EQ(result.at("a").at(0), sourcemeta::core::JSON{1});
}

TEST(anchored_sequence_indented_past_the_mapping) {
  const std::string input{"a:\n  b:\n    &x\n    - 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.at("a").at("b").size(), 1);
  EXPECT_EQ(result.at("a").at("b").at(0), sourcemeta::core::JSON{1});
}

TEST(anchored_indentless_sequence_on_the_key_line) {
  const std::string input{"a:\n  b: &x\n  - 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.at("a").at("b").size(), 1);
  EXPECT_EQ(result.at("a").at("b").at(0), sourcemeta::core::JSON{1});
}

TEST(tagged_indentless_sequence_on_the_key_line) {
  const std::string input{"a:\n  b: !!seq\n  - 1\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.at("a").at("b").size(), 1);
  EXPECT_EQ(result.at("a").at("b").at(0), sourcemeta::core::JSON{1});
}

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

// The same continuation indented one space past the block is accepted, with the
// tab treated as separation.
TEST(tab_after_sufficient_spaces_flow_scalar_continuation_is_accepted) {
  const std::string input{"a:\n b: \"x\n  \ty\"\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.at("a").at("b"), sourcemeta::core::JSON{"x y"});
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

// YAML 1.2.2 Section 6.8.1: a higher minor version of the same major version is
// accepted.
TEST(yaml_directive_higher_minor_version_is_accepted) {
  const std::string input{"%YAML 1.3\n--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
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

// A mapping that opens with a plain key reads its later entries through a
// different walk than one that opens with an explicit key, and the two have to
// answer for the same document the same way
TEST(block_plain_key_then_empty_explicit_key) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n?\n: 2")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.defines("a"));
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.defines(""));
  EXPECT_EQ(result.at("").to_integer(), 2);
}

TEST(block_explicit_key_then_empty_explicit_key) {
  const auto result{sourcemeta::core::parse_yaml("? a\n: 1\n?\n: 2")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.defines("a"));
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.defines(""));
  EXPECT_EQ(result.at("").to_integer(), 2);
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

// YAML 1.2.2 Section 10.3.2 resolves an empty node to null, which this module
// spells as the empty string when it stands as a key, and Example 7.3 reads a
// leading value indicator into that key holding the scalar beside it.
TEST(leading_colon_value_indicator_mapping) {
  const auto result{sourcemeta::core::parse_yaml(": v")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.defines(""));
  EXPECT_EQ(result.at(""), sourcemeta::core::JSON{"v"});
}

TEST(leading_colon_value_indicator_mapping_with_an_empty_value) {
  const auto result{sourcemeta::core::parse_yaml(":")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.defines(""));
  EXPECT_TRUE(result.at("").is_null());
}

TEST(anchor_empty_value_before_document_start) {
  const auto result{sourcemeta::core::parse_yaml("&a\n---\nb")};
  EXPECT_TRUE(result.is_null());
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

TEST(explicit_key_alias_resolves_to_its_anchor_value) {
  const std::string input{"x: &a zzz\n? *a\n: 2"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "x": "zzz", "zzz": 2 })JSON")};
  EXPECT_EQ(result, expected);
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

TEST(flow_sequence_single_pair_entry_still_parses) {
  const std::string input{"[a: 1]"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_object());
  EXPECT_EQ(result.at(0).at("a"), sourcemeta::core::JSON{1});
}

TEST(carriage_return_block_mapping) {
  const std::string input{"foo: 1\rbar: 2"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{1});
  expected.assign("bar", sourcemeta::core::JSON{2});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_block_sequence) {
  const std::string input{"- 1\r- 2\r- 3"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_array()};
  expected.push_back(sourcemeta::core::JSON{1});
  expected.push_back(sourcemeta::core::JSON{2});
  expected.push_back(sourcemeta::core::JSON{3});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_nested_block_mapping) {
  const std::string input{"foo:\r  bar: 1\r  baz: 2"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto inner{sourcemeta::core::JSON::make_object()};
  inner.assign("bar", sourcemeta::core::JSON{1});
  inner.assign("baz", sourcemeta::core::JSON{2});
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", std::move(inner));
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_comment) {
  const std::string input{"# comment\rfoo: 1"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{1});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_literal_block_scalar) {
  const std::string input{"foo: |\r  one\r  two"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{"one\ntwo"});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_literal_block_scalar_clipped) {
  const std::string input{"foo: |\r  one\r  two\r"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{"one\ntwo\n"});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_literal_block_scalar_kept) {
  const std::string input{"foo: |+\r  one\r  two\r\r"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{"one\ntwo\n\n"});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_literal_block_scalar_stripped) {
  const std::string input{"foo: |-\r  one\r  two\r"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{"one\ntwo"});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_folded_block_scalar) {
  const std::string input{"foo: >\r  one\r  two"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{"one two"});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_folded_block_scalar_keeps_blank_line) {
  const std::string input{"foo: >\r  one\r\r  two\r"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{"one\ntwo\n"});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_flow_mapping) {
  const std::string input{"{ foo: 1,\r  bar: 2 }"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{1});
  expected.assign("bar", sourcemeta::core::JSON{2});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_document_markers) {
  const std::string input{"---\rfoo: 1\r..."};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{1});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_line_feed_block_mapping) {
  const std::string input{"foo: 1\r\nbar: 2"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{1});
  expected.assign("bar", sourcemeta::core::JSON{2});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_multiline_plain_scalar) {
  const std::string input{"foo: one\r  two"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{"one two"});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_comment_between_entries) {
  const std::string input{"foo: 1\r# comment\rbar: 2"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{1});
  expected.assign("bar", sourcemeta::core::JSON{2});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_trailing_comment) {
  const std::string input{"foo: 1 # comment\rbar: 2"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{1});
  expected.assign("bar", sourcemeta::core::JSON{2});
  EXPECT_EQ(result, expected);
}

TEST(carriage_return_block_scalar_header_comment) {
  const std::string input{"foo: | # comment\r  one\r  two"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("foo", sourcemeta::core::JSON{"one\ntwo"});
  EXPECT_EQ(result, expected);
}

// YAML 1.2.2 production 194 writes the value of an entry as the indicator
// followed by either a node or nothing at all, so an indicator with nothing on
// either side is a whole entry rather than the opening of the explicit one
// after it, and a second such entry names the same empty key
TEST(leading_colon_value_indicator_opens_an_entry_of_its_own) {
  const auto result{sourcemeta::core::parse_yaml(":\n? a\n: b\n")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at("").is_null());
  EXPECT_EQ(result.at("a").to_string(), "b");
}

// Section 6.5 folds a line break in a quoted scalar into a space and a second
// break into a newline, which two consecutive breaks are what reach
TEST(two_consecutive_breaks_in_a_quoted_scalar_fold_to_a_newline) {
  const std::string input{"\"a\r\n\r\nb\""};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "a\nb");
}

// YAML 1.2.2 Section 6.8 separates a directive from what follows it with white
// space, which a tab satisfies as much as a space
TEST(yaml_directive_version_followed_by_a_tab_is_accepted) {
  const std::string input{"%YAML 1.2\t\n--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"value"});
}

TEST(yaml_directive_version_followed_by_a_tab_then_a_comment_is_accepted) {
  const std::string input{"%YAML 1.2\t# a comment\n--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"value"});
}

TEST(yaml_tag_directive_separated_by_tabs_is_accepted) {
  const std::string input{"%TAG\t!e!\thttp://example.com/\n--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"value"});
}

// The single exclamation mark on its own is the non-specific tag of Section
// 6.9.1 rather than a handle short of a suffix
TEST(yaml_a_lone_exclamation_mark_is_the_non_specific_tag) {
  const std::string input{"! value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"value"});
}

TEST(yaml_local_tag_with_a_name) {
  const std::string input{"!custom value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"value"});
}

// YAML 1.2.2 Section 9.2 lets an explicit document carry no node, so a
// marker with nothing after it is one empty document
TEST(yaml_a_directives_end_marker_with_nothing_after_it) {
  std::istringstream stream{"---\n"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(first.is_null());
  EXPECT_EQ(stream.peek(), EOF);
}

// A leading document end marker closes a document that was never opened,
// and the marker that follows opens the next one
TEST(yaml_a_document_end_marker_then_a_directives_end_marker) {
  std::istringstream stream{"...\n---\nfoo"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(first.is_null());
  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"foo"});
  EXPECT_EQ(stream.peek(), EOF);
}

// Section 9.2 admits a run of document end markers, which close nothing
// more than the one empty document between them
TEST(yaml_two_document_end_markers_then_a_scalar) {
  std::istringstream stream{"...\n...\nfoo"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(first.is_null());
  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"foo"});
  EXPECT_EQ(stream.peek(), EOF);
}

// YAML 1.2.2 Section 6.8.3 has a processor ignore a directive it does not
// know rather than refuse the document
TEST(yaml_a_reserved_directive_before_a_document) {
  std::istringstream stream{"%FOO bar\n---\nfoo"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first, sourcemeta::core::JSON{"foo"});
  EXPECT_EQ(stream.peek(), EOF);
}

TEST(yaml_a_scalar_then_a_document_end_marker) {
  std::istringstream stream{"---\nfoo\n...\n"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first, sourcemeta::core::JSON{"foo"});
  EXPECT_EQ(stream.peek(), EOF);
}

// A directives end marker closes the document before it, so the first of
// two stands for an empty document of its own
TEST(yaml_two_directives_end_markers_then_a_scalar) {
  std::istringstream stream{"---\n---\nfoo"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(first.is_null());
  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"foo"});
  EXPECT_EQ(stream.peek(), EOF);
}

// The shorter spelling of the extension names the same format
TEST(yaml_or_json_short_yaml_extension) {
  const auto result{sourcemeta::core::read_yaml_or_json(
      std::filesystem::path{STUBS_PATH} / "test_4.yml")};
  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "baz": 2
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(yaml_or_json_short_yaml_extension_with_a_callback) {
  sourcemeta::core::JSON result{nullptr};
  std::size_t visits{0};
  sourcemeta::core::read_yaml_or_json(
      std::filesystem::path{STUBS_PATH} / "test_4.yml", result,
      [&visits](const sourcemeta::core::JSON::ParsePhase,
                const sourcemeta::core::JSON::Type, const std::uint64_t,
                const std::uint64_t, const sourcemeta::core::JSON::ParseContext,
                const std::size_t,
                const sourcemeta::core::JSON::String &) { visits += 1; });
  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "baz": 2
  })JSON");

  EXPECT_EQ(result, expected);
  EXPECT_TRUE(visits > 0);
}

// YAML 1.2.2 Section 5.4 admits a carriage return and line feed pair, and a
// lone carriage return, as line breaks, so every construct has to read the
// same when its lines end that way
TEST(crlf_object) {
  const std::string input{"hello: world\r\nfoo: 1\r\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "hello": "world", "foo": 1
  })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(crlf_comment) {
  const std::string input{"hello: world # a comment\r\nfoo: 1\r\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "hello": "world", "foo": 1
  })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(crlf_sequence) {
  const std::string input{"- 1\r\n- 2\r\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json("[ 1, 2 ]")};
  EXPECT_EQ(result, expected);
}

TEST(crlf_flow_sequence) {
  const std::string input{"[1,\r\n2]"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json("[ 1, 2 ]")};
  EXPECT_EQ(result, expected);
}

TEST(crlf_literal_block_scalar) {
  const std::string input{"text: |\r\n  one\r\n  two\r\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "text": "one\ntwo\n"
  })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(crlf_folded_block_scalar) {
  const std::string input{"text: >\r\n  one\r\n  two\r\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "text": "one two\n"
  })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(crlf_double_quoted_scalar_over_two_lines) {
  const std::string input{"text: \"one\r\n  two\"\r\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "text": "one two"
  })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(crlf_plain_scalar_over_two_lines) {
  const std::string input{"text: one\r\n  two\r\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "text": "one two"
  })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(lone_carriage_return_ends_the_stream) {
  const std::string input{"foo: 1\r"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(lone_carriage_return_between_two_entries) {
  const std::string input{"foo: 1\rbar: 2"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "foo": 1, "bar": 2
  })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(comment_at_the_end_of_the_stream) {
  const std::string input{"foo: 1 # note"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(anchor_at_the_end_of_the_stream) {
  const std::string input{"foo: &anchor"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": null })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(tag_at_the_end_of_the_stream) {
  const std::string input{"foo: !tag"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": null })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(literal_block_scalar_header_at_the_end_of_the_stream) {
  const std::string input{"foo: |"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "foo": "" })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(folded_block_scalar_header_at_the_end_of_the_stream) {
  const std::string input{"foo: >"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "foo": "" })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(block_scalar_header_with_an_indentation_indicator_at_the_end) {
  const std::string input{"foo: |2"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "foo": "" })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(mapping_value_missing_at_the_end_of_the_stream) {
  const std::string input{"foo:"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": null })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(tabulation_after_a_scalar_at_the_end_of_the_stream) {
  const std::string input{"foo: 1\t"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

// YAML 1.2.2 Section 5.4 counts a lone carriage return as a line break, so a
// document that ends on one ends on an empty line rather than mid-token
TEST(yaml_document_ending_in_a_lone_carriage_return) {
  const std::string input{"a: 1\r"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

// A comment runs to the end of its line, which the end of the document is one
// of (YAML 1.2.2 Section 6.6)
TEST(yaml_comment_closed_by_the_end_of_the_document) {
  const std::string input{"a: 1 # note"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

// YAML 1.2.2 Section 6.1 excludes a tabulation from indentation but admits it
// as separation space, so one between a key and its value separates them
TEST(yaml_tabulation_separates_a_key_from_its_value) {
  const std::string input{"a:\tb"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": "b" })JSON")};
  EXPECT_EQ(result, expected);
}

// A block scalar header may carry a comment, which YAML 1.2.2 Section 6.6 ends
// at a line break or at the end of the document
TEST(yaml_block_scalar_header_comment_closed_by_the_end_of_the_document) {
  const std::string input{"a: | # note"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": "" })JSON")};
  EXPECT_EQ(result, expected);
}

// YAML 1.2.2 Section 6.1 excludes a tabulation from indentation, so a line
// holding nothing else indents nothing and the document ends there
TEST(yaml_document_ending_in_a_tabulation_only_line) {
  const std::string input{"a: 1\n\t"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

// A tabulation is separation space rather than indentation, so what follows one
// continues the plain scalar of the line before (YAML 1.2.2 Section 7.3.3)
TEST(yaml_tabulation_before_a_plain_scalar_continuation) {
  const std::string input{"a: 1\n\tb\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "1 b" })JSON")};
  EXPECT_EQ(result, expected);
}

// YAML 1.2.2 Section 9.1: a stream carries documents independently of one
// another, and parsing a string reads the first while holding the rest to the
// same grammar. Section 9.2 writes the end marker as "..." and the directives
// end marker as "---", and admits either directly after the other
TEST(yaml_string_with_a_document_end_marker_after_the_document) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"foo\n...\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo"});
}

TEST(yaml_string_with_a_directives_end_marker_and_nothing_after_it) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"foo\n...\n---\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo"});
}

TEST(yaml_string_with_two_directives_end_markers_in_a_row) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"foo\n...\n---\n---\nbar\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo"});
}

TEST(yaml_string_with_a_document_end_marker_closing_an_empty_document) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"foo\n...\n---\n...\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo"});
}

// Section 6.8 reserves the directives it does not define for future use, and a
// document end marker opens the scope the next one declares them in
TEST(yaml_string_with_a_reserved_directive_in_a_later_document) {
  const auto result{sourcemeta::core::parse_yaml(
      std::string{"foo\n...\n%FOO bar\n---\nbaz\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo"});
}

TEST(yaml_string_with_a_later_document_closed_by_its_own_end_marker) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"foo\n...\nbar\n...\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo"});
}

// Reading a file holds every document of the stream to the grammar rather than
// only the first, which is where YAML 1.2.2 Section 9.2 production 211 is read:
// a document end marker may be followed by another document, by a directives
// end marker, or by nothing at all
TEST(read_file_with_a_document_end_marker_closing_the_document) {
  const auto result{sourcemeta::core::read_yaml(
      std::filesystem::path{STUBS_PATH} / "documents_closed_by_a_marker.yaml")};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo"});
}

TEST(read_file_with_a_directives_end_marker_and_nothing_after_it) {
  const auto result{
      sourcemeta::core::read_yaml(std::filesystem::path{STUBS_PATH} /
                                  "documents_with_a_bare_directives_end.yaml")};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo"});
}

TEST(read_file_with_two_directives_end_markers_in_a_row) {
  const auto result{
      sourcemeta::core::read_yaml(std::filesystem::path{STUBS_PATH} /
                                  "documents_with_two_directives_ends.yaml")};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo"});
}

// Section 8.2.3 admits an empty node, so a document opened by a directives end
// marker may carry nothing and be closed by its own end marker
TEST(read_file_with_an_empty_document_closed_by_its_marker) {
  const auto result{sourcemeta::core::read_yaml(
      std::filesystem::path{STUBS_PATH} /
      "documents_with_an_empty_later_document.yaml")};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo"});
}

// Section 6.8 reserves the directives it does not define, and an end marker
// opens the scope the document after it declares them in
TEST(read_file_with_a_reserved_directive_in_a_later_document) {
  const auto result{
      sourcemeta::core::read_yaml(std::filesystem::path{STUBS_PATH} /
                                  "documents_with_a_reserved_directive.yaml")};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo"});
}

TEST(read_file_with_every_document_closed_by_its_own_marker) {
  const auto result{
      sourcemeta::core::read_yaml(std::filesystem::path{STUBS_PATH} /
                                  "documents_each_closed_by_a_marker.yaml")};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo"});
}

// YAML 1.2.2 production 205 writes the end of a document as its own marker, so
// one standing after the document a caller asked for accounts for what follows
// rather than leaving it unreachable
TEST(yaml_string_with_a_trailing_document_end_marker) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"key: 1\n...\n"})};
  EXPECT_EQ(result, sourcemeta::core::parse_yaml("key: 1"));
}

TEST(yaml_string_with_two_trailing_document_end_markers) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"key: 1\n...\n...\n"})};
  EXPECT_EQ(result, sourcemeta::core::parse_yaml("key: 1"));
}

// The stream overloads read one document at a time by design, so they leave
// what follows where the next call will find it rather than turning it down
TEST(yaml_stream_leaves_a_second_document_for_the_next_read) {
  std::istringstream stream{"key: 1\n---\nv\n"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(first.is_object());
  EXPECT_EQ(first.at("key"), sourcemeta::core::JSON{1});

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"v"});
}

// YAML 1.2.2 production 192 writes a block mapping entry as
// `( ns-s-block-map-implicit-key | e-node ) c-l-block-map-implicit-value`, so
// the key of any entry may be empty rather than only the key of the first, and
// Section 8.2.2 settles what makes it one: "while both the implicit key and
// the value following it may be empty, the ':' indicator is mandatory"
TEST(yaml_empty_key_entry_after_another_entry) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a: 1\n: c\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": 1, "": "c" })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_empty_key_entry_with_an_empty_value) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a: 1\n:\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": 1, "": null })JSON")};
  EXPECT_EQ(result, expected);
}

// Section 6.7: "Implicit keys are restricted to a single line", so an
// indicator on a later line belongs to a mapping further out rather than
// making the scalar before it a key
TEST(yaml_value_indicator_on_a_later_line_closes_the_inner_mapping) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a:\n  b: 1\n: c\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": { "b": 1 }, "": "c" })JSON")};
  EXPECT_EQ(result, expected);
}

// YAML 1.2.2 production 63 writes indentation as `s-indent(n+1) ::= s-space
// s-indent(n)`, so a tab can be no part of it, and Section 6.1 says as much:
// "tab characters must not be used in indentation". Production 197 has a flow
// node at block level preceded by `s-separate` rather than by indentation, and
// that does admit a tab, which is what the two cases below tell apart
TEST(yaml_tab_before_a_flow_mapping_on_the_first_line) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"\t{a: 1}\n"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_tab_before_a_flow_sequence_on_the_first_line) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"\t[1]\n"})};
  const auto expected{sourcemeta::core::parse_json("[ 1 ]")};
  EXPECT_EQ(result, expected);
}

// YAML 1.2.2 Section 8.1.1 writes the header as an indicator, a chomping
// indicator and a comment, with separation between them, and Section 6.2 has
// that separation admit a tabulation as readily as a space
TEST(yaml_block_scalar_header_separated_by_a_tabulation) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a: |\t\n  x\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "x\n" })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_block_scalar_with_carriage_return_line_breaks) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: |\r\n  x\r\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "x\n" })JSON")};
  EXPECT_EQ(result, expected);
}

// Below the root, a line standing no further in than the node holding the
// scalar is a sibling of that node rather than content, so the scalar holds no
// non-empty line and Section 8.1.1.1 takes "the number of spaces on the
// longest line" for its level
TEST(yaml_block_scalar_of_empty_lines_does_not_take_in_the_next_key) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: |\n  \nb: 1\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "", "b": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_block_scalar_of_one_empty_line_does_not_take_in_the_next_key) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: |\n\nb: 1\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "", "b": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

// A scalar whose leading empty lines are no deeper than its content is read as
// it stands, which is what the error above is told apart from
TEST(yaml_block_scalar_leading_empty_line_shallower_than_its_content) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: |\n \n  x\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "\nx\n" })JSON")};
  EXPECT_EQ(result, expected);
}

// YAML 1.2.2 production 207 writes the bare document as
// `s-l+block-node(-1,BLOCK-IN)`, so a block scalar at the root reads its
// content at the leftmost column
TEST(yaml_root_block_scalar_with_content_at_the_leftmost_column) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"|\nfoo\nbar\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo\nbar\n"});
}

// Production 206 writes the forbidden content as
// `( c-directives-end | c-document-end ) ( b-char | s-white | <end-of-input> )`
// so a line that merely begins with those three characters is content
TEST(yaml_root_block_scalar_line_starting_with_three_dashes_and_content) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"|\n---foo\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"---foo\n"});
}

TEST(yaml_root_block_scalar_line_starting_with_three_dots_and_content) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"|\n...foo\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"...foo\n"});
}

TEST(yaml_root_block_scalar_closed_by_a_document_end_marker) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"|\nfoo\n...\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"foo\n"});
}

TEST(yaml_root_block_scalar_closed_by_a_directives_end_marker) {
  std::istringstream stream{"|\nfoo\n---\nbar\n"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(first, sourcemeta::core::JSON{"foo\n"});

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"bar"});
}

// A root scalar whose every line is empty has no content line to measure, so
// Section 8.1.1.1 takes "the number of spaces on the longest line" and there
// is no node holding it to stand further in than
TEST(yaml_root_block_scalar_of_only_an_empty_line) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"|\n  \n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{""});
}

TEST(yaml_block_scalar_with_a_carriage_return_empty_line_before_its_content) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: |\r\n\r\n  x\r\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "\nx\n" })JSON")};
  EXPECT_EQ(result, expected);
}

// Section 8.1.1.1 measures the content indentation in leading spaces, so a
// tabulation is none of it and stands as the first character of the content
TEST(yaml_root_block_scalar_content_line_starting_with_a_tabulation) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"|\n\tx\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"\tx\n"});
}

// Section 7.3.1: "In a multi-line single-quoted scalar, line breaks are
// subject to flow line folding, which discards any trailing white space
// characters"
TEST(yaml_single_quoted_scalar_with_a_trailing_tabulation_before_a_fold) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: 'x\t\n  y'\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "x y" })JSON")};
  EXPECT_EQ(result, expected);
}

// Section 5.4 counts a lone carriage return as a line break
TEST(yaml_single_quoted_scalar_folded_over_a_lone_carriage_return) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a: 'x\r  y'\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "x y" })JSON")};
  EXPECT_EQ(result, expected);
}

// Section 6.1 only demands that a folded line reach the block indentation when
// there is one to reach, and inside a flow collection there is not
TEST(
    yaml_single_quoted_scalar_folded_over_a_tabulation_inside_a_flow_sequence) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: [ 'x\n\ty' ]\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": [ "x y" ] })JSON")};
  EXPECT_EQ(result, expected);
}

// Production 68 writes the escaped tabulation as `ns-esc-tab ::= "t" | #x9`,
// so a backslash before a tabulation character stands for one
TEST(yaml_double_quoted_scalar_with_an_escaped_tabulation_character) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: \"x\\\ty\"\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "x\ty" })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_plain_scalar_closed_by_a_document_end_marker) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a\n...\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a"});
}

// Production 130 writes `ns-plain-char(c)` without excluding the dash, so a
// line of a plain scalar may open on one, and production 206 only forbids a
// document marker that white space or the end of input follows
TEST(yaml_plain_scalar_continued_on_a_line_starting_with_three_dashes) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a\n---foo\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a ---foo"});
}

TEST(yaml_plain_scalar_continued_on_a_line_starting_with_three_dots) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a\n...foo\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a ...foo"});
}

// Section 7.3.3: "Plain scalars are further restricted to a single line when
// contained inside an implicit key", and otherwise fold with the trailing
// white space of each line discarded
TEST(yaml_plain_scalar_whose_last_line_ends_in_a_space) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"x\ny  \n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"x y"});
}

TEST(yaml_plain_scalar_whose_last_line_ends_in_a_tabulation) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"x\ny\t\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"x y"});
}

// Section 6.6 reaches a comment through `s-separate-in-line`, which a
// tabulation satisfies just as a space does
TEST(yaml_plain_scalar_followed_by_a_comment_after_a_tabulation) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a: x\t#c\n"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": "x" })JSON")};
  EXPECT_EQ(result, expected);
}

// Production 130 writes `ns-plain-char(c)` as admitting a colon that an
// `ns-plain-safe(c)` follows, so one inside a word is content
TEST(yaml_colon_inside_a_word_inside_a_flow_sequence) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"[ a:b ]"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON([ "a:b" ])JSON")};
  EXPECT_EQ(result, expected);
}

// Section 6.1 only demands spaces where indentation is called for, and a flow
// collection at the root has none to reach, so a tabulation before a space is
// separation
TEST(yaml_root_flow_sequence_continued_after_a_tabulation_and_a_space) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"[\n\t 1 ]\n"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON([ 1 ])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_plain_scalar_opening_on_a_question_mark) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"?x"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"?x"});
}

TEST(yaml_plain_scalar_opening_on_a_colon) {
  const auto result{sourcemeta::core::parse_yaml(std::string{":x"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{":x"});
}

// Production 130 writes `ns-plain-char(c)` as every `ns-plain-safe(c)` but a
// colon and a comment indicator, so a dash is content wherever it stands, and
// the root node is settled as a scalar by its first line
TEST(yaml_root_plain_scalar_continued_on_a_line_opening_with_a_dash) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a\n- b\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a - b"});
}

TEST(yaml_root_plain_scalar_continued_on_a_line_opening_with_a_question_mark) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a\n? b\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a ? b"});
}

TEST(yaml_sequence_entry_continued_on_a_deeper_line_opening_with_a_dash) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"- a\n  - b\n"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON([ "a - b" ])JSON")};
  EXPECT_EQ(result, expected);
}

// Section 5.4 counts a lone carriage return as a line break, including the one
// that closes a directives end marker at the very end of the input
TEST(yaml_directives_end_marker_followed_by_a_lone_carriage_return) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"---\r"})};
  EXPECT_TRUE(result.is_null());
}

// Production 126 admits a colon that an `ns-plain-safe(c)` follows as the
// first character of a plain scalar
TEST(yaml_plain_scalar_opening_on_a_colon_inside_a_flow_sequence) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"[ :a ]"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON([ ":a" ])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_single_quoted_scalar_folded_over_a_tabulation_and_a_space) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"[ 'x\n\t y' ]"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON([ "x y" ])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_single_quoted_scalar_folded_over_two_breaks_ending_in_a_lone_return) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: 'x\n\r y'\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "x\ny" })JSON")};
  EXPECT_EQ(result, expected);
}

// Section 8.1.1.1: "It is an error for any of the leading empty lines to
// contain more spaces than the first non-empty line". An empty line past the
// first non-empty one is no leading line, and the spaces it carries beyond the
// content indentation are content
TEST(yaml_literal_block_scalar_with_an_empty_line_deeper_than_its_content) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: |\n  x\n    \n  y\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "x\n  \ny\n" })JSON")};
  EXPECT_EQ(result, expected);
}

// Section 8.1.3: "Lines starting with white space characters (more-indented
// lines) are not folded"
TEST(yaml_folded_block_scalar_with_a_line_opening_on_a_tabulation) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: >\n  x\n  \ty\n  z\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "x\n\ty\nz\n" })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_folded_block_scalar_opening_with_an_empty_line) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a: >\n\n  x\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "\nx\n" })JSON")};
  EXPECT_EQ(result, expected);
}

// Production 130 admits a comment indicator only where an `ns-char` precedes
// it, so a line opening on one is no line of the scalar
TEST(yaml_plain_scalar_followed_by_a_line_opening_with_a_comment) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a\n#c\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a"});
}

TEST(yaml_directive_ended_by_a_lone_carriage_return) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"%YAML 1.2\r--- a\r"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a"});
}

// Section 10.2.1.2 gives the boolean tag the canonical forms "true" and
// "false", so a value that is none of the true ones is false
TEST(yaml_boolean_tag_on_a_capitalised_false) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: !!bool False\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": false })JSON")};
  EXPECT_EQ(result, expected);
}

// Section 10.3.2 writes the float form as `[-+]? ( \. [0-9]+ | [0-9]+ ( \.
// [0-9]* )? ) ( [eE] [-+]? [0-9]+ )?`, so a lone decimal point carries no
// digit and resolves to a string
TEST(yaml_lone_decimal_point_is_a_string) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"a: .\n"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": "." })JSON")};
  EXPECT_EQ(result, expected);
}

// Production 86 writes `ns-yaml-directive ::= "YAML" s-separate-in-line
// ns-yaml-version`, and production 66 admits a tabulation as that separation
TEST(yaml_version_directive_separated_by_a_tabulation) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"%YAML\t1.2\n--- a\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a"});
}

// Section 6.8.2 ends the prefix of a tag directive at the separation space, so
// a comment after it is no part of the prefix
TEST(yaml_tag_directive_followed_by_a_comment) {
  const auto result{sourcemeta::core::parse_yaml(
      std::string{"%TAG !e! tag:example.com,2000: # c\n--- !e!foo a\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a"});
}

TEST(yaml_tag_directive_prefix_followed_by_a_tabulation) {
  const auto result{sourcemeta::core::parse_yaml(
      std::string{"%TAG !e! tag:example.com,2000:\t# c\n--- !e!foo a\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a"});
}

// Production 150 writes `ns-flow-pair` for a flow collection entry, and
// Section 7.4 leaves the value of such a pair optional
TEST(yaml_flow_mapping_key_without_a_value) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"{ a }"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": null })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_flow_mapping_two_keys_without_values) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"{ a, b }"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": null, "b": null })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_flow_mapping_entries_separated_without_a_space) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"{a: b,c: d}"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "b", "c": "d" })JSON")};
  EXPECT_EQ(result, expected);
}

// Section 10.2.1.3 gives the string tag, which keeps a key that would
// otherwise resolve to a number as the text it was written as
TEST(yaml_flow_mapping_key_carrying_a_string_tag) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"{ !!str 1: b }"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "1": "b" })JSON")};
  EXPECT_EQ(result, expected);
}

// Production 149 writes the explicit key of a flow pair, and such a pair
// stands as a single entry of the collection holding it
TEST(yaml_flow_sequence_with_an_explicit_key_and_a_value) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"[? a: b]"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON([ { "a": "b" } ])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_block_sequence_closed_by_a_document_end_marker) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"- a\n...\n"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON([ "a" ])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_block_sequence_closed_by_a_directives_end_marker) {
  std::istringstream stream{"- a\n---\n- b\n"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  const auto first_expected{sourcemeta::core::parse_json(R"JSON([ "a" ])JSON")};
  EXPECT_EQ(first, first_expected);

  const auto second{sourcemeta::core::parse_yaml(stream)};
  const auto second_expected{
      sourcemeta::core::parse_json(R"JSON([ "b" ])JSON")};
  EXPECT_EQ(second, second_expected);
}

// Production 187 writes `l+block-mapping(n) ::= ( s-indent(n+m)
// ns-l-block-map-entry(n+m) )+`, so an entry standing further in than the
// mapping holding it belongs to a mapping of its own, which is the value of
// the entry before it
TEST(yaml_block_mapping_value_opening_with_an_explicit_key) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a:\n  ? b\n  : c\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": { "b": "c" } })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_block_mapping_value_with_a_scalar_key_before_an_explicit_key) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a:\n  b: 1\n  ? c\n  : d\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": { "b": 1, "c": "d" } })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_explicit_key_at_the_level_of_the_mapping_holding_it) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: 1\n? b\n: c\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": 1, "b": "c" })JSON")};
  EXPECT_EQ(result, expected);
}

// Production 104 writes `c-ns-alias-node ::= "*" ns-anchor-name` and
// production 102 writes `ns-anchor-char ::= ns-char - c-flow-indicator`, so a
// colon belongs to the name and the indicator needs a space before it
TEST(yaml_alias_as_a_mapping_key) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: &k key\n*k : 1\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": "key", "key": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

// Section 10.2.1.4 gives the mapping tag, which a block mapping may carry on
// the line before its first entry
TEST(yaml_block_mapping_carrying_a_mapping_tag) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"!!map\na: 1\n"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

// Production 183 writes `c-l-block-seq-entry(n) ::= "-" s-l+block-indented`,
// whose node may be empty, and production 206 closes the document at a marker
// of its own
TEST(yaml_sequence_entry_without_a_value_before_a_document_end_marker) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"- a\n-\n...\n"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON([ "a", null ])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_lone_sequence_entry_before_a_document_end_marker) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"-\n...\n"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON([ null ])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_lone_sequence_entry_before_a_directives_end_marker) {
  std::istringstream stream{"-\n---\na\n"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  const auto expected{sourcemeta::core::parse_json(R"JSON([ null ])JSON")};
  EXPECT_EQ(first, expected);

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"a"});
}

// Section 10.2.1.3 gives the string tag, which a later key of a mapping opened
// by an explicit key carries the same way any other node does
TEST(yaml_tagged_key_after_an_explicit_key_entry) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"? a\n: 1\n!!str b: 2\n"})};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "a": 1, "b": 2 })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_mapping_opened_by_an_explicit_key_before_a_document_end_marker) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"? a\n: 1\n...\n"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(yaml_mapping_opened_by_an_explicit_key_before_a_directives_end_marker) {
  std::istringstream stream{"? a\n: 1\n---\nb\n"};
  const auto first{sourcemeta::core::parse_yaml(stream)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  EXPECT_EQ(first, expected);

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second, sourcemeta::core::JSON{"b"});
}

// Production 211 writes the stream as `l-document-prefix* l-any-document? (
// ( l-document-suffix+ l-document-prefix* l-any-document? ) | c-byte-order-mark
// | l-comment | l-explicit-document )*`, so another document may carry
// directives of its own only where an end marker closed the one before it
TEST(yaml_version_directive_in_a_document_after_an_end_marker) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a\n...\n%YAML 1.2\n--- b\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a"});
}

// Production 130 writes `ns-plain-char(c)` as every `ns-plain-safe(c)` but a
// colon and a comment indicator, so a directive indicator opening a later line
// of a plain scalar is content of it rather than a directive
TEST(yaml_plain_scalar_continued_on_a_line_opening_with_a_directive_indicator) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a\n%YAML 1.2\n--- b\n"})};
  EXPECT_EQ(result, sourcemeta::core::JSON{"a %YAML 1.2"});
}

// A round-trip describes the one document it was read from, which an end
// marker closes without anything following it
TEST(yaml_roundtrip_of_a_document_closed_by_its_end_marker) {
  sourcemeta::core::YAMLRoundTrip roundtrip;
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: 1\n...\n"}, roundtrip)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  EXPECT_EQ(result, expected);
}

// The allowance has a ceiling, so a document long enough to claim it does not
// go on buying more with every further byte
TEST(alias_expansion_allowance_reaches_a_ceiling) {
  const std::string input{"filler: " + std::string(100001, 'x') +
                          "\na: &a x\nb: *a\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result.at("b"), sourcemeta::core::JSON{"x"});
}

// YAML 1.2.2 Section 10.3.2 resolves an empty key to null, which this module
// spells as the empty string, and a tag before it names the type that key is
// read as rather than one it has to spell out
TEST(flow_mapping_empty_key_without_a_tag) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"{: 1}"})};
  EXPECT_EQ(result, sourcemeta::core::parse_json(R"JSON({ "": 1 })JSON"));
}

TEST(flow_mapping_empty_key_tagged_as_a_string) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"{!!str : 1}"})};
  EXPECT_EQ(result, sourcemeta::core::parse_json(R"JSON({ "": 1 })JSON"));
}

TEST(flow_mapping_empty_key_tagged_as_null) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"{!!null : 1}"})};
  EXPECT_EQ(result, sourcemeta::core::parse_json(R"JSON({ "": 1 })JSON"));
}

// YAML 1.2.2 production 140 admits a single pair as a flow sequence entry, and
// Section 10.3.2 resolves the value it leaves out to null, whether what
// follows the entry is the end of the sequence or another entry
TEST(flow_sequence_pair_without_a_value_before_another_entry) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"[a: , b]"})};
  EXPECT_EQ(result,
            sourcemeta::core::parse_json(R"JSON([ { "a": null }, "b" ])JSON"));
}

TEST(flow_sequence_pair_without_a_value_at_the_end) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"[b, a: ]"})};
  EXPECT_EQ(result,
            sourcemeta::core::parse_json(R"JSON([ "b", { "a": null } ])JSON"));
}

// YAML 1.2.2 production 194 writes an explicit key after a question mark, and
// production 104 lets an alias node stand wherever a node stands, so one may
// stand as that key and name whatever it was anchored to
TEST(alias_as_an_explicit_mapping_key) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: &k name\n? *k\n: 1\n"})};
  EXPECT_EQ(result, sourcemeta::core::parse_json(
                        R"JSON({ "a": "name", "name": 1 })JSON"));
}

TEST(alias_as_an_explicit_mapping_key_without_a_value) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: &k name\n? *k\n"})};
  EXPECT_EQ(result, sourcemeta::core::parse_json(
                        R"JSON({ "a": "name", "name": null })JSON"));
}

// Production 104 lets an alias node stand wherever a node stands, so one may
// stand as the key of a nested mapping, and production 102 keeps the colon out
// of the anchor name only when a space separates them
TEST(alias_as_a_key_of_a_nested_mapping) {
  const auto result{sourcemeta::core::parse_yaml(
      std::string{"a: &k name\nouter:\n  *k : 1\n"})};
  EXPECT_EQ(result, sourcemeta::core::parse_json(
                        R"JSON({ "a": "name", "outer": { "name": 1 } })JSON"));
}

// Without the colon the alias names no key, so it stands as the value of the
// key above it instead
TEST(alias_alone_under_a_key_is_its_value) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"a: &k name\nouter:\n  *k\n"})};
  EXPECT_EQ(result, sourcemeta::core::parse_json(
                        R"JSON({ "a": "name", "outer": "name" })JSON"));
}

// Production 104 lets an alias node stand wherever a node stands, so one may
// stand as an explicit key, and it names whatever was anchored rather than
// anchoring anything itself
TEST(alias_as_an_explicit_key_naming_an_anchored_value) {
  const auto result{sourcemeta::core::parse_yaml(
      std::string{"? one\n: &k val\n? *k\n: 2\n"})};
  EXPECT_EQ(result, sourcemeta::core::parse_json(
                        R"JSON({ "one": "val", "val": 2 })JSON"));
}

// Production 195 lets an explicit entry leave its value out, which holds for
// an alias standing as that key just as it does for a scalar
TEST(alias_as_an_explicit_key_without_a_value) {
  const auto result{
      sourcemeta::core::parse_yaml(std::string{"? one\n: &k val\n? *k\n"})};
  EXPECT_EQ(result, sourcemeta::core::parse_json(
                        R"JSON({ "one": "val", "val": null })JSON"));
}

// Production 158 admits an empty plain scalar in a flow mapping, which stands
// for nothing rather than for the empty string
TEST(empty_plain_scalar_in_a_flow_mapping) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"{a: }"})};
  EXPECT_EQ(result, sourcemeta::core::parse_json(R"JSON({ "a": null })JSON"));
}

// Production 195 writes the key of an explicit entry as a node, and a question
// mark with no node after it names the empty key
TEST(explicit_key_with_nothing_after_the_question_mark) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"?\n: 1\n"})};
  EXPECT_EQ(result, sourcemeta::core::parse_json(R"JSON({ "": 1 })JSON"));
}

// Production 158 admits an empty value before the comma that separates one
// flow entry from the next
TEST(flow_entry_following_an_empty_value) {
  const auto result{sourcemeta::core::parse_yaml(std::string{"{a: , b: 1}"})};
  EXPECT_EQ(result,
            sourcemeta::core::parse_json(R"JSON({ "a": null, "b": 1 })JSON"));
}

// A block mapping may run to the end of the input with its last value left
// out, with nothing following the separator to mark where the document stops
TEST(block_mapping_in_a_sequence_entry_ending_without_a_newline) {
  const auto result{sourcemeta::core::parse_yaml("- a:")};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON([
    { "a": null }
  ])JSON");

  EXPECT_EQ(result, expected);
}

TEST(nested_block_mapping_ending_without_a_newline) {
  const auto result{sourcemeta::core::parse_yaml("a:\n  b:")};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "a": { "b": null }
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(block_mapping_in_a_sequence_entry_with_a_later_value_left_out) {
  const auto result{sourcemeta::core::parse_yaml("- a: 1\n  b:")};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON([
    { "a": 1, "b": null }
  ])JSON");

  EXPECT_EQ(result, expected);
}

// The end of a document may be marked explicitly, which leaves the value of
// the key before it out just as the end of the input does
TEST(block_mapping_followed_by_an_explicit_document_end) {
  const auto result{sourcemeta::core::parse_yaml("- a:\n...\n")};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON([
    { "a": null }
  ])JSON");

  EXPECT_EQ(result, expected);
}

// A comment carries no value, so a key followed by one holds the null that the
// absent value reads as
TEST(block_mapping_key_with_no_value_followed_by_a_comment) {
  const auto result{sourcemeta::core::parse_yaml("- a:\n# trailing\n")};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON([
    { "a": null }
  ])JSON");

  EXPECT_EQ(result, expected);
}

// An explicitly marked key stands on its own, with the value it would be
// paired with left out entirely
TEST(explicit_key_with_no_value) {
  const auto result{sourcemeta::core::parse_yaml("? a\n")};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "a": null
  })JSON");

  EXPECT_EQ(result, expected);
}

// YAML 1.2.2 Section 9.2 makes a stream "zero or more documents", so a file
// that carries more than one is well formed rather than malformed. An entry
// point that hands back a single document has nowhere to put the rest, so
// what it can say is that there was more to read
TEST(parse_yaml_reports_a_single_document) {
  bool more_documents{true};
  const auto result{sourcemeta::core::parse_yaml("foo: 1", more_documents)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_FALSE(more_documents);
}

TEST(parse_yaml_reports_a_document_that_another_follows) {
  bool more_documents{false};
  const auto result{
      sourcemeta::core::parse_yaml("foo: 1\n---\nbar: 2", more_documents)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(more_documents);
}

TEST(parse_yaml_reports_a_document_closed_by_an_end_marker) {
  bool more_documents{true};
  const auto result{
      sourcemeta::core::parse_yaml("foo: 1\n...\n", more_documents)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_FALSE(more_documents);
}

// YAML 1.2.2 Section 9.1.4: "Since the existence of the document is indicated
// by this marker, the document itself may be completely empty". So a
// directives end marker with nothing after it opens a document of its own
// rather than trailing the one before it
TEST(parse_yaml_reports_an_empty_document_that_follows) {
  bool more_documents{false};
  const auto result{
      sourcemeta::core::parse_yaml("foo: 1\n---\n", more_documents)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(more_documents);
}

TEST(parse_yaml_with_a_callback_reports_a_document_that_another_follows) {
  bool more_documents{false};
  sourcemeta::core::JSON result{nullptr};
  sourcemeta::core::parse_yaml("foo: 1\n---\nbar: 2", more_documents, result,
                               nullptr);
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(more_documents);
}

TEST(parse_yaml_with_a_callback_reports_a_single_document) {
  bool more_documents{true};
  sourcemeta::core::JSON result{nullptr};
  sourcemeta::core::parse_yaml("foo: 1", more_documents, result, nullptr);
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_FALSE(more_documents);
}

TEST(read_yaml_reports_a_document_that_another_follows) {
  bool more_documents{false};
  const auto result{sourcemeta::core::read_yaml(
      std::filesystem::path{STUBS_PATH} / "multi_document_objects.yaml",
      more_documents)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(more_documents);
}

TEST(read_yaml_reports_a_single_document) {
  bool more_documents{true};
  const auto result{sourcemeta::core::read_yaml(
      std::filesystem::path{STUBS_PATH} / "test_1.yaml", more_documents)};
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": "bar", "baz": 2 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_FALSE(more_documents);
}

TEST(read_yaml_with_a_callback_reports_a_document_that_another_follows) {
  bool more_documents{false};
  sourcemeta::core::JSON result{nullptr};
  sourcemeta::core::read_yaml(std::filesystem::path{STUBS_PATH} /
                                  "multi_document_objects.yaml",
                              more_documents, result, nullptr);
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(more_documents);
}

TEST(read_yaml_with_a_callback_reports_a_single_document) {
  bool more_documents{true};
  sourcemeta::core::JSON result{nullptr};
  sourcemeta::core::read_yaml(std::filesystem::path{STUBS_PATH} / "test_1.yaml",
                              more_documents, result, nullptr);
  const sourcemeta::core::JSON expected{
      sourcemeta::core::parse_json(R"JSON({ "foo": "bar", "baz": 2 })JSON")};
  EXPECT_EQ(result, expected);
  EXPECT_FALSE(more_documents);
}

// YAML 1.2.2 production 192 writes an entry as
// `( ns-s-block-map-implicit-key | e-node ) c-l-block-map-implicit-value`, so
// an explicit key whose value is left out takes the empty node for it
TEST(block_plain_key_then_explicit_key_without_a_value) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n? b")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("b").is_null());
}

TEST(block_plain_key_then_two_explicit_keys_where_the_first_has_no_value) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n? b\n? c\n: 3")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("b").is_null());
  EXPECT_EQ(result.at("c").to_integer(), 3);
}

TEST(block_plain_key_then_explicit_key_without_a_value_before_document_end) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n? b\n...\n")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("b").is_null());
}

TEST(block_plain_key_then_explicit_key_without_a_value_before_document_start) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n? b\n---\nc: 3")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("b").is_null());
}

TEST(block_plain_key_then_explicit_entry_then_explicit_key_without_a_value) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n? b\n: 2\n? c")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_EQ(result.at("b").to_integer(), 2);
  EXPECT_TRUE(result.at("c").is_null());
}

// YAML 1.2.2 production 188 writes an explicit key as the indicator followed
// by a node, which production 106 leaves free to hold nothing, so the
// indicator on its own still opens an entry whose key and value are both
// empty, as the trailing entry of Example 7.16 shows. The walk that opens on a
// plain key has to answer for that the same way as the one that opens on an
// explicit key
TEST(block_explicit_key_indicator_alone) {
  const auto result{sourcemeta::core::parse_yaml("?")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at("").is_null());
}

TEST(block_plain_key_then_explicit_key_indicator_alone) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n?")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("").is_null());
}

TEST(block_explicit_entry_then_explicit_key_indicator_alone) {
  const auto result{sourcemeta::core::parse_yaml("? a\n: 1\n?")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("").is_null());
}

TEST(block_plain_key_then_explicit_key_indicator_before_document_start) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n?\n---\nb: 2")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("").is_null());
}

TEST(block_plain_key_then_explicit_key_indicator_before_document_end) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n?\n...\n")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("").is_null());
}

// YAML 1.2.2 Section 8.2.2 holds every entry of a block mapping to the same
// indentation, so an entry at a lesser one belongs to whatever encloses the
// mapping. A nested mapping answers for that the same way whether it opens on
// a plain key or on an explicit one
TEST(block_nested_mapping_opening_on_a_plain_key_ends_at_a_dedented_key) {
  const auto result{sourcemeta::core::parse_yaml("o:\n  a: 1\nb: 2")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("o").size(), 1);
  EXPECT_EQ(result.at("o").at("a").to_integer(), 1);
  EXPECT_EQ(result.at("b").to_integer(), 2);
}

TEST(block_nested_mapping_opening_on_an_explicit_key_ends_at_a_dedented_key) {
  const auto result{sourcemeta::core::parse_yaml("o:\n  ? a\n  : 1\nb: 2")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("o").size(), 1);
  EXPECT_EQ(result.at("o").at("a").to_integer(), 1);
  EXPECT_EQ(result.at("b").to_integer(), 2);
}

TEST(
    block_nested_mapping_opening_on_an_explicit_key_ends_at_a_dedented_explicit_key) {
  const auto result{sourcemeta::core::parse_yaml("o:\n  ? a\n  : 1\n? b\n: 2")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("o").size(), 1);
  EXPECT_EQ(result.at("o").at("a").to_integer(), 1);
  EXPECT_EQ(result.at("b").to_integer(), 2);
}

TEST(
    block_nested_mapping_opening_on_a_plain_key_ends_at_a_dedented_explicit_key) {
  const auto result{sourcemeta::core::parse_yaml("o:\n  a: 1\n? b\n: 2")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("o").size(), 1);
  EXPECT_EQ(result.at("o").at("a").to_integer(), 1);
  EXPECT_EQ(result.at("b").to_integer(), 2);
}

TEST(block_nested_mapping_keeps_the_explicit_entries_at_its_own_indentation) {
  const auto result{
      sourcemeta::core::parse_yaml("o:\n  ? a\n  : 1\n  ? c\n  : 3\nb: 2")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("o").size(), 2);
  EXPECT_EQ(result.at("o").at("a").to_integer(), 1);
  EXPECT_EQ(result.at("o").at("c").to_integer(), 3);
  EXPECT_EQ(result.at("b").to_integer(), 2);
}

// YAML 1.2.2 production 192 takes the empty node for a value that is left out,
// which an explicit key may do wherever the mapping ends
TEST(block_plain_key_then_explicit_entry_with_a_separator_and_no_value) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n? b\n:")};
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("b").is_null());
}

TEST(block_plain_key_then_explicit_entry_with_no_value_before_document_end) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n? b\n:\n...\n")};
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("b").is_null());
}

TEST(block_plain_key_then_explicit_entry_with_no_value_before_document_start) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n? b\n:\n---\nc: 1")};
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("b").is_null());
}

TEST(block_plain_key_then_explicit_entry_with_no_value_before_another_key) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n? b\n:\n? c\n: 2")};
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("b").is_null());
  EXPECT_EQ(result.at("c").to_integer(), 2);
}

TEST(block_plain_key_then_explicit_entry_with_no_value_before_a_separator) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n? b\n:\n: 2")};
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("b").is_null());
  EXPECT_EQ(result.at("").to_integer(), 2);
}

// RFC 8259 Section 4 needs a string for a member name, and a tag standing
// without a scalar beside it names no key, so the empty node answers for it
TEST(block_plain_key_then_a_tag_with_no_scalar) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n!!str\n: 2")};
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_EQ(result.at("").to_integer(), 2);
}

// YAML 1.2.2 Section 7.1: an alias in key position stands for the value of the
// anchor it names, and the entry it opens may still leave its value out
TEST(block_alias_key_with_a_separator_and_no_value) {
  const auto result{sourcemeta::core::parse_yaml("first: &k v\nb: 1\n*k :")};
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("first").to_string(), "v");
  EXPECT_EQ(result.at("b").to_integer(), 1);
  EXPECT_TRUE(result.at("v").is_null());
}

TEST(block_alias_key_with_no_value_before_document_end) {
  const auto result{
      sourcemeta::core::parse_yaml("first: &k v\nb: 1\n*k :\n...\n")};
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("first").to_string(), "v");
  EXPECT_EQ(result.at("b").to_integer(), 1);
  EXPECT_TRUE(result.at("v").is_null());
}

// YAML 1.2.2 production 140 writes a flow mapping over entries separated by
// commas, so one standing before the close names no further entry
TEST(flow_mapping_with_a_trailing_comma) {
  const auto result{sourcemeta::core::parse_yaml("{a: b,}")};
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("a").to_string(), "b");
}

// YAML 1.2.2 production 150 reads a single pair of a flow sequence as a
// mapping, whose value may be left out
TEST(flow_sequence_explicit_pair_without_a_value) {
  const auto result{sourcemeta::core::parse_yaml("[? a : ]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).at("a").is_null());
}

TEST(flow_sequence_explicit_pair_without_a_value_before_another_entry) {
  const auto result{sourcemeta::core::parse_yaml("[? a : , b]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).at("a").is_null());
  EXPECT_EQ(result.at(1).to_string(), "b");
}

// YAML 1.2.2 production 142 writes an entry of a flow mapping with an optional
// explicit key indicator, after which Section 7.4.2 leaves the rest of the
// entry free to be left out altogether
TEST(flow_mapping_explicit_key) {
  const auto result{sourcemeta::core::parse_yaml("{? a : b}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("a").to_string(), "b");
}

// Example 7.16 reads an explicit entry, an implicit entry and an entry that is
// nothing but the indicator from the one flow mapping
TEST(flow_mapping_explicit_and_implicit_entries) {
  const auto result{sourcemeta::core::parse_yaml(
      "{\n? explicit: entry,\nimplicit: entry,\n?\n}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("explicit").to_string(), "entry");
  EXPECT_EQ(result.at("implicit").to_string(), "entry");
  EXPECT_TRUE(result.at("").is_null());
}

// Example 7.3 leaves the value of one entry and the key of the next empty
TEST(flow_mapping_completely_empty_nodes) {
  const auto result{sourcemeta::core::parse_yaml("{\n  ? foo :,\n  : bar,\n}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at("foo").is_null());
  EXPECT_EQ(result.at("").to_string(), "bar");
}

TEST(flow_mapping_explicit_key_without_a_value) {
  const auto result{sourcemeta::core::parse_yaml("{? a :}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at("a").is_null());
}

TEST(flow_mapping_explicit_key_indicator_alone) {
  const auto result{sourcemeta::core::parse_yaml("{? }")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at("").is_null());
}

TEST(flow_mapping_explicit_key_indicator_before_another_entry) {
  const auto result{sourcemeta::core::parse_yaml("{? , a: 1}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at("").is_null());
  EXPECT_EQ(result.at("a").to_integer(), 1);
}

// YAML 1.2.2 Section 7.1 reads an alias node in key position as the value of
// the anchor it names
TEST(flow_mapping_alias_key) {
  const auto result{sourcemeta::core::parse_yaml("{a: &x v, *x : b}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_string(), "v");
  EXPECT_EQ(result.at("v").to_string(), "b");
}

TEST(flow_mapping_explicit_alias_key) {
  const auto result{sourcemeta::core::parse_yaml("{a: &x v, ? *x : b}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_string(), "v");
  EXPECT_EQ(result.at("v").to_string(), "b");
}

TEST(flow_mapping_anchored_key) {
  const auto result{sourcemeta::core::parse_yaml("{&x a : b, c: *x}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_string(), "b");
  EXPECT_EQ(result.at("c").to_string(), "a");
}

TEST(flow_mapping_anchored_explicit_key) {
  const auto result{sourcemeta::core::parse_yaml("{? &x a : b, c: *x}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_string(), "b");
  EXPECT_EQ(result.at("c").to_string(), "a");
}

TEST(flow_mapping_anchored_empty_key) {
  const auto result{sourcemeta::core::parse_yaml("{&x : b, c: *x}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("").to_string(), "b");
  EXPECT_TRUE(result.at("c").is_null());
}

TEST(flow_mapping_anchored_empty_explicit_key) {
  const auto result{sourcemeta::core::parse_yaml("{? &x : b, c: *x}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("").to_string(), "b");
  EXPECT_TRUE(result.at("c").is_null());
}

TEST(flow_mapping_tagged_explicit_key) {
  const auto result{sourcemeta::core::parse_yaml("{? !!int 1 : b}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("1").to_string(), "b");
}

// YAML 1.2.2 production 161 writes a flow node as node properties followed by
// either content or nothing at all
TEST(flow_sequence_anchored_empty_entry) {
  const auto result{sourcemeta::core::parse_yaml("[ &a ]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_null());
}

TEST(flow_sequence_anchored_empty_entry_aliased) {
  const auto result{sourcemeta::core::parse_yaml("[ &a , *a ]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_null());
  EXPECT_TRUE(result.at(1).is_null());
}

TEST(flow_sequence_anchored_tagged_empty_entry_aliased) {
  const auto result{sourcemeta::core::parse_yaml("[ &a !!str , *a ]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).to_string(), "");
  EXPECT_EQ(result.at(1).to_string(), "");
}

TEST(flow_sequence_tagged_empty_entry) {
  const auto result{sourcemeta::core::parse_yaml("[ !!str ]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).to_string(), "");
}

TEST(flow_mapping_anchored_empty_value) {
  const auto result{sourcemeta::core::parse_yaml("{a: &x , b: 1}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at("a").is_null());
  EXPECT_EQ(result.at("b").to_integer(), 1);
}

TEST(flow_mapping_anchored_empty_value_aliased) {
  const auto result{sourcemeta::core::parse_yaml("{a: &x , b: *x}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at("a").is_null());
  EXPECT_TRUE(result.at("b").is_null());
}

TEST(flow_mapping_anchored_tagged_empty_value_aliased) {
  const auto result{sourcemeta::core::parse_yaml("{a: &x !!str , b: *x}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_string(), "");
  EXPECT_EQ(result.at("b").to_string(), "");
}

// YAML 1.2.2 production 150 reads a single pair of a flow sequence as a
// mapping, whose key and value production 143 leaves free to hold nothing
TEST(flow_sequence_explicit_pair_indicator_alone) {
  const auto result{sourcemeta::core::parse_yaml("[? ]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_TRUE(result.at(0).at("").is_null());
}

TEST(flow_sequence_explicit_pair_indicator_before_another_entry) {
  const auto result{sourcemeta::core::parse_yaml("[? , a]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_TRUE(result.at(0).at("").is_null());
  EXPECT_EQ(result.at(1).to_string(), "a");
}

TEST(flow_sequence_explicit_pair_with_an_empty_key_and_value) {
  const auto result{sourcemeta::core::parse_yaml("[? :]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_TRUE(result.at(0).at("").is_null());
}

// Production 146 writes a pair whose key is left out as the value indicator
// alone, and production 147 leaves the value after it free to be empty too
TEST(flow_sequence_pair_with_no_key_or_value) {
  const auto result{sourcemeta::core::parse_yaml("[ : ]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_TRUE(result.at(0).at("").is_null());
}

TEST(flow_sequence_pair_with_no_key_before_another_entry) {
  const auto result{sourcemeta::core::parse_yaml("[ : , a]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_TRUE(result.at(0).at("").is_null());
  EXPECT_EQ(result.at(1).to_string(), "a");
}

TEST(flow_sequence_anchored_explicit_pair_key) {
  const auto result{sourcemeta::core::parse_yaml("[? &x a : b, *x]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_EQ(result.at(0).at("a").to_string(), "b");
  EXPECT_EQ(result.at(1).to_string(), "a");
}

TEST(flow_sequence_anchored_empty_explicit_pair_key) {
  const auto result{sourcemeta::core::parse_yaml("[? &x : b, *x]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_EQ(result.at(0).at("").to_string(), "b");
  EXPECT_TRUE(result.at(1).is_null());
}

TEST(flow_sequence_tagged_explicit_pair_key) {
  const auto result{sourcemeta::core::parse_yaml("[? !!int 1 : b]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_EQ(result.at(0).at("1").to_string(), "b");
}

TEST(flow_sequence_pair_quoted_key_with_an_adjacent_value) {
  const std::string input{R"([ "JSON like":adjacent ])"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_EQ(result.at(0).at("JSON like").to_string(), "adjacent");
}

// A plain key is not JSON-like, so the colon inside it is content rather than
// an indicator standing next to its value
TEST(flow_mapping_plain_key_holding_a_colon) {
  const auto result{sourcemeta::core::parse_yaml("{a:b}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at("a:b").is_null());
}

// YAML 1.2.2 production 193 reads node properties standing before a value
// indicator as the implicit key of the entry, which the mapping is then read
// against rather than against where the indicator stands
TEST(block_mapping_tagged_empty_key_then_another_entry) {
  const auto result{sourcemeta::core::parse_yaml("!!null : a\nb: 1")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("").to_string(), "a");
  EXPECT_EQ(result.at("b").to_integer(), 1);
}

TEST(block_mapping_tagged_empty_key_in_a_sequence_entry) {
  const auto result{sourcemeta::core::parse_yaml("-\n  !!null : a\n  b: 1")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).size(), 2);
  EXPECT_EQ(result.at(0).at("").to_string(), "a");
  EXPECT_EQ(result.at(0).at("b").to_integer(), 1);
}

TEST(block_mapping_tagged_empty_key_as_a_value) {
  const auto result{sourcemeta::core::parse_yaml("x:\n  !!null : a\n  b: 1")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("x").size(), 2);
  EXPECT_EQ(result.at("x").at("").to_string(), "a");
  EXPECT_EQ(result.at("x").at("b").to_integer(), 1);
}

TEST(block_mapping_anchored_empty_key) {
  const auto result{sourcemeta::core::parse_yaml("&x : b\nc: *x")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("").to_string(), "b");
  EXPECT_TRUE(result.at("c").is_null());
}

TEST(block_mapping_anchored_empty_key_after_another_entry) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n&x : b\nc: *x")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_EQ(result.at("").to_string(), "b");
  EXPECT_TRUE(result.at("c").is_null());
}

TEST(block_mapping_tagged_empty_key_after_another_entry) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n!!str : b")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_EQ(result.at("").to_string(), "b");
}

TEST(block_mapping_anchored_key_after_another_entry) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n&x b: c\nd: *x")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_EQ(result.at("b").to_string(), "c");
  EXPECT_EQ(result.at("d").to_string(), "b");
}

TEST(block_mapping_anchored_and_tagged_key_after_another_entry) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n&x !!str b: c\nd: *x")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_EQ(result.at("b").to_string(), "c");
  EXPECT_EQ(result.at("d").to_string(), "b");
}

TEST(block_mapping_explicit_anchored_key_with_an_anchored_value) {
  const auto result{sourcemeta::core::parse_yaml("? &a a\n: &b b\n: 1")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_string(), "b");
  EXPECT_EQ(result.at("").to_integer(), 1);
}

TEST(block_mapping_explicit_anchored_key_aliased_in_a_later_key) {
  const auto result{sourcemeta::core::parse_yaml("? &a a\n: &b b\n: *a")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_string(), "b");
  EXPECT_EQ(result.at("").to_string(), "a");
}

TEST(block_mapping_explicit_tagged_key_after_another_entry) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n? !!int 2\n: 3")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_EQ(result.at("2").to_integer(), 3);
}

TEST(block_mapping_explicit_anchored_and_tagged_key_after_another_entry) {
  const auto result{
      sourcemeta::core::parse_yaml("a: 1\n? &x !!str b\n: 2\nc: *x")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_EQ(result.at("b").to_integer(), 2);
  EXPECT_EQ(result.at("c").to_string(), "b");
}

TEST(block_mapping_explicit_key_indicator_alone_in_a_sequence_entry) {
  const auto result{sourcemeta::core::parse_yaml("-\n  ? &d\n-\n  a")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_TRUE(result.at(0).at("").is_null());
  EXPECT_EQ(result.at(1).to_string(), "a");
}

TEST(block_mapping_explicit_anchored_key_indicator_alone_aliased) {
  const auto result{sourcemeta::core::parse_yaml("? &d\nc: *d")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at("").is_null());
  EXPECT_TRUE(result.at("c").is_null());
}

// YAML 1.2.2 production 190 reads the key of an explicit entry as a node
// indented further than the mapping, so a key standing at the indentation of
// the mapping itself opens the next entry instead
TEST(block_mapping_explicit_key_indicator_then_a_key_at_the_indentation) {
  const auto result{sourcemeta::core::parse_yaml("?\na: 1")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at("").is_null());
  EXPECT_EQ(result.at("a").to_integer(), 1);
}

TEST(
    block_mapping_explicit_key_indicator_then_a_key_at_the_indentation_after_an_entry) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n?\nb: 2")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("").is_null());
  EXPECT_EQ(result.at("b").to_integer(), 2);
}

TEST(block_mapping_explicit_key_indicator_then_another_explicit_entry) {
  const auto result{sourcemeta::core::parse_yaml("?\n? b\n: 2")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at("").is_null());
  EXPECT_EQ(result.at("b").to_integer(), 2);
}

TEST(
    block_mapping_explicit_key_indicator_then_another_explicit_entry_after_an_entry) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n?\n? b\n: 2")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_TRUE(result.at("").is_null());
  EXPECT_EQ(result.at("b").to_integer(), 2);
}

// A colon that no space follows is content of the plain scalar rather than the
// indicator of a compact mapping
TEST(block_mapping_explicit_key_holding_a_colon) {
  const auto result{sourcemeta::core::parse_yaml("? a:b\n: c")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("a:b").to_string(), "c");
}

TEST(block_mapping_explicit_block_scalar_key) {
  const auto result{sourcemeta::core::parse_yaml("? |\n  text\n: value")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("text\n").to_string(), "value");
}

TEST(block_mapping_flow_sequence_followed_by_a_comment) {
  const auto result{sourcemeta::core::parse_yaml("[a] # c: d")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at(0).to_string(), "a");
}

TEST(block_mapping_flow_sequence_then_an_entry_with_no_key) {
  const auto result{sourcemeta::core::parse_yaml("k: [a]\n: b")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("k").size(), 1);
  EXPECT_EQ(result.at("k").at(0).to_string(), "a");
  EXPECT_EQ(result.at("").to_string(), "b");
}

// Production 201 reads a sequence at the indentation of the mapping as the
// value of its entry, so one standing before that belongs to whatever encloses
// the mapping even where node properties left the value empty
TEST(block_sequence_entry_after_a_tagged_empty_value) {
  const auto result{sourcemeta::core::parse_yaml("-\n  b: !!str\n- x")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_EQ(result.at(0).at("b").to_string(), "");
  EXPECT_EQ(result.at(1).to_string(), "x");
}

TEST(block_sequence_entry_after_an_anchored_empty_value) {
  const auto result{sourcemeta::core::parse_yaml("-\n  b: &y\n- x")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_TRUE(result.at(0).at("b").is_null());
  EXPECT_EQ(result.at(1).to_string(), "x");
}

TEST(block_sequence_entry_after_an_empty_value) {
  const auto result{sourcemeta::core::parse_yaml("-\n  b:\n- x")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_TRUE(result.at(0).at("b").is_null());
  EXPECT_EQ(result.at(1).to_string(), "x");
}

TEST(block_mapping_value_indicator_alone) {
  const auto result{sourcemeta::core::parse_yaml(":\n\n\n")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at("").is_null());
}

TEST(block_mapping_entries_with_nothing_on_either_side) {
  const std::string input{
      "plain key: in-line value\n: # Both empty\n\"quoted key\":\n- entry"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("plain key").to_string(), "in-line value");
  EXPECT_TRUE(result.at("").is_null());
  EXPECT_EQ(result.at("quoted key").size(), 1);
  EXPECT_EQ(result.at("quoted key").at(0).to_string(), "entry");
}

// Example 7.17 separates one value from its key, leaves another value out and
// leaves one key out
TEST(flow_mapping_separate_values) {
  const std::string input{"{\nunquoted : \"separate\",\nhttp://foo.com,\n"
                          "omitted value:,\n: omitted key,\n}"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 4);
  EXPECT_EQ(result.at("unquoted").to_string(), "separate");
  EXPECT_TRUE(result.at("http://foo.com").is_null());
  EXPECT_TRUE(result.at("omitted value").is_null());
  EXPECT_EQ(result.at("").to_string(), "omitted key");
}

TEST(block_sequence_of_tagged_empty_nodes) {
  const std::string input{
      "- !!str\n-\n  !!null : a\n  b: !!str\n- !!str : !!null"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at(0).to_string(), "");
  EXPECT_EQ(result.at(1).size(), 2);
  EXPECT_EQ(result.at(1).at("").to_string(), "a");
  EXPECT_EQ(result.at(1).at("b").to_string(), "");
  EXPECT_EQ(result.at(2).size(), 1);
  EXPECT_TRUE(result.at(2).at("").is_null());
}

TEST(block_sequence_of_anchored_empty_nodes) {
  const std::string input{"- &a\n- a\n-\n  &a : a\n  b: &b\n-\n  &c : &a\n-\n "
                          " ? &d\n-\n  ? &e\n  : &a"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 6);
  EXPECT_TRUE(result.at(0).is_null());
  EXPECT_EQ(result.at(1).to_string(), "a");
  EXPECT_EQ(result.at(2).size(), 2);
  EXPECT_EQ(result.at(2).at("").to_string(), "a");
  EXPECT_TRUE(result.at(2).at("b").is_null());
  EXPECT_EQ(result.at(3).size(), 1);
  EXPECT_TRUE(result.at(3).at("").is_null());
  EXPECT_EQ(result.at(4).size(), 1);
  EXPECT_TRUE(result.at(4).at("").is_null());
  EXPECT_EQ(result.at(5).size(), 1);
  EXPECT_TRUE(result.at(5).at("").is_null());
}

TEST(flow_sequence_pairs_with_no_key) {
  const auto result{
      sourcemeta::core::parse_yaml("- [ : empty key ]\n- [: another empty]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_EQ(result.at(0).at(0).size(), 1);
  EXPECT_EQ(result.at(0).at(0).at("").to_string(), "empty key");
  EXPECT_EQ(result.at(1).size(), 1);
  EXPECT_EQ(result.at(1).at(0).size(), 1);
  EXPECT_EQ(result.at(1).at(0).at("").to_string(), "another empty");
}

TEST(multi_document_entries_with_no_key) {
  const std::string input{
      "---\nkey: value\n: empty key\n---\n{\n key: value, : empty key\n}\n"
      "---\n# empty key and value\n:\n---\n# empty key and value\n:\n"};
  std::istringstream stream{input};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(first.is_object());
  EXPECT_EQ(first.size(), 2);
  EXPECT_EQ(first.at("key").to_string(), "value");
  EXPECT_EQ(first.at("").to_string(), "empty key");

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(second.is_object());
  EXPECT_EQ(second.size(), 2);
  EXPECT_EQ(second.at("key").to_string(), "value");
  EXPECT_EQ(second.at("").to_string(), "empty key");

  const auto third{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(third.is_object());
  EXPECT_EQ(third.size(), 1);
  EXPECT_TRUE(third.at("").is_null());

  const auto fourth{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(fourth.is_object());
  EXPECT_EQ(fourth.size(), 1);
  EXPECT_TRUE(fourth.at("").is_null());

  EXPECT_EQ(stream.peek(), EOF);
}

TEST(block_mapping_explicit_alias_key_at_the_indentation) {
  const auto result{sourcemeta::core::parse_yaml("? x\n: &v 1\n?\n*v : 2")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("x").to_integer(), 1);
  EXPECT_TRUE(result.at("").is_null());
  EXPECT_EQ(result.at("1").to_integer(), 2);
}

TEST(block_mapping_explicit_alias_key_at_the_indentation_after_an_entry) {
  const auto result{
      sourcemeta::core::parse_yaml("a: 1\n? x\n: &v 2\n?\n*v : 3")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 4);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_EQ(result.at("x").to_integer(), 2);
  EXPECT_TRUE(result.at("").is_null());
  EXPECT_EQ(result.at("2").to_integer(), 3);
}

TEST(block_mapping_value_indicator_alone_before_an_explicit_entry) {
  const auto result{sourcemeta::core::parse_yaml(":\n? a\n: b")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at("").is_null());
  EXPECT_EQ(result.at("a").to_string(), "b");
}

TEST(block_mapping_indented_scalar_after_an_anchored_value) {
  const auto result{sourcemeta::core::parse_yaml("a: &x\n  b")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("a").to_string(), "b");
}

// An anchor on the line a mapping opens on names the empty key of that entry,
// where one on a line of its own names the mapping the entries make up
TEST(block_mapping_anchor_before_an_explicit_key_on_a_later_line) {
  const auto result{sourcemeta::core::parse_yaml("&x\n? a\n: 1")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("a").to_integer(), 1);
}

TEST(block_mapping_anchor_before_a_value_indicator_on_a_later_line) {
  const auto result{sourcemeta::core::parse_yaml("&x\n: 1")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("").to_integer(), 1);
}

TEST(
    block_mapping_explicit_key_indicator_alone_before_a_dedented_sequence_entry) {
  const auto result{sourcemeta::core::parse_yaml("-\n  a: 1\n  ? &d\n- x")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).size(), 2);
  EXPECT_EQ(result.at(0).at("a").to_integer(), 1);
  EXPECT_TRUE(result.at(0).at("").is_null());
  EXPECT_EQ(result.at(1).to_string(), "x");
}

TEST(block_mapping_explicit_key_with_no_value_before_another_explicit_entry) {
  const auto result{sourcemeta::core::parse_yaml("? a\n:\n? b\n: 1")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at("a").is_null());
  EXPECT_EQ(result.at("b").to_integer(), 1);
}

TEST(multi_document_entry_with_no_value_before_a_document_start) {
  std::istringstream stream{"a: 1\nb:\n---\nc: 2"};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(first.is_object());
  EXPECT_EQ(first.size(), 2);
  EXPECT_EQ(first.at("a").to_integer(), 1);
  EXPECT_TRUE(first.at("b").is_null());

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(second.is_object());
  EXPECT_EQ(second.size(), 1);
  EXPECT_EQ(second.at("c").to_integer(), 2);

  EXPECT_EQ(stream.peek(), EOF);
}

TEST(multi_document_explicit_entry_with_no_value_before_a_document_start) {
  std::istringstream stream{"? a\n: 1\n? b\n:\n---\nc"};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(first.is_object());
  EXPECT_EQ(first.size(), 2);
  EXPECT_EQ(first.at("a").to_integer(), 1);
  EXPECT_TRUE(first.at("b").is_null());

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second.to_string(), "c");

  EXPECT_EQ(stream.peek(), EOF);
}

// YAML 1.2.2 Section 8.2.1 leaves the value of a sequence entry free to be
// empty, which the entry standing after it or the end of the stream shows
TEST(block_sequence_entry_with_no_value_between_two_entries) {
  const auto result{sourcemeta::core::parse_yaml("- a\n-\n- b")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at(0).to_string(), "a");
  EXPECT_TRUE(result.at(1).is_null());
  EXPECT_EQ(result.at(2).to_string(), "b");
}

TEST(block_sequence_of_entries_with_no_value) {
  const auto result{sourcemeta::core::parse_yaml("-\n-")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_null());
  EXPECT_TRUE(result.at(1).is_null());
}

// YAML 1.2.2 production 194 leaves the value of an entry free to be empty, so
// an alias key whose value is left out still names an entry, and a document
// boundary standing there closes the mapping
TEST(multi_document_alias_key_with_no_value_before_a_document_start) {
  std::istringstream stream{"a: &v 1\n*v :\n---\nx"};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(first.is_object());
  EXPECT_EQ(first.size(), 2);
  EXPECT_EQ(first.at("a").to_integer(), 1);
  EXPECT_TRUE(first.at("1").is_null());

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second.to_string(), "x");

  EXPECT_EQ(stream.peek(), EOF);
}

TEST(multi_document_alias_key_with_no_value_before_a_document_end) {
  std::istringstream stream{"a: &v 1\n*v :\n...\nx"};

  const auto first{sourcemeta::core::parse_yaml(stream)};
  EXPECT_TRUE(first.is_object());
  EXPECT_EQ(first.size(), 2);
  EXPECT_EQ(first.at("a").to_integer(), 1);
  EXPECT_TRUE(first.at("1").is_null());

  const auto second{sourcemeta::core::parse_yaml(stream)};
  EXPECT_EQ(second.to_string(), "x");

  EXPECT_EQ(stream.peek(), EOF);
}

// Production 150 reads a single pair of a flow sequence as a mapping whether
// the key carries the explicit indicator or not, so an alias names the key of
// one as readily as of the other
TEST(flow_sequence_explicit_alias_pair_key) {
  const auto result{sourcemeta::core::parse_yaml("[&v q, ? *v : b]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).to_string(), "q");
  EXPECT_EQ(result.at(1).size(), 1);
  EXPECT_EQ(result.at(1).at("q").to_string(), "b");
}

TEST(flow_sequence_alias_pair_key) {
  const auto result{sourcemeta::core::parse_yaml("[&v q, *v : b]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).to_string(), "q");
  EXPECT_EQ(result.at(1).size(), 1);
  EXPECT_EQ(result.at(1).at("q").to_string(), "b");
}

// YAML 1.2.2 Section 10.3.2 resolves a node that holds nothing to null, which
// a tag may make stand for something else, so an anchor on an empty key names
// what the tag leaves that key as
TEST(flow_mapping_anchored_tagged_empty_key_aliased) {
  const auto result{sourcemeta::core::parse_yaml("{? &x !!str : b, c: *x}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("").to_string(), "b");
  EXPECT_EQ(result.at("c").to_string(), "");
}

TEST(flow_mapping_anchored_empty_key_aliased) {
  const auto result{sourcemeta::core::parse_yaml("{? &x : b, c: *x}")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("").to_string(), "b");
  EXPECT_TRUE(result.at("c").is_null());
}

TEST(block_mapping_anchored_tagged_empty_key_aliased) {
  const auto result{sourcemeta::core::parse_yaml("&x !!str : b\nc: *x")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("").to_string(), "b");
  EXPECT_EQ(result.at("c").to_string(), "");
}

TEST(block_mapping_anchored_tagged_empty_explicit_key_aliased) {
  const auto result{sourcemeta::core::parse_yaml("? &x !!str\n: b\nc: *x")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("").to_string(), "b");
  EXPECT_EQ(result.at("c").to_string(), "");
}

TEST(block_mapping_anchored_tagged_empty_key_aliased_after_another_entry) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n&x !!str : b\nc: *x")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_EQ(result.at("").to_string(), "b");
  EXPECT_EQ(result.at("c").to_string(), "");
}

TEST(
    block_mapping_anchored_tagged_empty_explicit_key_aliased_after_another_entry) {
  const auto result{
      sourcemeta::core::parse_yaml("a: 1\n? &x !!str\n: b\nc: *x")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at("a").to_integer(), 1);
  EXPECT_EQ(result.at("").to_string(), "b");
  EXPECT_EQ(result.at("c").to_string(), "");
}

TEST(flow_sequence_anchored_tagged_empty_explicit_pair_key_aliased) {
  const auto result{sourcemeta::core::parse_yaml("[? &x !!str : b, *x]")};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_EQ(result.at(0).at("").to_string(), "b");
  EXPECT_EQ(result.at(1).to_string(), "");
}

// A node property introduces the key it decorates, so one standing before
// something that can be no key at all leaves the mapping with nothing further
// to read
TEST(block_mapping_node_property_with_no_key_after_it) {
  const auto result{sourcemeta::core::parse_yaml("a: 1\n&x")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("a").to_integer(), 1);
}

// An alias key standing at a lesser indentation than the mapping it follows
// names an entry of whatever encloses that mapping
TEST(block_mapping_dedented_alias_key_closes_the_nested_mapping) {
  const auto result{sourcemeta::core::parse_yaml("x:\n  a: &v 1\n*v : b")};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result.at("x").size(), 1);
  EXPECT_EQ(result.at("x").at("a").to_integer(), 1);
  EXPECT_EQ(result.at("1").to_string(), "b");
}

// YAML 1.2.2 Section 6.8.1: a document whose minor version the reader does not
// know is still read, where one naming a major version it does not know is not,
// so the version this reader is written against is read on its own terms
TEST(yaml_directive_supported_version_is_accepted) {
  const std::string input{"%YAML 1.2\n--- value\n"};
  const auto result{sourcemeta::core::parse_yaml(input)};
  EXPECT_EQ(result, sourcemeta::core::JSON{"value"});
  EXPECT_TRUE(result.is_string());
}

// The allowance an alias draws on is a ratio to the text that has been read
// rather than a fixed count, so a chain that is turned away in a document
// carrying nothing else is expanded in full once the document is longer
TEST(alias_expansion_allowance_scales_with_the_input) {
  const std::string aliases{"a: &a [ x, x, x, x, x, x, x, x, x, x ]\n"
                            "b: &b [ *a, *a, *a, *a, *a, *a, *a, *a, *a, *a ]\n"
                            "c: &c [ *b, *b, *b, *b, *b, *b, *b, *b, *b, *b ]\n"
                            "d: &d [ *c, *c, *c, *c, *c, *c, *c, *c, *c, *c ]\n"
                            "e: [ *d, *d, *d, *d, *d, *d, *d, *d, *d, *d ]\n"};
  const auto result{sourcemeta::core::parse_yaml(
      "filler: " + std::string(2000, 'x') + "\n" + aliases)};

  const sourcemeta::core::JSON expected_leaf{sourcemeta::core::parse_json(
      R"JSON([ "x", "x", "x", "x", "x", "x", "x", "x", "x", "x" ])JSON")};

  EXPECT_EQ(result.at("e").size(), 10);
  EXPECT_EQ(result.at("e").at(9).at(9).at(9).at(9), expected_leaf);
}

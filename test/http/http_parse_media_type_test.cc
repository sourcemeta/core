#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

TEST(simple) {
  const auto result{
      sourcemeta::core::http_parse_media_type("application/json")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "application");
  EXPECT_EQ(result.value().subtype, "json");
  EXPECT_EQ(result.value().suffix, "");
  EXPECT_EQ(result.value().parameters, "");
}

TEST(structured_syntax_suffix) {
  const auto result{
      sourcemeta::core::http_parse_media_type("application/geo+json")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "application");
  EXPECT_EQ(result.value().subtype, "geo+json");
  EXPECT_EQ(result.value().suffix, "+json");
  EXPECT_EQ(result.value().parameters, "");
}

TEST(structured_syntax_suffix_sequence) {
  const auto result{
      sourcemeta::core::http_parse_media_type("application/geo+json-seq")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "application");
  EXPECT_EQ(result.value().subtype, "geo+json-seq");
  EXPECT_EQ(result.value().suffix, "+json-seq");
  EXPECT_EQ(result.value().parameters, "");
}

TEST(structured_syntax_suffix_last_plus_wins) {
  const auto result{
      sourcemeta::core::http_parse_media_type("application/vnd.foo+bar+json")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "application");
  EXPECT_EQ(result.value().subtype, "vnd.foo+bar+json");
  EXPECT_EQ(result.value().suffix, "+json");
  EXPECT_EQ(result.value().parameters, "");
}

TEST(structured_syntax_suffix_leading_plus) {
  const auto result{
      sourcemeta::core::http_parse_media_type("application/+json")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "application");
  EXPECT_EQ(result.value().subtype, "+json");
  EXPECT_EQ(result.value().suffix, "+json");
  EXPECT_EQ(result.value().parameters, "");
}

TEST(structured_syntax_suffix_trailing_plus) {
  const auto result{
      sourcemeta::core::http_parse_media_type("application/json+")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "application");
  EXPECT_EQ(result.value().subtype, "json+");
  EXPECT_EQ(result.value().suffix, "");
  EXPECT_EQ(result.value().parameters, "");
}

TEST(single_parameter) {
  const auto result{
      sourcemeta::core::http_parse_media_type("text/html; charset=UTF-8")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "text");
  EXPECT_EQ(result.value().subtype, "html");
  EXPECT_EQ(result.value().suffix, "");
  EXPECT_EQ(result.value().parameters, "; charset=UTF-8");
}

TEST(single_parameter_without_space) {
  const auto result{
      sourcemeta::core::http_parse_media_type("text/html;charset=UTF-8")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "text");
  EXPECT_EQ(result.value().subtype, "html");
  EXPECT_EQ(result.value().suffix, "");
  EXPECT_EQ(result.value().parameters, ";charset=UTF-8");
}

TEST(multiple_parameters) {
  const auto result{sourcemeta::core::http_parse_media_type(
      "multipart/form-data; boundary=abc; charset=UTF-8")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "multipart");
  EXPECT_EQ(result.value().subtype, "form-data");
  EXPECT_EQ(result.value().suffix, "");
  EXPECT_EQ(result.value().parameters, "; boundary=abc; charset=UTF-8");
}

TEST(parameter_with_quoted_semicolon) {
  const auto result{
      sourcemeta::core::http_parse_media_type("text/html; x=\"a;b\"")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "text");
  EXPECT_EQ(result.value().subtype, "html");
  EXPECT_EQ(result.value().suffix, "");
  EXPECT_EQ(result.value().parameters, "; x=\"a;b\"");
}

TEST(whitespace_before_parameters) {
  const auto result{
      sourcemeta::core::http_parse_media_type("text/html ; charset=UTF-8")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "text");
  EXPECT_EQ(result.value().subtype, "html");
  EXPECT_EQ(result.value().suffix, "");
  EXPECT_EQ(result.value().parameters, "; charset=UTF-8");
}

TEST(leading_whitespace) {
  const auto result{
      sourcemeta::core::http_parse_media_type("   application/json")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "application");
  EXPECT_EQ(result.value().subtype, "json");
  EXPECT_EQ(result.value().suffix, "");
  EXPECT_EQ(result.value().parameters, "");
}

TEST(trailing_whitespace) {
  const auto result{
      sourcemeta::core::http_parse_media_type("application/json   ")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "application");
  EXPECT_EQ(result.value().subtype, "json");
  EXPECT_EQ(result.value().suffix, "");
  EXPECT_EQ(result.value().parameters, "");
}

TEST(case_is_preserved) {
  const auto result{
      sourcemeta::core::http_parse_media_type("Application/JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "Application");
  EXPECT_EQ(result.value().subtype, "JSON");
  EXPECT_EQ(result.value().suffix, "");
  EXPECT_EQ(result.value().parameters, "");
}

TEST(range_all) {
  const auto result{sourcemeta::core::http_parse_media_type("*/*")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "*");
  EXPECT_EQ(result.value().subtype, "*");
  EXPECT_EQ(result.value().suffix, "");
  EXPECT_EQ(result.value().parameters, "");
}

TEST(range_subtype) {
  const auto result{sourcemeta::core::http_parse_media_type("text/*")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().type, "text");
  EXPECT_EQ(result.value().subtype, "*");
  EXPECT_EQ(result.value().suffix, "");
  EXPECT_EQ(result.value().parameters, "");
}

TEST(invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::http_parse_media_type("").has_value());
}

TEST(invalid_no_slash) {
  EXPECT_FALSE(
      sourcemeta::core::http_parse_media_type("application").has_value());
}

TEST(invalid_empty_type) {
  EXPECT_FALSE(sourcemeta::core::http_parse_media_type("/json").has_value());
}

TEST(invalid_empty_subtype) {
  EXPECT_FALSE(
      sourcemeta::core::http_parse_media_type("application/").has_value());
}

TEST(invalid_second_slash) {
  EXPECT_FALSE(
      sourcemeta::core::http_parse_media_type("application/a/b").has_value());
}

TEST(invalid_space_within) {
  EXPECT_FALSE(
      sourcemeta::core::http_parse_media_type("application/j son").has_value());
}

TEST(invalid_mime_comment) {
  EXPECT_FALSE(sourcemeta::core::http_parse_media_type("text/plain(comment)")
                   .has_value());
}

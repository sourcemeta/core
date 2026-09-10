#include <sourcemeta/core/json.h>
#include <sourcemeta/core/openapi.h>

#include <sourcemeta/core/test.h>

#include <optional> // std::optional

namespace {

auto version_of(const sourcemeta::core::JSON::String &version)
    -> std::optional<sourcemeta::core::OpenAPIVersion> {
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("openapi", sourcemeta::core::JSON{version});
  return sourcemeta::core::openapi_version(document);
}

} // namespace

TEST(patch_zero) {
  EXPECT_EQ(version_of("3.1.0").value(),
            sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(patch_one) {
  EXPECT_EQ(version_of("3.1.1").value(),
            sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(patch_two) {
  EXPECT_EQ(version_of("3.1.2").value(),
            sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(patch_multiple_digits) {
  EXPECT_EQ(version_of("3.1.10").value(),
            sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(patch_leading_zero) {
  EXPECT_EQ(version_of("3.1.01").value(),
            sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(pre_release_suffix) {
  EXPECT_EQ(version_of("3.1.0-rc0").value(),
            sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(pre_release_suffix_with_dots) {
  EXPECT_EQ(version_of("3.1.0-beta.1").value(),
            sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(no_patch_component) { EXPECT_FALSE(version_of("3.1").has_value()); }

TEST(empty_patch_component) { EXPECT_FALSE(version_of("3.1.").has_value()); }

TEST(empty_pre_release_suffix) {
  EXPECT_FALSE(version_of("3.1.0-").has_value());
}

TEST(pre_release_suffix_with_line_feed) {
  EXPECT_FALSE(version_of("3.1.0-rc\x0A"
                          "0")
                   .has_value());
}

TEST(pre_release_suffix_ending_in_line_feed) {
  EXPECT_FALSE(version_of("3.1.0-\x0A").has_value());
}

TEST(pre_release_suffix_with_carriage_return) {
  EXPECT_FALSE(version_of("3.1.0-rc\x0D"
                          "0")
                   .has_value());
}

TEST(pre_release_suffix_with_line_separator) {
  EXPECT_FALSE(version_of("3.1.0-rc\xE2\x80\xA8"
                          "0")
                   .has_value());
}

TEST(pre_release_suffix_with_paragraph_separator) {
  EXPECT_FALSE(version_of("3.1.0-rc\xE2\x80\xA9"
                          "0")
                   .has_value());
}

TEST(pre_release_suffix_with_horizontal_tab) {
  EXPECT_EQ(version_of("3.1.0-rc\x09"
                       "0")
                .value(),
            sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(pre_release_suffix_with_form_feed) {
  EXPECT_EQ(version_of("3.1.0-rc\x0C"
                       "0")
                .value(),
            sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(pre_release_suffix_with_next_line) {
  EXPECT_EQ(version_of("3.1.0-rc\xC2\x85"
                       "0")
                .value(),
            sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(non_numeric_patch_component) {
  EXPECT_FALSE(version_of("3.1.x").has_value());
}

TEST(fourth_version_component) {
  EXPECT_FALSE(version_of("3.1.0.0").has_value());
}

TEST(leading_whitespace) { EXPECT_FALSE(version_of(" 3.1.0").has_value()); }

TEST(trailing_whitespace) { EXPECT_FALSE(version_of("3.1.0 ").has_value()); }

TEST(unrelated_major_minor_prefix) {
  EXPECT_FALSE(version_of("13.1.0").has_value());
}

TEST(earlier_minor) { EXPECT_FALSE(version_of("3.0.0").has_value()); }

TEST(later_minor) { EXPECT_FALSE(version_of("3.2.0").has_value()); }

TEST(later_major) { EXPECT_FALSE(version_of("4.0.0").has_value()); }

TEST(swagger) { EXPECT_FALSE(version_of("2.0").has_value()); }

TEST(empty_string) { EXPECT_FALSE(version_of("").has_value()); }

TEST(missing_field) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "info": { "title": "Example", "version": "1.0.0" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::openapi_version(document).has_value());
}

TEST(field_is_not_a_string) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": 3.1
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::openapi_version(document).has_value());
}

TEST(document_is_not_an_object) {
  const auto document{sourcemeta::core::parse_json("\"3.1.1\"")};
  EXPECT_FALSE(sourcemeta::core::openapi_version(document).has_value());
}

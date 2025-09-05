#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/schemaconfig.h>
#include <sourcemeta/core/uri.h>

#define EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(                             \
    input, base, expected_message, expected_location)                          \
  try {                                                                        \
    (void)sourcemeta::core::SchemaConfig::from_json((input), (base));          \
    FAIL() << "The function was expected to throw";                            \
  } catch (const sourcemeta::core::SchemaConfigParseError &error) {            \
    EXPECT_STREQ(error.what(), (expected_message));                            \
    EXPECT_EQ(sourcemeta::core::to_string(error.location()),                   \
              (expected_location));                                            \
  } catch (...) {                                                              \
    FAIL() << "The function was expected to throw a parse error";              \
  }

TEST(SchemaConfig_from_json, valid_1) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "title": "Sourcemeta",
    "description": "The JSON Schema company",
    "email": "hello@sourcemeta.com",
    "github": "sourcemeta",
    "website": "https://www.sourcemeta.com",
    "baseUri": "https://schemas.sourcemeta.com",
    "path": "./schemas",
    "defaultDialect": "http://json-schema.org/draft-07/schema#",
    "resolve": {
      "https://other.com/single.json": "../single.json"
    }
  })JSON")};

  const auto manifest{
      sourcemeta::core::SchemaConfig::from_json(input, TEST_DIRECTORY)};

  EXPECT_TRUE(manifest.title.has_value());
  EXPECT_EQ(manifest.title.value(), "Sourcemeta");
  EXPECT_TRUE(manifest.description.has_value());
  EXPECT_EQ(manifest.description.value(), "The JSON Schema company");
  EXPECT_TRUE(manifest.email.has_value());
  EXPECT_EQ(manifest.email.value(), "hello@sourcemeta.com");
  EXPECT_TRUE(manifest.github.has_value());
  EXPECT_EQ(manifest.github.value(), "sourcemeta");
  EXPECT_TRUE(manifest.website.has_value());
  EXPECT_EQ(manifest.website.value(), "https://www.sourcemeta.com");
  EXPECT_EQ(manifest.absolute_path,
            std::filesystem::path{TEST_DIRECTORY} / "schemas");
  EXPECT_EQ(manifest.base, "https://schemas.sourcemeta.com");
  EXPECT_TRUE(manifest.default_dialect.has_value());
  EXPECT_EQ(manifest.default_dialect.value(),
            "http://json-schema.org/draft-07/schema#");
  EXPECT_EQ(manifest.resolve.size(), 1);
  EXPECT_TRUE(manifest.resolve.contains("https://other.com/single.json"));
  EXPECT_EQ(manifest.resolve.at("https://other.com/single.json"),
            "../single.json");
  EXPECT_EQ(manifest.extra.size(), 0);
}

TEST(SchemaConfig_from_json, valid_2) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "path": "./schemas"
  })JSON")};

  const auto manifest{
      sourcemeta::core::SchemaConfig::from_json(input, TEST_DIRECTORY)};

  EXPECT_FALSE(manifest.title.has_value());
  EXPECT_FALSE(manifest.description.has_value());
  EXPECT_FALSE(manifest.email.has_value());
  EXPECT_FALSE(manifest.github.has_value());
  EXPECT_FALSE(manifest.website.has_value());
  EXPECT_EQ(manifest.absolute_path,
            std::filesystem::path{TEST_DIRECTORY} / "schemas");
  EXPECT_EQ(
      manifest.base,
      sourcemeta::core::URI::from_path(manifest.absolute_path).recompose());
  EXPECT_FALSE(manifest.default_dialect.has_value());
  EXPECT_EQ(manifest.resolve.size(), 0);
  EXPECT_EQ(manifest.extra.size(), 0);
}

TEST(SchemaConfig_from_json, valid_3) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "path": "/foo/bar/schemas"
  })JSON")};

  const auto manifest{
      sourcemeta::core::SchemaConfig::from_json(input, TEST_DIRECTORY)};

  EXPECT_FALSE(manifest.title.has_value());
  EXPECT_FALSE(manifest.description.has_value());
  EXPECT_FALSE(manifest.email.has_value());
  EXPECT_FALSE(manifest.github.has_value());
  EXPECT_FALSE(manifest.website.has_value());
  EXPECT_TRUE(manifest.absolute_path.is_absolute());
  EXPECT_EQ(
      manifest.base,
      sourcemeta::core::URI::from_path(manifest.absolute_path).recompose());
  EXPECT_FALSE(manifest.default_dialect.has_value());
  EXPECT_EQ(manifest.resolve.size(), 0);
  EXPECT_EQ(manifest.extra.size(), 0);
}

TEST(SchemaConfig_from_json, valid_4) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "path": "/foo/bar/schemas",
    "x-foo": "bar"
  })JSON")};

  const auto manifest{
      sourcemeta::core::SchemaConfig::from_json(input, TEST_DIRECTORY)};

  EXPECT_FALSE(manifest.title.has_value());
  EXPECT_FALSE(manifest.description.has_value());
  EXPECT_FALSE(manifest.email.has_value());
  EXPECT_FALSE(manifest.github.has_value());
  EXPECT_FALSE(manifest.website.has_value());
  EXPECT_TRUE(manifest.absolute_path.is_absolute());
  EXPECT_EQ(
      manifest.base,
      sourcemeta::core::URI::from_path(manifest.absolute_path).recompose());
  EXPECT_FALSE(manifest.default_dialect.has_value());
  EXPECT_EQ(manifest.resolve.size(), 0);
  EXPECT_EQ(manifest.extra.size(), 1);
  EXPECT_TRUE(manifest.extra.defines("x-foo"));
  EXPECT_TRUE(manifest.extra.at("x-foo").is_string());
  EXPECT_EQ(manifest.extra.at("x-foo").to_string(), "bar");
}

TEST(SchemaConfig_from_json, invalid_1) {
  const auto input{sourcemeta::core::parse_json("{}")};
  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The path property is required", "/path");
}

TEST(SchemaConfig_from_json, invalid_2) {
  const auto input{sourcemeta::core::parse_json("[]")};
  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The configuration must be an object", "");
}

TEST(SchemaConfig_from_json, invalid_3) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "title": 1,
    "path": "./schemas"
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The title property must be a string", "/title");
}

TEST(SchemaConfig_from_json, invalid_4) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "description": 1,
    "path": "./schemas"
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The description property must be a string",
      "/description");
}

TEST(SchemaConfig_from_json, invalid_5) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "email": 1,
    "path": "./schemas"
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The email property must be a string", "/email");
}

TEST(SchemaConfig_from_json, invalid_6) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "github": 1,
    "path": "./schemas"
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The github property must be a string", "/github");
}

TEST(SchemaConfig_from_json, invalid_7) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "website": 1,
    "path": "./schemas"
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The website property must be a string",
      "/website");
}

TEST(SchemaConfig_from_json, invalid_8) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "path": 1
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The path property must be a string", "/path");
}

TEST(SchemaConfig_from_json, invalid_9) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "baseUri": 1,
    "path": "./schemas"
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The baseUri property must be a string",
      "/baseUri");
}

TEST(SchemaConfig_from_json, invalid_10) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "defaultDialect": 1,
    "path": "./schemas"
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The defaultDialect property must be a string",
      "/defaultDialect");
}

TEST(SchemaConfig_from_json, invalid_11) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "resolve": 1,
    "path": "./schemas"
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The resolve property must be an object",
      "/resolve");
}

TEST(SchemaConfig_from_json, invalid_12) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "resolve": {
      "foo": 1
    },
    "path": "./schemas"
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The values in the resolve object must be strings",
      "/resolve/foo");
}

TEST(SchemaConfig_from_json, invalid_13) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "baseUri": "%",
    "path": "./schemas"
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The baseUri property must represent a valid URI",
      "/baseUri");
}

TEST(SchemaConfig_from_json, invalid_14) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "baseUri": "relative",
    "path": "./schemas"
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY, "The baseUri property must be an absolute URI",
      "/baseUri");
}

TEST(SchemaConfig_from_json, invalid_15) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "resolve": {
      "foo": "%"
    },
    "path": "./schemas"
  })JSON")};

  EXPECT_SCHEMACONFIG_FROM_JSON_PARSE_ERROR(
      input, TEST_DIRECTORY,
      "The values in the resolve object must represent valid URIs",
      "/resolve/foo");
}

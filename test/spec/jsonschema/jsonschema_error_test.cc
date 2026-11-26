#include <sourcemeta/core/test.h>

#include <sourcemeta/core/jsonschema.h>

#include <exception>   // std::exception
#include <optional>    // std::nullopt
#include <string>      // std::string
#include <type_traits> // std::is_base_of_v

TEST(schema_error_throw) {
  static_assert(
      std::is_base_of_v<std::exception, sourcemeta::core::SchemaError>,
      "Must subclass std::exception");
  auto exception{sourcemeta::core::SchemaError("My error")};
  try {
    // Thrown by name on purpose, so that the assertion after the catch can
    // check the original object survived being thrown
    // NOLINTNEXTLINE(cert-err09-cpp,cert-err61-cpp,misc-throw-by-value-catch-by-reference)
    throw exception;
    FAIL();
  } catch (const sourcemeta::core::SchemaError &error) {
    EXPECT_STREQ(error.what(), "My error");
  }
  EXPECT_EQ(std::string{exception.what()}, "My error");
}

TEST(resolution_error_throw) {
  static_assert(std::is_base_of_v<std::exception,
                                  sourcemeta::core::SchemaResolutionError>,
                "Must subclass std::exception");
  auto exception{sourcemeta::core::SchemaResolutionError(
      "My error", "https://sourcemeta.com/test")};
  try {
    // Thrown by name on purpose, so that the assertion after the catch can
    // check the original object survived being thrown
    // NOLINTNEXTLINE(cert-err09-cpp,cert-err61-cpp,misc-throw-by-value-catch-by-reference,bugprone-exception-copy-constructor-throws,cert-err60-cpp)
    throw exception;
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(), "My error");
  }
  EXPECT_EQ(std::string{exception.what()}, "My error");
  EXPECT_EQ(exception.identifier(), "https://sourcemeta.com/test");
  EXPECT_TRUE(exception.location().empty());
  EXPECT_FALSE(exception.fragment().has_value());
}

TEST(resolution_error_throw_with_a_reference) {
  try {
    throw sourcemeta::core::SchemaResolutionError(
        "My error", "https://sourcemeta.com/test",
        sourcemeta::core::Pointer{"properties", "foo", "$ref"},
        "/components/schemas/Pet");
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(), "My error");
    EXPECT_EQ(error.identifier(), "https://sourcemeta.com/test");
    EXPECT_EQ(sourcemeta::core::to_string(error.location()),
              "/properties/foo/$ref");
    EXPECT_TRUE(error.fragment().has_value());
    EXPECT_EQ(error.fragment().value(), "/components/schemas/Pet");
  }
}

TEST(resolution_error_throw_with_a_reference_without_a_fragment) {
  try {
    throw sourcemeta::core::SchemaResolutionError(
        "My error", "https://sourcemeta.com/test",
        sourcemeta::core::Pointer{"properties", "foo", "$ref"}, std::nullopt);
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(), "My error");
    EXPECT_EQ(error.identifier(), "https://sourcemeta.com/test");
    EXPECT_EQ(sourcemeta::core::to_string(error.location()),
              "/properties/foo/$ref");
    EXPECT_FALSE(error.fragment().has_value());
  }
}

TEST(reference_error_throw) {
  static_assert(
      std::is_base_of_v<std::exception, sourcemeta::core::SchemaReferenceError>,
      "Must subclass std::exception");
  try {
    throw sourcemeta::core::SchemaReferenceError(
        "My error", "https://sourcemeta.com/test",
        sourcemeta::core::Pointer{"properties", "foo", "$ref"});
  } catch (const sourcemeta::core::SchemaReferenceError &error) {
    EXPECT_STREQ(error.what(), "My error");
    EXPECT_EQ(error.identifier(), "https://sourcemeta.com/test");
    EXPECT_EQ(sourcemeta::core::to_string(error.location()),
              "/properties/foo/$ref");
  }
}

TEST(reference_object_resource_error_throw) {
  static_assert(
      std::is_base_of_v<std::exception,
                        sourcemeta::core::SchemaReferenceObjectResourceError>,
      "Must subclass std::exception");
  try {
    throw sourcemeta::core::SchemaReferenceObjectResourceError(
        "https://sourcemeta.com/test");
  } catch (const sourcemeta::core::SchemaReferenceObjectResourceError &error) {
    EXPECT_EQ(error.identifier(), "https://sourcemeta.com/test");
  }
}

#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <exception>   // std::exception
#include <string>      // std::string
#include <type_traits> // std::is_base_of_v

TEST(parse_error) {
  static_assert(std::is_base_of_v<sourcemeta::core::Error,
                                  sourcemeta::core::PointerParseError>,
                "Must subclass sourcemeta::core::Error");
  static_assert(
      std::is_base_of_v<std::exception, sourcemeta::core::PointerParseError>,
      "Must subclass std::exception");
  const auto exception{sourcemeta::core::PointerParseError(5)};
  try {
    throw sourcemeta::core::PointerParseError{5};
    FAIL();
  } catch (const sourcemeta::core::PointerParseError &error) {
    EXPECT_STREQ(error.what(), "The input is not a valid JSON Pointer");
  }
  EXPECT_EQ(std::string{exception.what()},
            "The input is not a valid JSON Pointer");
  EXPECT_EQ(exception.column(), 5);
}

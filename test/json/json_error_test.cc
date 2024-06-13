#include <gtest/gtest.h>
#include <sourcemeta/jsontoolkit/json.h>

#include <exception>   // std::exception
#include <string>      // std::string
#include <type_traits> // std::is_base_of_v

TEST(JSON_error, parse_error) {
  static_assert(
      std::is_base_of_v<std::exception, sourcemeta::jsontoolkit::ParseError>,
      "Must subclass std::exception");
  auto exception{sourcemeta::jsontoolkit::ParseError(5, 6)};
  EXPECT_THROW(throw exception, sourcemeta::jsontoolkit::ParseError);
  EXPECT_EQ(std::string{exception.what()}, "Failed to parse the JSON document");
  EXPECT_EQ(exception.line(), 5);
  EXPECT_EQ(exception.column(), 6);
}

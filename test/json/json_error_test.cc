#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <exception>   // std::exception
#include <filesystem>  // std::filesystem::path
#include <string>      // std::string
#include <type_traits> // std::is_base_of_v

TEST(parse_error) {
  static_assert(
      std::is_base_of_v<std::exception, sourcemeta::core::JSONParseError>,
      "Must subclass std::exception");
  auto exception{sourcemeta::core::JSONParseError(5, 6)};
  EXPECT_STREQ(exception.what(), "Failed to parse the JSON document");
  EXPECT_EQ(exception.line(), 5);
  EXPECT_EQ(exception.column(), 6);
}

TEST(file_parse_error) {
  static_assert(std::is_base_of_v<sourcemeta::core::JSONParseError,
                                  sourcemeta::core::JSONFileParseError>,
                "Must subclass sourcemeta::core::JSONParseError");
  const sourcemeta::core::JSONFileParseError exception{
      std::filesystem::path{"document.json"}, 5, 6, "My error"};
  EXPECT_STREQ(exception.what(), "My error");
  EXPECT_EQ(exception.path(), std::filesystem::path{"document.json"});
  EXPECT_EQ(exception.line(), 5);
  EXPECT_EQ(exception.column(), 6);
}

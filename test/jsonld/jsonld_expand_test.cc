#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>

// TODO: Remove this placeholder and replace it with real unit tests once
// jsonld_expand is implemented
TEST(JSONLD_expand, returns_empty_array_for_now) {
  const auto document{
      sourcemeta::core::parse_json(R"({ "@id": "https://example.com" })")};
  const auto result{sourcemeta::core::jsonld_expand(document)};
  EXPECT_EQ(result, sourcemeta::core::JSON::make_array());
}

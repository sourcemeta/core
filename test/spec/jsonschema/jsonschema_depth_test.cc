#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <charconv>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

// Longer than the chain that overflowed the call stack when bundling ran on
// it, and no longer than that, as every document of one costs time to read
static constexpr std::string_view CHAIN_PREFIX{
    "https://www.sourcemeta.com/chain-"};
static constexpr std::size_t BUNDLE_CHAIN_LENGTH{9000};

// Handing the chain over a document at a time is what keeps this from needing
// a fixture of several megabytes
static auto chain_at(const std::string_view identifier,
                     const std::size_t length)
    -> sourcemeta::core::SchemaResolverResult {
  if (!identifier.starts_with(CHAIN_PREFIX)) {
    return sourcemeta::core::schema_resolver(identifier);
  }

  const auto digits{identifier.substr(CHAIN_PREFIX.size())};
  std::size_t index{0};
  const auto parsed{
      std::from_chars(digits.data(), digits.data() + digits.size(), index)};
  if (parsed.ec != std::errc{} || index == 0 || index > length) {
    return std::nullopt;
  }

  auto document{sourcemeta::core::JSON::make_object()};
  document.assign(
      "$schema",
      sourcemeta::core::JSON{"https://json-schema.org/draft/2020-12/schema"});
  document.assign("$id", sourcemeta::core::JSON{
                             sourcemeta::core::JSON::String{identifier}});
  if (index < length) {
    document.assign(
        "$ref", sourcemeta::core::JSON{"chain-" + std::to_string(index + 1)});
  } else {
    document.assign("type", sourcemeta::core::JSON{"string"});
  }

  return sourcemeta::core::SchemaResolverResult{std::move(document)};
}

static auto bundle_chain_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  return chain_at(identifier, BUNDLE_CHAIN_LENGTH);
}

TEST(bundling_a_long_chain_of_remotes) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "https://www.sourcemeta.com/chain-1"
  })JSON");

  sourcemeta::core::schema_bundle(document, sourcemeta::core::schema_walker,
                                  bundle_chain_resolver);

  EXPECT_TRUE(document.defines("$defs"));
  EXPECT_EQ(document.at("$defs").size(), BUNDLE_CHAIN_LENGTH);
}

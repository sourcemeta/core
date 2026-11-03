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

// The dependency suite can express this as a fixture, and deliberately does
// not. A chain long enough to overflow the call stack carries every one of its
// documents twice over, as what the resolver hands back and as the edge the
// walk reports, which runs to several megabytes in the repository and adds a
// noticeable stretch to every run of that suite for the one case that needs
// it. Synthesising the chain a document at a time here costs a single file
// instead
//
// Longer than the chain that overflowed the call stack when the walk ran on
// it, and no longer than that, as every document of one costs time to read
static constexpr std::string_view CHAIN_PREFIX{
    "https://www.sourcemeta.com/chain-"};
static constexpr std::size_t DEPENDENCIES_CHAIN_LENGTH{12000};

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

static auto dependencies_chain_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  return chain_at(identifier, DEPENDENCIES_CHAIN_LENGTH);
}

TEST(dependencies_of_a_long_chain_of_remotes) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "https://www.sourcemeta.com/chain-1"
  })JSON");

  std::size_t reported{0};
  sourcemeta::core::schema_dependencies(
      document, sourcemeta::core::schema_walker, dependencies_chain_resolver,
      [&reported](const auto &) -> void { reported += 1; });

  EXPECT_EQ(reported, DEPENDENCIES_CHAIN_LENGTH);
}

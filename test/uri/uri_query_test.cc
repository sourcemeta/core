#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <utility> // std::pair
#include <vector>  // std::vector

TEST(URI_query, https_example_url_no_query) {
  const sourcemeta::core::URI uri{"https://example.com/test"};
  EXPECT_FALSE(uri.query().has_value());
}

TEST(URI_query, https_example_with_query_single) {
  const sourcemeta::core::URI uri{"https://example.com/test?foo=bar"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo=bar");
}

// RFC 3986 itself does not understand the query part of a URI as a list of
// key/value pairs.
TEST(URI_query, https_example_with_query_double) {
  const sourcemeta::core::URI uri{"https://example.com/test?foo=bar&bar=baz"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo=bar&bar=baz");
}

TEST(URI_query, rfc3986_query_with_slash) {
  const sourcemeta::core::URI uri{"http://example.com/path?query/with/slashes"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "query/with/slashes");
}

TEST(URI_query, rfc3986_query_with_question_mark) {
  const sourcemeta::core::URI uri{"http://example.com/path?query=value?more"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "query=value?more");
}

TEST(URI_query, rfc3986_query_with_colon) {
  const sourcemeta::core::URI uri{"http://example.com/path?query:with:colons"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "query:with:colons");
}

TEST(URI_query, rfc3986_query_with_at_sign) {
  const sourcemeta::core::URI uri{"http://example.com/path?query@with@at"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "query@with@at");
}

TEST(URI_query, rfc3986_query_percent_encoded) {
  const sourcemeta::core::URI uri{
      "http://example.com/path?query%20with%20spaces"};
  EXPECT_TRUE(uri.query().has_value());
  // Space is not unreserved, so %20 must not be decoded
  EXPECT_EQ(uri.query().value().raw(), "query%20with%20spaces");
}

TEST(URI_query, rfc3986_query_with_fragment) {
  const sourcemeta::core::URI uri{
      "http://example.com/path?query=value#fragment"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "query=value");
}

TEST(URI_query, rfc3986_empty_query) {
  const sourcemeta::core::URI uri{"http://example.com/path?"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "");
}

TEST(URI_query, rfc3986_query_only_ampersands) {
  const sourcemeta::core::URI uri{"http://example.com/path?&&&"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "&&&");
}

TEST(URI_query, rfc3986_query_with_subdelims) {
  const sourcemeta::core::URI uri{"http://example.com/path?!$&'()*+,;="};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "!$&'()*+,;=");
}

TEST(URI_query, rfc3986_query_percent_encoded_brackets) {
  const sourcemeta::core::URI uri{
      "http://example.com/path?items%5B0%5D=a&items%5B1%5D=b"};
  EXPECT_TRUE(uri.query().has_value());
  // [ and ] are gen-delims (reserved), must not be decoded
  EXPECT_EQ(uri.query().value().raw(), "items%5B0%5D=a&items%5B1%5D=b");
}

TEST(URI_query, rfc3986_query_no_value) {
  const sourcemeta::core::URI uri{"http://example.com/path?key"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "key");
}

TEST(URI_query, rfc3986_query_multiple_equals) {
  const sourcemeta::core::URI uri{"http://example.com/path?key=value=extra"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "key=value=extra");
}

TEST(URI_query, encoded_ampersand_preserved) {
  const sourcemeta::core::URI uri{"http://example.com/path?foo%26bar=baz"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo%26bar=baz");
}

TEST(URI_query, encoded_hash_not_fragment_delimiter) {
  const sourcemeta::core::URI uri{"http://example.com/path?foo%23bar"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo%23bar");
  EXPECT_FALSE(uri.fragment().has_value());
}

TEST(URI_query, encoded_equals_preserved) {
  const sourcemeta::core::URI uri{"http://example.com/path?key%3Dvalue"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "key%3Dvalue");
}

TEST(URI_query, setter_on_uri_without_query) {
  sourcemeta::core::URI uri{"https://example.com/path"};
  uri.query("foo=bar");
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo=bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/path?foo=bar");
}

TEST(URI_query, setter_replace_existing_query) {
  sourcemeta::core::URI uri{"https://example.com/path?old=value"};
  uri.query("foo=bar");
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo=bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/path?foo=bar");
}

TEST(URI_query, setter_clear_with_empty_string) {
  sourcemeta::core::URI uri{"https://example.com/path?foo=bar"};
  uri.query("");
  EXPECT_FALSE(uri.query().has_value());
  EXPECT_EQ(uri.recompose(), "https://example.com/path");
}

TEST(URI_query, setter_clear_when_already_absent) {
  sourcemeta::core::URI uri{"https://example.com/path"};
  uri.query("");
  EXPECT_FALSE(uri.query().has_value());
  EXPECT_EQ(uri.recompose(), "https://example.com/path");
}

TEST(URI_query, setter_leading_question_mark_stripped) {
  sourcemeta::core::URI uri{"https://example.com/path"};
  uri.query("?foo=bar");
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo=bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/path?foo=bar");
}

TEST(URI_query, setter_clear_preserves_fragment) {
  sourcemeta::core::URI uri{"https://example.com/path?foo=bar#section"};
  uri.query("");
  EXPECT_FALSE(uri.query().has_value());
  EXPECT_EQ(uri.recompose(), "https://example.com/path#section");
}

TEST(URI_query, setter_percent_encoding_applied_on_recompose) {
  sourcemeta::core::URI uri{"https://example.com/path"};
  uri.query("foo=hello world");
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo=hello world");
  EXPECT_EQ(uri.recompose(), "https://example.com/path?foo=hello%20world");
}

TEST(URI_query, setter_returns_reference_for_chaining) {
  sourcemeta::core::URI uri{"https://example.com"};
  uri.query("a=1").fragment("section");
  EXPECT_EQ(uri.recompose(), "https://example.com?a=1#section");
}

TEST(URI_query, setter_normalizes_unreserved_percent_encoding) {
  sourcemeta::core::URI uri{"https://example.com"};
  uri.query("foo=%7Ebar");
  EXPECT_TRUE(uri.query().has_value());
  // %7E encodes ~, an unreserved character, so it must be decoded
  EXPECT_EQ(uri.query().value().raw(), "foo=~bar");
}

TEST(URI_query, setter_preserves_reserved_percent_encoding) {
  sourcemeta::core::URI uri{"https://example.com"};
  uri.query("foo=%23bar");
  EXPECT_TRUE(uri.query().has_value());
  // %23 encodes #, a reserved character, so it must not be decoded
  EXPECT_EQ(uri.query().value().raw(), "foo=%23bar");
}

TEST(URI_query, setter_matches_parsed_uri_for_unreserved_percent) {
  sourcemeta::core::URI built{"https://example.com"};
  built.query("foo=%7Ebar");
  const sourcemeta::core::URI parsed{"https://example.com?foo=%7Ebar"};
  EXPECT_EQ(built.query().value().raw(), parsed.query().value().raw());
  EXPECT_EQ(built.recompose(), parsed.recompose());
}

TEST(URI_query, iterator_no_query_has_no_iterable) {
  const sourcemeta::core::URI uri{"https://example.com/path"};
  EXPECT_FALSE(uri.query().has_value());
}

TEST(URI_query, iterator_empty_query_yields_no_pairs) {
  const sourcemeta::core::URI uri{"https://example.com/path?"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  EXPECT_EQ(query.value().begin(), query.value().end());
}

TEST(URI_query, iterator_single_pair) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  ASSERT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "bar");
}

TEST(URI_query, iterator_two_pairs_in_order) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar&baz=qux"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  ASSERT_EQ(pairs.size(), 2);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "bar");
  EXPECT_EQ(pairs[1].first, "baz");
  EXPECT_EQ(pairs[1].second, "qux");
}

TEST(URI_query, iterator_key_without_equals) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  ASSERT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "");
}

TEST(URI_query, iterator_key_with_empty_value) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo="};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  ASSERT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "");
}

TEST(URI_query, iterator_empty_key_with_value) {
  const sourcemeta::core::URI uri{"https://example.com/path?=bar"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  ASSERT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "");
  EXPECT_EQ(pairs[0].second, "bar");
}

TEST(URI_query, iterator_only_ampersands_yields_empty_pairs) {
  const sourcemeta::core::URI uri{"https://example.com/path?&&&"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  ASSERT_EQ(pairs.size(), 4);
  for (const auto &pair : pairs) {
    EXPECT_EQ(pair.first, "");
    EXPECT_EQ(pair.second, "");
  }
}

TEST(URI_query, iterator_multiple_equals_only_first_splits) {
  const sourcemeta::core::URI uri{"https://example.com/path?key=value=extra"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  ASSERT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "key");
  EXPECT_EQ(pairs[0].second, "value=extra");
}

TEST(URI_query, iterator_percent_encoding_left_untouched) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=%20"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  ASSERT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "%20");
}

TEST(URI_query, at_single_match) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "bar");
}

TEST(URI_query, at_missing_key_returns_nullopt) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  EXPECT_FALSE(query.value().at("missing").has_value());
}

TEST(URI_query, at_first_wins_on_duplicates) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=1&foo=2"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "1");
}

TEST(URI_query, at_second_among_distinct_keys) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar&baz=qux"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  const auto value{query.value().at("baz")};
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "qux");
}

TEST(URI_query, at_key_with_empty_value_returns_empty_string) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo="};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "");
}

TEST(URI_query, at_key_without_equals_returns_empty_string) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "");
}

TEST(URI_query, at_empty_key_lookup) {
  const sourcemeta::core::URI uri{"https://example.com/path?=bar"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  const auto value{query.value().at("")};
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "bar");
}

TEST(URI_query, at_percent_encoded_value_left_untouched) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=%20bar"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "%20bar");
}

TEST(URI_query, at_empty_query_returns_nullopt) {
  const sourcemeta::core::URI uri{"https://example.com/path?"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  EXPECT_FALSE(query.value().at("anything").has_value());
}

TEST(URI_query, at_value_view_points_into_uri_storage) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar&baz=qux"};
  const auto query{uri.query()};
  ASSERT_TRUE(query.has_value());
  const auto value{query.value().at("baz")};
  ASSERT_TRUE(value.has_value());
  EXPECT_EQ(value.value().data(),
            query.value().raw().data() + query.value().raw().find("qux"));
}

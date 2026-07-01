#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <utility> // std::pair
#include <vector>  // std::vector

TEST(https_example_url_no_query) {
  const sourcemeta::core::URI uri{"https://example.com/test"};
  EXPECT_FALSE(uri.query().has_value());
}

TEST(https_example_with_query_single) {
  const sourcemeta::core::URI uri{"https://example.com/test?foo=bar"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo=bar");
}

// RFC 3986 itself does not understand the query part of a URI as a list of
// key/value pairs.
TEST(https_example_with_query_double) {
  const sourcemeta::core::URI uri{"https://example.com/test?foo=bar&bar=baz"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo=bar&bar=baz");
}

TEST(rfc3986_query_with_slash) {
  const sourcemeta::core::URI uri{"http://example.com/path?query/with/slashes"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "query/with/slashes");
}

TEST(rfc3986_query_with_question_mark) {
  const sourcemeta::core::URI uri{"http://example.com/path?query=value?more"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "query=value?more");
}

TEST(rfc3986_query_with_colon) {
  const sourcemeta::core::URI uri{"http://example.com/path?query:with:colons"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "query:with:colons");
}

TEST(rfc3986_query_with_at_sign) {
  const sourcemeta::core::URI uri{"http://example.com/path?query@with@at"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "query@with@at");
}

TEST(rfc3986_query_percent_encoded) {
  const sourcemeta::core::URI uri{
      "http://example.com/path?query%20with%20spaces"};
  EXPECT_TRUE(uri.query().has_value());
  // Space is not unreserved, so %20 must not be decoded
  EXPECT_EQ(uri.query().value().raw(), "query%20with%20spaces");
}

TEST(rfc3986_query_with_fragment) {
  const sourcemeta::core::URI uri{
      "http://example.com/path?query=value#fragment"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "query=value");
}

TEST(rfc3986_empty_query) {
  const sourcemeta::core::URI uri{"http://example.com/path?"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "");
}

TEST(rfc3986_query_only_ampersands) {
  const sourcemeta::core::URI uri{"http://example.com/path?&&&"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "&&&");
}

TEST(rfc3986_query_with_subdelims) {
  const sourcemeta::core::URI uri{"http://example.com/path?!$&'()*+,;="};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "!$&'()*+,;=");
}

TEST(rfc3986_query_percent_encoded_brackets) {
  const sourcemeta::core::URI uri{
      "http://example.com/path?items%5B0%5D=a&items%5B1%5D=b"};
  EXPECT_TRUE(uri.query().has_value());
  // [ and ] are gen-delims (reserved), must not be decoded
  EXPECT_EQ(uri.query().value().raw(), "items%5B0%5D=a&items%5B1%5D=b");
}

TEST(rfc3986_query_no_value) {
  const sourcemeta::core::URI uri{"http://example.com/path?key"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "key");
}

TEST(rfc3986_query_multiple_equals) {
  const sourcemeta::core::URI uri{"http://example.com/path?key=value=extra"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "key=value=extra");
}

TEST(encoded_ampersand_preserved) {
  const sourcemeta::core::URI uri{"http://example.com/path?foo%26bar=baz"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo%26bar=baz");
}

TEST(encoded_hash_not_fragment_delimiter) {
  const sourcemeta::core::URI uri{"http://example.com/path?foo%23bar"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo%23bar");
  EXPECT_FALSE(uri.fragment().has_value());
}

TEST(encoded_equals_preserved) {
  const sourcemeta::core::URI uri{"http://example.com/path?key%3Dvalue"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "key%3Dvalue");
}

TEST(setter_on_uri_without_query) {
  sourcemeta::core::URI uri{"https://example.com/path"};
  uri.query("foo=bar");
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo=bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/path?foo=bar");
}

TEST(setter_replace_existing_query) {
  sourcemeta::core::URI uri{"https://example.com/path?old=value"};
  uri.query("foo=bar");
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo=bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/path?foo=bar");
}

TEST(setter_clear_with_empty_string) {
  sourcemeta::core::URI uri{"https://example.com/path?foo=bar"};
  uri.query("");
  EXPECT_FALSE(uri.query().has_value());
  EXPECT_EQ(uri.recompose(), "https://example.com/path");
}

TEST(setter_clear_when_already_absent) {
  sourcemeta::core::URI uri{"https://example.com/path"};
  uri.query("");
  EXPECT_FALSE(uri.query().has_value());
  EXPECT_EQ(uri.recompose(), "https://example.com/path");
}

TEST(setter_leading_question_mark_stripped) {
  sourcemeta::core::URI uri{"https://example.com/path"};
  uri.query("?foo=bar");
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo=bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/path?foo=bar");
}

TEST(setter_clear_preserves_fragment) {
  sourcemeta::core::URI uri{"https://example.com/path?foo=bar#section"};
  uri.query("");
  EXPECT_FALSE(uri.query().has_value());
  EXPECT_EQ(uri.recompose(), "https://example.com/path#section");
}

TEST(setter_percent_encoding_applied_on_recompose) {
  sourcemeta::core::URI uri{"https://example.com/path"};
  uri.query("foo=hello world");
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "foo=hello world");
  EXPECT_EQ(uri.recompose(), "https://example.com/path?foo=hello%20world");
}

TEST(setter_returns_reference_for_chaining) {
  sourcemeta::core::URI uri{"https://example.com"};
  uri.query("a=1").fragment("section");
  EXPECT_EQ(uri.recompose(), "https://example.com?a=1#section");
}

TEST(setter_normalizes_unreserved_percent_encoding) {
  sourcemeta::core::URI uri{"https://example.com"};
  uri.query("foo=%7Ebar");
  EXPECT_TRUE(uri.query().has_value());
  // %7E encodes ~, an unreserved character, so it must be decoded
  EXPECT_EQ(uri.query().value().raw(), "foo=~bar");
}

TEST(setter_preserves_reserved_percent_encoding) {
  sourcemeta::core::URI uri{"https://example.com"};
  uri.query("foo=%23bar");
  EXPECT_TRUE(uri.query().has_value());
  // %23 encodes #, a reserved character, so it must not be decoded
  EXPECT_EQ(uri.query().value().raw(), "foo=%23bar");
}

TEST(setter_matches_parsed_uri_for_unreserved_percent) {
  sourcemeta::core::URI built{"https://example.com"};
  built.query("foo=%7Ebar");
  const sourcemeta::core::URI parsed{"https://example.com?foo=%7Ebar"};
  EXPECT_EQ(built.query().value().raw(), parsed.query().value().raw());
  EXPECT_EQ(built.recompose(), parsed.recompose());
}

TEST(iterator_no_query_has_no_iterable) {
  const sourcemeta::core::URI uri{"https://example.com/path"};
  EXPECT_FALSE(uri.query().has_value());
}

TEST(iterator_empty_query_yields_no_pairs) {
  const sourcemeta::core::URI uri{"https://example.com/path?"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  EXPECT_EQ(query.value().begin(), query.value().end());
}

TEST(iterator_single_pair) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "bar");
}

TEST(iterator_two_pairs_in_order) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar&baz=qux"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 2);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "bar");
  EXPECT_EQ(pairs[1].first, "baz");
  EXPECT_EQ(pairs[1].second, "qux");
}

TEST(iterator_key_without_equals) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "");
}

TEST(iterator_key_with_empty_value) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo="};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "");
}

TEST(iterator_empty_key_with_value) {
  const sourcemeta::core::URI uri{"https://example.com/path?=bar"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "");
  EXPECT_EQ(pairs[0].second, "bar");
}

TEST(iterator_only_ampersands_yields_empty_pairs) {
  const sourcemeta::core::URI uri{"https://example.com/path?&&&"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 4);
  for (const auto &pair : pairs) {
    EXPECT_EQ(pair.first, "");
    EXPECT_EQ(pair.second, "");
  }
}

TEST(iterator_multiple_equals_only_first_splits) {
  const sourcemeta::core::URI uri{"https://example.com/path?key=value=extra"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "key");
  EXPECT_EQ(pairs[0].second, "value=extra");
}

TEST(iterator_percent_encoding_left_untouched) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=%20"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "%20");
}

TEST(at_single_match) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "bar");
}

TEST(at_missing_key_returns_nullopt) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  EXPECT_FALSE(query.value().at("missing").has_value());
}

TEST(at_first_wins_on_duplicates) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=1&foo=2"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "1");
}

TEST(at_second_among_distinct_keys) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar&baz=qux"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto value{query.value().at("baz")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "qux");
}

TEST(at_key_with_empty_value_returns_empty_string) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo="};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "");
}

TEST(at_key_without_equals_returns_empty_string) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "");
}

TEST(at_empty_key_lookup) {
  const sourcemeta::core::URI uri{"https://example.com/path?=bar"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto value{query.value().at("")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "bar");
}

TEST(at_percent_encoded_value_left_untouched) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=%20bar"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "%20bar");
}

TEST(at_empty_query_returns_nullopt) {
  const sourcemeta::core::URI uri{"https://example.com/path?"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  EXPECT_FALSE(query.value().at("anything").has_value());
}

TEST(at_value_view_points_into_uri_storage) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar&baz=qux"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto value{query.value().at("baz")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value().data(),
            query.value().raw().data() + query.value().raw().find("qux"));
}

TEST(iterator_trailing_ampersand) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar&"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 2);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "bar");
  EXPECT_EQ(pairs[1].first, "");
  EXPECT_EQ(pairs[1].second, "");
}

TEST(iterator_leading_ampersand) {
  const sourcemeta::core::URI uri{"https://example.com/path?&foo=bar"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 2);
  EXPECT_EQ(pairs[0].first, "");
  EXPECT_EQ(pairs[0].second, "");
  EXPECT_EQ(pairs[1].first, "foo");
  EXPECT_EQ(pairs[1].second, "bar");
}

TEST(iterator_consecutive_ampersands_between_pairs) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar&&baz=qux"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 3);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "bar");
  EXPECT_EQ(pairs[1].first, "");
  EXPECT_EQ(pairs[1].second, "");
  EXPECT_EQ(pairs[2].first, "baz");
  EXPECT_EQ(pairs[2].second, "qux");
}

TEST(iterator_only_equals_sign) {
  const sourcemeta::core::URI uri{"https://example.com/path?="};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "");
  EXPECT_EQ(pairs[0].second, "");
}

TEST(iterator_postfix_increment_returns_previous) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=1&bar=2"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  auto iterator{query.value().begin()};
  const auto previous{iterator++};
  EXPECT_EQ(previous->first, "foo");
  EXPECT_EQ(previous->second, "1");
  EXPECT_EQ(iterator->first, "bar");
  EXPECT_EQ(iterator->second, "2");
}

TEST(iterator_arrow_operator) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto iterator{query.value().begin()};
  EXPECT_EQ(iterator->first, "foo");
  EXPECT_EQ(iterator->second, "bar");
}

TEST(iterator_multipass_forward_guarantee) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=1&bar=2&baz=3"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto saved{query.value().begin()};
  auto advanced{saved};
  ++advanced;
  ++advanced;
  EXPECT_EQ(saved->first, "foo");
  EXPECT_EQ(saved->second, "1");
  EXPECT_EQ(advanced->first, "baz");
  EXPECT_EQ(advanced->second, "3");
}

TEST(iterator_pair_views_into_query_storage) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar&baz=qux"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto raw{query.value().raw()};
  auto iterator{query.value().begin()};
  EXPECT_EQ(iterator->first.data(), raw.data() + raw.find("foo"));
  EXPECT_EQ(iterator->second.data(), raw.data() + raw.find("bar"));
  ++iterator;
  EXPECT_EQ(iterator->first.data(), raw.data() + raw.find("baz"));
  EXPECT_EQ(iterator->second.data(), raw.data() + raw.find("qux"));
}

TEST(iterator_subdelims_in_value) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=!$'()*+,;"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "!$'()*+,;");
}

TEST(iterator_pchar_colon_at_in_value) {
  const sourcemeta::core::URI uri{
      "https://example.com/path?foo=user:pass@host"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query.value()) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "user:pass@host");
}

TEST(at_case_sensitive_no_match) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  EXPECT_FALSE(query.value().at("Foo").has_value());
  EXPECT_FALSE(query.value().at("FOO").has_value());
  EXPECT_FALSE(query.value().at("fOo").has_value());
}

TEST(at_three_duplicates_returns_first) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=1&foo=2&foo=3"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "1");
}

TEST(at_first_value_empty_among_duplicates) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=&foo=2"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "");
}

TEST(at_value_containing_equals_returned_intact) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=bar=baz"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "bar=baz");
}

TEST(at_plus_sign_not_decoded_as_space) {
  const sourcemeta::core::URI uri{"https://example.com/path?foo=a+b"};
  const auto query{uri.query()};
  EXPECT_TRUE(query.has_value());
  const auto value{query.value().at("foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "a+b");
}

TEST(standalone_raw_preserves_input) {
  const sourcemeta::core::URI::Query query{"foo=bar&baz=qux"};
  EXPECT_EQ(query.raw(), "foo=bar&baz=qux");
}

TEST(standalone_empty_input) {
  const sourcemeta::core::URI::Query query{""};
  EXPECT_EQ(query.raw(), "");
  EXPECT_EQ(query.begin(), query.end());
  EXPECT_FALSE(query.at("anything").has_value());
}

TEST(standalone_at_single_match) {
  const sourcemeta::core::URI::Query query{"foo=bar"};
  const auto value{query.at("foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "bar");
}

TEST(standalone_at_missing_key_returns_nullopt) {
  const sourcemeta::core::URI::Query query{"foo=bar"};
  EXPECT_FALSE(query.at("missing").has_value());
}

TEST(standalone_at_first_wins_on_duplicates) {
  const sourcemeta::core::URI::Query query{"foo=1&foo=2&foo=3"};
  const auto value{query.at("foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "1");
}

TEST(standalone_at_key_without_equals_returns_empty_string) {
  const sourcemeta::core::URI::Query query{"foo"};
  const auto value{query.at("foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "");
}

TEST(standalone_iterator_single_pair) {
  const sourcemeta::core::URI::Query query{"foo=bar"};
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "bar");
}

TEST(standalone_iterator_two_pairs_in_order) {
  const sourcemeta::core::URI::Query query{"foo=bar&baz=qux"};
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 2);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "bar");
  EXPECT_EQ(pairs[1].first, "baz");
  EXPECT_EQ(pairs[1].second, "qux");
}

TEST(standalone_iterator_consecutive_ampersands) {
  const sourcemeta::core::URI::Query query{"foo=bar&&baz=qux"};
  std::vector<std::pair<std::string_view, std::string_view>> pairs;
  for (const auto &pair : query) {
    pairs.emplace_back(pair);
  }
  EXPECT_EQ(pairs.size(), 3);
  EXPECT_EQ(pairs[0].first, "foo");
  EXPECT_EQ(pairs[0].second, "bar");
  EXPECT_EQ(pairs[1].first, "");
  EXPECT_EQ(pairs[1].second, "");
  EXPECT_EQ(pairs[2].first, "baz");
  EXPECT_EQ(pairs[2].second, "qux");
}

TEST(standalone_view_points_into_input_storage) {
  const std::string_view raw{"foo=bar&baz=qux"};
  const sourcemeta::core::URI::Query query{raw};
  EXPECT_EQ(query.raw().data(), raw.data());
  const auto value{query.at("baz")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value().data(), raw.data() + raw.find("qux"));
}

TEST(standalone_no_question_mark_prefix_expected) {
  const sourcemeta::core::URI::Query query{"?foo=bar"};
  EXPECT_EQ(query.raw(), "?foo=bar");
  const auto value{query.at("?foo")};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "bar");
  EXPECT_FALSE(query.at("foo").has_value());
}

TEST(iri_unicode_query) {
  const auto uri{
      sourcemeta::core::URI::from_iri("https://example.com/?key=caf\xC3\xA9")};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "key=caf\xC3\xA9");
}

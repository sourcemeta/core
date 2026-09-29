#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <type_traits> // std::is_same_v
#include <utility>     // std::move

// Two property names that are long enough to defeat the perfect hash, that
// agree on the leading bytes the hash is computed from, and that agree on the
// length and the boundary characters the hash mixes in, so that they end up
// with the same hash while still being different names
static const char *const COLLIDING_NAME_1{"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaXYa"};
static const char *const COLLIDING_NAME_2{"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaZWa"};

TEST(empty) {
  const sourcemeta::core::JSONPropertySet properties;
  EXPECT_TRUE(properties.empty());
  EXPECT_EQ(properties.size(), 0);
  EXPECT_TRUE(properties.cbegin() == properties.cend());
  EXPECT_FALSE(properties.contains("foo"));
}

TEST(insert_single) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert("foo");
  EXPECT_FALSE(properties.empty());
  EXPECT_EQ(properties.size(), 1);
  EXPECT_EQ(properties.at(0).first, "foo");
  EXPECT_EQ(properties.at(0).second,
            sourcemeta::core::JSON::Object::hash("foo"));
  EXPECT_TRUE(properties.contains("foo"));
}

TEST(insert_duplicate) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert("foo");
  properties.insert("foo");
  properties.insert("foo");
  EXPECT_EQ(properties.size(), 1);
  EXPECT_EQ(properties.at(0).first, "foo");
}

TEST(insert_sorts_entries) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert("foo");
  properties.insert("qux");
  properties.insert("bar");
  properties.insert("baz");
  EXPECT_EQ(properties.size(), 4);
  EXPECT_EQ(properties.at(0).first, "bar");
  EXPECT_EQ(properties.at(1).first, "baz");
  EXPECT_EQ(properties.at(2).first, "foo");
  EXPECT_EQ(properties.at(3).first, "qux");
}

TEST(insert_move) {
  sourcemeta::core::JSON::String foo{"foo"};
  sourcemeta::core::JSON::String bar{"bar"};
  sourcemeta::core::JSONPropertySet properties;
  properties.insert(std::move(foo));
  properties.insert(std::move(bar));
  EXPECT_EQ(properties.size(), 2);
  EXPECT_EQ(properties.at(0).first, "bar");
  EXPECT_EQ(properties.at(1).first, "foo");
  EXPECT_EQ(properties.at(0).second,
            sourcemeta::core::JSON::Object::hash("bar"));
  EXPECT_EQ(properties.at(1).second,
            sourcemeta::core::JSON::Object::hash("foo"));
}

TEST(insert_move_duplicate) {
  sourcemeta::core::JSON::String first{"foo"};
  sourcemeta::core::JSON::String second{"foo"};
  sourcemeta::core::JSONPropertySet properties;
  properties.insert(std::move(first));
  properties.insert(std::move(second));
  EXPECT_EQ(properties.size(), 1);
  EXPECT_EQ(properties.at(0).first, "foo");
}

TEST(insert_reports_whether_the_name_was_added) {
  sourcemeta::core::JSONPropertySet properties;
  EXPECT_TRUE(properties.insert("foo"));
  EXPECT_FALSE(properties.insert("foo"));
  EXPECT_TRUE(properties.insert("bar"));
  EXPECT_FALSE(properties.insert("bar"));
  EXPECT_EQ(properties.size(), 2);
}

TEST(insert_move_reports_whether_the_name_was_added) {
  sourcemeta::core::JSON::String first{"foo"};
  sourcemeta::core::JSON::String second{"foo"};
  sourcemeta::core::JSONPropertySet properties;
  EXPECT_TRUE(properties.insert(std::move(first)));
  EXPECT_FALSE(properties.insert(std::move(second)));
  EXPECT_EQ(properties.size(), 1);
}

TEST(insert_with_precomputed_hash) {
  sourcemeta::core::JSONPropertySet properties;
  const sourcemeta::core::JSON::String foo{"foo"};
  const sourcemeta::core::JSON::String bar{"bar"};
  EXPECT_TRUE(
      properties.insert(foo, sourcemeta::core::JSON::Object::hash(foo)));
  EXPECT_TRUE(
      properties.insert(bar, sourcemeta::core::JSON::Object::hash(bar)));
  EXPECT_FALSE(
      properties.insert(foo, sourcemeta::core::JSON::Object::hash(foo)));
  EXPECT_EQ(properties.size(), 2);
  EXPECT_EQ(properties.at(0).first, "bar");
  EXPECT_EQ(properties.at(1).first, "foo");
  EXPECT_EQ(properties.at(0).second,
            sourcemeta::core::JSON::Object::hash("bar"));
  EXPECT_EQ(properties.at(1).second,
            sourcemeta::core::JSON::Object::hash("foo"));
  EXPECT_TRUE(properties.contains("foo"));
  EXPECT_TRUE(properties.contains("bar"));
}

TEST(insert_move_with_precomputed_hash) {
  sourcemeta::core::JSON::String foo{"foo"};
  const auto foo_hash{sourcemeta::core::JSON::Object::hash(foo)};
  sourcemeta::core::JSONPropertySet properties;
  EXPECT_TRUE(properties.insert(std::move(foo), foo_hash));
  EXPECT_EQ(properties.size(), 1);
  EXPECT_EQ(properties.at(0).first, "foo");
  EXPECT_EQ(properties.at(0).second, foo_hash);
  EXPECT_TRUE(properties.contains("foo"));
}

TEST(insert_move_with_precomputed_hash_duplicate) {
  sourcemeta::core::JSON::String first{"foo"};
  sourcemeta::core::JSON::String second{"foo"};
  const auto hash{sourcemeta::core::JSON::Object::hash("foo")};
  sourcemeta::core::JSONPropertySet properties;
  EXPECT_TRUE(properties.insert(std::move(first), hash));
  EXPECT_FALSE(properties.insert(std::move(second), hash));
  EXPECT_EQ(properties.size(), 1);
  EXPECT_EQ(properties.at(0).first, "foo");
}

TEST(insert_with_the_hash_of_an_object_entry) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1, "bar": 2 })JSON")};
  const auto &object{document.as_object()};
  const auto foo{object.find("foo")};
  const auto bar{object.find("bar")};
  EXPECT_TRUE(foo != object.cend());
  EXPECT_TRUE(bar != object.cend());

  sourcemeta::core::JSONPropertySet properties;
  EXPECT_TRUE(properties.insert(foo->first, foo->hash));
  EXPECT_TRUE(properties.insert(bar->first, bar->hash));
  EXPECT_FALSE(properties.insert(foo->first, foo->hash));
  EXPECT_EQ(properties.size(), 2);
  EXPECT_TRUE(properties.contains("foo"));
  EXPECT_TRUE(properties.contains("bar"));
}

TEST(contains_miss) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert("foo");
  properties.insert("bar");
  EXPECT_FALSE(properties.contains("baz"));
  EXPECT_FALSE(properties.contains("fo"));
  EXPECT_FALSE(properties.contains("fooo"));
}

TEST(contains_precomputed_hash) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert("foo");
  properties.insert("bar");
  EXPECT_TRUE(
      properties.contains("foo", sourcemeta::core::JSON::Object::hash("foo")));
  EXPECT_TRUE(
      properties.contains("bar", sourcemeta::core::JSON::Object::hash("bar")));
  EXPECT_FALSE(
      properties.contains("baz", sourcemeta::core::JSON::Object::hash("baz")));
}

TEST(contains_empty_property_name) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert("");
  EXPECT_EQ(properties.size(), 1);
  EXPECT_TRUE(properties.contains(""));
  EXPECT_FALSE(properties.contains("foo"));
}

TEST(contains_trailing_nul_byte) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert("foo");
  const sourcemeta::core::JSON::String padded{"foo\0", 4};
  EXPECT_EQ(padded.size(), 4);
  EXPECT_EQ(sourcemeta::core::JSON::Object::hash(padded),
            sourcemeta::core::JSON::Object::hash("foo"));
  EXPECT_FALSE(properties.contains(padded));
}

TEST(contains_stored_trailing_nul_byte) {
  const sourcemeta::core::JSON::String padded{"foo\0", 4};
  sourcemeta::core::JSONPropertySet properties;
  properties.insert(padded);
  EXPECT_EQ(properties.size(), 1);
  EXPECT_EQ(properties.at(0).first.size(), 4);
  EXPECT_TRUE(properties.contains(padded));
  EXPECT_FALSE(properties.contains("foo"));
}

TEST(insert_keeps_names_that_differ_only_by_a_trailing_nul_byte) {
  const sourcemeta::core::JSON::String padded{"foo\0", 4};
  sourcemeta::core::JSONPropertySet properties;
  EXPECT_TRUE(properties.insert("foo"));
  EXPECT_TRUE(properties.insert(padded));
  EXPECT_EQ(properties.size(), 2);
  EXPECT_EQ(properties.at(0).first.size(), 3);
  EXPECT_EQ(properties.at(1).first.size(), 4);
  EXPECT_TRUE(properties.contains("foo"));
  EXPECT_TRUE(properties.contains(padded));
}

TEST(contains_single_nul_byte_against_empty_property_name) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert("");
  const sourcemeta::core::JSON::String padded{"\0", 1};
  EXPECT_EQ(padded.size(), 1);
  EXPECT_EQ(sourcemeta::core::JSON::Object::hash(padded),
            sourcemeta::core::JSON::Object::hash(""));
  EXPECT_FALSE(properties.contains(padded));
}

TEST(contains_colliding_long_property_names) {
  const sourcemeta::core::JSON::String first{COLLIDING_NAME_1};
  const sourcemeta::core::JSON::String second{COLLIDING_NAME_2};
  EXPECT_EQ(first.size(), 34);
  EXPECT_EQ(second.size(), 34);
  EXPECT_NE(first, second);
  EXPECT_EQ(sourcemeta::core::JSON::Object::hash(first),
            sourcemeta::core::JSON::Object::hash(second));
  sourcemeta::core::JSONPropertySet properties;
  properties.insert(first);
  EXPECT_EQ(properties.size(), 1);
  EXPECT_TRUE(properties.contains(first));
  EXPECT_FALSE(properties.contains(second));
}

TEST(insert_colliding_long_property_names) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert(COLLIDING_NAME_1);
  properties.insert(COLLIDING_NAME_2);
  EXPECT_EQ(properties.size(), 2);
  EXPECT_EQ(properties.at(0).first, COLLIDING_NAME_1);
  EXPECT_EQ(properties.at(1).first, COLLIDING_NAME_2);
  EXPECT_TRUE(properties.contains(COLLIDING_NAME_1));
  EXPECT_TRUE(properties.contains(COLLIDING_NAME_2));
}

TEST(iterators) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert("foo");
  properties.insert("bar");
  auto iterator{properties.cbegin()};
  EXPECT_EQ(iterator->first, "bar");
  ++iterator;
  EXPECT_EQ(iterator->first, "foo");
  ++iterator;
  EXPECT_TRUE(iterator == properties.cend());
  EXPECT_TRUE(properties.begin() == properties.cbegin());
  EXPECT_TRUE(properties.end() == properties.cend());
}

TEST(hash_type_matches_the_object_hash_type) {
  EXPECT_TRUE((std::is_same_v<sourcemeta::core::JSONPropertySet::hash_type,
                              sourcemeta::core::JSON::Object::hash_type>));
}

TEST(entries_feed_the_hash_taking_document_accessors) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert("foo");
  properties.insert("bar");
  properties.insert("baz");
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1, "bar": 2 })JSON")};

  const auto &bar{properties.at(0)};
  const auto &baz{properties.at(1)};
  const auto &foo{properties.at(2)};
  EXPECT_EQ(bar.first, "bar");
  EXPECT_EQ(baz.first, "baz");
  EXPECT_EQ(foo.first, "foo");

  EXPECT_TRUE(document.defines(foo.first, foo.second));
  EXPECT_TRUE(document.defines(bar.first, bar.second));
  EXPECT_FALSE(document.defines(baz.first, baz.second));

  EXPECT_EQ(document.at(foo.first, foo.second).to_integer(), 1);
  EXPECT_EQ(document.at(bar.first, bar.second).to_integer(), 2);

  EXPECT_TRUE(document.try_at(foo.first, foo.second) != nullptr);
  EXPECT_TRUE(document.try_at(baz.first, baz.second) == nullptr);

  const sourcemeta::core::JSON fallback{0};
  EXPECT_EQ(document.at_or(baz.first, baz.second, fallback).to_integer(), 0);

  const auto &object{document.as_object()};
  EXPECT_TRUE(object.defines(foo.first, foo.second));
  EXPECT_FALSE(object.defines(baz.first, baz.second));
  EXPECT_EQ(object.at(bar.first, bar.second).to_integer(), 2);
}

TEST(object_entries_feed_the_set_lookups) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert("foo");
  properties.insert("bar");
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "foo": 1, "qux": 2 })JSON")};
  const auto &object{document.as_object()};

  const auto foo{object.find("foo")};
  EXPECT_TRUE(foo != object.cend());
  EXPECT_TRUE(properties.contains(foo->first, foo->hash));

  const auto qux{object.find("qux")};
  EXPECT_TRUE(qux != object.cend());
  EXPECT_FALSE(properties.contains(qux->first, qux->hash));
}

TEST(to_json) {
  sourcemeta::core::JSONPropertySet properties;
  properties.insert("foo");
  properties.insert("qux");
  properties.insert("bar");
  const auto result{properties.to_json()};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON([ "bar", "foo", "qux" ])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(to_json_empty) {
  const sourcemeta::core::JSONPropertySet properties;
  const auto result{properties.to_json()};
  const auto expected{sourcemeta::core::parse_json(R"JSON([])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(from_json) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ "foo", "bar" ])JSON")};
  const auto result{sourcemeta::core::JSONPropertySet::from_json(document)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0).first, "bar");
  EXPECT_EQ(result.value().at(1).first, "foo");
  EXPECT_EQ(result.value().at(0).second,
            sourcemeta::core::JSON::Object::hash("bar"));
  EXPECT_EQ(result.value().at(1).second,
            sourcemeta::core::JSON::Object::hash("foo"));
}

TEST(from_json_empty_array) {
  const auto document{sourcemeta::core::parse_json(R"JSON([])JSON")};
  const auto result{sourcemeta::core::JSONPropertySet::from_json(document)};
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().empty());
}

TEST(from_json_deduplicates) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ "foo", "bar", "foo" ])JSON")};
  const auto result{sourcemeta::core::JSONPropertySet::from_json(document)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0).first, "bar");
  EXPECT_EQ(result.value().at(1).first, "foo");
}

TEST(from_json_not_an_array) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON")};
  const auto result{sourcemeta::core::JSONPropertySet::from_json(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(from_json_array_with_non_string) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ "foo", 1 ])JSON")};
  const auto result{sourcemeta::core::JSONPropertySet::from_json(document)};
  EXPECT_FALSE(result.has_value());
}

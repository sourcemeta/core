#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpath.h>
#include <sourcemeta/core/test.h>

namespace {

struct ResultNode {
  const sourcemeta::core::JSON *value;
  sourcemeta::core::WeakPointer location;
};

auto evaluate_nodes(const sourcemeta::core::JSONPath &path,
                    const sourcemeta::core::JSON &document)
    -> std::vector<ResultNode> {
  std::vector<ResultNode> result;
  path.evaluate(
      document,
      [&result](const sourcemeta::core::JSON &value,
                const sourcemeta::core::WeakPointer &location) -> void {
        result.push_back({&value, location});
      });
  return result;
}

} // namespace

TEST(jsonpath_normalize_root) {
  const auto document{sourcemeta::core::parse_json(R"JSON(1)JSON")};
  const sourcemeta::core::JSONPath path{"$"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location), "$");
}

TEST(jsonpath_normalize_property_and_index) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "a": [ { "b": 1 } ] })JSON")};
  const sourcemeta::core::JSONPath path{"$..b"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$['a'][0]['b']");
}

TEST(jsonpath_normalize_single_quote_escaped) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "a'b": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$.*"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$['a\\'b']");
}

TEST(jsonpath_normalize_backslash_escaped) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "a\\b": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$.*"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$['a\\\\b']");
}

TEST(jsonpath_normalize_double_quote_not_escaped) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "a\"b": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$.*"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$['a\"b']");
}

TEST(jsonpath_normalize_two_character_escapes) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "\b\f\n\r\t": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$.*"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$['\\b\\f\\n\\r\\t']");
}

TEST(jsonpath_normalize_control_hexadecimal) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "\u000b\u0000\u001f": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$.*"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$['\\u000b\\u0000\\u001f']");
}

TEST(jsonpath_normalize_non_ascii_verbatim) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "á🤔": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$.*"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$['á🤔']");
}

TEST(jsonpath_normalize_deep_indexes) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ [ [ 1 ] ] ])JSON")};
  const sourcemeta::core::JSONPath path{"$[0][0][0]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$[0][0][0]");
}

TEST(jsonpath_normalize_negative_index_resolved) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ 10, 20, 30 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[-1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$[2]");
}

TEST(jsonpath_normalize_slice_negative_step) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 0, 1, 2 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[::-1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 3);
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$[2]");
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(1).location),
            "$[1]");
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(2).location),
            "$[0]");
}

TEST(jsonpath_normalize_descendant_document_order) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON({ "a": { "a": 1 }, "b": [ { "a": 2 } ] })JSON")};
  const sourcemeta::core::JSONPath path{"$..a"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 3);
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$['a']");
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(1).location),
            "$['a']['a']");
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(2).location),
            "$['b'][0]['a']");
}

TEST(jsonpath_normalize_filter_locations) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": 1 }, { "b": 2 }, { "a": null } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$[0]");
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(1).location),
            "$[2]");
}

TEST(jsonpath_normalize_filter_object_member_location) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON({ "x": { "a": 1 }, "y": { "b": 2 } })JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(sourcemeta::core::JSONPath::normalize(nodes.at(0).location),
            "$['x']");
}

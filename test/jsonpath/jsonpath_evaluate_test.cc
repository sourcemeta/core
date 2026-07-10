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

TEST(jsonpath_evaluate_root) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_TRUE(*nodes.at(0).value == document);
}

TEST(jsonpath_evaluate_name) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "a": 1, "b": 2 })JSON")};
  const sourcemeta::core::JSONPath path{"$.b"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_TRUE(nodes.at(0).value->is_integer());
  EXPECT_EQ(nodes.at(0).value->to_integer(), 2);
}

TEST(jsonpath_evaluate_name_absent) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$.b"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_name_on_array) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2 ])JSON")};
  const sourcemeta::core::JSONPath path{"$.a"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_nested_names) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "a": { "b": { "c": 42 } } })JSON")};
  const sourcemeta::core::JSONPath path{"$.a.b.c"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 42);
}

TEST(jsonpath_evaluate_index) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ 10, 20, 30 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 20);
}

TEST(jsonpath_evaluate_index_negative) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ 10, 20, 30 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[-1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 30);
}

TEST(jsonpath_evaluate_index_out_of_bounds) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ 10, 20, 30 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[3]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_index_on_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "0": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$[0]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_wildcard_object) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "a": 1, "b": 2 })JSON")};
  const sourcemeta::core::JSONPath path{"$.*"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 1);
  EXPECT_EQ(nodes.at(1).value->to_integer(), 2);
}

TEST(jsonpath_evaluate_wildcard_array) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 10, 20 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[*]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 10);
  EXPECT_EQ(nodes.at(1).value->to_integer(), 20);
}

TEST(jsonpath_evaluate_wildcard_scalar) {
  const auto document{sourcemeta::core::parse_json("42")};
  const sourcemeta::core::JSONPath path{"$.*"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_slice_forward) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ 0, 1, 2, 3, 4, 5 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[1:5:2]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 1);
  EXPECT_EQ(nodes.at(1).value->to_integer(), 3);
}

TEST(jsonpath_evaluate_slice_negative_step) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 0, 1, 2 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[::-1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 3);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 2);
  EXPECT_EQ(nodes.at(1).value->to_integer(), 1);
  EXPECT_EQ(nodes.at(2).value->to_integer(), 0);
}

TEST(jsonpath_evaluate_slice_zero_step) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 0, 1, 2 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[::0]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_slice_negative_bounds) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ 0, 1, 2, 3, 4 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[-3:-1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 2);
  EXPECT_EQ(nodes.at(1).value->to_integer(), 3);
}

TEST(jsonpath_evaluate_multiple_selectors_duplicates) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 10, 20 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[0, 0, -2]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 3);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 10);
  EXPECT_EQ(nodes.at(1).value->to_integer(), 10);
  EXPECT_EQ(nodes.at(2).value->to_integer(), 10);
}

TEST(jsonpath_evaluate_descendant_names) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON({ "a": { "a": 1 }, "b": [ { "a": 2 } ] })JSON")};
  const sourcemeta::core::JSONPath path{"$..a"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 3);
  EXPECT_TRUE(nodes.at(0).value->is_object());
  EXPECT_EQ(nodes.at(1).value->to_integer(), 1);
  EXPECT_EQ(nodes.at(2).value->to_integer(), 2);
}

TEST(jsonpath_evaluate_descendant_wildcard) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "a": [ 1 ] })JSON")};
  const sourcemeta::core::JSONPath path{"$..*"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
  EXPECT_TRUE(nodes.at(0).value->is_array());
  EXPECT_EQ(nodes.at(1).value->to_integer(), 1);
}

TEST(jsonpath_evaluate_descendant_then_child) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "x": { "y": { "z": 1 } } })JSON")};
  const sourcemeta::core::JSONPath path{"$..y.z"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 1);
}

TEST(jsonpath_evaluate_callback_matches_vector) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[*]"};
  std::size_t count{0};
  path.evaluate(document, [&count](const sourcemeta::core::JSON &value,
                                   const sourcemeta::core::WeakPointer &) {
    EXPECT_TRUE(value.is_integer());
    count += 1;
  });
  EXPECT_EQ(count, 3);
}

TEST(jsonpath_evaluate_unicode_name) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "á": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$.á"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 1);
}

TEST(jsonpath_evaluate_escaped_name_decodes) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "A": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$['\\u0041']"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 1);
}

TEST(jsonpath_evaluate_descendant_deep_document) {
  std::string text;
  for (std::size_t index = 0; index < 10000; ++index) {
    text += "{\"a\":";
  }

  text += "{\"leaf\":1}";
  text += std::string(10000, '}');
  const auto document{sourcemeta::core::parse_json(text)};
  const sourcemeta::core::JSONPath path{"$..leaf"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 1);
}

TEST(jsonpath_evaluate_deep_name_chain_document) {
  std::string text;
  for (std::size_t index = 0; index < 10000; ++index) {
    text += "[";
  }

  text += "1";
  text += std::string(10000, ']');
  const auto document{sourcemeta::core::parse_json(text)};
  const sourcemeta::core::JSONPath path{"$..*"};
  std::size_t count{0};
  path.evaluate(document, [&count](const sourcemeta::core::JSON &,
                                   const sourcemeta::core::WeakPointer &) {
    count += 1;
  });
  EXPECT_EQ(count, 10000);
}

TEST(jsonpath_evaluate_copy_construction) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  const sourcemeta::core::JSONPath original{"$.a"};
  const sourcemeta::core::JSONPath copy{original};
  EXPECT_EQ(evaluate_nodes(original, document).size(), 1);
  EXPECT_EQ(evaluate_nodes(copy, document).size(), 1);
}

TEST(jsonpath_evaluate_move_construction) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  sourcemeta::core::JSONPath original{"$.a"};
  const sourcemeta::core::JSONPath moved{std::move(original)};
  const auto nodes{evaluate_nodes(moved, document)};
  EXPECT_EQ(nodes.size(), 1);
}

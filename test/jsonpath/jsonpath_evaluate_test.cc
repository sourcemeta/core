#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpath.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

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
        result.push_back({.value = &value, .location = location});
      });
  return result;
}

// Wraps the given document in three hundred nested single-element arrays so
// evaluation exceeds the recursion limit and continues on the iterative walk
auto deeply_nested_array(sourcemeta::core::JSON &&bottom)
    -> sourcemeta::core::JSON {
  auto current{std::move(bottom)};
  for (std::size_t depth{0}; depth < 300; depth += 1) {
    auto wrapper{sourcemeta::core::JSON::make_array()};
    wrapper.push_back(std::move(current));
    current = std::move(wrapper);
  }

  return current;
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
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
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

TEST(jsonpath_evaluate_deep_descendant_name) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": { "b": 7 } })JSON"))};
  const sourcemeta::core::JSONPath path{"$..b"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 7);
  EXPECT_EQ(nodes.at(0).location.size(), 302);
}

TEST(jsonpath_evaluate_deep_descendant_then_single_name) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": { "b": 7 } })JSON"))};
  const sourcemeta::core::JSONPath path{"$..a.b"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 7);
  EXPECT_EQ(nodes.at(0).location.size(), 302);
}

TEST(jsonpath_evaluate_deep_descendant_then_single_index) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "values": [ 1, 2, 3 ] })JSON"))};
  const sourcemeta::core::JSONPath path{"$..values[1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 2);
  EXPECT_EQ(nodes.at(0).location.size(), 302);
}

TEST(jsonpath_evaluate_deep_descendant_then_negative_index) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "values": [ 1, 2, 3 ] })JSON"))};
  const sourcemeta::core::JSONPath path{"$..values[-1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 3);
  EXPECT_EQ(nodes.at(0).location.size(), 302);
}

TEST(jsonpath_evaluate_deep_descendant_then_wildcard_array) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "values": [ 1, 2, 3 ] })JSON"))};
  const sourcemeta::core::JSONPath path{"$..values[*]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 3);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 1);
  EXPECT_EQ(nodes.at(1).value->to_integer(), 2);
  EXPECT_EQ(nodes.at(2).value->to_integer(), 3);
  EXPECT_EQ(nodes.at(0).location.size(), 302);
  EXPECT_EQ(nodes.at(1).location.size(), 302);
  EXPECT_EQ(nodes.at(2).location.size(), 302);
}

TEST(jsonpath_evaluate_deep_descendant_then_wildcard_object) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": { "b": 7 } })JSON"))};
  const sourcemeta::core::JSONPath path{"$..a[*]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 7);
  EXPECT_EQ(nodes.at(0).location.size(), 302);
}

TEST(jsonpath_evaluate_deep_descendant_then_slice) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "values": [ 1, 2, 3 ] })JSON"))};
  const sourcemeta::core::JSONPath path{"$..values[0:2]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 1);
  EXPECT_EQ(nodes.at(1).value->to_integer(), 2);
  EXPECT_EQ(nodes.at(0).location.size(), 302);
  EXPECT_EQ(nodes.at(1).location.size(), 302);
}

TEST(jsonpath_evaluate_deep_descendant_then_negative_step_slice) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "values": [ 1, 2, 3 ] })JSON"))};
  const sourcemeta::core::JSONPath path{"$..values[::-1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 3);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 3);
  EXPECT_EQ(nodes.at(1).value->to_integer(), 2);
  EXPECT_EQ(nodes.at(2).value->to_integer(), 1);
  EXPECT_EQ(nodes.at(0).location.size(), 302);
  EXPECT_EQ(nodes.at(1).location.size(), 302);
  EXPECT_EQ(nodes.at(2).location.size(), 302);
}

TEST(jsonpath_evaluate_deep_descendant_then_filter) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "values": [ 1, 2, 3 ] })JSON"))};
  const sourcemeta::core::JSONPath path{"$..values[?@ > 1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 2);
  EXPECT_EQ(nodes.at(1).value->to_integer(), 3);
  EXPECT_EQ(nodes.at(0).location.size(), 302);
  EXPECT_EQ(nodes.at(1).location.size(), 302);
}

TEST(jsonpath_evaluate_deep_descendant_then_index_pair) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "values": [ 1, 2, 3 ] })JSON"))};
  const sourcemeta::core::JSONPath path{"$..values[0, 2]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 1);
  EXPECT_EQ(nodes.at(1).value->to_integer(), 3);
  EXPECT_EQ(nodes.at(0).location.size(), 302);
  EXPECT_EQ(nodes.at(1).location.size(), 302);
}

TEST(jsonpath_evaluate_deep_descendant_then_zero_step_slice) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "values": [ 1, 2, 3 ] })JSON"))};
  const sourcemeta::core::JSONPath path{"$..values[0:2:0]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_descendant_then_filter_on_object) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": { "b": 7 } })JSON"))};
  const sourcemeta::core::JSONPath path{"$..a[?@ == 7]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 7);
  EXPECT_EQ(nodes.at(0).location.size(), 302);
}

TEST(jsonpath_evaluate_multibyte_shorthand) {
  const auto document{sourcemeta::core::parse_json("{ \"a\xc3\xa9\": 1 }")};
  const sourcemeta::core::JSONPath path{"$.a\xc3\xa9"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_TRUE(nodes.at(0).value->is_integer());
  EXPECT_EQ(nodes.at(0).value->to_integer(), 1);
  EXPECT_EQ(sourcemeta::core::to_string(nodes.at(0).location), "/a\xc3\xa9");
}

// RFC 9535 Section 2.3 selects nothing when the selector does not match the
// kind of value it is applied to, and Section 2.3.3 bounds an index to the
// array it indexes

TEST(jsonpath_evaluate_index_past_the_end) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[9]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_negative_index_before_the_start) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[-9]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_wildcard_on_scalar) {
  const auto document{sourcemeta::core::parse_json("1")};
  const sourcemeta::core::JSONPath path{"$[*]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_slice_on_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$[0:1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_negative_step_slice_on_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$[::-1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_filter_on_scalar) {
  const auto document{sourcemeta::core::parse_json("1")};
  const sourcemeta::core::JSONPath path{"$[?@.a]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_name_on_scalar) {
  const auto document{sourcemeta::core::parse_json("1")};
  const sourcemeta::core::JSONPath path{"$.a"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_index_on_object) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "0": 1 })JSON"))};
  const sourcemeta::core::JSONPath path{"$..[0]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 300);
}

TEST(jsonpath_evaluate_deep_index_past_the_end) {
  const auto document{
      deeply_nested_array(sourcemeta::core::parse_json(R"JSON([ 1 ])JSON"))};
  const sourcemeta::core::JSONPath path{"$..[9]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_negative_index_before_the_start) {
  const auto document{
      deeply_nested_array(sourcemeta::core::parse_json(R"JSON([ 1 ])JSON"))};
  const sourcemeta::core::JSONPath path{"$..[-9]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_name_on_scalar) {
  const auto document{deeply_nested_array(sourcemeta::core::parse_json("1"))};
  const sourcemeta::core::JSONPath path{"$..a"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

// A query inside a filter walks the document on its own, so a selector that
// does not match what it reaches selects nothing there either

TEST(jsonpath_evaluate_filter_with_an_index_on_an_object) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "0": 1 }, [ 1 ] ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@[0]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_TRUE(nodes.at(0).value->is_array());
}

TEST(jsonpath_evaluate_filter_with_an_index_past_the_end) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ [ 1 ] ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@[9]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_filter_with_a_negative_index_before_the_start) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ [ 1 ] ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@[-9]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_filter_with_a_wildcard_on_a_scalar) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, [ 2 ] ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@[*]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_TRUE(nodes.at(0).value->is_array());
}

TEST(jsonpath_evaluate_filter_with_a_slice_on_an_object) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": 1 }, [ 2 ] ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@[0:1]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_TRUE(nodes.at(0).value->is_array());
}

TEST(jsonpath_evaluate_filter_with_a_descendant_query) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": { "b": 1 } }, 2 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@..b]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_TRUE(nodes.at(0).value->is_object());
}

TEST(jsonpath_evaluate_filter_with_a_name_on_a_scalar) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ 1, { "a": 2 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_TRUE(nodes.at(0).value->is_object());
}

// A query inside a filter continues on an iterative walk of its own once it
// runs past the recursion limit, which is a second reading of every selector

TEST(jsonpath_evaluate_deep_filter_with_a_descendant_name) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")));
  document.push_back(deeply_nested_array(sourcemeta::core::parse_json("1")));
  const sourcemeta::core::JSONPath path{"$[?@..a]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_evaluate_deep_filter_with_a_descendant_index) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(
      deeply_nested_array(sourcemeta::core::parse_json(R"JSON([ 1 ])JSON")));
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "0": 1 })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..[0]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

TEST(jsonpath_evaluate_deep_filter_with_a_descendant_index_past_the_end) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(
      deeply_nested_array(sourcemeta::core::parse_json(R"JSON([ 1 ])JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..[9]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_filter_with_a_descendant_negative_index) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(
      deeply_nested_array(sourcemeta::core::parse_json(R"JSON([ 1 ])JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..[-1]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(
    jsonpath_evaluate_deep_filter_with_a_descendant_negative_index_before_the_start) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(
      deeply_nested_array(sourcemeta::core::parse_json(R"JSON([ 1 ])JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..[-9]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_filter_with_a_descendant_wildcard) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..[*]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_evaluate_deep_filter_with_a_descendant_slice) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..[0:1]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_evaluate_deep_filter_with_a_descendant_filter) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON([ { "a": 1 } ])JSON")));
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "b": { "a": 1 } })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..[?@.a]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

// RFC 9535 Section 2.3.1: a name selector "selects at most one object member
// value", so it selects nothing from an array. The nesting is what carries the
// query inside the filter past the recursion limit and onto the iterative
// walk, where every selector is read a second time
TEST(jsonpath_evaluate_deep_filter_with_a_name_on_an_array) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": [ 1, 2 ] })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..a.b]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_filter_with_a_name_the_object_lacks) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": { "c": 1 } })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..a.b]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

// RFC 9535 Section 2.3.3: an index selector "selects at most one array
// element", so it selects nothing from an object
TEST(jsonpath_evaluate_deep_filter_with_an_index_on_an_object) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": { "c": 1 } })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..a[0]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

// That section counts a negative index from the end of the array
TEST(jsonpath_evaluate_deep_filter_with_a_negative_index) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": [ 1, 2 ] })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..a[-1]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_evaluate_deep_filter_with_a_negative_index_past_the_front) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": [ 1, 2 ] })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..a[-5]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_filter_with_an_index_past_the_end) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": [ 1, 2 ] })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..a[5]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

// RFC 9535 Section 2.3.2: a wildcard selector "selects the nodes of all
// children of an object or array", so it selects nothing from a scalar
TEST(jsonpath_evaluate_deep_filter_with_a_wildcard_on_a_scalar) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..a.*]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

// RFC 9535 Section 2.3.4: a slice selector applies to an array, so it selects
// nothing from an object
TEST(jsonpath_evaluate_deep_filter_with_a_slice_on_an_object) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": { "c": 1 } })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..a[0:1]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

// RFC 9535 Section 2.3.5 applies a filter selector to the children of an
// object as well as to the elements of an array, and selects the ones it holds
// of
TEST(jsonpath_evaluate_deep_filter_over_object_members_that_do_not_match) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": { "m": { "n": 1 } } })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..a[?@.z]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_filter_over_a_scalar) {
  auto document{sourcemeta::core::JSON::make_array()};
  document.push_back(deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")));
  const sourcemeta::core::JSONPath path{"$[?@..a[?@.z]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

// The same selectors, read a second time by the iterative walk the main
// evaluation falls back on once the document runs past the recursion limit
TEST(jsonpath_evaluate_deep_single_name_on_an_array) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": [ 1, 2 ] })JSON"))};
  const sourcemeta::core::JSONPath path{"$..a.b"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_single_name_the_object_lacks) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": { "c": 1 } })JSON"))};
  const sourcemeta::core::JSONPath path{"$..a.b"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_single_index_on_an_object) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": { "c": 1 } })JSON"))};
  const sourcemeta::core::JSONPath path{"$..a[0]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_single_negative_index) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": [ 1, 2 ] })JSON"))};
  const sourcemeta::core::JSONPath path{"$..a[-1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 2);
}

TEST(jsonpath_evaluate_deep_single_negative_index_past_the_front) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": [ 1, 2 ] })JSON"))};
  const sourcemeta::core::JSONPath path{"$..a[-5]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_single_index_past_the_end) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": [ 1, 2 ] })JSON"))};
  const sourcemeta::core::JSONPath path{"$..a[5]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_slice_on_an_object) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": { "c": 1 } })JSON"))};
  const sourcemeta::core::JSONPath path{"$..a[0:1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_filter_selector_over_object_members) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": { "m": { "n": 1 } } })JSON"))};
  const sourcemeta::core::JSONPath path{"$..a[?@.z]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_deep_filter_selector_over_a_scalar) {
  const auto document{deeply_nested_array(
      sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON"))};
  const sourcemeta::core::JSONPath path{"$..a[?@.z]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

// The recursive walk reads a lone index selector through a shape check of its
// own, which an object does not answer
TEST(jsonpath_evaluate_single_index_on_an_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$[0]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_single_index_past_the_end) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[5]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_single_negative_index_past_the_front) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[-5]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

// RFC 9535 Section 2.5.1 lets a segment carry several selectors, which the
// general loop reads one at a time rather than through the shape checks above
TEST(jsonpath_evaluate_two_name_selectors_in_one_segment) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "a": 1, "b": 2 })JSON")};
  const sourcemeta::core::JSONPath path{"$['a','b']"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

TEST(jsonpath_evaluate_two_name_selectors_on_an_array) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2 ])JSON")};
  const sourcemeta::core::JSONPath path{"$['a','b']"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_two_name_selectors_the_object_lacks) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "c": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$['a','b']"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

// A segment carrying several selectors reads an index selector through the
// general loop, which asks the same question of the value's shape
TEST(jsonpath_evaluate_two_index_selectors_on_an_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({ "a": 1 })JSON")};
  const sourcemeta::core::JSONPath path{"$[0,1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_evaluate_two_index_selectors_one_past_the_end) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[0,5]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 1);
}

TEST(jsonpath_evaluate_two_index_selectors_one_past_the_front) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[0,-5]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 1);
}

// A path of three hundred segments over a chain of three hundred nested
// single-member objects, so evaluation runs past the recursion limit on the
// length of the path rather than on the depth of the document
TEST(jsonpath_evaluate_a_path_longer_than_the_recursion_limit) {
  auto document{sourcemeta::core::JSON{7}};
  std::string expression{"$"};
  expression.reserve(601);
  for (std::size_t level{0}; level < 300; level += 1) {
    auto wrapper{sourcemeta::core::JSON::make_object()};
    wrapper.assign("a", std::move(document));
    document = std::move(wrapper);
    expression.append(".a");
  }

  const sourcemeta::core::JSONPath path{expression};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_integer(), 7);
  EXPECT_EQ(nodes.at(0).location.size(), 300);
}

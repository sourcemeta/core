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

TEST(jsonpath_filter_existence) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": 1 }, { "b": 2 }, { "a": null } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

TEST(jsonpath_filter_existence_null_is_present) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": null } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_negated_existence) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": 1 }, { "b": 2 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?!@.a]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_equals_integer) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": 1 }, { "a": 2 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a == 2]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_equals_integer_and_real) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": 1 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a == 1.0]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_equals_deep) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": [ 1, 2 ] }, { "a": [ 1, 3 ] }, { "a": [ 1, 2 ] } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a == $[0].a]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

TEST(jsonpath_filter_equals_absent_both_sides) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "b": 1 }, { "c": 2 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.x == @.y]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

TEST(jsonpath_filter_equals_absent_one_side) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": null } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a == @.b]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_filter_not_equals) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": 1 }, { "a": 2 }, {} ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a != 1]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

TEST(jsonpath_filter_less_than_numbers) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": 1 }, { "a": 2.5 }, { "a": 3 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a < 3]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

TEST(jsonpath_filter_less_than_strings) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": "apple" }, { "a": "banana" } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a < 'b']"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_less_than_mixed_types) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": 1 }, { "a": "1" } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a < '2']"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_less_equal_absent) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ {} ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a <= @.b]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_boolean_literals) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": true }, { "a": false } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a == true]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_null_literal) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": null }, { "a": 0 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a == null]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_conjunction) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": 1, "b": 2 }, { "a": 1 }, { "b": 2 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a && @.b]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_disjunction) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": 1 }, { "b": 2 }, { "c": 3 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a || @.b]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

TEST(jsonpath_filter_negated_parenthesized) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": 1 }, { "b": 2 }, { "a": 1, "b": 2 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?!(@.a && @.b)]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

TEST(jsonpath_filter_on_object_members) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON({ "x": { "a": 1 }, "y": { "b": 2 } })JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_absolute_query) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON({ "limit": 2, "values": [ 1, 2, 3 ] })JSON")};
  const sourcemeta::core::JSONPath path{"$.values[?@ <= $.limit]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

TEST(jsonpath_filter_nested_filter) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": [ { "b": 1 } ] }, { "a": [ { "c": 2 } ] } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a[?@.b]]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_length_string) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": "ab" }, { "a": "abc" } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?length(@.a) == 2]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_length_counts_code_points) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": "héllo" } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?length(@.a) == 5]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_length_array_and_object) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": [ 1, 2, 3 ] }, { "a": { "x": 1 } } ])JSON")};
  const sourcemeta::core::JSONPath first{"$[?length(@.a) == 3]"};
  EXPECT_EQ(evaluate_nodes(first, document).size(), 1);
  const sourcemeta::core::JSONPath second{"$[?length(@.a) == 1]"};
  EXPECT_EQ(evaluate_nodes(second, document).size(), 1);
}

TEST(jsonpath_filter_length_of_number_is_nothing) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": 5 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?length(@.a) == @.b]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_count) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": [ 1, 2 ] }, { "a": [ 1 ] } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?count(@.a[*]) == 2]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_value_single_node) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": [ { "b": 7 } ] }, { "a": [ { "b": 7 }, { "b": 7 } ] } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?value(@..b) == 7]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_match_full) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": "ab" }, { "a": "xabx" } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?match(@.a, 'ab')]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_search_substring) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": "ab" }, { "a": "xabx" }, { "a": "xx" } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?search(@.a, 'ab')]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

TEST(jsonpath_filter_match_non_string_input) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": 1 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?match(@.a, 'a.*')]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_filter_match_invalid_pattern) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": "x" } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?match(@.a, '\\\\d')]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 0);
}

TEST(jsonpath_filter_match_dynamic_pattern) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": "ab", "p": "a." }, { "a": "ab", "p": "b." } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?match(@.a, @.p)]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_match_dot_excludes_line_feed) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": "x\ny" }, { "a": "xzy" } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?match(@.a, 'x.y')]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_match_dot_accepts_line_separator) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ { "a": "x\u2028y" } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?match(@.a, 'x.y')]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_less_than_decimal) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": 1e400 }, { "a": 5 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a > 100]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_less_than_exact_beyond_double_precision) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ { "a": 9007199254740993 }, { "a": 9007199254740991 } ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@.a > 9007199254740992.0]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
}

TEST(jsonpath_filter_match_literal_caret) {
  // NOTE: RFC 9485 Section 4 normatively makes an unescaped caret a literal
  // character, unlike the not normative Section 5 engine mappings that the
  // compliance suite anchor cases codify
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ "^ab", "^abc", "ab", "abc" ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?match(@, '^ab.*')]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
  EXPECT_EQ(nodes.at(0).value->to_string(), "^ab");
  EXPECT_EQ(nodes.at(1).value->to_string(), "^abc");
}

TEST(jsonpath_filter_match_literal_middle_dollar) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ "a$b", "ab" ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?match(@, 'a$b')]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_string(), "a$b");
}

TEST(jsonpath_filter_match_dollar_in_class) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ "a$b", "ab" ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?match(@, 'a[$]b')]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_string(), "a$b");
}

TEST(jsonpath_filter_match_quantified_caret) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ "^^", "", "a" ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?match(@, '^*')]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
  EXPECT_EQ(nodes.at(0).value->to_string(), "^^");
  EXPECT_EQ(nodes.at(1).value->to_string(), "");
}

TEST(jsonpath_filter_search_literal_dollar) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON([ "xab$cx", "xabcx" ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?search(@, 'ab$c')]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes.at(0).value->to_string(), "xab$cx");
}

TEST(jsonpath_filter_current_node_comparison) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 5, 10 ])JSON")};
  const sourcemeta::core::JSONPath path{"$[?@ > 4]"};
  const auto nodes{evaluate_nodes(path, document)};
  EXPECT_EQ(nodes.size(), 2);
}

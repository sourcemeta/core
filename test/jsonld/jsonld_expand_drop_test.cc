#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string
#include <vector> // std::vector

// Assert what the drop at the given index of the local drop list carries,
// naming the reason by its enumerator alone
// NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
#define EXPECT_JSONLD_EXPAND_DROP(index, expected_reason, expected_pointer,    \
                                  expected_value)                              \
  do {                                                                         \
    EXPECT_EQ(drops.at(index).reason,                                          \
              sourcemeta::core::JSONLDExpandDropReason::expected_reason);      \
    EXPECT_EQ(drops.at(index).pointer, (expected_pointer));                    \
    EXPECT_EQ(drops.at(index).value,                                           \
              sourcemeta::core::parse_json(expected_value));                   \
  } while (false)
// NOLINTEND(cppcoreguidelines-avoid-do-while)

namespace {

struct Drop {
  sourcemeta::core::JSONLDExpandDropReason reason;
  std::string pointer;
  sourcemeta::core::JSON value;
};

auto drop_collector(std::vector<Drop> &drops)
    -> sourcemeta::core::JSONLDExpandDropCallback {
  return [&drops](const sourcemeta::core::JSONLDExpandDropReason reason,
                  const sourcemeta::core::WeakPointer &pointer,
                  const sourcemeta::core::JSON &value) -> void {
    drops.push_back({.reason = reason,
                     .pointer = sourcemeta::core::to_string(pointer),
                     .value = value});
  };
}

auto collect_drops(const sourcemeta::core::JSON &input,
                   const sourcemeta::core::JSONLDVersion version =
                       sourcemeta::core::JSONLDVersion::V1_1)
    -> std::vector<Drop> {
  std::vector<Drop> drops;
  [[maybe_unused]] const auto result{sourcemeta::core::jsonld_expand(
      input, "", {}, version, drop_collector(drops))};
  return drops;
}

// What was reported up to the point expansion failed, where locating the
// error must not add to it
auto collect_drops_from_failure(const sourcemeta::core::JSON &input)
    -> std::vector<Drop> {
  std::vector<Drop> drops;
  bool failed{false};
  try {
    [[maybe_unused]] const auto result{sourcemeta::core::jsonld_expand(
        input, "", {}, sourcemeta::core::JSONLDVersion::V1_1,
        drop_collector(drops))};
  } catch (const sourcemeta::core::JSONLDError &) {
    failed = true;
  }

  EXPECT_TRUE(failed);
  return drops;
}

} // namespace

TEST(an_undefined_key_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "foo": "bar"
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, UndefinedKey, "/foo", R"("bar")");
}

TEST(a_key_with_keyword_form_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@foo": "bar"
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, KeywordFormKey, "/@foo", R"("bar")");
}

TEST(a_retired_key_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "foo": null },
    "foo": "bar"
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, RetiredKey, "/foo", R"("bar")");
}

TEST(a_lone_undefined_type_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": "@unknown",
    "http://example.com/p": "v"
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, UndefinedType, "/@type", R"("@unknown")");
}

TEST(an_undefined_type_among_others_is_reported_by_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": [ "@unknown", "http://example.com/T" ],
    "http://example.com/p": "v"
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, UndefinedType, "/@type/0", R"("@unknown")");
}

TEST(a_free_floating_list_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@list": [ "foo" ]
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, FreeFloatingValue, "/@list", R"([ "foo" ])");
}

TEST(a_free_floating_scalar_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"("hello")");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, FreeFloatingValue, "", R"("hello")");
}

TEST(a_free_floating_node_reference_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "http://example.com/a"
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, FreeFloatingValue, "",
                            R"({ "@id": "http://example.com/a" })");
}

TEST(a_free_floating_value_object_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": "@json",
    "@value": 42
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, FreeFloatingValue, "",
                            R"({ "@type": "@json", "@value": 42 })");
}

TEST(a_null_property_value_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": null
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, NullValue, "/p", "null");
}

TEST(a_null_among_array_items_is_reported_at_the_item) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": [ null, "bar" ]
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, NullValue, "/p/0", "null");
}

TEST(a_null_set_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@set": null
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, NullValue, "/@set", "null");
}

TEST(a_null_language_map_value_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@language" }
    },
    "p": { "en": null }
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, NullValue, "/p/en", "null");
}

TEST(a_null_value_object_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": { "@value": null }
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, NullValueObject, "/p", R"({ "@value": null })");
}

TEST(a_value_object_without_a_value_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": { "@language": "en", "@index": "i" }
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, IncompleteValueObject, "/p",
                            R"({ "@language": "en", "@index": "i" })");
}

TEST(a_keyword_with_no_handler_is_reported) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@vocab": "http://example.com/",
    "http://example.com/p": "v"
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, UnhandledKeyword, "/@vocab",
                            R"("http://example.com/")");
}

TEST(several_drops_are_reported_in_expansion_order) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "http://example.com/a",
    "beta": 2,
    "alpha": 1
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 3);
  EXPECT_JSONLD_EXPAND_DROP(0, UndefinedKey, "/alpha", "1");
  EXPECT_JSONLD_EXPAND_DROP(1, UndefinedKey, "/beta", "2");
  EXPECT_JSONLD_EXPAND_DROP(2, FreeFloatingValue, "",
                            R"({
                                 "@id": "http://example.com/a",
                                 "beta": 2,
                                 "alpha": 1
                               })");
}

TEST(locating_an_error_does_not_report_again) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "r": { "@reverse": "urn:p" } },
    "r": [ null, { "@value": 1 } ]
  })");

  const auto drops{collect_drops_from_failure(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, NullValue, "/r/0", "null");
}

TEST(a_dropped_key_reports_its_whole_subtree_once) {
  const auto input = sourcemeta::core::parse_json(R"({
    "foo": { "bar": { "baz": 1 } }
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, UndefinedKey, "/foo",
                            R"({ "bar": { "baz": 1 } })");
}

TEST(an_index_map_null_reports_once_through_the_nested_expansion) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@index" }
    },
    "p": { "i": null }
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, NullValue, "/p/i", "null");
}

TEST(a_reverse_map_reports_its_vanished_keys_and_not_the_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "foo": { "@id": "http://example.com/a" } },
    "http://example.com/p": "v"
  })");

  const auto drops{collect_drops(input)};

  EXPECT_EQ(drops.size(), 1);
  EXPECT_JSONLD_EXPAND_DROP(0, UndefinedKey, "/@reverse/foo",
                            R"({ "@id": "http://example.com/a" })");
}

TEST(an_empty_reverse_map_reports_nothing) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": {},
    "http://example.com/p": "v"
  })");

  EXPECT_EQ(collect_drops(input).size(), 0);
}

TEST(an_empty_object_reports_nothing) {
  const auto input = sourcemeta::core::parse_json("{}");

  EXPECT_EQ(collect_drops(input).size(), 0);
}

TEST(a_document_that_drops_nothing_reports_nothing) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": "v"
  })");

  EXPECT_EQ(collect_drops(input).size(), 0);
}

TEST(a_direction_under_1_0_processing_reports_nothing) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": { "@value": "v", "@direction": "ltr" }
  })");

  EXPECT_EQ(collect_drops(input, sourcemeta::core::JSONLDVersion::V1_0).size(),
            0);
}

TEST(an_index_map_none_key_reports_nothing) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@version": 1.1,
      "p": { "@id": "http://example.com/p", "@container": "@index" }
    },
    "p": { "@none": "v" }
  })");

  EXPECT_EQ(collect_drops(input).size(), 0);
}

TEST(expansion_is_unchanged_without_a_callback) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "foo": "bar",
    "p": [ null, "v" ]
  })");

  const auto without{sourcemeta::core::jsonld_expand(input)};
  const sourcemeta::core::JSONLDExpandDropCallback callback =
      [](const sourcemeta::core::JSONLDExpandDropReason,
         const sourcemeta::core::WeakPointer &,
         const sourcemeta::core::JSON &) -> void {};
  const auto with{sourcemeta::core::jsonld_expand(
      input, "", {}, sourcemeta::core::JSONLDVersion::V1_1, callback)};

  EXPECT_EQ(with, without);
}

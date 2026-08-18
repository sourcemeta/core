#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uritemplate.h>

#include <cstddef>       // std::size_t
#include <map>           // std::map
#include <optional>      // std::optional, std::nullopt
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <tuple>         // std::make_tuple
#include <unordered_map> // std::unordered_map

static auto single_value(const std::string_view value) {
  return
      [value](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          return std::make_tuple(value, std::nullopt, false);
        }
        return std::nullopt;
      };
}

TEST(simple_two_byte_character_passes_through) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      single_value("café"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "café");
}

TEST(simple_two_byte_character_default_mode_still_encodes) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(single_value("café"));

  EXPECT_EQ(result, "caf%C3%A9");
}

TEST(simple_two_byte_character_explicit_uri_mode_matches_default) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      single_value("café"), sourcemeta::core::URITemplateExpansionMode::URI);

  EXPECT_EQ(result, "caf%C3%A9");
}

TEST(reserved_full_identifier_passes_through) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result =
      uri_template.expand(single_value("https://example.com/café"),
                          sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "https://example.com/café");
}

TEST(reserved_full_identifier_default_mode_still_encodes) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result =
      uri_template.expand(single_value("https://example.com/café"));

  EXPECT_EQ(result, "https://example.com/caf%C3%A9");
}

TEST(fragment_two_byte_character_passes_through) {
  const sourcemeta::core::URITemplate uri_template{"{#var}"};
  const auto result = uri_template.expand(
      single_value("café"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "#café");
}

TEST(label_two_byte_character_passes_through) {
  const sourcemeta::core::URITemplate uri_template{"{.var}"};
  const auto result = uri_template.expand(
      single_value("café"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, ".café");
}

TEST(path_two_byte_character_passes_through) {
  const sourcemeta::core::URITemplate uri_template{"{/var}"};
  const auto result = uri_template.expand(
      single_value("café"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "/café");
}

TEST(path_style_two_byte_character_passes_through) {
  const sourcemeta::core::URITemplate uri_template{"{;var}"};
  const auto result = uri_template.expand(
      single_value("café"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, ";var=café");
}

TEST(query_two_byte_character_passes_through) {
  const sourcemeta::core::URITemplate uri_template{"{?var}"};
  const auto result = uri_template.expand(
      single_value("café"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "?var=café");
}

TEST(query_continuation_two_byte_character_passes_through) {
  const sourcemeta::core::URITemplate uri_template{"{&var}"};
  const auto result = uri_template.expand(
      single_value("café"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "&var=café");
}

TEST(simple_three_byte_character_passes_through) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      single_value("a→b"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "a→b");
}

TEST(simple_four_byte_character_passes_through) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      single_value("😀"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "😀");
}

TEST(simple_han_characters_pass_through) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      single_value("東京"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "東京");
}

TEST(reserved_mixed_widths_pass_through) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result =
      uri_template.expand(single_value("café/→/東京/😀"),
                          sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "café/→/東京/😀");
}

TEST(ascii_only_value_matches_uri_mode_simple) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto uri_result = uri_template.expand(single_value("plain-value"));
  const auto iri_result =
      uri_template.expand(single_value("plain-value"),
                          sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(uri_result, "plain-value");
  EXPECT_EQ(iri_result, "plain-value");
}

TEST(ascii_only_value_matches_uri_mode_reserved) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto uri_result =
      uri_template.expand(single_value("https://example.com/a?b=c#d"));
  const auto iri_result =
      uri_template.expand(single_value("https://example.com/a?b=c#d"),
                          sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(uri_result, "https://example.com/a?b=c#d");
  EXPECT_EQ(iri_result, "https://example.com/a?b=c#d");
}

TEST(reserved_existing_triplets_are_not_decoded) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result =
      uri_template.expand(single_value("caf%C3%A9"),
                          sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "caf%C3%A9");
}

TEST(reserved_mixed_raw_and_encoded_keeps_both_spellings) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result =
      uri_template.expand(single_value("café%20menu"),
                          sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "café%20menu");
}

TEST(simple_still_encodes_percent_sign) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result =
      uri_template.expand(single_value("caf%C3%A9"),
                          sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "caf%25C3%25A9");
}

TEST(simple_still_encodes_space) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      single_value("a b"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "a%20b");
}

TEST(reserved_still_encodes_space) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result = uri_template.expand(
      single_value("a b"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "a%20b");
}

TEST(reserved_still_encodes_control_character) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result = uri_template.expand(
      single_value("a\nb"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "a%0Ab");
}

TEST(reserved_still_encodes_angle_brackets_and_quote) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result =
      uri_template.expand(single_value("<a>\"b\""),
                          sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "%3Ca%3E%22b%22");
}

TEST(simple_still_encodes_reserved_characters) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  const auto result = uri_template.expand(
      single_value("a/b?c#d"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "a%2Fb%3Fc%23d");
}

TEST(reserved_still_passes_reserved_characters) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const auto result = uri_template.expand(
      single_value("a/b?c#d"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "a/b?c#d");
}

TEST(literal_and_variable_both_stay_raw) {
  const sourcemeta::core::URITemplate uri_template{
      "https://example.com/café/{var}"};
  const auto result = uri_template.expand(
      single_value("menü"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "https://example.com/café/menü");
}

TEST(literal_stays_raw_while_default_mode_encodes_the_variable) {
  const sourcemeta::core::URITemplate uri_template{
      "https://example.com/café/{var}"};
  const auto result = uri_template.expand(single_value("menü"));

  EXPECT_EQ(result, "https://example.com/café/men%C3%BC");
}

TEST(prefix_modifier_counts_characters_and_passes_them_through) {
  const sourcemeta::core::URITemplate uri_template{"{var:3}"};
  const auto result = uri_template.expand(
      single_value("café"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "caf");
}

TEST(prefix_modifier_covering_multi_byte_character_passes_it_through) {
  const sourcemeta::core::URITemplate uri_template{"{var:4}"};
  const auto result = uri_template.expand(
      single_value("café!"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "café");
}

TEST(multiple_variables_mix_raw_and_encoded_rules) {
  const sourcemeta::core::URITemplate uri_template{"{x,y}"};
  const auto result = uri_template.expand(
      [](const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "x") {
          return std::make_tuple(std::string_view{"café"}, std::nullopt, false);
        }
        if (name == "y") {
          return std::make_tuple(std::string_view{"a b"}, std::nullopt, false);
        }
        return std::nullopt;
      },
      sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "café,a%20b");
}

TEST(list_values_pass_through) {
  const sourcemeta::core::URITemplate uri_template{"{var}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"café"}, std::nullopt,
                                   true);
          }
          return std::make_tuple(std::string_view{"menü"}, std::nullopt, false);
        }
        return std::nullopt;
      },
      sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "café,menü");
}

TEST(list_explode_path_values_pass_through) {
  const sourcemeta::core::URITemplate uri_template{"{/var*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"café"}, std::nullopt,
                                   true);
          }
          return std::make_tuple(std::string_view{"東京"}, std::nullopt, false);
        }
        return std::nullopt;
      },
      sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "/café/東京");
}

TEST(object_explode_keys_and_values_pass_through) {
  const sourcemeta::core::URITemplate uri_template{"{?var*}"};
  std::size_t call_count = 0;
  const auto result = uri_template.expand(
      [&call_count](
          const std::string_view name) -> sourcemeta::core::URITemplateValue {
        if (name == "var") {
          call_count++;
          if (call_count == 1) {
            return std::make_tuple(std::string_view{"café"},
                                   std::optional<std::string_view>{"clé"},
                                   true);
          }
          return std::make_tuple(std::string_view{"menü"},
                                 std::optional<std::string_view>{"öl"}, false);
        }
        return std::nullopt;
      },
      sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "?clé=café&öl=menü");
}

TEST(map_container_overload_passes_through) {
  const sourcemeta::core::URITemplate uri_template{"/results/{var}"};
  const std::map<std::string, std::string> variables{{"var", "café"}};
  const auto result = uri_template.expand(
      variables, sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "/results/café");
}

TEST(map_container_overload_default_mode_still_encodes) {
  const sourcemeta::core::URITemplate uri_template{"/results/{var}"};
  const std::map<std::string, std::string> variables{{"var", "café"}};
  const auto result = uri_template.expand(variables);

  EXPECT_EQ(result, "/results/caf%C3%A9");
}

TEST(unordered_map_container_overload_passes_through) {
  const sourcemeta::core::URITemplate uri_template{"{+var}"};
  const std::unordered_map<std::string, std::string> variables{
      {"var", "https://example.com/café"}};
  const auto result = uri_template.expand(
      variables, sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "https://example.com/café");
}

TEST(fragment_existing_triplets_are_not_decoded) {
  const sourcemeta::core::URITemplate uri_template{"{#var}"};
  const auto result =
      uri_template.expand(single_value("caf%C3%A9"),
                          sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "#caf%C3%A9");
}

TEST(query_still_encodes_ampersand_in_value) {
  const sourcemeta::core::URITemplate uri_template{"{?var}"};
  const auto result = uri_template.expand(
      single_value("a&café"), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "?var=a%26café");
}

TEST(missing_variable_expands_to_nothing) {
  const sourcemeta::core::URITemplate uri_template{"/a/{var}"};
  const auto result = uri_template.expand(
      [](const std::string_view) -> sourcemeta::core::URITemplateValue {
        return std::nullopt;
      },
      sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "/a/");
}

TEST(empty_value_expands_to_nothing) {
  const sourcemeta::core::URITemplate uri_template{"/a/{var}"};
  const auto result = uri_template.expand(
      single_value(""), sourcemeta::core::URITemplateExpansionMode::IRI);

  EXPECT_EQ(result, "/a/");
}

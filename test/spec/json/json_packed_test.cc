#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <sstream>
#include <stdexcept>
#include <string>

TEST(packed_matches_owning_parser) {
  sourcemeta::core::JSONPacked packed;
  for (
      const auto *input :
      {R"({"outer":{"inner":0},"a\u0062":[null,true,false,-42,2.5,1.2345678901234567890123456789],"\u0000":"\uD83D\uDE00"})",
       R"(["line\nfeed",{},[],[1]])", "null", "1.2", "-9223372036854775808",
       "9223372036854775808", "{}", "[]", "\"string\"", "false",
       " \r\n  true \t "}) {
    packed.parse(input);
    EXPECT_EQ(packed.materialize(), sourcemeta::core::parse_json(input));
  }
}

TEST(packed_materializes_duplicate_keys_like_the_owning_parser) {
  constexpr std::string_view input{R"({"key":1,"key":2})"};
  sourcemeta::core::JSONPacked packed;
  packed.parse(std::string{input});
  std::ostringstream actual;
  std::ostringstream expected;
  sourcemeta::core::stringify(packed.materialize(), actual);
  sourcemeta::core::stringify(sourcemeta::core::parse_json(input), expected);
  EXPECT_EQ(actual.str(), expected.str());
}

TEST(packed_root_lookup_skips_nested_values_and_decodes_keys) {
  sourcemeta::core::JSONPacked packed;
  packed.parse(
      R"({"a":[{"nested":true}],"b":{"deep":[[]]},"c\u0064":0,"\u0000":null})");
  EXPECT_TRUE(packed.contains_root_key("a"));
  EXPECT_TRUE(packed.contains_root_key("b"));
  EXPECT_TRUE(packed.contains_root_key("cd"));
  EXPECT_TRUE(packed.contains_root_key(std::string_view{"\0", 1}));
  EXPECT_FALSE(packed.contains_root_key("nested"));
  EXPECT_FALSE(packed.contains_root_key("deep"));
  EXPECT_FALSE(packed.contains_root_key(""));
  packed.parse(R"([{"a":0}])");
  EXPECT_FALSE(packed.contains_root_key("a"));
}

TEST(packed_owns_input_and_materialized_value_survives_reuse) {
  sourcemeta::core::JSONPacked packed;
  std::string source{
      R"({"longer_key_than_short_string_storage":"a long value for this test"})"};
  packed.parse(source);
  source.clear();
  auto value = packed.materialize();
  packed.reset();
  EXPECT_FALSE(
      packed.contains_root_key("longer_key_than_short_string_storage"));
  packed.parse(R"({"next":2})");
  EXPECT_TRUE(packed.contains_root_key("next"));
  EXPECT_EQ(
      value,
      sourcemeta::core::parse_json(
          R"({"longer_key_than_short_string_storage":"a long value for this test"})"));
}

TEST(packed_parse_failure_clears_previous_document) {
  sourcemeta::core::JSONPacked packed;
  for (const auto *invalid :
       {"", " \t", "true false", R"({"key":[})", R"("\uD800")", "1e+"}) {
    packed.parse(R"({"key":1})");
    try {
      packed.parse(invalid);
      FAIL();
    } catch (const sourcemeta::core::JSONParseError &) {
      EXPECT_FALSE(packed.contains_root_key("key"));
    }
    try {
      static_cast<void>(packed.materialize());
      FAIL();
    } catch (const std::logic_error &) {
      EXPECT_FALSE(packed.contains_root_key("key"));
    }
  }
  packed.parse("42");
  EXPECT_EQ(packed.materialize(), sourcemeta::core::parse_json("42"));
}

TEST(packed_materializes_deeply_nested_input) {
  const std::string input{std::string(4096, '[') + "0" +
                          std::string(4096, ']')};
  sourcemeta::core::JSONPacked packed;
  packed.parse(input);
  EXPECT_EQ(packed.materialize(), sourcemeta::core::parse_json(input));
}

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

TEST(empty_on_integer) {
  const sourcemeta::core::JSON document{5};
  const sourcemeta::core::Pointer pointer;
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document);
}

TEST(empty_on_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON")};

  const sourcemeta::core::Pointer pointer;
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document);
}

TEST(empty_on_array) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON")};
  const sourcemeta::core::Pointer pointer;
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document);
}

TEST(top_level_property_true) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON")};

  const sourcemeta::core::Pointer pointer{"foo"};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document.at("foo"));
}

TEST(top_level_property_false) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON")};

  const sourcemeta::core::Pointer pointer{"bar"};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(top_level_index_true) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON")};

  const sourcemeta::core::Pointer pointer{2};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document.at(2));
}

TEST(top_level_index_false) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON")};

  const sourcemeta::core::Pointer pointer{3};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(top_level_numeric_property_true) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "0": 1
  })JSON")};

  const sourcemeta::core::Pointer pointer{"0"};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document.at("0"));
}

TEST(top_level_numeric_property_as_index_true) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "0": 1
  })JSON")};

  const sourcemeta::core::Pointer pointer{0};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document.at("0"));
}

TEST(complex_true) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": {
      "bar": [ 1, 2, { "baz": "qux" } ]
    }
  })JSON")};

  const sourcemeta::core::Pointer pointer{"foo", "bar", 2, "baz"};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document.at("foo").at("bar").at(2).at("baz"));
}

TEST(complex_false) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": {
      "bar": [ 1, 2, { "baz": "qux" } ]
    }
  })JSON")};

  const sourcemeta::core::Pointer pointer{"foo", 2, "baz"};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(complex_non_existent_property) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": {
      "bar": [ 1, 2, { "baz": "qux" } ]
    }
  })JSON")};

  const sourcemeta::core::Pointer pointer{"foo", 2, "xxx"};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(complex_non_existent_index) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": {
      "bar": [ 1, 2, { "baz": "qux" } ]
    }
  })JSON")};

  const sourcemeta::core::Pointer pointer{"foo", 9, "xxx"};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(non_object) {
  const sourcemeta::core::JSON document{"foo"};
  const sourcemeta::core::Pointer pointer{"bar"};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(non_array) {
  const sourcemeta::core::JSON document{"foo"};
  const sourcemeta::core::Pointer pointer{2};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(out_of_range_index_resolves_object_member) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "18446744073709551616": 42
  })JSON")};

  const auto pointer{sourcemeta::core::to_pointer("/18446744073709551616")};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document.at("18446744073709551616"));
}

TEST(out_of_range_index_not_found_against_array) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON")};
  const auto pointer{sourcemeta::core::to_pointer("/18446744073709551616")};
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(empty_weak_pointer_on_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON")};

  const sourcemeta::core::WeakPointer pointer;
  const auto *const result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document);
}

TEST(property_token) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON");

  const sourcemeta::core::Pointer::Token token{"foo"};
  const auto *result{sourcemeta::core::try_get(document, token)};
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 1);
}

TEST(missing_property_token) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON");

  const sourcemeta::core::Pointer::Token token{"bar"};
  EXPECT_FALSE(sourcemeta::core::try_get(document, token));
}

TEST(property_token_on_array) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON([ 1, 2 ])JSON");

  const sourcemeta::core::Pointer::Token token{"foo"};
  EXPECT_FALSE(sourcemeta::core::try_get(document, token));
}

TEST(index_token_on_array) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON([ 1, 2 ])JSON");

  const sourcemeta::core::Pointer::Token token{1};
  const auto *result{sourcemeta::core::try_get(document, token)};
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 2);
}

TEST(out_of_range_index_token_on_array) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON([ 1, 2 ])JSON");

  const sourcemeta::core::Pointer::Token token{2};
  EXPECT_FALSE(sourcemeta::core::try_get(document, token));
}

TEST(index_token_on_object) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "0": 1
  })JSON");

  const sourcemeta::core::Pointer::Token token{0};
  const auto *result{sourcemeta::core::try_get(document, token)};
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 1);
}

TEST(missing_index_token_on_object) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "0": 1
  })JSON");

  const sourcemeta::core::Pointer::Token token{1};
  EXPECT_FALSE(sourcemeta::core::try_get(document, token));
}

TEST(index_token_on_scalar) {
  const sourcemeta::core::JSON document{5};

  const sourcemeta::core::Pointer::Token token{0};
  EXPECT_FALSE(sourcemeta::core::try_get(document, token));
}

TEST(weak_property_token) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON");

  const std::string property{"foo"};
  const sourcemeta::core::WeakPointer::Token token{std::cref(property)};
  const auto *result{sourcemeta::core::try_get(document, token)};
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 1);
}

TEST(weak_index_token_on_object) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "0": 1
  })JSON");

  const sourcemeta::core::WeakPointer::Token token{0};
  const auto *result{sourcemeta::core::try_get(document, token)};
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 1);
}

#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

TEST(JSONPointer_try_get, empty_on_integer) {
  const sourcemeta::core::JSON document{5};
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document);
}

TEST(JSONPointer_try_get, empty_on_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON")};

  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document);
}

TEST(JSONPointer_try_get, empty_on_array) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON")};
  const sourcemeta::core::Pointer pointer;
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document);
}

TEST(JSONPointer_try_get, top_level_property_true) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON")};

  const sourcemeta::core::Pointer pointer{"foo"};
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document.at("foo"));
}

TEST(JSONPointer_try_get, top_level_property_false) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON")};

  const sourcemeta::core::Pointer pointer{"bar"};
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(JSONPointer_try_get, top_level_index_true) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON")};

  const sourcemeta::core::Pointer pointer{2};
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document.at(2));
}

TEST(JSONPointer_try_get, top_level_index_false) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON")};

  const sourcemeta::core::Pointer pointer{3};
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(JSONPointer_try_get, top_level_numeric_property_true) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "0": 1
  })JSON")};

  const sourcemeta::core::Pointer pointer{"0"};
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document.at("0"));
}

TEST(JSONPointer_try_get, top_level_numeric_property_as_index_true) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "0": 1
  })JSON")};

  const sourcemeta::core::Pointer pointer{0};
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document.at("0"));
}

TEST(JSONPointer_try_get, complex_true) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": {
      "bar": [ 1, 2, { "baz": "qux" } ]
    }
  })JSON")};

  const sourcemeta::core::Pointer pointer{"foo", "bar", 2, "baz"};
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_TRUE(result);
  EXPECT_EQ(*result, document.at("foo").at("bar").at(2).at("baz"));
}

TEST(JSONPointer_try_get, complex_false) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": {
      "bar": [ 1, 2, { "baz": "qux" } ]
    }
  })JSON")};

  const sourcemeta::core::Pointer pointer{"foo", 2, "baz"};
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(JSONPointer_try_get, complex_non_existent_property) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": {
      "bar": [ 1, 2, { "baz": "qux" } ]
    }
  })JSON")};

  const sourcemeta::core::Pointer pointer{"foo", 2, "xxx"};
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(JSONPointer_try_get, complex_non_existent_index) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "foo": {
      "bar": [ 1, 2, { "baz": "qux" } ]
    }
  })JSON")};

  const sourcemeta::core::Pointer pointer{"foo", 9, "xxx"};
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(JSONPointer_try_get, non_object) {
  const sourcemeta::core::JSON document{"foo"};
  const sourcemeta::core::Pointer pointer{"bar"};
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

TEST(JSONPointer_try_get, non_array) {
  const sourcemeta::core::JSON document{"foo"};
  const sourcemeta::core::Pointer pointer{2};
  const auto result{sourcemeta::core::try_get(document, pointer)};
  EXPECT_FALSE(result);
}

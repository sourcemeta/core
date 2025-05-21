#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

TEST(JSONPointer_set, property_to_integer) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1 }");
  const sourcemeta::core::Pointer pointer{"foo"};
  sourcemeta::core::set(document, pointer, sourcemeta::core::JSON{2});
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_integer());
  EXPECT_EQ(document.at("foo").to_integer(), 2);
}

TEST(JSONPointer_set, property_to_integer_copy) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1 }");
  const sourcemeta::core::Pointer pointer{"foo"};
  const sourcemeta::core::JSON value{2};
  sourcemeta::core::set(document, pointer, value);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_integer());
  EXPECT_EQ(document.at("foo").to_integer(), 2);
}

TEST(JSONPointer_set, top_level_to_integer) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1 }");
  const sourcemeta::core::Pointer pointer;
  sourcemeta::core::set(document, pointer, sourcemeta::core::JSON{2});
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 2);
}

TEST(JSONPointer_set, top_level_to_integer_copy) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1 }");
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::JSON value{2};
  sourcemeta::core::set(document, pointer, value);
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 2);
}

TEST(JSONPointer_set, array_element_to_integer) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": [ 1, 2, 3 ] }");
  const sourcemeta::core::Pointer pointer{"foo", 1};
  sourcemeta::core::set(document, pointer, sourcemeta::core::JSON{9});
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_array());
  EXPECT_EQ(document.at("foo").size(), 3);
  EXPECT_TRUE(document.at("foo").at(0).is_integer());
  EXPECT_TRUE(document.at("foo").at(1).is_integer());
  EXPECT_TRUE(document.at("foo").at(2).is_integer());
  EXPECT_EQ(document.at("foo").at(0).to_integer(), 1);
  EXPECT_EQ(document.at("foo").at(1).to_integer(), 9);
  EXPECT_EQ(document.at("foo").at(2).to_integer(), 3);
}

TEST(JSONPointer_set, array_element_to_integer_copy) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": [ 1, 2, 3 ] }");
  const sourcemeta::core::Pointer pointer{"foo", 1};
  const sourcemeta::core::JSON value{9};
  sourcemeta::core::set(document, pointer, value);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_array());
  EXPECT_EQ(document.at("foo").size(), 3);
  EXPECT_TRUE(document.at("foo").at(0).is_integer());
  EXPECT_TRUE(document.at("foo").at(1).is_integer());
  EXPECT_TRUE(document.at("foo").at(2).is_integer());
  EXPECT_EQ(document.at("foo").at(0).to_integer(), 1);
  EXPECT_EQ(document.at("foo").at(1).to_integer(), 9);
  EXPECT_EQ(document.at("foo").at(2).to_integer(), 3);
}

TEST(JSONPointer_set, hyphen_property_in_object) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": { \"-\": 1 } }");
  const sourcemeta::core::Pointer pointer{"foo", "-"};
  sourcemeta::core::set(document, pointer, sourcemeta::core::JSON{2});
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_EQ(document.at("foo").size(), 1);
  EXPECT_TRUE(document.at("foo").defines("-"));
  EXPECT_TRUE(document.at("foo").at("-").is_integer());
  EXPECT_EQ(document.at("foo").at("-").to_integer(), 2);
}

TEST(JSONPointer_set, hyphen_in_array_single_token) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[ 1, 2, 3 ]");
  const sourcemeta::core::Pointer pointer{"-"};
  sourcemeta::core::set(document, pointer, sourcemeta::core::JSON{4});
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 4);
  EXPECT_TRUE(document.at(0).is_integer());
  EXPECT_TRUE(document.at(1).is_integer());
  EXPECT_TRUE(document.at(2).is_integer());
  EXPECT_TRUE(document.at(3).is_integer());
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 3);
  EXPECT_EQ(document.at(3).to_integer(), 4);
}

TEST(JSONPointer_set, hyphen_in_array_single_token_copy) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("[ 1, 2, 3 ]");
  const sourcemeta::core::Pointer pointer{"-"};
  const sourcemeta::core::JSON value{4};
  sourcemeta::core::set(document, pointer, value);
  EXPECT_TRUE(document.is_array());
  EXPECT_EQ(document.size(), 4);
  EXPECT_TRUE(document.at(0).is_integer());
  EXPECT_TRUE(document.at(1).is_integer());
  EXPECT_TRUE(document.at(2).is_integer());
  EXPECT_TRUE(document.at(3).is_integer());
  EXPECT_EQ(document.at(0).to_integer(), 1);
  EXPECT_EQ(document.at(1).to_integer(), 2);
  EXPECT_EQ(document.at(2).to_integer(), 3);
  EXPECT_EQ(document.at(3).to_integer(), 4);
}

TEST(JSONPointer_set, hyphen_in_array_multiple_tokens) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": [ 1, 2, 3 ] }");
  const sourcemeta::core::Pointer pointer{"foo", "-"};
  sourcemeta::core::set(document, pointer, sourcemeta::core::JSON{4});
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_array());
  EXPECT_TRUE(document.at("foo").at(0).is_integer());
  EXPECT_TRUE(document.at("foo").at(1).is_integer());
  EXPECT_TRUE(document.at("foo").at(2).is_integer());
  EXPECT_TRUE(document.at("foo").at(3).is_integer());
  EXPECT_EQ(document.at("foo").at(0).to_integer(), 1);
  EXPECT_EQ(document.at("foo").at(1).to_integer(), 2);
  EXPECT_EQ(document.at("foo").at(2).to_integer(), 3);
  EXPECT_EQ(document.at("foo").at(3).to_integer(), 4);
}

TEST(JSONPointer_set, hyphen_in_array_multiple_tokens_copy) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": [ 1, 2, 3 ] }");
  const sourcemeta::core::Pointer pointer{"foo", "-"};
  const sourcemeta::core::JSON value{4};
  sourcemeta::core::set(document, pointer, value);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_array());
  EXPECT_TRUE(document.at("foo").at(0).is_integer());
  EXPECT_TRUE(document.at("foo").at(1).is_integer());
  EXPECT_TRUE(document.at("foo").at(2).is_integer());
  EXPECT_TRUE(document.at("foo").at(3).is_integer());
  EXPECT_EQ(document.at("foo").at(0).to_integer(), 1);
  EXPECT_EQ(document.at("foo").at(1).to_integer(), 2);
  EXPECT_EQ(document.at("foo").at(2).to_integer(), 3);
  EXPECT_EQ(document.at("foo").at(3).to_integer(), 4);
}

TEST(JSONPointer_set, hyphen_in_array_multiple_tokens_with_inner_hyphen) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"-\": [ 1, 2, 3 ] }");
  const sourcemeta::core::Pointer pointer{"-", "-"};
  sourcemeta::core::set(document, pointer, sourcemeta::core::JSON{4});
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("-"));
  EXPECT_TRUE(document.at("-").is_array());
  EXPECT_TRUE(document.at("-").at(0).is_integer());
  EXPECT_TRUE(document.at("-").at(1).is_integer());
  EXPECT_TRUE(document.at("-").at(2).is_integer());
  EXPECT_TRUE(document.at("-").at(3).is_integer());
  EXPECT_EQ(document.at("-").at(0).to_integer(), 1);
  EXPECT_EQ(document.at("-").at(1).to_integer(), 2);
  EXPECT_EQ(document.at("-").at(2).to_integer(), 3);
  EXPECT_EQ(document.at("-").at(3).to_integer(), 4);
}

TEST(JSONPointer_set, hyphen_in_array_multiple_tokens_with_inner_hyphen_copy) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"-\": [ 1, 2, 3 ] }");
  const sourcemeta::core::Pointer pointer{"-", "-"};
  const sourcemeta::core::JSON value{4};
  sourcemeta::core::set(document, pointer, value);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("-"));
  EXPECT_TRUE(document.at("-").is_array());
  EXPECT_TRUE(document.at("-").at(0).is_integer());
  EXPECT_TRUE(document.at("-").at(1).is_integer());
  EXPECT_TRUE(document.at("-").at(2).is_integer());
  EXPECT_TRUE(document.at("-").at(3).is_integer());
  EXPECT_EQ(document.at("-").at(0).to_integer(), 1);
  EXPECT_EQ(document.at("-").at(1).to_integer(), 2);
  EXPECT_EQ(document.at("-").at(2).to_integer(), 3);
  EXPECT_EQ(document.at("-").at(3).to_integer(), 4);
}

TEST(JSONPointer_set, positive_integer_property) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "0": 1
  })JSON");

  const sourcemeta::core::Pointer pointer{0};
  const sourcemeta::core::JSON value{4};
  sourcemeta::core::set(document, pointer, value);
  EXPECT_TRUE(document.at("0").is_integer());
  EXPECT_EQ(document.at("0").to_integer(), 4);
}

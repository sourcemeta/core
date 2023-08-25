#include <gtest/gtest.h>
#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonpointer.h>

TEST(JSONPointer_set, property_to_integer) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1 }");
  const sourcemeta::jsontoolkit::Pointer pointer{"foo"};
  sourcemeta::jsontoolkit::set(document, pointer,
                               sourcemeta::jsontoolkit::JSON{2});
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_integer());
  EXPECT_EQ(document.at("foo").to_integer(), 2);
}

TEST(JSONPointer_set, property_to_integer_copy) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1 }");
  const sourcemeta::jsontoolkit::Pointer pointer{"foo"};
  const sourcemeta::jsontoolkit::JSON value{2};
  sourcemeta::jsontoolkit::set(document, pointer, value);
  EXPECT_TRUE(document.is_object());
  EXPECT_EQ(document.size(), 1);
  EXPECT_TRUE(document.defines("foo"));
  EXPECT_TRUE(document.at("foo").is_integer());
  EXPECT_EQ(document.at("foo").to_integer(), 2);
}

TEST(JSONPointer_set, top_level_to_integer) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1 }");
  const sourcemeta::jsontoolkit::Pointer pointer;
  sourcemeta::jsontoolkit::set(document, pointer,
                               sourcemeta::jsontoolkit::JSON{2});
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 2);
}

TEST(JSONPointer_set, top_level_to_integer_copy) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1 }");
  const sourcemeta::jsontoolkit::Pointer pointer;
  const sourcemeta::jsontoolkit::JSON value{2};
  sourcemeta::jsontoolkit::set(document, pointer, value);
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 2);
}

TEST(JSONPointer_set, array_element_to_integer) {
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse("{ \"foo\": [ 1, 2, 3 ] }");
  const sourcemeta::jsontoolkit::Pointer pointer{"foo", 1};
  sourcemeta::jsontoolkit::set(document, pointer,
                               sourcemeta::jsontoolkit::JSON{9});
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
  sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse("{ \"foo\": [ 1, 2, 3 ] }");
  const sourcemeta::jsontoolkit::Pointer pointer{"foo", 1};
  const sourcemeta::jsontoolkit::JSON value{9};
  sourcemeta::jsontoolkit::set(document, pointer, value);
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

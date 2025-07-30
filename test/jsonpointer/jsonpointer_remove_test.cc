#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

TEST(JSONPointer_remove, array_element) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON");

  sourcemeta::core::remove(document, sourcemeta::core::Pointer{1});
  EXPECT_EQ(document, sourcemeta::core::parse_json(R"JSON([ 1, 3 ])JSON"));
}

TEST(JSONPointer_remove, empty_property) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON({ "foo": 1, "": 2 })JSON");

  sourcemeta::core::remove(document, sourcemeta::core::Pointer{""});
  EXPECT_EQ(document, sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON"));
}

TEST(JSONPointer_remove, nonexistent_property) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON");

  sourcemeta::core::remove(document, sourcemeta::core::Pointer{"bar"});
  EXPECT_EQ(document, sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON"));
}

TEST(JSONPointer_remove, cannot_remove_self) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON({ "foo": 1, "": 2 })JSON");

  sourcemeta::core::remove(document, sourcemeta::core::Pointer{});
  EXPECT_EQ(document,
            sourcemeta::core::parse_json(R"JSON({ "foo": 1, "": 2 })JSON"));
}

TEST(JSONPointer_remove, positive_integer_property) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON({ "1": 1, "foo": 2 })JSON");

  sourcemeta::core::remove(document, sourcemeta::core::Pointer{1});
  EXPECT_EQ(document, sourcemeta::core::parse_json(R"JSON({ "foo": 2 })JSON"));
}

TEST(JSONPointer_remove, positive_integer_property_string_pointer) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON({ "1": 1, "foo": 2 })JSON");

  sourcemeta::core::remove(document, sourcemeta::core::Pointer{"1"});
  EXPECT_EQ(document, sourcemeta::core::parse_json(R"JSON({ "foo": 2 })JSON"));
}

TEST(JSONWeakPointer_remove, array_element) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON");

  sourcemeta::core::remove(document, sourcemeta::core::WeakPointer{1});
  EXPECT_EQ(document, sourcemeta::core::parse_json(R"JSON([ 1, 3 ])JSON"));
}

TEST(JSONWeakPointer_remove, empty_property) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON({ "foo": 1, "": 2 })JSON");

  const std::string empty = "";
  sourcemeta::core::remove(document,
                           sourcemeta::core::WeakPointer{std::cref(empty)});
  EXPECT_EQ(document, sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON"));
}

TEST(JSONWeakPointer_remove, nonexistent_property) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON");

  const std::string bar = "bar";
  sourcemeta::core::remove(document,
                           sourcemeta::core::WeakPointer{std::cref(bar)});
  EXPECT_EQ(document, sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON"));
}

TEST(JSONWeakPointer_remove, cannot_remove_self) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON({ "foo": 1, "": 2 })JSON");

  sourcemeta::core::remove(document, sourcemeta::core::WeakPointer{});
  EXPECT_EQ(document,
            sourcemeta::core::parse_json(R"JSON({ "foo": 1, "": 2 })JSON"));
}

TEST(JSONWeakPointer_remove, positive_integer_property) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON({ "1": 1, "foo": 2 })JSON");

  sourcemeta::core::remove(document, sourcemeta::core::WeakPointer{1});
  EXPECT_EQ(document, sourcemeta::core::parse_json(R"JSON({ "foo": 2 })JSON"));
}

TEST(JSONWeakPointer_remove, positive_integer_property_string_pointer) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON({ "1": 1, "foo": 2 })JSON");

  const std::string one = "1";
  sourcemeta::core::remove(document,
                           sourcemeta::core::WeakPointer{std::cref(one)});
  EXPECT_EQ(document, sourcemeta::core::parse_json(R"JSON({ "foo": 2 })JSON"));
}

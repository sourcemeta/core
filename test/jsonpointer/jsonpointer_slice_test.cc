#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

TEST(JSONPointer_slice, from_zero) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer result{pointer.slice(0)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_TRUE(result.at(0).is_property());
  EXPECT_EQ(result.at(0).to_property(), "foo");
  EXPECT_TRUE(result.at(1).is_property());
  EXPECT_EQ(result.at(1).to_property(), "bar");
  EXPECT_TRUE(result.at(2).is_property());
  EXPECT_EQ(result.at(2).to_property(), "baz");
}

TEST(JSONPointer_slice, from_one) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer result{pointer.slice(1)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_property());
  EXPECT_EQ(result.at(0).to_property(), "bar");
  EXPECT_TRUE(result.at(1).is_property());
  EXPECT_EQ(result.at(1).to_property(), "baz");
}

TEST(JSONPointer_slice, from_two) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer result{pointer.slice(2)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_property());
  EXPECT_EQ(result.at(0).to_property(), "baz");
}

TEST(JSONPointer_slice, from_end) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer result{pointer.slice(3)};
  EXPECT_EQ(result.size(), 0);
  EXPECT_TRUE(result.empty());
}

TEST(JSONPointer_slice, empty_from_zero) {
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::Pointer result{pointer.slice(0)};
  EXPECT_EQ(result.size(), 0);
  EXPECT_TRUE(result.empty());
}

TEST(JSONPointer_slice, with_indices) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar", 2};
  const sourcemeta::core::Pointer result{pointer.slice(1)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_TRUE(result.at(0).is_index());
  EXPECT_EQ(result.at(0).to_index(), 1);
  EXPECT_TRUE(result.at(1).is_property());
  EXPECT_EQ(result.at(1).to_property(), "bar");
  EXPECT_TRUE(result.at(2).is_index());
  EXPECT_EQ(result.at(2).to_index(), 2);
}

TEST(JSONPointer_slice, single_element_from_zero) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const sourcemeta::core::Pointer result{pointer.slice(0)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_property());
  EXPECT_EQ(result.at(0).to_property(), "foo");
}

TEST(JSONPointer_slice, single_element_from_one) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const sourcemeta::core::Pointer result{pointer.slice(1)};
  EXPECT_EQ(result.size(), 0);
  EXPECT_TRUE(result.empty());
}

TEST(JSONPointer_slice, range_middle) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz", "qux"};
  const sourcemeta::core::Pointer result{pointer.slice(1, 3)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_property());
  EXPECT_EQ(result.at(0).to_property(), "bar");
  EXPECT_TRUE(result.at(1).is_property());
  EXPECT_EQ(result.at(1).to_property(), "baz");
}

TEST(JSONPointer_slice, range_from_start) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer result{pointer.slice(0, 2)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_property());
  EXPECT_EQ(result.at(0).to_property(), "foo");
  EXPECT_TRUE(result.at(1).is_property());
  EXPECT_EQ(result.at(1).to_property(), "bar");
}

TEST(JSONPointer_slice, range_to_end) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer result{pointer.slice(1, 3)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_property());
  EXPECT_EQ(result.at(0).to_property(), "bar");
  EXPECT_TRUE(result.at(1).is_property());
  EXPECT_EQ(result.at(1).to_property(), "baz");
}

TEST(JSONPointer_slice, range_full) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer result{pointer.slice(0, 3)};
  EXPECT_EQ(result.size(), 3);
  EXPECT_TRUE(result.at(0).is_property());
  EXPECT_EQ(result.at(0).to_property(), "foo");
  EXPECT_TRUE(result.at(1).is_property());
  EXPECT_EQ(result.at(1).to_property(), "bar");
  EXPECT_TRUE(result.at(2).is_property());
  EXPECT_EQ(result.at(2).to_property(), "baz");
}

TEST(JSONPointer_slice, range_empty_same_indices) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer result{pointer.slice(1, 1)};
  EXPECT_EQ(result.size(), 0);
  EXPECT_TRUE(result.empty());
}

TEST(JSONPointer_slice, range_single_element) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer result{pointer.slice(1, 2)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_property());
  EXPECT_EQ(result.at(0).to_property(), "bar");
}

TEST(JSONPointer_slice, range_empty_pointer) {
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::Pointer result{pointer.slice(0, 0)};
  EXPECT_EQ(result.size(), 0);
  EXPECT_TRUE(result.empty());
}

TEST(JSONPointer_slice, range_with_indices) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar", 2};
  const sourcemeta::core::Pointer result{pointer.slice(1, 3)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_index());
  EXPECT_EQ(result.at(0).to_index(), 1);
  EXPECT_TRUE(result.at(1).is_property());
  EXPECT_EQ(result.at(1).to_property(), "bar");
}

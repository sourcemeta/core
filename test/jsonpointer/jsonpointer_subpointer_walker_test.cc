#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <memory>
#include <string>
#include <vector>

TEST(JSONPointer_subpointer_walker, equality_null_subpointer_iterators) {
  const sourcemeta::core::SubPointerWalker::const_iterator first{nullptr};
  const sourcemeta::core::SubPointerWalker::const_iterator second{nullptr};
  EXPECT_EQ(first, second);
  EXPECT_EQ(second, first);
  EXPECT_FALSE(first != second);
  EXPECT_FALSE(second != first);
}

TEST(JSONPointer_subpointer_walker,
     equality_null_and_not_null_subpointer_iterators) {
  sourcemeta::core::Pointer pointer;
  const sourcemeta::core::SubPointerWalker::const_iterator first{&pointer};
  const sourcemeta::core::SubPointerWalker::const_iterator second{nullptr};
  EXPECT_FALSE(first == second);
  EXPECT_FALSE(second == first);
  EXPECT_TRUE(first != second);
  EXPECT_TRUE(second != first);
}

TEST(JSONPointer_subpointer_walker, equality_same_subpointer_iterators) {
  sourcemeta::core::Pointer pointer_1;
  sourcemeta::core::Pointer pointer_2;
  const sourcemeta::core::SubPointerWalker::const_iterator first{&pointer_1};
  const sourcemeta::core::SubPointerWalker::const_iterator second{&pointer_2};
  EXPECT_EQ(first, second);
  EXPECT_EQ(second, first);
  EXPECT_FALSE(first != second);
  EXPECT_FALSE(second != first);
}

TEST(JSONPointer_subpointer_walker, equality_different_subpointer_iterators) {
  sourcemeta::core::Pointer pointer_1{"foo"};
  sourcemeta::core::Pointer pointer_2{"bar"};
  const sourcemeta::core::SubPointerWalker::const_iterator first{&pointer_1};
  const sourcemeta::core::SubPointerWalker::const_iterator second{&pointer_2};
  EXPECT_FALSE(first == second);
  EXPECT_FALSE(second == first);
  EXPECT_TRUE(first != second);
  EXPECT_TRUE(second != first);
}

TEST(JSONPointer_subpointer_walker, subpointer_walker) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  std::vector<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::SubPointerWalker{pointer}) {
    subpointers.push_back(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 4);

  // First subpointer
  EXPECT_EQ(subpointers.at(0).size(), 3);
  EXPECT_TRUE(subpointers.at(0).at(0).is_property());
  EXPECT_TRUE(subpointers.at(0).at(1).is_property());
  EXPECT_TRUE(subpointers.at(0).at(2).is_property());
  EXPECT_EQ(subpointers.at(0).at(0).to_property(), "foo");
  EXPECT_EQ(subpointers.at(0).at(1).to_property(), "bar");
  EXPECT_EQ(subpointers.at(0).at(2).to_property(), "baz");

  // Second subpointer
  EXPECT_EQ(subpointers.at(1).size(), 2);
  EXPECT_TRUE(subpointers.at(1).at(0).is_property());
  EXPECT_TRUE(subpointers.at(1).at(1).is_property());
  EXPECT_EQ(subpointers.at(1).at(0).to_property(), "foo");
  EXPECT_EQ(subpointers.at(1).at(1).to_property(), "bar");

  // Third subpointer
  EXPECT_EQ(subpointers.at(2).size(), 1);
  EXPECT_TRUE(subpointers.at(2).at(0).is_property());
  EXPECT_EQ(subpointers.at(2).at(0).to_property(), "foo");

  // Fourth subpointer
  EXPECT_EQ(subpointers.at(3).size(), 0);
  EXPECT_TRUE(subpointers.at(3).empty());

  // Original remains intact
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
  EXPECT_EQ(pointer.at(2).to_property(), "baz");
}

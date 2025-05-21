#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <memory>
#include <set>
#include <string>
#include <vector>

TEST(JSONPointer_walker, null) {
  const sourcemeta::core::JSON document{nullptr};
  std::vector<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), sourcemeta::core::Pointer{});
}

TEST(JSONPointer_walker, boolean_false) {
  const sourcemeta::core::JSON document{false};
  std::vector<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), sourcemeta::core::Pointer{});
}

TEST(JSONPointer_walker, boolean_true) {
  const sourcemeta::core::JSON document{true};
  std::vector<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), sourcemeta::core::Pointer{});
}

TEST(JSONPointer_walker, integer) {
  const sourcemeta::core::JSON document{5};
  std::vector<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), sourcemeta::core::Pointer{});
}

TEST(JSONPointer_walker, real) {
  const sourcemeta::core::JSON document{3.14};
  std::vector<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), sourcemeta::core::Pointer{});
}

TEST(JSONPointer_walker, string) {
  const sourcemeta::core::JSON document{"foo bar"};
  std::vector<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), sourcemeta::core::Pointer{});
}

TEST(JSONPointer_walker, array_empty) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("[]");
  std::vector<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), sourcemeta::core::Pointer{});
}

TEST(JSONPointer_walker, array_scalars) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ 1, 2, 3, 4 ]");
  std::vector<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 5);
  EXPECT_EQ(subpointers.at(0), sourcemeta::core::Pointer{});
  EXPECT_EQ(subpointers.at(1), sourcemeta::core::Pointer({0}));
  EXPECT_EQ(subpointers.at(2), sourcemeta::core::Pointer({1}));
  EXPECT_EQ(subpointers.at(3), sourcemeta::core::Pointer({2}));
  EXPECT_EQ(subpointers.at(4), sourcemeta::core::Pointer({3}));
}

TEST(JSONPointer_walker, array_deep) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"EOF([
    [],
    [ 1, 2 ],
    [ "foo" ]
  ])EOF");
  std::vector<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 7);
  EXPECT_EQ(subpointers.at(0), sourcemeta::core::Pointer{});
  EXPECT_EQ(subpointers.at(1), sourcemeta::core::Pointer({0}));
  EXPECT_EQ(subpointers.at(2), sourcemeta::core::Pointer({1}));
  EXPECT_EQ(subpointers.at(3), sourcemeta::core::Pointer({1, 0}));
  EXPECT_EQ(subpointers.at(4), sourcemeta::core::Pointer({1, 1}));
  EXPECT_EQ(subpointers.at(5), sourcemeta::core::Pointer({2}));
  EXPECT_EQ(subpointers.at(6), sourcemeta::core::Pointer({2, 0}));
}

TEST(JSONPointer_walker, object_empty) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  std::vector<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), sourcemeta::core::Pointer{});
}

TEST(JSONPointer_walker, object_scalars) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1, \"bar\": 2, \"baz\": 3 }");
  // Do a set to not dependent on object property ordering
  std::set<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.insert(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 4);
  EXPECT_TRUE(subpointers.contains(sourcemeta::core::Pointer{}));
  EXPECT_TRUE(subpointers.contains(sourcemeta::core::Pointer({"foo"})));
  EXPECT_TRUE(subpointers.contains(sourcemeta::core::Pointer({"bar"})));
  EXPECT_TRUE(subpointers.contains(sourcemeta::core::Pointer({"baz"})));
}

TEST(JSONPointer_walker, object_nested) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": { \"bar\": 1 } }");
  std::vector<sourcemeta::core::Pointer> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(subpointer);
  }

  EXPECT_EQ(subpointers.size(), 3);
  EXPECT_EQ(subpointers.at(0), sourcemeta::core::Pointer{});
  EXPECT_EQ(subpointers.at(1), sourcemeta::core::Pointer({"foo"}));
  EXPECT_EQ(subpointers.at(2), sourcemeta::core::Pointer({"foo", "bar"}));
}

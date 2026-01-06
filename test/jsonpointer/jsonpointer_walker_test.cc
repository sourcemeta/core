#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <set>
#include <string>
#include <vector>

TEST(JSONPointer_walker, null) {
  const sourcemeta::core::JSON document{nullptr};
  std::vector<std::string> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(sourcemeta::core::to_string(subpointer));
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), "");
}

TEST(JSONPointer_walker, boolean_false) {
  const sourcemeta::core::JSON document{false};
  std::vector<std::string> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(sourcemeta::core::to_string(subpointer));
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), "");
}

TEST(JSONPointer_walker, boolean_true) {
  const sourcemeta::core::JSON document{true};
  std::vector<std::string> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(sourcemeta::core::to_string(subpointer));
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), "");
}

TEST(JSONPointer_walker, integer) {
  const sourcemeta::core::JSON document{5};
  std::vector<std::string> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(sourcemeta::core::to_string(subpointer));
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), "");
}

TEST(JSONPointer_walker, real) {
  const sourcemeta::core::JSON document{3.14};
  std::vector<std::string> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(sourcemeta::core::to_string(subpointer));
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), "");
}

TEST(JSONPointer_walker, string) {
  const sourcemeta::core::JSON document{"foo bar"};
  std::vector<std::string> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(sourcemeta::core::to_string(subpointer));
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), "");
}

TEST(JSONPointer_walker, array_empty) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("[]");
  std::vector<std::string> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(sourcemeta::core::to_string(subpointer));
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), "");
}

TEST(JSONPointer_walker, array_scalars) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("[ 1, 2, 3, 4 ]");
  std::vector<std::string> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(sourcemeta::core::to_string(subpointer));
  }

  EXPECT_EQ(subpointers.size(), 5);
  EXPECT_EQ(subpointers.at(0), "");
  EXPECT_EQ(subpointers.at(1), "/0");
  EXPECT_EQ(subpointers.at(2), "/1");
  EXPECT_EQ(subpointers.at(3), "/2");
  EXPECT_EQ(subpointers.at(4), "/3");
}

TEST(JSONPointer_walker, array_deep) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"EOF([
    [],
    [ 1, 2 ],
    [ "foo" ]
  ])EOF");
  std::vector<std::string> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(sourcemeta::core::to_string(subpointer));
  }

  EXPECT_EQ(subpointers.size(), 7);
  EXPECT_EQ(subpointers.at(0), "");
  EXPECT_EQ(subpointers.at(1), "/0");
  EXPECT_EQ(subpointers.at(2), "/1");
  EXPECT_EQ(subpointers.at(3), "/1/0");
  EXPECT_EQ(subpointers.at(4), "/1/1");
  EXPECT_EQ(subpointers.at(5), "/2");
  EXPECT_EQ(subpointers.at(6), "/2/0");
}

TEST(JSONPointer_walker, object_empty) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  std::vector<std::string> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(sourcemeta::core::to_string(subpointer));
  }

  EXPECT_EQ(subpointers.size(), 1);
  EXPECT_EQ(subpointers.at(0), "");
}

TEST(JSONPointer_walker, object_scalars) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": 1, \"bar\": 2, \"baz\": 3 }");
  // Do a set to not depend on object property ordering
  std::set<std::string> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.insert(sourcemeta::core::to_string(subpointer));
  }

  EXPECT_EQ(subpointers.size(), 4);
  EXPECT_TRUE(subpointers.contains(""));
  EXPECT_TRUE(subpointers.contains("/foo"));
  EXPECT_TRUE(subpointers.contains("/bar"));
  EXPECT_TRUE(subpointers.contains("/baz"));
}

TEST(JSONPointer_walker, object_nested) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{ \"foo\": { \"bar\": 1 } }");
  std::vector<std::string> subpointers;
  for (const auto &subpointer : sourcemeta::core::PointerWalker{document}) {
    subpointers.push_back(sourcemeta::core::to_string(subpointer));
  }

  EXPECT_EQ(subpointers.size(), 3);
  EXPECT_EQ(subpointers.at(0), "");
  EXPECT_EQ(subpointers.at(1), "/foo");
  EXPECT_EQ(subpointers.at(2), "/foo/bar");
}

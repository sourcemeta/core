#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

TEST(JSONPointer_position, track_1) {
  const auto input{R"JSON([
  {
    "foo": {
      "bar": 3
    }
  }
])JSON"};

  sourcemeta::core::PointerPositionTracker tracker;
  sourcemeta::core::parse_json(input, std::ref(tracker));

  EXPECT_EQ(tracker.size(), 4);

  sourcemeta::core::Pointer pointer_1;
  EXPECT_TRUE(tracker.get(pointer_1).has_value());
  EXPECT_EQ(tracker.get(pointer_1).value(),
            sourcemeta::core::PointerPositionTracker::Position({1, 1, 7, 1}));

  sourcemeta::core::Pointer pointer_2{0};
  EXPECT_TRUE(tracker.get(pointer_2).has_value());
  EXPECT_EQ(tracker.get(pointer_2).value(),
            sourcemeta::core::PointerPositionTracker::Position({2, 3, 6, 3}));

  sourcemeta::core::Pointer pointer_3{0, "foo"};
  EXPECT_TRUE(tracker.get(pointer_3).has_value());
  EXPECT_EQ(tracker.get(pointer_3).value(),
            sourcemeta::core::PointerPositionTracker::Position({3, 5, 5, 5}));

  sourcemeta::core::Pointer pointer_4{0, "foo", "bar"};
  EXPECT_TRUE(tracker.get(pointer_4).has_value());
  EXPECT_EQ(tracker.get(pointer_4).value(),
            sourcemeta::core::PointerPositionTracker::Position({4, 7, 4, 14}));
}

TEST(JSONPointer_position, to_json_1) {
  const auto input{R"JSON([
  {
    "foo": {
      "bar": 3
    }
  }
])JSON"};

  sourcemeta::core::PointerPositionTracker tracker;
  sourcemeta::core::parse_json(input, std::ref(tracker));

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "": [ 1, 1, 7, 1 ],
    "/0": [ 2, 3, 6, 3 ],
    "/0/foo": [ 3, 5, 5, 5 ],
    "/0/foo/bar": [ 4, 7, 4, 14 ]
  })JSON")};

  EXPECT_EQ(tracker.to_json(), expected);
  EXPECT_EQ(sourcemeta::core::to_json(tracker), expected);
}

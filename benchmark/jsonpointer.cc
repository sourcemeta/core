#include <benchmark/benchmark.h>

#include <cassert>    // assert
#include <filesystem> // std::filesystem
#include <fstream>    // std::ifstream
#include <functional> // std::ref
#include <sstream>    // std::ostringstream

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

static void Pointer_Object_Traverse(benchmark::State &state) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "one": {
      "two": {
        "three": {
          "four": {
            "five": {
              "six": {
                "seven": {
                  "eight": {
                    "nine": {
                      "ten": true
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  })JSON")};

  const sourcemeta::core::Pointer pointer{"one",  "two", "three", "four",
                                          "five", "six", "seven", "eight",
                                          "nine", "ten"};

  for (auto _ : state) {
    auto result{sourcemeta::core::get(document, pointer)};
    assert(result.is_boolean());
    assert(result.to_boolean());
    benchmark::DoNotOptimize(result);
  }
}

static void Pointer_Object_Try_Traverse(benchmark::State &state) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "one": {
      "two": {
        "three": {
          "four": {
            "five": {
              "six": {
                "seven": {
                  "eight": {
                    "nine": {
                      "ten": true
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  })JSON")};

  const sourcemeta::core::Pointer pointer{"one",  "two", "three", "four",
                                          "five", "six", "seven", "eight",
                                          "nine", "ten"};

  for (auto _ : state) {
    auto result{sourcemeta::core::try_get(document, pointer)};
    assert(result);
    assert(result->is_boolean());
    assert(result->to_boolean());
    benchmark::DoNotOptimize(result);
  }
}

static void Pointer_Push_Back_Pointer_To_Weak_Pointer(benchmark::State &state) {
  const sourcemeta::core::Pointer pointer{"QG5",
                                          "hzh4HOy0CDatvDds",
                                          "A2Fmfu3",
                                          "SsDf",
                                          "IWKDjcbspyIi",
                                          "ShnIIXXJ16rJqU",
                                          "kr20Vl0fTl",
                                          "k86GNqPRqYcrshgW",
                                          "MZasv",
                                          "mu12raEM0",
                                          "ERmTaob",
                                          "N4051TZZgmHJxw0kA2",
                                          "JyYLja7uU4rGiPnO0HaF",
                                          "z39HymGubGiCkAj",
                                          "pBTXBPoibBe",
                                          "jO6qKBYc",
                                          "RSYx",
                                          "v5qyRqbc0t1Stb5",
                                          "cNSajmQSUeImEznA",
                                          "86R5NCY4bGJBB",
                                          "7NGrw3bVRH",
                                          "USCmFPlMHJ1bZWi",
                                          "Er9UMJ6yL8ny1x3qj1c",
                                          "260JkYXaCtfYo0QC",
                                          "SPtyuBHw",
                                          "O6QI5wHP1",
                                          "1iDJs1u5i",
                                          "onYiosjbLHA6QuiNidB",
                                          "cetyYrbEvZ",
                                          "KAWwqfXHPMtL",
                                          "NvzaQYhMBmad",
                                          "B68v67OylYcwROoaZxb",
                                          "aG9u12vgak",
                                          "5thqNjai",
                                          "bIOCVt3JEZO2",
                                          "rVZLu42FBk",
                                          "Wjm",
                                          "z23mpKaOHDU7utfy9euC",
                                          "2k9DYyIr4uQKSUfKPV",
                                          "53ITE3Rxdsy862S",
                                          "7NR",
                                          "ByKyUzTz1uLQOt",
                                          "ReSknCy",
                                          "VbD8",
                                          "fHugvDUJe",
                                          "japh5",
                                          "6rhjhRDxhd",
                                          "Q7sX8",
                                          "VtFTA5YnQhbAPhxbdi",
                                          "BuY1vG92USX0Y6NvEGcf",
                                          "y4fs5dRPK",
                                          "5iz39wrHyeQQHb1",
                                          "4GrQCWp74AnBSL9",
                                          "m6KK",
                                          "6buqIqqDVJwu",
                                          "4FSzmT06",
                                          "2CUY",
                                          "f1P3LTAP",
                                          "hlFnm",
                                          "Q6OQxzHNwZsSpM0Fib",
                                          "ltLJ5HS1fUXQIIE"};

  for (auto _ : state) {
    sourcemeta::core::WeakPointer destination;
    destination.push_back(pointer);
    assert(destination.size() == pointer.size());
    benchmark::DoNotOptimize(destination);
  }
}

static void Pointer_Walker_Schema_ISO_Language(benchmark::State &state) {
  const auto schema{sourcemeta::core::read_json(
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "2020_12_iso_language_2023_set_3.json")};

  for (auto _ : state) {
    sourcemeta::core::PointerWalker walker{schema};
    auto pointer_count =
        static_cast<std::size_t>(std::distance(walker.cbegin(), walker.cend()));
    benchmark::DoNotOptimize(pointer_count);
  }
}

static void Pointer_Maybe_Tracked_Deeply_Nested(benchmark::State &state) {
  const auto enable_tracker{static_cast<bool>(state.range(0))};
  const std::filesystem::path path{std::filesystem::path{CURRENT_DIRECTORY} /
                                   "files" / "deeply_nested.json"};
  std::ifstream file{path};
  std::ostringstream buffer;
  buffer << file.rdbuf();
  const auto content{buffer.str()};

  for (auto _ : state) {
    if (enable_tracker) {
      sourcemeta::core::PointerPositionTracker tracker;
      auto result{sourcemeta::core::parse_json(content, std::ref(tracker))};
      assert(result.is_object());
      benchmark::DoNotOptimize(result);
      benchmark::DoNotOptimize(tracker);
    } else {
      auto result{sourcemeta::core::parse_json(content)};
      assert(result.is_object());
      benchmark::DoNotOptimize(result);
    }
  }
}

static void
Pointer_Position_Tracker_Get_Deeply_Nested(benchmark::State &state) {
  const std::filesystem::path path{std::filesystem::path{CURRENT_DIRECTORY} /
                                   "files" / "deeply_nested.json"};
  std::ifstream file{path};
  std::ostringstream buffer;
  buffer << file.rdbuf();
  const auto content{buffer.str()};

  sourcemeta::core::PointerPositionTracker tracker;
  sourcemeta::core::parse_json(content, std::ref(tracker));

  const sourcemeta::core::Pointer pointer{
      "next", "next", "next", "next", "next", "next", "next", "next",
      "next", "next", "next", "next", "next", "next", "next", "next",
      "next", "next", "next", "next", "next", "next", "next", "next",
      "next", "next", "next", "next", "next", "next", "p0"};

  for (auto _ : state) {
    auto result{tracker.get(pointer)};
    assert(result.has_value());
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(Pointer_Object_Traverse);
BENCHMARK(Pointer_Object_Try_Traverse);
BENCHMARK(Pointer_Push_Back_Pointer_To_Weak_Pointer);
BENCHMARK(Pointer_Walker_Schema_ISO_Language);
BENCHMARK(Pointer_Maybe_Tracked_Deeply_Nested)->Arg(false)->Arg(true);
BENCHMARK(Pointer_Position_Tracker_Get_Deeply_Nested);

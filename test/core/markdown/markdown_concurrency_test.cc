#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

#include <cstddef> // std::size_t
#include <string>  // std::string
#include <thread>  // std::thread
#include <vector>  // std::vector

TEST(concurrent_calls) {
  static constexpr std::size_t THREAD_COUNT{16};
  std::vector<std::thread> threads;
  threads.reserve(THREAD_COUNT);
  std::vector<std::string> results(THREAD_COUNT);

  for (std::size_t index{0}; index < THREAD_COUNT; ++index) {
    threads.emplace_back([&results, index]() {
      results[index] = sourcemeta::core::markdown_to_html("**hello** world");
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(results[0], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[1], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[2], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[3], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[4], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[5], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[6], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[7], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[8], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[9], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[10], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[11], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[12], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[13], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[14], "<p><strong>hello</strong> world</p>\n");
  EXPECT_EQ(results[15], "<p><strong>hello</strong> world</p>\n");
}

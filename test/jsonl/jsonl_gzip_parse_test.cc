#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/jsonl.h>

#include <cstdint> // std::uint8_t
#include <sstream> // std::istringstream
#include <string>  // std::string
#include <vector>  // std::vector

TEST(JSONL_gzip_parse, empty) {
  const std::string input;
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document :
       sourcemeta::core::JSONL{stream, sourcemeta::core::JSONL::Mode::GZIP}) {
    result.push_back(document);
  }

  EXPECT_TRUE(result.empty());
}

TEST(JSONL_gzip_parse, integers_n) {
  const std::string input{"1\n2\n3\n4"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document :
       sourcemeta::core::JSONL{stream, sourcemeta::core::JSONL::Mode::GZIP}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 4);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_TRUE(result.at(1).is_integer());
  EXPECT_TRUE(result.at(2).is_integer());
  EXPECT_TRUE(result.at(3).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 1);
  EXPECT_EQ(result.at(1).to_integer(), 2);
  EXPECT_EQ(result.at(2).to_integer(), 3);
  EXPECT_EQ(result.at(3).to_integer(), 4);
}

TEST(JSONL_gzip_parse, integers_rn) {
  const std::string input{"1\r\n2\r\n3\r\n4"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document :
       sourcemeta::core::JSONL{stream, sourcemeta::core::JSONL::Mode::GZIP}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 4);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_TRUE(result.at(1).is_integer());
  EXPECT_TRUE(result.at(2).is_integer());
  EXPECT_TRUE(result.at(3).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 1);
  EXPECT_EQ(result.at(1).to_integer(), 2);
  EXPECT_EQ(result.at(2).to_integer(), 3);
  EXPECT_EQ(result.at(3).to_integer(), 4);
}

TEST(JSONL_gzip_parse, integers_trailing_n) {
  const std::string input{"1\n2\n3\n4\n"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document :
       sourcemeta::core::JSONL{stream, sourcemeta::core::JSONL::Mode::GZIP}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 4);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_TRUE(result.at(1).is_integer());
  EXPECT_TRUE(result.at(2).is_integer());
  EXPECT_TRUE(result.at(3).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 1);
  EXPECT_EQ(result.at(1).to_integer(), 2);
  EXPECT_EQ(result.at(2).to_integer(), 3);
  EXPECT_EQ(result.at(3).to_integer(), 4);
}

TEST(JSONL_gzip_parse, whitespace_before_after) {
  const std::string input{" \t\r   1  \r\r\n2  \t\n3   \n4   \t\r   "};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document :
       sourcemeta::core::JSONL{stream, sourcemeta::core::JSONL::Mode::GZIP}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 4);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_TRUE(result.at(1).is_integer());
  EXPECT_TRUE(result.at(2).is_integer());
  EXPECT_TRUE(result.at(3).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 1);
  EXPECT_EQ(result.at(1).to_integer(), 2);
  EXPECT_EQ(result.at(2).to_integer(), 3);
  EXPECT_EQ(result.at(3).to_integer(), 4);
}

TEST(JSONL_gzip_parse, objects) {
  const std::string input{
      R"EOF({"name": "Gilbert", "wins": [["straight", "7h"]]}
{"name": "Alexa", "wins": [["two pair", "4s"]]}
{"name": "May", "wins": []}
{"name": "Deloise", "wins": [["three of a kind", "5c"]]})EOF"};

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document :
       sourcemeta::core::JSONL{stream, sourcemeta::core::JSONL::Mode::GZIP}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 4);

  EXPECT_TRUE(result.at(0).is_object());
  EXPECT_TRUE(result.at(1).is_object());
  EXPECT_TRUE(result.at(2).is_object());
  EXPECT_TRUE(result.at(3).is_object());

  EXPECT_EQ(result.at(0).size(), 2);
  EXPECT_EQ(result.at(1).size(), 2);
  EXPECT_EQ(result.at(2).size(), 2);
  EXPECT_EQ(result.at(3).size(), 2);

  EXPECT_TRUE(result.at(0).defines("name"));
  EXPECT_TRUE(result.at(0).defines("wins"));
  EXPECT_TRUE(result.at(1).defines("name"));
  EXPECT_TRUE(result.at(1).defines("wins"));
  EXPECT_TRUE(result.at(2).defines("name"));
  EXPECT_TRUE(result.at(2).defines("wins"));
  EXPECT_TRUE(result.at(3).defines("name"));
  EXPECT_TRUE(result.at(3).defines("wins"));

  EXPECT_EQ(result.at(0).at("name").to_string(), "Gilbert");
  EXPECT_EQ(result.at(1).at("name").to_string(), "Alexa");
  EXPECT_EQ(result.at(2).at("name").to_string(), "May");
  EXPECT_EQ(result.at(3).at("name").to_string(), "Deloise");

  EXPECT_EQ(result.at(0).at("wins").size(), 1);
  EXPECT_EQ(result.at(1).at("wins").size(), 1);
  EXPECT_EQ(result.at(2).at("wins").size(), 0);
  EXPECT_EQ(result.at(3).at("wins").size(), 1);
}

TEST(JSONL_gzip_parse, large_dataset) {
  std::string input;
  for (int index = 0; index < 1000; ++index) {
    input += "{\"index\":" + std::to_string(index) + "}\n";
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document :
       sourcemeta::core::JSONL{stream, sourcemeta::core::JSONL::Mode::GZIP}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 1000);
  for (int index = 0; index < 1000; ++index) {
    EXPECT_TRUE(result.at(static_cast<std::size_t>(index)).is_object());
    EXPECT_EQ(
        result.at(static_cast<std::size_t>(index)).at("index").to_integer(),
        index);
  }
}

TEST(JSONL_gzip_parse, default_mode_is_raw) {
  std::istringstream stream{"1\n2\n3"};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result.at(0).to_integer(), 1);
  EXPECT_EQ(result.at(1).to_integer(), 2);
  EXPECT_EQ(result.at(2).to_integer(), 3);
}

#include <gtest/gtest.h>

#include <sourcemeta/core/jsonl.h>

#include <sstream>
#include <vector>

TEST(JSONL_parse, empty) {
  std::istringstream stream{""};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
    result.push_back(document);
  }

  EXPECT_TRUE(result.empty());
}

TEST(JSONL_parse, blank) {
  std::istringstream stream{"    "};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
    result.push_back(document);
  }

  EXPECT_TRUE(result.empty());
}

TEST(JSONL_parse, integers_n) {
  std::string input{"1\n2\n3\n4"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
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

TEST(JSONL_parse, whitespace_before_after_within) {
  std::string input{" \t\r   1  \r\r\n2  \t\n3   \n4   \t\r   "};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
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

TEST(JSONL_parse, integers_n_trailing_n) {
  std::string input{"1\n2\n3\n4\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
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

TEST(JSONL_parse, integers_n_trailing_rn) {
  std::string input{"1\n2\n3\n4\r\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
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

TEST(JSONL_parse, integers_rn) {
  std::string input{"1\r\n2\r\n3\r\n4"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
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

TEST(JSONL_parse, integers_rn_trailing_rn) {
  std::string input{"1\r\n2\r\n3\r\n4\r\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
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

TEST(JSONL_parse, integers_rn_trailing_n) {
  std::string input{"1\r\n2\r\n3\r\n4\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
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

TEST(JSONL_parse, integers_mixed) {
  std::string input{"1\r\n2\n3\r\n4\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
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

TEST(JSONL_parse, example_1) {
  std::string input{R"EOF(
    ["Name", "Session", "Score", "Completed"]
    ["Gilbert", "2013", 24, true]
    ["Alexa", "2013", 29, true]
    ["May", "2012B", 14, false]
    ["Deloise", "2012A", 19, true]
  )EOF"};

  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 5);

  EXPECT_TRUE(result.at(0).is_array());
  EXPECT_TRUE(result.at(1).is_array());
  EXPECT_TRUE(result.at(2).is_array());
  EXPECT_TRUE(result.at(3).is_array());
  EXPECT_TRUE(result.at(4).is_array());

  EXPECT_EQ(result.at(0).size(), 4);
  EXPECT_EQ(result.at(1).size(), 4);
  EXPECT_EQ(result.at(2).size(), 4);
  EXPECT_EQ(result.at(3).size(), 4);
  EXPECT_EQ(result.at(4).size(), 4);

  EXPECT_TRUE(result.at(0).at(0).is_string());
  EXPECT_TRUE(result.at(0).at(1).is_string());
  EXPECT_TRUE(result.at(0).at(2).is_string());
  EXPECT_TRUE(result.at(0).at(3).is_string());

  EXPECT_TRUE(result.at(1).at(0).is_string());
  EXPECT_TRUE(result.at(1).at(1).is_string());
  EXPECT_TRUE(result.at(1).at(2).is_integer());
  EXPECT_TRUE(result.at(1).at(3).is_boolean());

  EXPECT_TRUE(result.at(2).at(0).is_string());
  EXPECT_TRUE(result.at(2).at(1).is_string());
  EXPECT_TRUE(result.at(2).at(2).is_integer());
  EXPECT_TRUE(result.at(2).at(3).is_boolean());

  EXPECT_TRUE(result.at(3).at(0).is_string());
  EXPECT_TRUE(result.at(3).at(1).is_string());
  EXPECT_TRUE(result.at(3).at(2).is_integer());
  EXPECT_TRUE(result.at(3).at(3).is_boolean());

  EXPECT_EQ(result.at(0).at(0).to_string(), "Name");
  EXPECT_EQ(result.at(0).at(1).to_string(), "Session");
  EXPECT_EQ(result.at(0).at(2).to_string(), "Score");
  EXPECT_EQ(result.at(0).at(3).to_string(), "Completed");

  EXPECT_EQ(result.at(1).at(0).to_string(), "Gilbert");
  EXPECT_EQ(result.at(1).at(1).to_string(), "2013");
  EXPECT_EQ(result.at(1).at(2).to_integer(), 24);
  EXPECT_TRUE(result.at(1).at(3).to_boolean());

  EXPECT_EQ(result.at(2).at(0).to_string(), "Alexa");
  EXPECT_EQ(result.at(2).at(1).to_string(), "2013");
  EXPECT_EQ(result.at(2).at(2).to_integer(), 29);
  EXPECT_TRUE(result.at(2).at(3).to_boolean());

  EXPECT_EQ(result.at(3).at(0).to_string(), "May");
  EXPECT_EQ(result.at(3).at(1).to_string(), "2012B");
  EXPECT_EQ(result.at(3).at(2).to_integer(), 14);
  EXPECT_FALSE(result.at(3).at(3).to_boolean());

  EXPECT_EQ(result.at(4).at(0).to_string(), "Deloise");
  EXPECT_EQ(result.at(4).at(1).to_string(), "2012A");
  EXPECT_EQ(result.at(4).at(2).to_integer(), 19);
  EXPECT_TRUE(result.at(4).at(3).to_boolean());
}

TEST(JSONL_parse, example_2) {
  std::string input{R"EOF(
    {"name": "Gilbert", "wins": [["straight", "7♣"], ["one pair", "10♥"]]}
    {"name": "Alexa", "wins": [["two pair", "4♠"], ["two pair", "9♠"]]}
    {"name": "May", "wins": []}
    {"name": "Deloise", "wins": [["three of a kind", "5♣"]]}
  )EOF"};

  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream}) {
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

  EXPECT_TRUE(result.at(0).at("name").is_string());
  EXPECT_TRUE(result.at(1).at("name").is_string());
  EXPECT_TRUE(result.at(2).at("name").is_string());
  EXPECT_TRUE(result.at(3).at("name").is_string());

  EXPECT_TRUE(result.at(0).at("wins").is_array());
  EXPECT_TRUE(result.at(1).at("wins").is_array());
  EXPECT_TRUE(result.at(2).at("wins").is_array());
  EXPECT_TRUE(result.at(3).at("wins").is_array());

  EXPECT_EQ(result.at(0).at("wins").size(), 2);
  EXPECT_EQ(result.at(1).at("wins").size(), 2);
  EXPECT_EQ(result.at(2).at("wins").size(), 0);
  EXPECT_EQ(result.at(3).at("wins").size(), 1);

  EXPECT_TRUE(result.at(0).at("wins").at(0).is_array());
  EXPECT_TRUE(result.at(0).at("wins").at(1).is_array());
  EXPECT_TRUE(result.at(1).at("wins").at(0).is_array());
  EXPECT_TRUE(result.at(1).at("wins").at(1).is_array());
  EXPECT_TRUE(result.at(3).at("wins").at(0).is_array());
}

#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

TEST(URLPattern_match, protocol_char_exact_match) {
  const sourcemeta::core::URLPattern pattern{.protocol = "https"};
  const auto result{pattern.protocol.match("https")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, protocol_char_no_match) {
  const sourcemeta::core::URLPattern pattern{.protocol = "https"};
  const auto result{pattern.protocol.match("http")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, protocol_name_basic) {
  const sourcemeta::core::URLPattern pattern{.protocol = ":proto"};
  const auto result{pattern.protocol.match("https")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "https");
  EXPECT_TRUE(result.value().at("proto").has_value());
  EXPECT_EQ(result.value().at("proto").value(), "https");
}

TEST(URLPattern_match, protocol_name_any_value) {
  const sourcemeta::core::URLPattern pattern{.protocol = ":proto"};
  const auto result{pattern.protocol.match("ftp")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "ftp");
  EXPECT_TRUE(result.value().at("proto").has_value());
  EXPECT_EQ(result.value().at("proto").value(), "ftp");
}

TEST(URLPattern_match, protocol_name_regex_match) {
  const sourcemeta::core::URLPattern pattern{.protocol = ":proto(https?)"};
  const auto result{pattern.protocol.match("https")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "https");
  EXPECT_TRUE(result.value().at("proto").has_value());
  EXPECT_EQ(result.value().at("proto").value(), "https");
}

TEST(URLPattern_match, protocol_name_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.protocol = ":proto(https?)"};
  const auto result{pattern.protocol.match("ftp")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, protocol_regex_match) {
  const sourcemeta::core::URLPattern pattern{.protocol = "(https?)"};
  const auto result{pattern.protocol.match("http")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "http");
}

TEST(URLPattern_match, protocol_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.protocol = "(https?)"};
  const auto result{pattern.protocol.match("ftp")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, protocol_asterisk) {
  const sourcemeta::core::URLPattern pattern{.protocol = "*"};
  const auto result{pattern.protocol.match("https")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "https");
}

TEST(URLPattern_match, protocol_group_match) {
  const sourcemeta::core::URLPattern pattern{.protocol = "{https}"};
  const auto result{pattern.protocol.match("https")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, protocol_group_no_match) {
  const sourcemeta::core::URLPattern pattern{.protocol = "{https}"};
  const auto result{pattern.protocol.match("http")};
  EXPECT_FALSE(result.has_value());
}

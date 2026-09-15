#include <sourcemeta/core/email.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(valid_simple_address) {
  EXPECT_TRUE(sourcemeta::core::is_html_email("user@example.com"));
}

TEST(valid_single_characters) {
  EXPECT_TRUE(sourcemeta::core::is_html_email("a@b"));
}

TEST(valid_dotted_local_part_with_tag) {
  EXPECT_TRUE(
      sourcemeta::core::is_html_email("first.last+tag@sub.example.com"));
}

TEST(valid_every_atext_character_in_local_part) {
  EXPECT_TRUE(
      sourcemeta::core::is_html_email("!#$%&'*+/=?^_`{|}~-@example.com"));
}

TEST(valid_leading_trailing_and_consecutive_dots_in_local_part) {
  EXPECT_TRUE(sourcemeta::core::is_html_email(".a..b.@example.com"));
}

TEST(valid_hyphen_inside_label) {
  EXPECT_TRUE(sourcemeta::core::is_html_email("a@my-host.example"));
}

TEST(valid_label_of_63_characters) {
  EXPECT_TRUE(
      sourcemeta::core::is_html_email("a@" + std::string(63, 'b') + ".com"));
}

TEST(invalid_label_of_64_characters) {
  EXPECT_FALSE(
      sourcemeta::core::is_html_email("a@" + std::string(64, 'b') + ".com"));
}

TEST(invalid_empty) { EXPECT_FALSE(sourcemeta::core::is_html_email("")); }

TEST(invalid_without_at_sign) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("example.com"));
}

TEST(invalid_empty_local_part) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("@example.com"));
}

TEST(invalid_empty_domain) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("a@"));
}

TEST(invalid_empty_first_label) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("a@.com"));
}

TEST(invalid_trailing_dot_in_domain) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("a@example."));
}

TEST(invalid_label_starting_with_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("a@-example.com"));
}

TEST(invalid_label_ending_with_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("a@example-.com"));
}

TEST(invalid_underscore_in_label) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("a@exa_mple.com"));
}

TEST(invalid_space_in_local_part) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("a b@example.com"));
}

TEST(invalid_second_at_sign) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("a@b@example.com"));
}

TEST(invalid_quoted_local_part) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("\"a\"@example.com"));
}

TEST(invalid_address_literal) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("user@[127.0.0.1]"));
}

TEST(invalid_non_ascii_local_part) {
  EXPECT_FALSE(sourcemeta::core::is_html_email("caf\xC3\xA9@example.com"));
}

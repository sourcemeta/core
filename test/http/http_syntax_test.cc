#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view

TEST(is_ows_accepts_space_and_tab) {
  EXPECT_TRUE(sourcemeta::core::http_is_ows(' '));
  EXPECT_TRUE(sourcemeta::core::http_is_ows('\t'));
}

TEST(is_ows_rejects_other_characters) {
  EXPECT_FALSE(sourcemeta::core::http_is_ows('\n'));
  EXPECT_FALSE(sourcemeta::core::http_is_ows('\r'));
  EXPECT_FALSE(sourcemeta::core::http_is_ows('x'));
}

TEST(is_tchar_accepts_token_characters) {
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('a'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('Z'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('0'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('!'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('~'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('+'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('*'));
}

TEST(is_tchar_rejects_separators) {
  EXPECT_FALSE(sourcemeta::core::http_is_tchar('"'));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar(' '));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar(','));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar('='));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar('('));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar('/'));
}

TEST(is_b64token_char_accepts_the_alphabet) {
  EXPECT_TRUE(sourcemeta::core::http_is_b64token_char('a'));
  EXPECT_TRUE(sourcemeta::core::http_is_b64token_char('0'));
  EXPECT_TRUE(sourcemeta::core::http_is_b64token_char('-'));
  EXPECT_TRUE(sourcemeta::core::http_is_b64token_char('_'));
  EXPECT_TRUE(sourcemeta::core::http_is_b64token_char('~'));
  EXPECT_TRUE(sourcemeta::core::http_is_b64token_char('+'));
  EXPECT_TRUE(sourcemeta::core::http_is_b64token_char('/'));
}

TEST(is_b64token_char_rejects_padding_and_separators) {
  EXPECT_FALSE(sourcemeta::core::http_is_b64token_char('='));
  EXPECT_FALSE(sourcemeta::core::http_is_b64token_char(' '));
  EXPECT_FALSE(sourcemeta::core::http_is_b64token_char('!'));
}

TEST(is_b64token_accepts_valid_credentials) {
  EXPECT_TRUE(sourcemeta::core::http_is_b64token("abc123"));
  EXPECT_TRUE(sourcemeta::core::http_is_b64token("mF_9.B5f-4.1JqM"));
  EXPECT_TRUE(sourcemeta::core::http_is_b64token("YWJj=="));
  EXPECT_TRUE(sourcemeta::core::http_is_b64token("a="));
}

TEST(is_b64token_rejects_invalid_credentials) {
  EXPECT_FALSE(sourcemeta::core::http_is_b64token(""));
  EXPECT_FALSE(sourcemeta::core::http_is_b64token("=="));
  EXPECT_FALSE(sourcemeta::core::http_is_b64token("a b"));
  EXPECT_FALSE(sourcemeta::core::http_is_b64token("a==b"));
  EXPECT_FALSE(sourcemeta::core::http_is_b64token("a,b"));
}

TEST(trim_leading_ows_removes_spaces_and_tabs) {
  EXPECT_EQ(sourcemeta::core::http_trim_leading_ows("  x"), "x");
  EXPECT_EQ(sourcemeta::core::http_trim_leading_ows("\t x"), "x");
  EXPECT_EQ(sourcemeta::core::http_trim_leading_ows("x"), "x");
}

TEST(trim_leading_ows_of_all_whitespace_is_empty) {
  EXPECT_TRUE(sourcemeta::core::http_trim_leading_ows("   ").empty());
  EXPECT_TRUE(sourcemeta::core::http_trim_leading_ows("").empty());
}

TEST(trim_trailing_ows_removes_spaces_and_tabs) {
  EXPECT_EQ(sourcemeta::core::http_trim_trailing_ows("x  "), "x");
  EXPECT_EQ(sourcemeta::core::http_trim_trailing_ows("x \t"), "x");
  EXPECT_EQ(sourcemeta::core::http_trim_trailing_ows("x"), "x");
}

TEST(scan_quoted_string_reads_a_plain_value) {
  std::string storage;
  std::string_view value;
  const auto next{sourcemeta::core::http_scan_quoted_string(R"("hello")", 0,
                                                            storage, value)};
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), 7);
  EXPECT_EQ(value, "hello");
}

TEST(scan_quoted_string_reads_an_empty_value) {
  std::string storage;
  std::string_view value;
  const auto next{
      sourcemeta::core::http_scan_quoted_string(R"("")", 0, storage, value)};
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), 2);
  EXPECT_TRUE(value.empty());
}

TEST(scan_quoted_string_keeps_a_comma_inside) {
  std::string storage;
  std::string_view value;
  const auto next{
      sourcemeta::core::http_scan_quoted_string(R"("a,b")", 0, storage, value)};
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(value, "a,b");
}

TEST(scan_quoted_string_unescapes_a_quoted_pair) {
  std::string storage;
  std::string_view value;
  const auto next{sourcemeta::core::http_scan_quoted_string(R"("say \"hi\"")",
                                                            0, storage, value)};
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(value, R"(say "hi")");
}

TEST(scan_quoted_string_resumes_after_the_closing_quote) {
  std::string storage;
  std::string_view value;
  const auto next{sourcemeta::core::http_scan_quoted_string(R"("a", "b")", 0,
                                                            storage, value)};
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), 3);
  EXPECT_EQ(value, "a");
}

TEST(scan_quoted_string_rejects_a_missing_opening_quote) {
  std::string storage;
  std::string_view value;
  EXPECT_FALSE(
      sourcemeta::core::http_scan_quoted_string("abc", 0, storage, value)
          .has_value());
}

TEST(scan_quoted_string_rejects_an_unterminated_value) {
  std::string storage;
  std::string_view value;
  EXPECT_FALSE(
      sourcemeta::core::http_scan_quoted_string(R"("abc)", 0, storage, value)
          .has_value());
}

TEST(scan_quoted_string_rejects_a_dangling_backslash) {
  std::string storage;
  std::string_view value;
  std::string input{"\"a\\"};
  EXPECT_FALSE(
      sourcemeta::core::http_scan_quoted_string(input, 0, storage, value)
          .has_value());
}

TEST(scan_quoted_string_rejects_a_control_character) {
  std::string storage;
  std::string_view value;
  std::string input{"\"a"};
  input.push_back('\x01');
  input.push_back('"');
  EXPECT_FALSE(
      sourcemeta::core::http_scan_quoted_string(input, 0, storage, value)
          .has_value());
}

TEST(is_tchar_accepts_the_remaining_punctuation) {
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('#'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('$'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('&'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('^'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('_'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('`'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('|'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('-'));
  EXPECT_TRUE(sourcemeta::core::http_is_tchar('.'));
}

TEST(is_tchar_rejects_the_remaining_separators_and_controls) {
  EXPECT_FALSE(sourcemeta::core::http_is_tchar('@'));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar(';'));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar(':'));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar('\\'));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar('{'));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar('?'));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar('\t'));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar('\x7f'));
  EXPECT_FALSE(sourcemeta::core::http_is_tchar('\x80'));
}

TEST(is_b64token_char_rejects_controls_and_high_bytes) {
  EXPECT_TRUE(sourcemeta::core::http_is_b64token_char('.'));
  EXPECT_TRUE(sourcemeta::core::http_is_b64token_char('Z'));
  EXPECT_FALSE(sourcemeta::core::http_is_b64token_char('\t'));
  EXPECT_FALSE(sourcemeta::core::http_is_b64token_char('\x7f'));
  EXPECT_FALSE(sourcemeta::core::http_is_b64token_char('\x80'));
}

TEST(is_b64token_rejects_a_lone_padding_run) {
  EXPECT_FALSE(sourcemeta::core::http_is_b64token("="));
}

TEST(trim_trailing_ows_of_all_whitespace_is_empty) {
  EXPECT_TRUE(sourcemeta::core::http_trim_trailing_ows("  \t").empty());
  EXPECT_TRUE(sourcemeta::core::http_trim_trailing_ows("").empty());
}

TEST(trims_preserve_interior_whitespace) {
  EXPECT_EQ(sourcemeta::core::http_trim_leading_ows("  a b "), "a b ");
  EXPECT_EQ(sourcemeta::core::http_trim_trailing_ows(" a b  "), " a b");
}

TEST(scan_quoted_string_accepts_a_horizontal_tab) {
  std::string storage;
  std::string_view value;
  std::string input{"\"a\tb\""};
  const auto next{
      sourcemeta::core::http_scan_quoted_string(input, 0, storage, value)};
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(value, std::string_view{"a\tb"});
}

TEST(scan_quoted_string_accepts_obs_text) {
  std::string storage;
  std::string_view value;
  std::string input{"\"a"};
  input.push_back('\x80');
  input.push_back('"');
  const auto next{
      sourcemeta::core::http_scan_quoted_string(input, 0, storage, value)};
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(value.size(), 2);
  EXPECT_EQ(static_cast<unsigned char>(value.back()), 0x80);
}

TEST(scan_quoted_string_rejects_delete) {
  std::string storage;
  std::string_view value;
  std::string input{"\"a"};
  input.push_back('\x7f');
  input.push_back('"');
  EXPECT_FALSE(
      sourcemeta::core::http_scan_quoted_string(input, 0, storage, value)
          .has_value());
}

TEST(scan_quoted_string_unescapes_a_backslash) {
  std::string storage;
  std::string_view value;
  const auto next{sourcemeta::core::http_scan_quoted_string(R"("a\\b")", 0,
                                                            storage, value)};
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(value, R"(a\b)");
}

TEST(scan_quoted_string_unescapes_a_plain_character) {
  std::string storage;
  std::string_view value;
  const auto next{sourcemeta::core::http_scan_quoted_string(R"("a\bc")", 0,
                                                            storage, value)};
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(value, "abc");
}

TEST(scan_quoted_string_scans_at_an_offset) {
  std::string storage;
  std::string_view value;
  const auto next{sourcemeta::core::http_scan_quoted_string(R"(realm="value")",
                                                            6, storage, value)};
  EXPECT_TRUE(next.has_value());
  EXPECT_EQ(next.value(), 13);
  EXPECT_EQ(value, "value");
}

TEST(scan_quoted_string_accumulates_into_shared_storage) {
  std::string storage;
  storage.reserve(64);
  std::string_view first;
  std::string_view second;
  const auto after_first{sourcemeta::core::http_scan_quoted_string(
      R"("a\"a")", 0, storage, first)};
  EXPECT_TRUE(after_first.has_value());
  const auto after_second{sourcemeta::core::http_scan_quoted_string(
      R"("b\"b")", 0, storage, second)};
  EXPECT_TRUE(after_second.has_value());
  // The first view must survive the second append, given the reserve
  EXPECT_EQ(first, R"(a"a)");
  EXPECT_EQ(second, R"(b"b)");
}

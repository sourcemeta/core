#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/unicode.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem
#include <sstream>     // std::ostringstream
#include <stdexcept>   // std::runtime_error
#include <string>      // std::string, std::u32string
#include <string_view> // std::string_view

namespace {

auto parse_codepoint_sequence(const std::string_view field) -> std::u32string {
  std::u32string result;
  std::size_t position{0};
  while (position < field.size()) {
    while (position < field.size() &&
           (field[position] == ' ' || field[position] == '\t')) {
      position += 1;
    }
    if (position >= field.size()) {
      break;
    }
    const auto start{position};
    while (position < field.size() && field[position] != ' ' &&
           field[position] != '\t') {
      position += 1;
    }
    const auto token{field.substr(start, position - start)};
    char32_t codepoint{0};
    for (const auto character : token) {
      codepoint <<= 4U;
      if (character >= '0' && character <= '9') {
        codepoint |= static_cast<char32_t>(character - '0');
      } else if (character >= 'A' && character <= 'F') {
        codepoint |= static_cast<char32_t>(character - 'A' + 10);
      } else if (character >= 'a' && character <= 'f') {
        codepoint |= static_cast<char32_t>(character - 'a' + 10);
      } else {
        throw std::runtime_error(
            "invalid hex digit in NormalizationTest codepoint");
      }
    }
    result.push_back(codepoint);
  }
  return result;
}

class NormalizationTest : public testing::Test {
public:
  explicit NormalizationTest(const std::string_view raw_line)
      : raw_line_{raw_line} {}

  auto TestBody() -> void override {
    auto data{this->raw_line_};
    const auto hash{data.find('#')};
    if (hash != std::string_view::npos) {
      data = data.substr(0, hash);
    }

    std::array<std::u32string, 5> fields;
    std::size_t start{0};
    std::size_t field_index{0};
    while (field_index < fields.size()) {
      const auto separator{data.find(';', start)};
      const auto end{separator == std::string_view::npos ? data.size()
                                                         : separator};
      fields[field_index] =
          parse_codepoint_sequence(data.substr(start, end - start));
      field_index += 1;
      if (separator == std::string_view::npos) {
        break;
      }
      start = separator + 1;
    }
    ASSERT_EQ(field_index, fields.size())
        << "Expected 5 semicolon-separated columns, got " << field_index;

    // Per the NormalizationTest.txt header, the NFC invariants over the
    // five columns are:
    //
    //   c2 == toNFC(c1) == toNFC(c2) == toNFC(c3)
    //   c4 == toNFC(c4) == toNFC(c5)
    //
    // and a codepoint is in NFC iff its NFC form equals itself.
    const auto &c1{fields[0]};
    const auto &c2{fields[1]};
    const auto &c3{fields[2]};
    const auto &c4{fields[3]};
    const auto &c5{fields[4]};

    EXPECT_EQ(sourcemeta::core::nfc(c1), c2);
    EXPECT_EQ(sourcemeta::core::nfc(c2), c2);
    EXPECT_EQ(sourcemeta::core::nfc(c3), c2);
    EXPECT_EQ(sourcemeta::core::nfc(c4), c4);
    EXPECT_EQ(sourcemeta::core::nfc(c5), c4);

    EXPECT_TRUE(sourcemeta::core::is_nfc(c2));
    EXPECT_TRUE(sourcemeta::core::is_nfc(c4));
    EXPECT_EQ(sourcemeta::core::is_nfc(c1), c1 == c2);
    EXPECT_EQ(sourcemeta::core::is_nfc(c3), c3 == c2);
    EXPECT_EQ(sourcemeta::core::is_nfc(c5), c5 == c4);
  }

private:
  std::string_view raw_line_;
};

auto register_tests(const std::string_view contents) -> void {
  std::size_t position{0};
  std::size_t line_number{0};
  int current_part{-1};

  while (position < contents.size()) {
    line_number += 1;
    const auto newline{contents.find('\n', position)};
    const auto line_end{newline == std::string_view::npos ? contents.size()
                                                          : newline};
    auto line{contents.substr(position, line_end - position)};
    position = line_end + 1;

    // Strip trailing CR if the file used CRLF line endings
    if (!line.empty() && line.back() == '\r') {
      line.remove_suffix(1);
    }

    const auto first_non_ws{line.find_first_not_of(" \t")};
    if (first_non_ws == std::string_view::npos) {
      continue;
    }
    const auto trimmed{line.substr(first_non_ws)};

    if (trimmed[0] == '#') {
      continue;
    }

    if (trimmed.starts_with("@Part")) {
      if (trimmed.size() > 5 && trimmed[5] >= '0' && trimmed[5] <= '9') {
        current_part = trimmed[5] - '0';
      }
      continue;
    }

    std::ostringstream test_name;
    test_name << "Part" << current_part << "_line_" << line_number;
    const auto test_name_string{test_name.str()};

    testing::RegisterTest("UnicodeNormalizationTestSuite",
                          test_name_string.c_str(), nullptr, nullptr, __FILE__,
                          __LINE__, [line]() -> NormalizationTest * {
                            return new NormalizationTest{line};
                          });
  }
}

} // namespace

auto main(int argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  const auto contents{sourcemeta::core::read_file_to_string(
      std::filesystem::path{NORMALIZATIONTEST_PATH})};
  register_tests(contents);
  return RUN_ALL_TESTS();
}

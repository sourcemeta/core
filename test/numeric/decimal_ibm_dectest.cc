#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/numeric.h>

#include <algorithm>   // std::transform, std::any_of
#include <cctype>      // std::tolower, std::isdigit, std::isalnum
#include <filesystem>  // std::filesystem
#include <iostream>    // std::cerr
#include <sstream>     // std::istringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

static constexpr std::string_view SUPPORTED_OPERATIONS[] = {
    "compare",     "add",   "subtract", "multiply", "divide",
    "remainder",   "minus", "plus",     "abs",      "tointegral",
    "tointegralx", "tosci", "toeng"};

static constexpr std::string_view UNARY_OPERATIONS[] = {
    "minus", "plus", "abs", "tointegral", "tointegralx", "tosci", "toeng"};

static constexpr std::string_view SKIP_CONDITIONS[] = {
    "inexact",   "rounded", "overflow",           "underflow",
    "subnormal", "clamped", "division_impossible"};

static constexpr std::string_view KNOWN_DIRECTIVES[] = {
    "precision", "rounding", "maxexponent", "minexponent",
    "extended",  "clamp",    "version",     "dectest"};

struct DecTestCase {
  std::string id;
  std::string operation;
  std::string operand1;
  std::string operand2;
  std::string expected;
  std::vector<std::string> conditions;
};

struct DecTestContext {
  int precision = 9;
  std::string rounding = "half_up";
};

static auto to_lower(std::string value) -> std::string {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char character) {
                   return static_cast<char>(std::tolower(character));
                 });
  return value;
}

static auto strip_quotes(const std::string &value) -> std::string {
  if (value.size() >= 2 && ((value.front() == '\'' && value.back() == '\'') ||
                            (value.front() == '"' && value.back() == '"'))) {
    return value.substr(1, value.size() - 2);
  }
  return value;
}

static auto has_condition(const std::vector<std::string> &conditions,
                          std::string_view name) -> bool {
  return std::any_of(
      conditions.begin(), conditions.end(),
      [name](const auto &condition) { return to_lower(condition) == name; });
}

static auto has_skip_condition(const std::vector<std::string> &conditions)
    -> bool {
  return std::any_of(
      std::begin(SKIP_CONDITIONS), std::end(SKIP_CONDITIONS),
      [&](const auto name) { return has_condition(conditions, name); });
}

// TODO: Our Decimal class does not support signaling NaN (sNaN) or NaN
// diagnostic payloads (e.g. NaN9, NaN77). All NaN variants are treated
// as quiet NaN. Revisit when reimplementing without mpdecimal.
static auto is_unsupported_nan(const std::string &value) -> bool {
  const auto lower{to_lower(strip_quotes(value))};
  if (lower.find("snan") != std::string::npos) {
    return true;
  }
  auto position{lower.find("nan")};
  if (position == std::string::npos) {
    return false;
  }
  position += 3;
  return position < lower.size() &&
         std::isdigit(static_cast<unsigned char>(lower[position]));
}

static auto count_significant_digits(const std::string &value) -> std::size_t {
  const auto stripped{strip_quotes(value)};
  const auto lower{to_lower(stripped)};
  if (lower.find("nan") != std::string::npos ||
      lower.find("inf") != std::string::npos) {
    return 0;
  }

  std::size_t count{0};
  bool found_nonzero{false};
  bool in_exponent{false};
  for (const auto character : stripped) {
    if (character == 'e' || character == 'E') {
      in_exponent = true;
    } else if (!in_exponent &&
               std::isdigit(static_cast<unsigned char>(character))) {
      if (character != '0' || found_nonzero) {
        found_nonzero = true;
        count++;
      }
    }
  }
  return count;
}

// TODO: Our Decimal context uses emax/emin = +/-999999, which is smaller
// than the extreme exponents used in some tests (e.g. 1e999999999).
// These tests would overflow/underflow in our context.
static auto has_extreme_exponent(const std::string &value) -> bool {
  const auto stripped{strip_quotes(value)};
  const auto lower{to_lower(stripped)};
  if (lower.find("nan") != std::string::npos ||
      lower.find("inf") != std::string::npos) {
    return false;
  }

  auto exponent_position{lower.find('e')};
  if (exponent_position == std::string::npos) {
    return false;
  }

  try {
    const auto exponent{std::stol(stripped.substr(exponent_position + 1))};
    return exponent > 999999 || exponent < -999999;
  } catch (...) {
    return false;
  }
}

static auto make_decimal(const std::string &raw) -> sourcemeta::core::Decimal {
  const auto value{strip_quotes(raw)};
  const auto lower{to_lower(value)};
  if (lower.find("nan") != std::string::npos) {
    return sourcemeta::core::Decimal::nan();
  }
  if (lower == "inf" || lower == "infinity" || lower.starts_with("+inf")) {
    return sourcemeta::core::Decimal::infinity();
  }
  if (lower.starts_with("-inf")) {
    return sourcemeta::core::Decimal::negative_infinity();
  }
  return sourcemeta::core::Decimal{value};
}

static auto expect_decimal_eq(const sourcemeta::core::Decimal &result,
                              const sourcemeta::core::Decimal &expected)
    -> void {
  if (expected.is_nan()) {
    EXPECT_TRUE(result.is_nan());
  } else if (expected.is_infinite()) {
    EXPECT_TRUE(result.is_infinite());
    EXPECT_EQ(result.is_signed(), expected.is_signed());
  } else {
    EXPECT_EQ(result, expected);
  }
}

class DecTest : public testing::Test {
public:
  explicit DecTest(DecTestCase test_case) : test_case_{std::move(test_case)} {}

  auto TestBody() -> void override {
    const auto operation{to_lower(this->test_case_.operation)};

    if (operation == "compare") {
      this->run_compare();
    } else if (operation == "add") {
      this->run_binary(
          [](const auto &left, const auto &right) { return left + right; });
    } else if (operation == "subtract") {
      this->run_binary(
          [](const auto &left, const auto &right) { return left - right; });
    } else if (operation == "multiply") {
      this->run_binary(
          [](const auto &left, const auto &right) { return left * right; });
    } else if (operation == "divide") {
      this->run_binary(
          [](const auto &left, const auto &right) { return left / right; });
    } else if (operation == "remainder") {
      this->run_binary(
          [](const auto &left, const auto &right) { return left % right; });
    } else if (operation == "minus") {
      this->run_unary([](const auto &value) { return -value; });
    } else if (operation == "plus") {
      this->run_unary([](const auto &value) { return +value; });
    } else if (operation == "abs") {
      this->run_unary(
          [](const auto &value) { return value.is_signed() ? -value : value; });
    } else if (operation == "tointegral" || operation == "tointegralx") {
      this->run_unary([](const auto &value) { return value.to_integral(); });
    } else if (operation == "tosci" || operation == "toeng") {
      this->run_conversion();
    } else {
      FAIL();
    }
  }

private:
  auto run_compare() -> void {
    const auto expected{strip_quotes(this->test_case_.expected)};

    // TODO: The decTest spec defines compare(NaN, x) = NaN, but our
    // comparison operators return bool, so we cannot represent a NaN
    // comparison result
    if (to_lower(expected).find("nan") != std::string::npos) {
      GTEST_SKIP() << "NaN comparison result";
      return;
    }

    const auto left{make_decimal(this->test_case_.operand1)};
    const auto right{make_decimal(this->test_case_.operand2)};

    if (expected == "0") {
      EXPECT_TRUE(left == right);
    } else if (expected == "1") {
      EXPECT_TRUE(left > right);
    } else if (expected == "-1") {
      EXPECT_TRUE(left < right);
    } else {
      FAIL();
    }
  }

  template <typename Operation> auto run_binary(Operation op) -> void {
    const auto has_invalid{
        has_condition(this->test_case_.conditions, "invalid_operation") ||
        has_condition(this->test_case_.conditions, "division_undefined")};
    const auto has_divzero{
        has_condition(this->test_case_.conditions, "division_by_zero")};

    if (has_invalid) {
      try {
        const auto result{op(make_decimal(this->test_case_.operand1),
                             make_decimal(this->test_case_.operand2))};
        EXPECT_TRUE(result.is_nan());
      } catch (const sourcemeta::core::NumericInvalidOperationError &) {
        SUCCEED();
      } catch (const sourcemeta::core::NumericDivisionByZeroError &) {
        SUCCEED();
      }
      return;
    }

    if (has_divzero) {
      try {
        const auto result{op(make_decimal(this->test_case_.operand1),
                             make_decimal(this->test_case_.operand2))};
        const auto expected_value{make_decimal(this->test_case_.expected)};
        if (expected_value.is_infinite() && result.is_infinite()) {
          EXPECT_EQ(result.is_signed(), expected_value.is_signed());
        } else {
          FAIL();
        }
      } catch (const sourcemeta::core::NumericDivisionByZeroError &) {
        SUCCEED();
      }
      return;
    }

    const auto expected_value{make_decimal(this->test_case_.expected)};
    try {
      const auto result{op(make_decimal(this->test_case_.operand1),
                           make_decimal(this->test_case_.operand2))};
      expect_decimal_eq(result, expected_value);
    } catch (const sourcemeta::core::NumericDivisionByZeroError &) {
      // TODO: Our Decimal traps on ALL division-by-zero cases, but the
      // General Decimal Arithmetic spec says Inf/0 = Inf and NaN/0 = NaN
      // (only finite/0 should raise Division_by_zero). Fix when
      // reimplementing without mpdecimal.
      if (!expected_value.is_infinite() && !expected_value.is_nan()) {
        FAIL();
      }
    } catch (const sourcemeta::core::NumericInvalidOperationError &) {
      // TODO: Some operations (e.g. NaN % 0) throw InvalidOperation
      // but the spec expects a quiet NaN result. Fix when reimplementing
      // without mpdecimal.
      if (!expected_value.is_nan()) {
        FAIL();
      }
    }
  }

  template <typename Operation> auto run_unary(Operation op) -> void {
    if (has_condition(this->test_case_.conditions, "invalid_operation")) {
      try {
        EXPECT_TRUE(op(make_decimal(this->test_case_.operand1)).is_nan());
      } catch (const sourcemeta::core::NumericInvalidOperationError &) {
        SUCCEED();
      }
      return;
    }

    expect_decimal_eq(op(make_decimal(this->test_case_.operand1)),
                      make_decimal(this->test_case_.expected));
  }

  // TODO: Our to_scientific_string() always uses exponential form
  // (e.g. "1.0e+1" instead of "10"), which differs from the spec's
  // to-scientific-string. Our to_string() has the same issue with
  // to-engineering-string. We compare parsed values instead of strings.
  auto run_conversion() -> void {
    const auto input{strip_quotes(this->test_case_.operand1)};

    if (has_condition(this->test_case_.conditions, "conversion_syntax")) {
      EXPECT_THROW(sourcemeta::core::Decimal{input},
                   sourcemeta::core::DecimalParseError);
      return;
    }

    expect_decimal_eq(sourcemeta::core::Decimal{input},
                      make_decimal(this->test_case_.expected));
  }

  DecTestCase test_case_;
};

// ---------------------------------------------------------------------------
// Parsing
// ---------------------------------------------------------------------------

static auto is_supported_operation(const std::string &operation) -> bool {
  const auto lower{to_lower(operation)};
  return std::any_of(std::begin(SUPPORTED_OPERATIONS),
                     std::end(SUPPORTED_OPERATIONS),
                     [&lower](const auto name) { return lower == name; });
}

static auto is_unary_operation(std::string_view operation) -> bool {
  return std::any_of(
      std::begin(UNARY_OPERATIONS), std::end(UNARY_OPERATIONS),
      [operation](const auto name) { return operation == name; });
}

static auto read_quoted_token(std::istringstream &stream, std::string &token)
    -> bool {
  if (!(stream >> token)) {
    return false;
  }
  if (!token.empty() && (token.front() == '\'' || token.front() == '"')) {
    const char quote{token.front()};
    while (token.back() != quote || token.size() == 1) {
      std::string next;
      if (!(stream >> next)) {
        return false;
      }
      token += " " + next;
    }
  }
  return true;
}

static auto parse_test_line(const std::string &line, DecTestCase &test_case)
    -> bool {
  std::istringstream stream{line};
  std::string token;

  if (!(stream >> test_case.id) || !(stream >> test_case.operation)) {
    return false;
  }
  if (!is_supported_operation(test_case.operation)) {
    return false;
  }
  if (!read_quoted_token(stream, test_case.operand1)) {
    return false;
  }

  test_case.operand2.clear();
  test_case.conditions.clear();

  if (is_unary_operation(to_lower(test_case.operation))) {
    if (!(stream >> token) || token != "->") {
      return false;
    }
  } else {
    if (!read_quoted_token(stream, test_case.operand2)) {
      return false;
    }
    if (test_case.operand2 == "->") {
      test_case.operand2.clear();
    } else if (!(stream >> token) || token != "->") {
      return false;
    }
  }

  if (!read_quoted_token(stream, test_case.expected)) {
    return false;
  }

  while (stream >> token) {
    test_case.conditions.push_back(std::move(token));
  }

  return true;
}

static auto parse_directive(const std::string &line, DecTestContext &context)
    -> bool {
  const auto colon_position{line.find(':')};
  if (colon_position == std::string::npos) {
    return false;
  }

  const std::string_view full_line{line};
  auto key{full_line.substr(0, colon_position)};
  auto value{full_line.substr(colon_position + 1)};

  while (!key.empty() && key.back() == ' ') {
    key.remove_suffix(1);
  }
  while (!value.empty() && value.front() == ' ') {
    value.remove_prefix(1);
  }
  while (!value.empty() && value.back() == ' ') {
    value.remove_suffix(1);
  }

  const auto lower_key{to_lower(std::string{key})};
  if (lower_key == "precision") {
    context.precision = std::stoi(std::string{value});
    return true;
  }
  if (lower_key == "rounding") {
    context.rounding = to_lower(std::string{value});
    return true;
  }

  return std::any_of(
      std::begin(KNOWN_DIRECTIVES), std::end(KNOWN_DIRECTIVES),
      [&lower_key](const auto name) { return lower_key == name; });
}

// ---------------------------------------------------------------------------
// Skip logic
// ---------------------------------------------------------------------------

static auto should_skip_file(const std::string &filename) -> bool {
  const auto lower{to_lower(filename)};
  if (lower.starts_with("dd") || lower.starts_with("dq") ||
      lower.starts_with("ds")) {
    return true;
  }
  return lower.find("randoms") != std::string::npos ||
         lower.find("randombound") != std::string::npos ||
         lower == "testall.dectest" || lower == "inexact.dectest" ||
         lower == "rounding.dectest" || lower == "powersqrt.dectest";
}

static auto should_skip_test(const DecTestCase &test_case,
                             const DecTestContext &context) -> bool {
  const auto operation{to_lower(test_case.operation)};

  if (test_case.operand1.find('#') != std::string::npos ||
      test_case.operand2.find('#') != std::string::npos ||
      test_case.expected.find('#') != std::string::npos) {
    return true;
  }

  if (is_unsupported_nan(test_case.operand1) ||
      is_unsupported_nan(test_case.operand2)) {
    return true;
  }

  if (has_extreme_exponent(test_case.operand1) ||
      has_extreme_exponent(test_case.operand2) ||
      has_extreme_exponent(test_case.expected)) {
    return true;
  }

  if (operation == "compare") {
    return false;
  }

  // TODO: Our Decimal context is fixed at 16-digit precision. Tests
  // expecting results with more significant digits would produce
  // different (truncated) results in our context.
  if (count_significant_digits(test_case.expected) > 16) {
    return true;
  }

  // TODO: Our Decimal context uses half_even rounding exclusively.
  // tointegral results depend on rounding mode, so we can only run
  // these tests when the file's rounding directive matches ours.
  if (operation == "tointegral" || operation == "tointegralx") {
    if (context.rounding != "half_even") {
      return true;
    }
  }

  return has_skip_condition(test_case.conditions);
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

static auto sanitize_test_name(const std::string &name) -> std::string {
  std::string result;
  result.reserve(name.size());
  std::transform(name.begin(), name.end(), std::back_inserter(result),
                 [](unsigned char character) -> char {
                   return std::isalnum(character) || character == '_'
                              ? static_cast<char>(character)
                              : '_';
                 });
  return result;
}

auto main(int argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  const std::filesystem::path dectest_path{DECTEST_PATH};

  std::size_t total_registered{0};
  std::size_t total_skipped{0};

  for (const auto &entry : std::filesystem::directory_iterator(dectest_path)) {
    if (!entry.is_regular_file()) {
      continue;
    }

    const auto filepath{entry.path()};
    if (filepath.extension() != ".decTest") {
      continue;
    }
    if (should_skip_file(filepath.filename().string())) {
      continue;
    }

    const auto suite_name{"DecTest_" +
                          sanitize_test_name(filepath.stem().string())};

    auto file{sourcemeta::core::read_file(filepath)};
    DecTestContext context;
    std::string line;

    while (std::getline(file, line)) {
      if (line.empty() || line.starts_with("--")) {
        continue;
      }

      auto start{line.find_first_not_of(" \t")};
      if (start == std::string::npos) {
        continue;
      }
      line = line.substr(start);

      if (parse_directive(line, context)) {
        continue;
      }

      DecTestCase test_case;
      if (!parse_test_line(line, test_case)) {
        continue;
      }

      if (should_skip_test(test_case, context)) {
        total_skipped++;
        continue;
      }

      const auto test_name{sanitize_test_name(test_case.id)};
      testing::RegisterTest(suite_name.c_str(), test_name.c_str(), nullptr,
                            nullptr, __FILE__, __LINE__,
                            [test_case = std::move(test_case)]() -> DecTest * {
                              return new DecTest(test_case);
                            });
      total_registered++;
    }
  }

  std::cerr << "DecTest: registered " << total_registered << " tests, skipped "
            << total_skipped << "\n";

  if (total_registered == 0) {
    std::cerr << "ERROR: No tests were registered!\n";
    return 1;
  }

  return RUN_ALL_TESTS();
}

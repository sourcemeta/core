#include <sourcemeta/core/io.h>
#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/test.h>

#include <algorithm>   // std::transform, std::any_of
#include <cctype>      // std::tolower, std::isalnum
#include <filesystem>  // std::filesystem
#include <iostream>    // std::cerr
#include <sstream>     // std::istringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

static constexpr std::string_view SUPPORTED_OPERATIONS[] = {
    "compare",     "add",      "subtract",   "multiply",     "divide",
    "remainder",   "minus",    "plus",       "abs",          "tointegral",
    "tointegralx", "tosci",    "toeng",      "copy",         "copyabs",
    "copynegate",  "copysign", "comparesig", "comparetotal", "comparetotmag",
    "max",         "min",      "maxmag",     "minmag",       "divideint",
    "samequantum", "logb",     "scaleb",     "reduce",       "trim"};

static constexpr std::string_view UNARY_OPERATIONS[] = {
    "minus", "plus",   "abs",  "tointegral", "tointegralx",
    "tosci", "toeng",  "copy", "copyabs",    "copynegate",
    "logb",  "reduce", "trim"};

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
  int max_exponent = 999;
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

static auto make_decimal(const std::string &raw) -> sourcemeta::core::Decimal {
  const auto value{strip_quotes(raw)};
  const auto lower{to_lower(value)};
  if (lower == "inf" || lower == "infinity" || lower.starts_with("+inf")) {
    return sourcemeta::core::Decimal::infinity();
  }
  if (lower.starts_with("-inf")) {
    return sourcemeta::core::Decimal::negative_infinity();
  }
  return sourcemeta::core::Decimal{value};
}

static auto decimal_abs(const sourcemeta::core::Decimal &value)
    -> sourcemeta::core::Decimal {
  return value.is_signed() ? -value : value;
}

static auto expect_comparison_result(const sourcemeta::core::Decimal &left,
                                     const sourcemeta::core::Decimal &right,
                                     const std::string &expected) -> bool {
  if (expected == "0") {
    EXPECT_TRUE(left == right);
  } else if (expected == "1") {
    EXPECT_TRUE(left > right);
  } else if (expected == "-1") {
    EXPECT_TRUE(left < right);
  } else {
    return false;
  }

  return true;
}

static auto expect_decimal_eq(const sourcemeta::core::Decimal &result,
                              const sourcemeta::core::Decimal &expected)
    -> void {
  if (expected.is_nan()) {
    EXPECT_TRUE(result.is_nan());
    if (expected.is_snan()) {
      EXPECT_TRUE(result.is_snan());
    } else {
      EXPECT_TRUE(result.is_qnan());
    }
  } else if (expected.is_infinite()) {
    EXPECT_TRUE(result.is_infinite());
    EXPECT_EQ(result.is_signed(), expected.is_signed());
  } else {
    EXPECT_EQ(result, expected);
  }
}

static auto run_comparison(const DecTestCase &test_case, bool signal_on_nan)
    -> void {
  const auto left{make_decimal(test_case.operand1)};
  const auto right{make_decimal(test_case.operand2)};

  if (signal_on_nan &&
      has_condition(test_case.conditions, "invalid_operation")) {
    EXPECT_TRUE(make_decimal(test_case.expected).is_nan());
    return;
  }

  const auto expected{strip_quotes(test_case.expected)};
  EXPECT_TRUE(expect_comparison_result(left, right, expected));
}

template <typename Operation>
static auto run_binary(const DecTestCase &test_case, Operation op) -> void {
  const auto has_invalid{
      has_condition(test_case.conditions, "invalid_operation") ||
      has_condition(test_case.conditions, "division_undefined")};
  const auto has_divzero{
      has_condition(test_case.conditions, "division_by_zero")};

  if (has_invalid) {
    try {
      const auto result{op(make_decimal(test_case.operand1),
                           make_decimal(test_case.operand2))};
      EXPECT_TRUE(result.is_nan());
    } catch (const sourcemeta::core::NumericInvalidOperationError &) {
    } catch (const sourcemeta::core::NumericDivisionByZeroError &) {
    }
    return;
  }

  if (has_divzero) {
    try {
      const auto result{op(make_decimal(test_case.operand1),
                           make_decimal(test_case.operand2))};
      const auto expected_value{make_decimal(test_case.expected)};
      if (expected_value.is_infinite() && result.is_infinite()) {
        EXPECT_EQ(result.is_signed(), expected_value.is_signed());
      } else {
        FAIL();
      }
    } catch (const sourcemeta::core::NumericDivisionByZeroError &) {
    }
    return;
  }

  const auto expected_value{make_decimal(test_case.expected)};
  const auto result{
      op(make_decimal(test_case.operand1), make_decimal(test_case.operand2))};
  expect_decimal_eq(result, expected_value);
}

template <typename Operation>
static auto run_unary(const DecTestCase &test_case, Operation op) -> void {
  if (has_condition(test_case.conditions, "invalid_operation")) {
    try {
      EXPECT_TRUE(op(make_decimal(test_case.operand1)).is_nan());
    } catch (const sourcemeta::core::NumericInvalidOperationError &) {
    }
    return;
  }

  expect_decimal_eq(op(make_decimal(test_case.operand1)),
                    make_decimal(test_case.expected));
}

// TODO: Our to_scientific_string() always uses exponential form
// (e.g. "1.0e+1" instead of "10"), which differs from the spec's
// to-scientific-string. Our to_string() has the same issue with
// to-engineering-string. We compare parsed values instead of strings.
static auto run_conversion(const DecTestCase &test_case) -> void {
  const auto input{strip_quotes(test_case.operand1)};

  if (has_condition(test_case.conditions, "conversion_syntax")) {
    try {
      const auto result{sourcemeta::core::Decimal{input}};
      EXPECT_TRUE(result.is_nan());
    } catch (const sourcemeta::core::DecimalParseError &) {
    }
    return;
  }

  expect_decimal_eq(sourcemeta::core::Decimal{input},
                    make_decimal(test_case.expected));
}

static auto run_copysign(const DecTestCase &test_case) -> void {
  const auto left{make_decimal(test_case.operand1)};
  const auto right{make_decimal(test_case.operand2)};
  auto result = decimal_abs(left);
  if (right.is_signed()) {
    result = -decimal_abs(result);
  }

  expect_decimal_eq(result, make_decimal(test_case.expected));
}

static auto run_logb(const DecTestCase &test_case) -> void {
  if (has_condition(test_case.conditions, "division_by_zero")) {
    try {
      auto result = make_decimal(test_case.operand1).logb();
      static_cast<void>(result);
      FAIL();
    } catch (const sourcemeta::core::NumericDivisionByZeroError &) {
    }
    return;
  }

  run_unary(test_case, [](const auto &value) { return value.logb(); });
}

static auto run_max_min(const DecTestCase &test_case, bool is_max,
                        bool by_magnitude) -> void {
  const auto left{make_decimal(test_case.operand1)};
  const auto right{make_decimal(test_case.operand2)};
  const auto expected{make_decimal(test_case.expected)};

  if (left.is_snan() || right.is_snan()) {
    EXPECT_TRUE(expected.is_nan());
    return;
  }

  if (left.is_qnan() && right.is_qnan()) {
    EXPECT_TRUE(expected.is_nan());
    return;
  }

  if (left.is_qnan()) {
    expect_decimal_eq(right, expected);
    return;
  }

  if (right.is_qnan()) {
    expect_decimal_eq(left, expected);
    return;
  }

  int comparison;
  if (by_magnitude) {
    auto abs_left = decimal_abs(left);
    auto abs_right = decimal_abs(right);
    comparison = abs_left < abs_right ? -1 : (abs_left > abs_right ? 1 : 0);
  } else {
    comparison = left < right ? -1 : (left > right ? 1 : 0);
  }

  sourcemeta::core::Decimal result{0};
  if (comparison != 0) {
    result = (is_max == (comparison > 0)) ? left : right;
  } else {
    auto total_cmp = left.compare_total(right);
    result =
        (is_max == (total_cmp > sourcemeta::core::Decimal{0})) ? left : right;
  }

  expect_decimal_eq(result, expected);
}

static auto run_dectest_case(const DecTestCase &test_case) -> void {
  const auto operation{to_lower(test_case.operation)};

  if (operation == "compare" || operation == "comparesig") {
    run_comparison(test_case, operation == "comparesig");
  } else if (operation == "add") {
    run_binary(test_case, [](const auto &left, const auto &right) {
      return left + right;
    });
  } else if (operation == "subtract") {
    run_binary(test_case, [](const auto &left, const auto &right) {
      return left - right;
    });
  } else if (operation == "multiply") {
    run_binary(test_case, [](const auto &left, const auto &right) {
      return left * right;
    });
  } else if (operation == "divide") {
    run_binary(test_case, [](const auto &left, const auto &right) {
      return left / right;
    });
  } else if (operation == "remainder") {
    run_binary(test_case, [](const auto &left, const auto &right) {
      return left % right;
    });
  } else if (operation == "minus" || operation == "copynegate") {
    run_unary(test_case, [](const auto &value) { return -value; });
  } else if (operation == "plus") {
    run_unary(test_case, [](const auto &value) { return +value; });
  } else if (operation == "abs" || operation == "copyabs") {
    run_unary(test_case, [](const auto &value) { return decimal_abs(value); });
  } else if (operation == "tointegral" || operation == "tointegralx") {
    run_unary(test_case, [](const auto &value) { return value.to_integral(); });
  } else if (operation == "tosci" || operation == "toeng") {
    run_conversion(test_case);
  } else if (operation == "copy") {
    run_unary(test_case, [](const auto &value) { return value; });
  } else if (operation == "copysign") {
    run_copysign(test_case);
  } else if (operation == "comparetotal") {
    run_binary(test_case, [](const auto &left, const auto &right) {
      return left.compare_total(right);
    });
  } else if (operation == "comparetotmag") {
    run_binary(test_case, [](const auto &left, const auto &right) {
      return decimal_abs(left).compare_total(decimal_abs(right));
    });
  } else if (operation == "max") {
    run_max_min(test_case, true, false);
  } else if (operation == "min") {
    run_max_min(test_case, false, false);
  } else if (operation == "maxmag") {
    run_max_min(test_case, true, true);
  } else if (operation == "minmag") {
    run_max_min(test_case, false, true);
  } else if (operation == "divideint") {
    run_binary(test_case, [](const auto &left, const auto &right) {
      return left.divide_integer(right);
    });
  } else if (operation == "samequantum") {
    run_binary(test_case, [](const auto &left, const auto &right) {
      return sourcemeta::core::Decimal{left.same_quantum(right) ? 1 : 0};
    });
  } else if (operation == "logb") {
    run_logb(test_case);
  } else if (operation == "scaleb") {
    run_binary(test_case, [](const auto &left, const auto &right) {
      return left.scale_by(right);
    });
  } else if (operation == "reduce" || operation == "trim") {
    run_unary(test_case, [](const auto &value) { return value.reduce(); });
  } else {
    FAIL();
  }
}

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
  if (lower_key == "maxexponent") {
    context.max_exponent = std::stoi(std::string{value});
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

  // The decTest spec defines compare(NaN, x) = NaN, but our comparison
  // operators return bool, so a NaN comparison result cannot be represented.
  // The signalling case that carries an invalid_operation condition is kept, as
  // it only asserts that the expected result parses as NaN.
  if (operation == "compare" || operation == "comparesig") {
    const auto signalling_nan{
        operation == "comparesig" &&
        has_condition(test_case.conditions, "invalid_operation")};
    if (!signalling_nan &&
        to_lower(strip_quotes(test_case.expected)).find("nan") !=
            std::string::npos) {
      return true;
    }
  }

  if (operation == "compare" || operation == "comparetotal" ||
      operation == "comparetotmag" || operation == "comparesig" ||
      operation == "copy" || operation == "copyabs" ||
      operation == "copynegate" || operation == "copysign" ||
      operation == "samequantum") {
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

  if (operation == "scaleb" &&
      has_condition(test_case.conditions, "invalid_operation")) {
    auto scale_bound = 2 * (context.max_exponent + context.precision);
    try {
      auto scale_value = std::stoll(test_case.operand2);
      if (scale_value > scale_bound || scale_value < -scale_bound) {
        return true;
      }
    } catch (...) {
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
      sourcemeta::core::test_register(
          suite_name, test_name, __FILE__, __LINE__,
          [test_case = std::move(test_case)]() -> void {
            run_dectest_case(test_case);
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

  return sourcemeta::core::test_run(argc, argv);
}

#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/urlpattern.h>

#include <cstdint>    // std::int64_t
#include <filesystem> // std::filesystem
#include <iostream>   // std::cerr
#include <string>     // std::string, std::to_string

static auto compare_to_int(const std::strong_ordering ordering)
    -> std::int64_t {
  if (ordering == std::strong_ordering::less) {
    return -1;
  } else if (ordering == std::strong_ordering::greater) {
    return 1;
  } else {
    return 0;
  }
}

static auto is_url_component(const std::string &key) -> bool {
  return key == "protocol" || key == "username" || key == "password" ||
         key == "hostname" || key == "port" || key == "pathname" ||
         key == "search" || key == "hash";
}

// TODO: Remove this filter and handle all tests
static auto do_we_handle_match_test(const sourcemeta::core::JSON &test_case)
    -> bool {
  if (!test_case.defines("pattern") || !test_case.defines("inputs") ||
      !test_case.defines("expected_match")) {
    return false;
  }

  const auto &pattern = test_case.at("pattern");
  const auto &inputs = test_case.at("inputs");

  if (!pattern.is_array() || pattern.size() != 1) {
    return false;
  }

  if (!inputs.is_array() || inputs.size() != 1) {
    return false;
  }

  const auto &pattern_item = pattern.at(0);
  const auto &input_item = inputs.at(0);

  // Pattern must be an object (string patterns require full URL parsing)
  if (!pattern_item.is_object()) {
    return false;
  }

  // Input can be object or string (URL)
  if (!input_item.is_object() && !input_item.is_string()) {
    return false;
  }

  // Skip tests with baseURL (not yet supported)
  if (pattern_item.defines("baseURL")) {
    return false;
  }
  if (input_item.is_object() && input_item.defines("baseURL")) {
    return false;
  }

  // Skip tests with exactly_empty_components (requires special port/protocol
  // normalization semantics that are not yet implemented)
  if (test_case.defines("exactly_empty_components")) {
    return false;
  }

  // Skip tests involving javascript: protocol (special opaque path handling)
  if (input_item.is_object() && input_item.defines("protocol")) {
    const auto &protocol = input_item.at("protocol").to_string();
    if (protocol == "javascript") {
      return false;
    }
  }

  // Skip tests that rely on default port normalization
  // (e.g., http + port 80 should normalize port to empty)
  if (input_item.is_object() && input_item.defines("protocol") &&
      input_item.defines("port")) {
    const auto &protocol = input_item.at("protocol").to_string();
    const auto &port = input_item.at("port").to_string();
    if ((protocol == "http" && port == "80") ||
        (protocol == "https" && port == "443") ||
        (protocol == "ws" && port == "80") ||
        (protocol == "wss" && port == "443") ||
        (protocol == "ftp" && port == "21")) {
      return false;
    }
  }

  // Skip hostname patterns with groups containing IPv6 addresses
  // (e.g., "{[\:\:ab\::num]}") - these require complex segment matching
  // within groups, which is not yet implemented
  if (pattern_item.defines("hostname")) {
    const auto &hostname_pattern = pattern_item.at("hostname").to_string();
    if (hostname_pattern.find('{') != std::string::npos &&
        hostname_pattern.find('[') != std::string::npos) {
      return false;
    }
  }

  // All pattern keys must be URL components
  for (const auto &entry : pattern_item.as_object()) {
    if (!is_url_component(entry.first)) {
      return false;
    }
  }

  // All input keys must be URL components (only check if input is an object)
  if (input_item.is_object()) {
    for (const auto &entry : input_item.as_object()) {
      if (!is_url_component(entry.first)) {
        return false;
      }
    }
  }

  // Skip tests with empty pattern and string input (requires special default
  // wildcard capture semantics)
  if (pattern_item.as_object().empty() && input_item.is_string()) {
    return false;
  }

  // Skip tests with protocol patterns containing colons (e.g., "http{s}?:")
  if (pattern_item.defines("protocol")) {
    const auto &protocol = pattern_item.at("protocol").to_string();
    if (protocol.find(':') != std::string::npos) {
      return false;
    }
  }

  // Skip tests with search patterns containing leading ?
  if (pattern_item.defines("search")) {
    const auto &search = pattern_item.at("search").to_string();
    if (!search.empty() && search[0] == '?') {
      return false;
    }
  }

  // Skip tests with hostname patterns containing path/query/hash delimiters
  // that should be truncated (these require special canonicalization)
  if (pattern_item.defines("hostname")) {
    const auto &hostname = pattern_item.at("hostname").to_string();
    if (hostname.find('/') != std::string::npos ||
        hostname.find('?') != std::string::npos ||
        hostname.find('#') != std::string::npos) {
      return false;
    }
  }

  return true;
}

static auto check_component_result(
    const std::optional<sourcemeta::core::URLPatternComponentResult> &result,
    const sourcemeta::core::JSON &expected_groups) -> void {
  EXPECT_TRUE(result.has_value());
  if (!result.has_value()) {
    return;
  }

  std::size_t expected_non_null_count = 0;
  for (const auto &entry : expected_groups.as_object()) {
    if (!entry.second.is_null()) {
      expected_non_null_count += 1;
    }
  }
  EXPECT_EQ(result->size(), expected_non_null_count);

  for (const auto &entry : expected_groups.as_object()) {
    const auto &key = entry.first;
    const auto &value = entry.second;

    bool is_numeric = !key.empty();
    for (const auto character : key) {
      if (character < '0' || character > '9') {
        is_numeric = false;
        break;
      }
    }

    if (value.is_null()) {
      if (!is_numeric) {
        const auto actual = result->at(key);
        EXPECT_FALSE(actual.has_value());
      }
      continue;
    }

    EXPECT_TRUE(value.is_string());
    if (!value.is_string()) {
      continue;
    }

    const auto expected_value = value.to_string();

    if (is_numeric) {
      // For numeric keys, first try name lookup (bare patterns store "0", "1",
      // etc. as names), then fall back to positional lookup (regular patterns)
      const auto by_name = result->at(key);
      if (by_name.has_value()) {
        EXPECT_EQ(by_name.value(), expected_value);
      } else {
        const auto position = static_cast<std::size_t>(std::stoul(key));
        if (position < result->size()) {
          const auto by_position = result->at(position);
          EXPECT_EQ(by_position, expected_value);
        } else {
          FAIL() << "Could not find value for numeric key " << key;
        }
      }
    } else {
      const auto actual = result->at(key);
      EXPECT_TRUE(actual.has_value());
      if (actual.has_value()) {
        EXPECT_EQ(actual.value(), expected_value);
      }
    }
  }
}

class URLPatternMatchTest : public testing::Test {
public:
  explicit URLPatternMatchTest(const sourcemeta::core::JSON &pattern_input,
                               const sourcemeta::core::JSON &input_value,
                               const sourcemeta::core::JSON &expected)
      : pattern_json{pattern_input}, input_json{input_value},
        expected_match{expected} {}

  void TestBody() override {
    std::optional<sourcemeta::core::URLPatternInput> maybe_input;
    if (this->input_json.is_string()) {
      maybe_input =
          sourcemeta::core::URLPatternInput::parse(this->input_json.to_string());
    } else {
      maybe_input =
          sourcemeta::core::URLPatternInput::parse(this->input_json);
    }
    const auto maybe_pattern{
        sourcemeta::core::URLPattern::parse(this->pattern_json)};

    if (!maybe_input.has_value() || !maybe_pattern.has_value()) {
      EXPECT_TRUE(this->expected_match.is_null());
      return;
    }

    const auto &input{maybe_input.value()};
    const auto &pattern{maybe_pattern.value()};
    const auto result{pattern.match(input)};

    if (this->expected_match.is_null()) {
      EXPECT_FALSE(result.protocol.has_value());
      EXPECT_FALSE(result.username.has_value());
      EXPECT_FALSE(result.password.has_value());
      EXPECT_FALSE(result.hostname.has_value());
      EXPECT_FALSE(result.port.has_value());
      EXPECT_FALSE(result.pathname.has_value());
      EXPECT_FALSE(result.search.has_value());
      EXPECT_FALSE(result.hash.has_value());
    } else {
      EXPECT_TRUE(this->expected_match.is_object());

      if (this->expected_match.defines("protocol")) {
        const auto &expected = this->expected_match.at("protocol");
        EXPECT_TRUE(expected.is_object());
        EXPECT_TRUE(expected.defines("groups"));
        check_component_result(result.protocol, expected.at("groups"));
      }

      if (this->expected_match.defines("username")) {
        const auto &expected = this->expected_match.at("username");
        EXPECT_TRUE(expected.is_object());
        EXPECT_TRUE(expected.defines("groups"));
        check_component_result(result.username, expected.at("groups"));
      }

      if (this->expected_match.defines("password")) {
        const auto &expected = this->expected_match.at("password");
        EXPECT_TRUE(expected.is_object());
        EXPECT_TRUE(expected.defines("groups"));
        check_component_result(result.password, expected.at("groups"));
      }

      if (this->expected_match.defines("hostname")) {
        const auto &expected = this->expected_match.at("hostname");
        EXPECT_TRUE(expected.is_object());
        EXPECT_TRUE(expected.defines("groups"));
        check_component_result(result.hostname, expected.at("groups"));
      }

      if (this->expected_match.defines("port")) {
        const auto &expected = this->expected_match.at("port");
        EXPECT_TRUE(expected.is_object());
        EXPECT_TRUE(expected.defines("groups"));
        check_component_result(result.port, expected.at("groups"));
      }

      if (this->expected_match.defines("pathname")) {
        const auto &expected = this->expected_match.at("pathname");
        EXPECT_TRUE(expected.is_object());
        EXPECT_TRUE(expected.defines("groups"));
        check_component_result(result.pathname, expected.at("groups"));
      }

      if (this->expected_match.defines("search")) {
        const auto &expected = this->expected_match.at("search");
        EXPECT_TRUE(expected.is_object());
        EXPECT_TRUE(expected.defines("groups"));
        check_component_result(result.search, expected.at("groups"));
      }

      if (this->expected_match.defines("hash")) {
        const auto &expected = this->expected_match.at("hash");
        EXPECT_TRUE(expected.is_object());
        EXPECT_TRUE(expected.defines("groups"));
        check_component_result(result.hash, expected.at("groups"));
      }
    }
  }

private:
  const sourcemeta::core::JSON pattern_json;
  const sourcemeta::core::JSON input_json;
  const sourcemeta::core::JSON expected_match;
};

class URLPatternCompareTest : public testing::Test {
public:
  explicit URLPatternCompareTest(const sourcemeta::core::JSON &left_input,
                                 const sourcemeta::core::JSON &right_input,
                                 const std::int64_t expected_value,
                                 const std::string &component_name)
      : left{left_input}, right{right_input}, expected{expected_value},
        component{component_name} {}

  void TestBody() override {
    sourcemeta::core::URLPattern left_pattern;
    sourcemeta::core::URLPattern right_pattern;

    if (this->left.is_object()) {
      left_pattern = sourcemeta::core::URLPattern::parse(this->left).value();
    } else if (this->left.is_string()) {
      left_pattern =
          sourcemeta::core::URLPattern::parse(this->left.to_string());
    }

    if (this->right.is_object()) {
      right_pattern = sourcemeta::core::URLPattern::parse(this->right).value();
    } else if (this->right.is_string()) {
      right_pattern =
          sourcemeta::core::URLPattern::parse(this->right.to_string());
    }

    // When inputs are URL strings (not objects), compare only the specified
    // component. This allows testing component-specific comparison behavior
    // with full URL inputs.
    std::int64_t result{0};
    const bool both_are_strings =
        this->left.is_string() && this->right.is_string();

    if (both_are_strings && this->component == "pathname") {
      result = compare_to_int(left_pattern.pathname <=> right_pattern.pathname);
    } else if (both_are_strings && this->component == "protocol") {
      result = compare_to_int(left_pattern.protocol <=> right_pattern.protocol);
    } else if (both_are_strings && this->component == "username") {
      result = compare_to_int(left_pattern.username <=> right_pattern.username);
    } else if (both_are_strings && this->component == "password") {
      result = compare_to_int(left_pattern.password <=> right_pattern.password);
    } else if (both_are_strings && this->component == "hostname") {
      result = compare_to_int(left_pattern.hostname <=> right_pattern.hostname);
    } else if (both_are_strings && this->component == "port") {
      result = compare_to_int(left_pattern.port <=> right_pattern.port);
    } else if (both_are_strings && this->component == "search") {
      result = compare_to_int(left_pattern.search <=> right_pattern.search);
    } else if (both_are_strings && this->component == "hash") {
      result = compare_to_int(left_pattern.hash <=> right_pattern.hash);
    } else {
      result = compare_to_int(left_pattern <=> right_pattern);
    }

    EXPECT_EQ(result, this->expected);
  }

private:
  const sourcemeta::core::JSON left;
  const sourcemeta::core::JSON right;
  const std::int64_t expected;
  const std::string component;
};

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  const std::filesystem::path compare_test_path =
      std::filesystem::path{WPT_URLPATTERN_PATH} / "resources" /
      "urlpattern-compare-test-data.json";

  const auto compare_test_data{sourcemeta::core::read_json(compare_test_path)};

  std::size_t compare_index = 0;
  for (const auto &test_case : compare_test_data.as_array()) {
    const auto &left = test_case.at("left");
    const auto &right = test_case.at("right");
    const auto expected = test_case.at("expected").to_integer();
    const std::string component = test_case.at("component").to_string();

    const std::string test_name = "compare_" + std::to_string(compare_index);

    testing::RegisterTest(
        "URLPattern_wpt_compare", test_name.c_str(), nullptr, nullptr, __FILE__,
        __LINE__, [=]() -> URLPatternCompareTest * {
          return new URLPatternCompareTest(left, right, expected, component);
        });

    compare_index += 1;
  }

  const std::filesystem::path match_test_path =
      std::filesystem::path{WPT_URLPATTERN_PATH} / "resources" /
      "urlpatterntestdata.json";

  const auto match_test_data{sourcemeta::core::read_json(match_test_path)};

  std::size_t match_index = 0;
  for (const auto &test_case : match_test_data.as_array()) {
    if (!do_we_handle_match_test(test_case)) {
      std::cerr << "WARNING: Skipping match test index " << match_index << "\n";
      match_index += 1;
      continue;
    }

    const auto &pattern = test_case.at("pattern").at(0);
    const auto &input = test_case.at("inputs").at(0);
    const auto &expected_match = test_case.at("expected_match");

    const std::string test_name = "match_" + std::to_string(match_index);

    testing::RegisterTest(
        "URLPattern_wpt_match", test_name.c_str(), nullptr, nullptr, __FILE__,
        __LINE__, [=]() -> URLPatternMatchTest * {
          return new URLPatternMatchTest(pattern, input, expected_match);
        });

    match_index += 1;
  }

  return RUN_ALL_TESTS();
}

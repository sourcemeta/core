#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uritemplate.h>

#include <cassert>       // assert
#include <filesystem>    // std::filesystem
#include <iterator>      // std::advance
#include <optional>      // std::nullopt
#include <sstream>       // std::ostringstream
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <tuple>         // std::make_tuple
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <variant>       // std::variant, std::get

class JSONExpander {
public:
  using CacheEntry =
      std::variant<std::string, sourcemeta::core::JSON::Array::const_iterator,
                   sourcemeta::core::JSON::Object::const_iterator>;

  JSONExpander(const sourcemeta::core::JSON &variables)
      : variables_{variables} {}

  auto operator()(const std::string_view name)
      -> sourcemeta::core::URITemplateValue {
    const std::string key{name};
    if (!this->variables_.defines(key)) {
      return std::nullopt;
    }

    const auto &value = this->variables_.at(key);

    if (value.is_string()) {
      return std::make_tuple(std::string_view{value.to_string()}, std::nullopt,
                             false);
    } else if (value.is_array()) {
      if (value.empty()) {
        return std::nullopt;
      }

      const auto &array = value.as_array();
      auto cache_entry = this->cache_.find(name);
      if (cache_entry == this->cache_.end()) {
        cache_entry = this->cache_.emplace(name, array.cbegin()).first;
      }

      auto &iterator = std::get<sourcemeta::core::JSON::Array::const_iterator>(
          cache_entry->second);
      const auto &element = *iterator;
      std::advance(iterator, 1);
      return std::make_tuple(std::string_view{element.to_string()},
                             std::nullopt, iterator != array.cend());
    } else if (value.is_object()) {
      if (value.empty()) {
        return std::nullopt;
      }

      const auto &object = value.as_object();
      auto cache_entry = this->cache_.find(name);
      if (cache_entry == this->cache_.end()) {
        cache_entry = this->cache_.emplace(name, object.cbegin()).first;
      }

      auto &iterator = std::get<sourcemeta::core::JSON::Object::const_iterator>(
          cache_entry->second);
      const auto &entry = *iterator;
      std::advance(iterator, 1);
      return std::make_tuple(std::string_view{entry.second.to_string()},
                             std::string_view{entry.first},
                             iterator != object.cend());
    } else {
      auto cache_entry = this->cache_.find(name);
      if (cache_entry == this->cache_.end()) {
        std::ostringstream stream;
        sourcemeta::core::stringify(value, stream);
        cache_entry = this->cache_.emplace(name, stream.str()).first;
      }

      return std::make_tuple(
          std::string_view{std::get<std::string>(cache_entry->second)},
          std::nullopt, false);
    }
  }

private:
  const sourcemeta::core::JSON &variables_;
  std::unordered_map<std::string_view, CacheEntry> cache_;
};

class URITemplateTest : public testing::Test {
public:
  URITemplateTest(const std::string_view template_source,
                  const std::unordered_set<std::string_view> &expected_results,
                  const sourcemeta::core::JSON &variables)
      : template_{template_source}, expected_{expected_results},
        variables_{variables} {}

  auto TestBody() -> void override {
    try {
      const sourcemeta::core::URITemplate uri_template{this->template_};
      const auto result{uri_template.expand(JSONExpander{this->variables_})};
      if (this->expected_.empty()) {
        FAIL();
      } else {
        EXPECT_TRUE(this->expected_.contains(result));
      }
    } catch (const sourcemeta::core::URITemplateParseError &) {
      if (this->expected_.empty()) {
        SUCCEED();
      } else {
        throw;
      }
    } catch (const sourcemeta::core::URITemplateExpansionError &) {
      if (this->expected_.empty()) {
        SUCCEED();
      } else {
        throw;
      }
    }
  }

private:
  const std::string_view template_;
  const std::unordered_set<std::string_view> &expected_;
  const sourcemeta::core::JSON &variables_;
};

static auto register_tests(const sourcemeta::core::JSON &document,
                           const std::string_view prefix) -> void {
  for (const auto &entry : document.as_object()) {
    const auto &section{entry.second};
    if (!section.is_object()) {
      continue;
    }

    std::size_t index{0};
    for (const auto &testcase : section.at("testcases").as_array()) {
      assert(testcase.is_array());
      assert(testcase.size() >= 2);
      const auto &template_value{testcase.at(0)};
      assert(template_value.is_string());
      const auto &expected_value{testcase.at(1)};

      std::unordered_set<std::string_view> expected_results;
      if (expected_value.is_string()) {
        expected_results.insert(expected_value.to_string());
      } else if (expected_value.is_array()) {
        for (const auto &item : expected_value.as_array()) {
          expected_results.insert(item.to_string());
        }
      } else {
        assert(expected_value.is_boolean());
        assert(!expected_value.to_boolean());
      }

      std::ostringstream test_name;
      test_name << prefix << "_";
      for (const auto character : entry.first) {
        test_name << (std::isalnum(character) ? character : '_');
      }

      test_name << "_testcase_" << index;

      testing::RegisterTest(
          "URITemplateSuite", test_name.str().c_str(), nullptr, nullptr,
          __FILE__, __LINE__,
          [&template_value, expected_results, &section]() -> testing::Test * {
            return new URITemplateTest(template_value.to_string(),
                                       expected_results,
                                       section.at("variables"));
          });

      index += 1;
    }
  }
}

auto main(int argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  const std::filesystem::path suite_path{URITEMPLATE_SUITE_PATH};

  const auto spec_examples{
      sourcemeta::core::read_json(suite_path / "spec-examples.json")};
  register_tests(spec_examples, "spec_examples");

  const auto extended_tests{
      sourcemeta::core::read_json(suite_path / "extended-tests.json")};
  register_tests(extended_tests, "extended_tests");

  const auto negative_tests{
      sourcemeta::core::read_json(suite_path / "negative-tests.json")};
  register_tests(negative_tests, "negative_tests");

  return RUN_ALL_TESTS();
}

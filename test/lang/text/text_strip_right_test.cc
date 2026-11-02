#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string_view> // std::string_view

namespace {

// A predicate that never throws when called but may throw when copied. Its
// copy constructor is only declared, as the noexcept operator does not call
// it, and GCC treats a defaulted one as not throwing whatever it declares
struct CopyMayThrow {
  CopyMayThrow() = default;
  CopyMayThrow(const CopyMayThrow &);
  auto operator()(const char) const noexcept -> bool { return false; }
};

// A predicate that never throws when called but whose result throws when it
// turns into a boolean, which a condition does
struct ResultMayThrow {
  struct Result {
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    operator bool() const noexcept(false) { return false; }
  };

  auto operator()(const char) const noexcept -> Result { return {}; }
};

} // namespace

TEST(no_trailing_character) {
  EXPECT_EQ(sourcemeta::core::strip_right("abc", '0'), "abc");
}

TEST(single_trailing_character) {
  EXPECT_EQ(sourcemeta::core::strip_right("abc0", '0'), "abc");
}

TEST(multiple_trailing_characters) {
  EXPECT_EQ(sourcemeta::core::strip_right("123000", '0'), "123");
}

TEST(only_trailing_stripped) {
  EXPECT_EQ(sourcemeta::core::strip_right("0a0b00", '0'), "0a0b");
}

TEST(all_characters_stripped) {
  EXPECT_EQ(sourcemeta::core::strip_right("0000", '0'), "");
}

TEST(empty_input) { EXPECT_EQ(sourcemeta::core::strip_right("", '0'), ""); }

TEST(strips_carriage_return) {
  EXPECT_EQ(sourcemeta::core::strip_right("line\r", '\r'), "line");
}

TEST(strips_trailing_spaces) {
  EXPECT_EQ(sourcemeta::core::strip_right("value   ", ' '), "value");
}

TEST(predicate_trailing_characters) {
  EXPECT_EQ(
      sourcemeta::core::strip_right(
          "123000", [](const char character) { return character == '0'; }),
      "123");
}

TEST(predicate_only_trailing_stripped) {
  EXPECT_EQ(
      sourcemeta::core::strip_right(
          "0a0b00", [](const char character) { return character == '0'; }),
      "0a0b");
}

TEST(predicate_matching_a_range) {
  EXPECT_EQ(sourcemeta::core::strip_right("abc123",
                                          [](const char character) {
                                            return character >= '0' &&
                                                   character <= '9';
                                          }),
            "abc");
}

TEST(predicate_all_characters_stripped) {
  EXPECT_EQ(sourcemeta::core::strip_right(
                "0000", [](const char character) { return character == '0'; }),
            "");
}

TEST(predicate_empty_input) {
  EXPECT_EQ(sourcemeta::core::strip_right(
                "", [](const char character) { return character == '0'; }),
            "");
}

TEST(predicate_that_never_throws_is_noexcept) {
  const std::string_view input{"x"};
  EXPECT_TRUE(noexcept(sourcemeta::core::strip_right(
      input, [](const char) noexcept { return false; })));
}

TEST(predicate_with_a_copy_that_may_throw_is_not_noexcept) {
  const std::string_view input{"x"};
  EXPECT_FALSE(noexcept(sourcemeta::core::strip_right(input, CopyMayThrow{})));
}

TEST(predicate_with_a_result_that_may_throw_is_not_noexcept) {
  const std::string_view input{"x"};
  EXPECT_FALSE(
      noexcept(sourcemeta::core::strip_right(input, ResultMayThrow{})));
}

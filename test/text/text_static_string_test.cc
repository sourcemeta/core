#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <concepts>
#include <cstring>
#include <string>
#include <string_view>

static_assert(
    !std::constructible_from<sourcemeta::core::StaticString, const char *>);
static_assert(
    !std::constructible_from<sourcemeta::core::StaticString, std::string>);
static_assert(
    !std::constructible_from<sourcemeta::core::StaticString, std::string_view>);
static_assert(std::constructible_from<sourcemeta::core::StaticString,
                                      decltype("literal")>);

TEST(data_returns_the_literal) {
  const sourcemeta::core::StaticString message{"Something went wrong"};
  EXPECT_STREQ(message.data(), "Something went wrong");
}

TEST(data_of_an_empty_literal) {
  const sourcemeta::core::StaticString message{""};
  EXPECT_EQ(std::strlen(message.data()), 0);
}

TEST(a_copy_points_to_the_same_literal) {
  const sourcemeta::core::StaticString message{"Something went wrong"};
  const sourcemeta::core::StaticString copy{message};
  EXPECT_EQ(copy.data(), message.data());
}

TEST(is_usable_at_compile_time) {
  constexpr sourcemeta::core::StaticString MESSAGE{"Something went wrong"};
  static_assert(MESSAGE.data()[0] == 'S');
  EXPECT_STREQ(MESSAGE.data(), "Something went wrong");
}

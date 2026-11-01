#include <sourcemeta/core/error.h>
#include <sourcemeta/core/test.h>

#include <concepts>
#include <exception>

namespace {

class ExampleError : public sourcemeta::core::Error {
public:
  ExampleError() : sourcemeta::core::Error{"Something went wrong"} {}
};

class ExampleWrapperError : public sourcemeta::core::Error {
public:
  ExampleWrapperError(const sourcemeta::core::StaticString message)
      : sourcemeta::core::Error{message} {}
};

} // namespace

static_assert(!std::constructible_from<sourcemeta::core::Error,
                                       sourcemeta::core::StaticString>);
static_assert(std::derived_from<sourcemeta::core::Error, std::exception>);

TEST(what_returns_the_message) {
  const ExampleError error;
  EXPECT_STREQ(error.what(), "Something went wrong");
}

TEST(message_returns_the_message) {
  const ExampleError error;
  EXPECT_STREQ(error.message().data(), "Something went wrong");
}

TEST(catchable_as_the_base) {
  try {
    throw ExampleError{};
  } catch (const sourcemeta::core::Error &error) {
    EXPECT_STREQ(error.what(), "Something went wrong");
  }
}

TEST(catchable_as_a_standard_exception) {
  try {
    throw ExampleError{};
  } catch (const std::exception &error) {
    EXPECT_STREQ(error.what(), "Something went wrong");
  }
}

TEST(message_passes_to_another_error) {
  try {
    throw ExampleError{};
  } catch (const sourcemeta::core::Error &error) {
    const ExampleWrapperError wrapped{error.message()};
    EXPECT_STREQ(wrapped.what(), "Something went wrong");
  }
}

TEST(message_passes_the_same_literal) {
  const ExampleError error;
  const ExampleWrapperError wrapped{error.message()};
  EXPECT_EQ(wrapped.message().data(), error.message().data());
}

TEST(what_survives_a_copy) {
  const ExampleError error;
  // Taking a reference instead would stop exercising the copy constructor
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  const ExampleError copy{error};
  EXPECT_STREQ(copy.what(), "Something went wrong");
}

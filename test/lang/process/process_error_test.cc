#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view
#include <vector>      // std::vector

TEST(program_not_found_error_message) {
  const sourcemeta::core::ProcessProgramNotFoundError error{"my-program"};
  EXPECT_STREQ(error.what(), "Could not locate the requested program");
}

TEST(program_not_found_error_program) {
  const sourcemeta::core::ProcessProgramNotFoundError error{"my-program"};
  EXPECT_EQ(error.program(), "my-program");
}

TEST(spawn_error_message) {
  const sourcemeta::core::ProcessSpawnError error{"my-program",
                                                  {"--foo", "bar"}};
  EXPECT_STREQ(error.what(), "Process terminated abnormally");
}

TEST(spawn_error_program) {
  const sourcemeta::core::ProcessSpawnError error{"my-program",
                                                  {"--foo", "bar"}};
  EXPECT_EQ(error.program(), "my-program");
}

TEST(spawn_error_arguments) {
  const sourcemeta::core::ProcessSpawnError error{"my-program",
                                                  {"--foo", "bar"}};
  EXPECT_EQ(error.arguments().size(), 2);
  EXPECT_EQ(error.arguments().at(0), "--foo");
  EXPECT_EQ(error.arguments().at(1), "bar");
}

TEST(spawn_error_arguments_from_a_span) {
  const std::vector<std::string_view> arguments{"--foo", "bar"};
  const sourcemeta::core::ProcessSpawnError error{"my-program", arguments};
  EXPECT_EQ(error.program(), "my-program");
  EXPECT_EQ(error.arguments().size(), 2);
  EXPECT_EQ(error.arguments().at(0), "--foo");
  EXPECT_EQ(error.arguments().at(1), "bar");
}

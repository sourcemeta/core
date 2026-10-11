#include <sourcemeta/core/system.h>
#include <sourcemeta/core/test.h>

#include <filesystem>  // std::filesystem::path
#include <string>      // std::string
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

TEST(fileview_error_message) {
  const sourcemeta::core::FileViewError error{"/foo/bar.json",
                                              "Failed to memory-map the file"};
  EXPECT_STREQ(error.what(), "Failed to memory-map the file");
}

TEST(fileview_error_path) {
  const sourcemeta::core::FileViewError error{"/foo/bar.json",
                                              "Failed to memory-map the file"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar.json"});
}

TEST(fileview_error_message_outlives_its_argument) {
  const auto error{[]() {
    std::string message{"Failed to memory-map the file"};
    return sourcemeta::core::FileViewError{"/foo/bar.json", message};
  }()};
  EXPECT_STREQ(error.what(), "Failed to memory-map the file");
}

TEST(file_not_found_error_message) {
  const sourcemeta::core::FileNotFoundError error{"/foo/bar.json"};
  EXPECT_STREQ(error.what(), "File not found");
}

TEST(file_not_found_error_path) {
  const sourcemeta::core::FileNotFoundError error{"/foo/bar.json"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar.json"});
}

TEST(file_permission_error_message) {
  const sourcemeta::core::FilePermissionError error{"/foo/bar.json"};
  EXPECT_STREQ(error.what(), "Permission denied");
}

TEST(file_permission_error_path) {
  const sourcemeta::core::FilePermissionError error{"/foo/bar.json"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar.json"});
}

TEST(not_a_directory_error_message) {
  const sourcemeta::core::NotADirectoryError error{"/foo/bar.json"};
  EXPECT_STREQ(error.what(), "Expected a directory but got a file");
}

TEST(not_a_directory_error_path) {
  const sourcemeta::core::NotADirectoryError error{"/foo/bar.json"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar.json"});
}

TEST(file_already_exists_error_message) {
  const sourcemeta::core::FileAlreadyExistsError error{"/foo/bar.json"};
  EXPECT_STREQ(error.what(), "File already exists");
}

TEST(file_already_exists_error_path) {
  const sourcemeta::core::FileAlreadyExistsError error{"/foo/bar.json"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar.json"});
}

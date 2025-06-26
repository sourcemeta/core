#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_from_path, unix_absolute) {
  const std::filesystem::path example{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(example)};
  EXPECT_EQ(uri.recompose(), "file:///foo/bar/baz");
}

TEST(URI_from_path, unix_with_space_and_reserved) {
  const std::filesystem::path example{"/foo/My Folder/has#hash?value%"};
  const auto uri{sourcemeta::core::URI::from_path(example)};
  EXPECT_EQ(uri.recompose(), "file:///foo/My%20Folder/has%23hash%3Fvalue%25");
}

TEST(URI_from_path, unix_trailing_slash) {
  const std::filesystem::path example{"/foo/bar/"};
  const auto uri{sourcemeta::core::URI::from_path(example)};
  EXPECT_EQ(uri.recompose(), "file:///foo/bar/");
}

TEST(URI_from_path, windows_drive_absolute) {
  const std::filesystem::path example{R"(C:\Program Files\Test)"};
  const auto uri{sourcemeta::core::URI::from_path(example)};
  EXPECT_EQ(uri.recompose(), "file:///C:/Program%20Files/Test");
}

TEST(URI_from_path, windows_drive_lowercase) {
  const std::filesystem::path example{R"(c:\temp\logs)"};
  const auto uri{sourcemeta::core::URI::from_path(example)};
  EXPECT_EQ(uri.recompose(), "file:///c:/temp/logs");
}

TEST(URI_from_path, windows_drive_root) {
  const std::filesystem::path example{R"(D:\)"};
  const auto uri{sourcemeta::core::URI::from_path(example)};
  EXPECT_EQ(uri.recompose(), "file:///D:/");
}

TEST(URI_from_path, windows_trailing_slash) {
  const std::filesystem::path example{R"(C:\foo\bar\)"};
  const auto uri{sourcemeta::core::URI::from_path(example)};
  EXPECT_EQ(uri.recompose(), "file:///C:/foo/bar/");
}

TEST(URI_from_path, windows_percent_and_plus) {
  // '%' → %25, '+' is allowed unencoded
  const std::filesystem::path example{R"(C:\path\50%+plus.txt)"};
  const auto uri{sourcemeta::core::URI::from_path(example)};
  EXPECT_EQ(uri.recompose(), "file:///C:/path/50%25+plus.txt");
}

TEST(URI_from_path, windows_unc_simple) {
  const std::filesystem::path example{R"(\\server\share\file.txt)"};
  const auto uri{sourcemeta::core::URI::from_path(example)};
  EXPECT_EQ(uri.recompose(),
            // For UNC, host=server, path=/share/file.txt
            "file://server/share/file.txt");
}

TEST(URI_from_path, windows_unc_with_space) {
  const std::filesystem::path example{R"(\\srv\My Docs\a b.txt)"};
  const auto uri{sourcemeta::core::URI::from_path(example)};
  EXPECT_EQ(uri.recompose(), "file://srv/My%20Docs/a%20b.txt");
}

TEST(URI_from_path, unicode_unix) {
  // U+00E9 (é) should be UTF-8 percent-encoded as %C3%A9
  const std::filesystem::path example{u8"/data/éclair.txt"};
  const auto uri{sourcemeta::core::URI::from_path(example)};
  EXPECT_EQ(uri.recompose(), "file:///data/%C3%A9clair.txt");
}

TEST(URI_from_path, unicode_windows) {
  // U+00E9 (é) should be UTF-8 percent-encoded as %C3%A9
  const std::filesystem::path example{u8R"(C:\data\résumé.doc)"};
  const auto uri{sourcemeta::core::URI::from_path(example)};
  EXPECT_EQ(uri.recompose(), "file:///C:/data/r%C3%A9sum%C3%A9.doc");
}

TEST(URI_from_path, unix_relative_simple) {
  const std::filesystem::path example{"foo/bar/baz"};
  EXPECT_THROW(sourcemeta::core::URI::from_path(example),
               sourcemeta::core::URIError);
}

TEST(URI_from_path, unix_relative_with_dot) {
  const std::filesystem::path example{"./foo/bar"};
  EXPECT_THROW(sourcemeta::core::URI::from_path(example),
               sourcemeta::core::URIError);
}

TEST(URI_from_path, unix_relative_with_dotdot) {
  const std::filesystem::path example{"../parent/dir"};
  EXPECT_THROW(sourcemeta::core::URI::from_path(example),
               sourcemeta::core::URIError);
}

TEST(URI_from_path, unix_empty_path) {
  const std::filesystem::path example{""};
  EXPECT_THROW(sourcemeta::core::URI::from_path(example),
               sourcemeta::core::URIError);
}

TEST(URI_from_path, windows_relative_simple) {
  const std::filesystem::path example{"folder\\file.txt"};
  EXPECT_THROW(sourcemeta::core::URI::from_path(example),
               sourcemeta::core::URIError);
}

TEST(URI_from_path, windows_relative_with_dot) {
  const std::filesystem::path example{".\\foo\\bar"};
  EXPECT_THROW(sourcemeta::core::URI::from_path(example),
               sourcemeta::core::URIError);
}

TEST(URI_from_path, windows_relative_with_dotdot) {
  const std::filesystem::path example{"..\\up\\one\\level"};
  EXPECT_THROW(sourcemeta::core::URI::from_path(example),
               sourcemeta::core::URIError);
}

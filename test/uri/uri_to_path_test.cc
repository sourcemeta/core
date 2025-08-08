#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, tag) {
  const sourcemeta::core::URI uri{"tag:yaml.org,2002:int"};
  const std::filesystem::path expected{"yaml.org,2002:int"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  const std::filesystem::path expected{"example:schema"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, https_url_without_path) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  const std::filesystem::path expected{""};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, https_url_with_path) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo/bar"};
  const std::filesystem::path expected{"/foo/bar"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, relative) {
  const sourcemeta::core::URI uri{"../foo"};
  const std::filesystem::path expected{"../foo"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const std::filesystem::path expected{"/foo/bar/baz"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, unix_with_space_and_reserved) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  const std::filesystem::path expected{"/foo/My Folder/has#hash?value%"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  const std::filesystem::path expected{"/foo/bar/"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const std::filesystem::path expected{R"(C:\Program Files\Test)"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const std::filesystem::path expected{R"(c:\temp\logs)"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const std::filesystem::path expected{R"(D:\)"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const std::filesystem::path expected{R"(C:\foo\bar\)"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const std::filesystem::path expected{R"(C:\path\50%+plus.txt)"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const std::filesystem::path expected{u8"/data/éclair.txt"};
  EXPECT_EQ(uri.to_path(), expected);
}

TEST(URI_to_path, unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const std::filesystem::path expected{u8R"(C:\data\résumé.doc)"};
  EXPECT_EQ(uri.to_path(), expected);
}

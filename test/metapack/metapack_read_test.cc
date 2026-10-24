#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/metapack.h>
#include <sourcemeta/core/test.h>

#include <array>      // std::array
#include <chrono>     // std::chrono
#include <cstdint>    // std::uint8_t
#include <filesystem> // std::filesystem
#include <span>       // std::span
#include <string>     // std::string

TEST(read_json_identity_from_a_path) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::core::metapack_write_json(
      path, document, "application/json",
      sourcemeta::core::MetapackEncoding::Identity);

  const auto result{sourcemeta::core::metapack_read_json(path).value()};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("hello").to_string(), "world");
}

TEST(read_json_gzip_from_a_path) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("foo", sourcemeta::core::JSON{42});

  sourcemeta::core::metapack_write_json(
      path, document, "application/json",
      sourcemeta::core::MetapackEncoding::GZIP);

  const auto result{sourcemeta::core::metapack_read_json(path).value()};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at("foo").is_integer());
  EXPECT_EQ(result.at("foo").to_integer(), 42);
}

TEST(read_json_pretty_from_a_path) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("key", sourcemeta::core::JSON{"value"});

  sourcemeta::core::metapack_write_json_pretty(
      path, document, "application/schema+json",
      sourcemeta::core::MetapackEncoding::GZIP);

  const auto result{sourcemeta::core::metapack_read_json(path).value()};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("key").to_string(), "value");
}

TEST(read_json_from_a_view) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::core::metapack_write_json(
      path, document, "application/json",
      sourcemeta::core::MetapackEncoding::GZIP);

  const sourcemeta::core::FileView view{path};
  const auto result{sourcemeta::core::metapack_read_json(view).value()};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("hello").to_string(), "world");
}

TEST(read_json_from_a_parsed_file) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::core::metapack_write_json(
      path, document, "application/json",
      sourcemeta::core::MetapackEncoding::Identity);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  const auto result{sourcemeta::core::metapack_read_json(parsed).value()};
  EXPECT_TRUE(result.is_object());
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at("hello").to_string(), "world");
}

TEST(read_json_no_value_when_the_path_does_not_exist) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  EXPECT_FALSE(sourcemeta::core::metapack_read_json(directory.path() /
                                                    "missing.metapack")
                   .has_value());
}

TEST(read_json_no_value_when_the_file_has_no_header) {
  const std::array<std::uint8_t, 2> bytes{{0x7B, 0x7D}};
  const sourcemeta::core::MetapackView parsed{
      .header = nullptr,
      .media_type = {},
      .extension = {},
      .payload = std::span<const std::uint8_t>{bytes}};

  EXPECT_FALSE(sourcemeta::core::metapack_read_json(parsed).has_value());
}

TEST(read_json_no_value_when_the_contents_are_empty) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "empty.metapack"};

  sourcemeta::core::metapack_write_bytes(
      path, {}, "application/json",
      sourcemeta::core::MetapackEncoding::Identity);

  EXPECT_FALSE(sourcemeta::core::metapack_read_json(path).has_value());
}

TEST(checksum_hex_renders_every_digest_byte) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "greeting.metapack"};

  sourcemeta::core::metapack_write_text(
      path, "Hello World", "text/plain",
      sourcemeta::core::MetapackEncoding::Identity);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  const auto checksum{sourcemeta::core::metapack_checksum_hex(*parsed.header)};

  EXPECT_EQ(checksum.size(), 64);
  // The SHA-256 of `Hello World` followed by a line feed
  EXPECT_EQ(std::string(checksum.data(), checksum.size()),
            "d2a84f4b8b650937ec8f73cd8be2c74add5a911ba64df27458ed8229da804a26");
}

TEST(last_modified_falls_between_the_writes_around_it) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "greeting.metapack"};

  const auto before{std::chrono::system_clock::now()};
  sourcemeta::core::metapack_write_text(
      path, "Hello World", "text/plain",
      sourcemeta::core::MetapackEncoding::Identity);
  const auto after{std::chrono::system_clock::now()};

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  const auto last_modified{
      sourcemeta::core::metapack_last_modified(*parsed.header)};

  EXPECT_GE(last_modified, before);
  EXPECT_LE(last_modified, after);
}

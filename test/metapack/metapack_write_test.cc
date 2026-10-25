#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/metapack.h>
#include <sourcemeta/core/test.h>

#include <algorithm>   // std::ranges::equal
#include <array>       // std::array
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

static auto payload_as_text(const sourcemeta::core::MetapackView &parsed)
    -> std::string_view {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return std::string_view{reinterpret_cast<const char *>(parsed.payload.data()),
                          parsed.payload.size()};
}

TEST(write_json_identity) {
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
  // The magic number spells META when read as the four bytes it occupies
  EXPECT_EQ(parsed.header->magic, 0x4154454D);
  EXPECT_EQ(parsed.header->format_version, 1);
  EXPECT_EQ(parsed.header->encoding,
            sourcemeta::core::MetapackEncoding::Identity);
  EXPECT_EQ(parsed.header->reserved, 0);
  EXPECT_EQ(parsed.media_type, "application/json");
  EXPECT_TRUE(parsed.extension.empty());
  EXPECT_EQ(payload_as_text(parsed), "{\"hello\":\"world\"}");
  EXPECT_EQ(parsed.header->content_bytes, parsed.payload.size());
}

TEST(write_json_gzip) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{std::string(4096, 'x')});

  sourcemeta::core::metapack_write_json(
      path, document, "application/json",
      sourcemeta::core::MetapackEncoding::GZIP);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.header->encoding, sourcemeta::core::MetapackEncoding::GZIP);
  EXPECT_EQ(parsed.media_type, "application/json");
  EXPECT_EQ(parsed.header->content_bytes, 4108);
  EXPECT_LT(parsed.payload.size(), parsed.header->content_bytes);
}

TEST(write_json_pretty_identity) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::core::metapack_write_json_pretty(
      path, document, "application/schema+json",
      sourcemeta::core::MetapackEncoding::Identity);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.media_type, "application/schema+json");
  EXPECT_EQ(payload_as_text(parsed), "{\n  \"hello\": \"world\"\n}");
}

TEST(write_text_appends_a_trailing_line_feed) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "greeting.metapack"};

  sourcemeta::core::metapack_write_text(
      path, "Hello World", "text/plain",
      sourcemeta::core::MetapackEncoding::Identity);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.media_type, "text/plain");
  EXPECT_EQ(parsed.header->encoding,
            sourcemeta::core::MetapackEncoding::Identity);
  EXPECT_TRUE(parsed.extension.empty());
  EXPECT_EQ(payload_as_text(parsed), "Hello World\n");
  EXPECT_EQ(parsed.header->content_bytes, 12);
}

TEST(write_text_gzip_records_the_decoded_size) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "greeting.metapack"};
  const std::string contents(4096, 'a');

  sourcemeta::core::metapack_write_text(
      path, contents, "text/plain", sourcemeta::core::MetapackEncoding::GZIP);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.media_type, "text/plain");
  EXPECT_EQ(parsed.header->encoding, sourcemeta::core::MetapackEncoding::GZIP);
  EXPECT_EQ(parsed.header->content_bytes, 4097);
  EXPECT_LT(parsed.payload.size(), parsed.header->content_bytes);
}

TEST(write_bytes_identity_keeps_the_contents_exact) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "blob.metapack"};
  const std::array<std::uint8_t, 5> contents{{0x00, 0x01, 0xFF, 0x0A, 0x00}};

  sourcemeta::core::metapack_write_bytes(
      path, contents, "application/octet-stream",
      sourcemeta::core::MetapackEncoding::Identity);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.media_type, "application/octet-stream");
  EXPECT_EQ(parsed.header->encoding,
            sourcemeta::core::MetapackEncoding::Identity);
  EXPECT_TRUE(parsed.extension.empty());
  EXPECT_EQ(parsed.payload.size(), 5);
  EXPECT_EQ(parsed.header->content_bytes, 5);
  EXPECT_TRUE(std::ranges::equal(parsed.payload, contents));
}

TEST(write_bytes_gzip_records_the_decoded_size) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "blob.metapack"};
  const std::vector<std::uint8_t> contents(4096, 0x41);

  sourcemeta::core::metapack_write_bytes(
      path, contents, "application/octet-stream",
      sourcemeta::core::MetapackEncoding::GZIP);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.media_type, "application/octet-stream");
  EXPECT_EQ(parsed.header->encoding, sourcemeta::core::MetapackEncoding::GZIP);
  EXPECT_EQ(parsed.header->content_bytes, 4096);
  EXPECT_LT(parsed.payload.size(), parsed.header->content_bytes);
}

TEST(write_file_stores_the_source_contents) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto source{directory.path() / "page.html"};
  const auto path{directory.path() / "page.metapack"};
  sourcemeta::core::write_file(source, std::string_view{"<p>Hello</p>"});

  sourcemeta::core::metapack_write_file(
      path, source, "text/html", sourcemeta::core::MetapackEncoding::Identity);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.media_type, "text/html");
  EXPECT_EQ(payload_as_text(parsed), "<p>Hello</p>");
  EXPECT_EQ(parsed.header->content_bytes, 12);
}

TEST(write_json_records_the_checksum_of_the_decoded_contents) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto identity{directory.path() / "identity.metapack"};
  const auto compressed{directory.path() / "compressed.metapack"};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{"world"});

  sourcemeta::core::metapack_write_json(
      identity, document, "application/json",
      sourcemeta::core::MetapackEncoding::Identity);
  sourcemeta::core::metapack_write_json(
      compressed, document, "application/json",
      sourcemeta::core::MetapackEncoding::GZIP);

  const sourcemeta::core::FileView identity_view{identity};
  const sourcemeta::core::FileView compressed_view{compressed};
  const auto parsed_identity{
      sourcemeta::core::metapack_parse(identity_view).value()};
  const auto parsed_compressed{
      sourcemeta::core::metapack_parse(compressed_view).value()};

  const auto identity_checksum{
      sourcemeta::core::metapack_checksum_hex(*parsed_identity.header)};
  const auto compressed_checksum{
      sourcemeta::core::metapack_checksum_hex(*parsed_compressed.header)};

  // The SHA-256 of `{"hello":"world"}`
  EXPECT_EQ(
      std::string_view(identity_checksum.data(), identity_checksum.size()),
      "93a23971a914e5eacbf0a8d25154cda309c3c1c72fbb9914d47c60f3cb681588");
  EXPECT_EQ(
      std::string_view(compressed_checksum.data(), compressed_checksum.size()),
      "93a23971a914e5eacbf0a8d25154cda309c3c1c72fbb9914d47c60f3cb681588");
}

TEST(write_file_gzip_records_the_decoded_size) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto source{directory.path() / "page.html"};
  const auto path{directory.path() / "page.metapack"};
  const std::string contents(4096, 'z');
  sourcemeta::core::write_file(source, contents);

  sourcemeta::core::metapack_write_file(
      path, source, "text/html", sourcemeta::core::MetapackEncoding::GZIP);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.media_type, "text/html");
  EXPECT_EQ(parsed.header->encoding, sourcemeta::core::MetapackEncoding::GZIP);
  EXPECT_EQ(parsed.header->content_bytes, 4096);
  EXPECT_LT(parsed.payload.size(), parsed.header->content_bytes);
}

TEST(write_refuses_a_media_type_the_header_cannot_record) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  const auto document{sourcemeta::core::JSON::make_object()};
  const std::string media_type(
      sourcemeta::core::METAPACK_MAXIMUM_MEDIA_TYPE_BYTES + 1, 'a');

  try {
    sourcemeta::core::metapack_write_json(
        path, document, media_type,
        sourcemeta::core::MetapackEncoding::Identity);
    FAIL();
  } catch (const sourcemeta::core::MetapackError &error) {
    EXPECT_STREQ(error.what(), "The media type is too long for this format");
  }

  EXPECT_FALSE(std::filesystem::exists(path));
}

TEST(write_refuses_an_encoding_this_revision_lacks) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  const auto document{sourcemeta::core::JSON::make_object()};

  try {
    sourcemeta::core::metapack_write_json(
        path, document, "application/json",
        static_cast<sourcemeta::core::MetapackEncoding>(7));
    FAIL();
  } catch (const sourcemeta::core::MetapackError &error) {
    EXPECT_STREQ(error.what(), "Unknown encoding");
  }

  EXPECT_FALSE(std::filesystem::exists(path));
}

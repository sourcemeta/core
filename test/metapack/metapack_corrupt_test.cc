#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/metapack.h>
#include <sourcemeta/core/test.h>

#include <cstddef>    // offsetof, std::size_t
#include <cstdint>    // std::uint16_t, std::uint64_t
#include <filesystem> // std::filesystem
#include <fstream>    // std::fstream, std::ofstream
#include <ios>        // std::ios, std::streamoff
#include <string>     // std::string

namespace {

class MetapackCorruption {
protected:
  // Overwrite the trailing bytes so that compressed contents fail to inflate
  static auto corrupt_last_bytes(const std::filesystem::path &path,
                                 const std::size_t count) -> void {
    const auto size{std::filesystem::file_size(path)};
    std::fstream file{path, std::ios::binary | std::ios::in | std::ios::out};
    EXPECT_TRUE(file.good());
    file.seekp(static_cast<std::streamoff>(size - count), std::ios::beg);
    const std::string replacement(count, static_cast<char>(0xFF));
    file.write(replacement.data(), static_cast<std::streamsize>(count));
  }

  // Rewrite the decoded size the header advertises
  static auto set_content_bytes(const std::filesystem::path &path,
                                const std::uint64_t value) -> void {
    std::fstream file{path, std::ios::binary | std::ios::in | std::ios::out};
    EXPECT_TRUE(file.good());
    file.seekp(offsetof(sourcemeta::core::MetapackHeader, content_bytes),
               std::ios::beg);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    file.write(reinterpret_cast<const char *>(&value), sizeof(value));
  }

  // Rewrite the media type length the header advertises
  static auto set_media_type_length(const std::filesystem::path &path,
                                    const std::uint16_t value) -> void {
    std::fstream file{path, std::ios::binary | std::ios::in | std::ios::out};
    EXPECT_TRUE(file.good());
    file.seekp(offsetof(sourcemeta::core::MetapackHeader, media_type_length),
               std::ios::beg);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    file.write(reinterpret_cast<const char *>(&value), sizeof(value));
  }

  // Rewrite the revision of the format the header declares
  static auto set_format_version(const std::filesystem::path &path,
                                 const std::uint16_t value) -> void {
    std::fstream file{path, std::ios::binary | std::ios::in | std::ios::out};
    EXPECT_TRUE(file.good());
    file.seekp(offsetof(sourcemeta::core::MetapackHeader, format_version),
               std::ios::beg);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    file.write(reinterpret_cast<const char *>(&value), sizeof(value));
  }

  // Rewrite the encoding the header declares
  static auto set_encoding(const std::filesystem::path &path,
                           const std::uint8_t value) -> void {
    std::fstream file{path, std::ios::binary | std::ios::in | std::ios::out};
    EXPECT_TRUE(file.good());
    file.seekp(offsetof(sourcemeta::core::MetapackHeader, encoding),
               std::ios::beg);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    file.write(reinterpret_cast<const char *>(&value), sizeof(value));
  }

  // Rewrite the extension length, which the media type decides the position of
  static auto set_extension_size(const std::filesystem::path &path,
                                 const std::size_t media_type_length,
                                 const std::uint32_t value) -> void {
    std::fstream file{path, std::ios::binary | std::ios::in | std::ios::out};
    EXPECT_TRUE(file.good());
    const auto offset{sizeof(sourcemeta::core::MetapackHeader) +
                      media_type_length};
    file.seekp(static_cast<std::streamoff>(offset), std::ios::beg);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    file.write(reinterpret_cast<const char *>(&value), sizeof(value));
  }

  // The mapping is released before returning, as Windows refuses to open a
  // mapped file for writing and every caller goes on to rewrite the header
  static auto payload_size_of(const std::filesystem::path &path)
      -> std::size_t {
    const sourcemeta::core::FileView view{path};
    return sourcemeta::core::metapack_parse(view).value().payload.size();
  }

  static auto write_document(const std::filesystem::path &path,
                             const sourcemeta::core::MetapackEncoding encoding)
      -> void {
    auto document{sourcemeta::core::JSON::make_object()};
    document.assign("hello", sourcemeta::core::JSON{"world"});
    sourcemeta::core::metapack_write_json(path, document, "application/json",
                                          encoding);
  }
};

} // namespace

TEST_F(MetapackCorruption, parse_no_value_on_an_unknown_format_version) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  write_document(path, sourcemeta::core::MetapackEncoding::Identity);

  // A revision that this implementation does not speak
  set_format_version(path, 2);

  const sourcemeta::core::FileView view{path};
  EXPECT_FALSE(sourcemeta::core::metapack_parse(view).has_value());
}

TEST_F(MetapackCorruption,
       parse_no_value_when_the_media_type_runs_past_the_end) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  write_document(path, sourcemeta::core::MetapackEncoding::Identity);

  set_media_type_length(path, 4096);

  const sourcemeta::core::FileView view{path};
  EXPECT_FALSE(sourcemeta::core::metapack_parse(view).has_value());
}

TEST_F(MetapackCorruption, read_json_no_value_when_content_bytes_overstates) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  write_document(path, sourcemeta::core::MetapackEncoding::Identity);

  set_content_bytes(path, 999999);

  EXPECT_FALSE(sourcemeta::core::metapack_read_json(path).has_value());
}

TEST_F(MetapackCorruption, read_json_no_value_on_a_damaged_gzip_trailer) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  write_document(path, sourcemeta::core::MetapackEncoding::GZIP);

  corrupt_last_bytes(path, 4);

  EXPECT_FALSE(sourcemeta::core::metapack_read_json(path).has_value());
}

TEST_F(MetapackCorruption, read_json_no_value_on_contents_that_are_not_json) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};

  sourcemeta::core::metapack_write_text(
      path, "this is not json {{{", "text/plain",
      sourcemeta::core::MetapackEncoding::Identity);

  EXPECT_FALSE(sourcemeta::core::metapack_read_json(path).has_value());
}

TEST_F(MetapackCorruption, read_json_no_value_when_the_decoded_size_disagrees) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  write_document(path, sourcemeta::core::MetapackEncoding::GZIP);

  // A small decoded size survives the ratio guard but no longer matches what
  // the contents actually inflate to
  set_content_bytes(path, 2);

  EXPECT_FALSE(sourcemeta::core::metapack_read_json(path).has_value());
}

TEST_F(MetapackCorruption, read_json_no_value_past_the_decompression_ratio) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{std::string(4096, 'a')});

  sourcemeta::core::metapack_write_json(
      path, document, "application/json",
      sourcemeta::core::MetapackEncoding::GZIP);

  EXPECT_TRUE(sourcemeta::core::metapack_read_json(path).has_value());
  EXPECT_FALSE(sourcemeta::core::metapack_read_json(path, 1).has_value());
}

TEST_F(MetapackCorruption, read_json_no_value_when_content_bytes_is_enormous) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  write_document(path, sourcemeta::core::MetapackEncoding::GZIP);

  set_content_bytes(path, 1099511627776);

  EXPECT_FALSE(sourcemeta::core::metapack_read_json(path).has_value());
}

TEST_F(MetapackCorruption, parse_no_value_on_an_encoding_this_revision_lacks) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  write_document(path, sourcemeta::core::MetapackEncoding::Identity);

  // Were this read as contents stored the way they are, the file would parse
  // and hand back a payload that nothing knows how to interpret
  set_encoding(path, 7);

  const sourcemeta::core::FileView view{path};
  EXPECT_FALSE(sourcemeta::core::metapack_parse(view).has_value());
  EXPECT_FALSE(sourcemeta::core::metapack_read_json(path).has_value());
}

TEST_F(MetapackCorruption,
       parse_no_value_when_the_extension_runs_past_the_end) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  write_document(path, sourcemeta::core::MetapackEncoding::Identity);

  set_extension_size(path, std::string_view{"application/json"}.size(), 4096);

  const sourcemeta::core::FileView view{path};
  EXPECT_FALSE(sourcemeta::core::metapack_parse(view).has_value());
}

TEST_F(MetapackCorruption, read_json_no_value_when_content_bytes_understates) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};

  sourcemeta::core::metapack_write_text(
      path, "12345", "application/json",
      sourcemeta::core::MetapackEncoding::Identity);

  // Were the declared size taken as a prefix of the contents, this would
  // parse as the number one rather than being refused
  set_content_bytes(path, 1);

  EXPECT_FALSE(sourcemeta::core::metapack_read_json(path).has_value());
}

TEST_F(MetapackCorruption, read_json_no_value_on_contents_appended_to) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  write_document(path, sourcemeta::core::MetapackEncoding::Identity);

  std::ofstream file{path, std::ios::binary | std::ios::app};
  EXPECT_TRUE(file.good());
  file << "trailing";
  file.close();

  EXPECT_FALSE(sourcemeta::core::metapack_read_json(path).has_value());
}

TEST_F(MetapackCorruption,
       read_json_no_value_on_a_ratio_that_lands_over_by_one) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  write_document(path, sourcemeta::core::MetapackEncoding::GZIP);

  const auto payload_size{payload_size_of(path)};

  // One byte beyond what a ratio of two allows, which integer division alone
  // would floor back into range
  set_content_bytes(path, (payload_size * 2) + 1);

  EXPECT_FALSE(sourcemeta::core::metapack_read_json(path, 2).has_value());
}

TEST_F(MetapackCorruption, read_json_no_value_when_contents_inflate_short) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  write_document(path, sourcemeta::core::MetapackEncoding::GZIP);

  // Large enough to overstate what the contents inflate to, yet small enough
  // that the expansion it declares is still a plausible one
  set_content_bytes(path, 100);

  EXPECT_FALSE(sourcemeta::core::metapack_read_json(path).has_value());
}

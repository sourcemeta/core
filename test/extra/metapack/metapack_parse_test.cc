#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/metapack.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <cstdint>     // std::int32_t, std::uint8_t, std::uint16_t
#include <filesystem>  // std::filesystem
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

#pragma pack(push, 1)
struct TestExtension {
  std::int32_t value;
  std::uint16_t name_length;
};
#pragma pack(pop)

static auto extension_bytes() -> std::vector<std::uint8_t> {
  TestExtension header{};
  header.value = 42;
  header.name_length = 5;

  std::vector<std::uint8_t> result;
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *begin{reinterpret_cast<const std::uint8_t *>(&header)};
  result.insert(result.end(), begin, begin + sizeof(TestExtension));
  result.push_back('h');
  result.push_back('e');
  result.push_back('l');
  result.push_back('l');
  result.push_back('o');
  return result;
}

TEST(parse_no_value_on_an_empty_file) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "empty.bin"};
  sourcemeta::core::write_file(path, std::string_view{""});

  const sourcemeta::core::FileView view{path};
  EXPECT_FALSE(sourcemeta::core::metapack_parse(view).has_value());
}

TEST(parse_no_value_on_a_file_shorter_than_a_header) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "short.bin"};
  sourcemeta::core::write_file(path, std::string_view{"META"});

  const sourcemeta::core::FileView view{path};
  EXPECT_FALSE(sourcemeta::core::metapack_parse(view).has_value());
}

TEST(parse_no_value_on_a_foreign_magic_number) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "foreign.bin"};
  const std::string contents(128, '\0');
  sourcemeta::core::write_file(path, contents);

  const sourcemeta::core::FileView view{path};
  EXPECT_FALSE(sourcemeta::core::metapack_parse(view).has_value());
}

TEST(parse_without_an_extension) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  const auto document{sourcemeta::core::JSON::make_object()};

  sourcemeta::core::metapack_write_json(
      path, document, "application/json",
      sourcemeta::core::MetapackEncoding::Identity);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.media_type, "application/json");
  EXPECT_TRUE(parsed.extension.empty());
  EXPECT_EQ(parsed.payload.size(), 2);
}

TEST(parse_with_an_empty_media_type) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  const auto document{sourcemeta::core::JSON::make_object()};

  sourcemeta::core::metapack_write_json(
      path, document, "", sourcemeta::core::MetapackEncoding::Identity);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.header->media_type_length, 0);
  EXPECT_TRUE(parsed.media_type.empty());
  EXPECT_TRUE(parsed.extension.empty());
  EXPECT_EQ(parsed.payload.size(), 2);
}

TEST(parse_with_an_extension) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  const auto document{sourcemeta::core::JSON::make_object()};
  const auto extension{extension_bytes()};

  sourcemeta::core::metapack_write_json(
      path, document, "application/json",
      sourcemeta::core::MetapackEncoding::Identity, extension);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.extension.size(), 11);

  const auto *read_extension{
      sourcemeta::core::metapack_extension_as<TestExtension>(parsed.extension)};
  EXPECT_NE(read_extension, nullptr);
  EXPECT_EQ(read_extension->value, 42);
  EXPECT_EQ(read_extension->name_length, 5);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *name{reinterpret_cast<const char *>(parsed.extension.data() +
                                                  sizeof(TestExtension))};
  EXPECT_EQ(std::string_view(name, read_extension->name_length), "hello");
  EXPECT_EQ(parsed.payload.size(), 2);
}

TEST(extension_as_no_value_when_it_is_too_small) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  const auto document{sourcemeta::core::JSON::make_object()};
  const std::array<std::uint8_t, 2> extension{{0x01, 0x02}};

  sourcemeta::core::metapack_write_json(
      path, document, "application/json",
      sourcemeta::core::MetapackEncoding::Identity, extension);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.extension.size(), 2);
  EXPECT_EQ(
      sourcemeta::core::metapack_extension_as<TestExtension>(parsed.extension),
      nullptr);
}

TEST(parse_with_an_extension_and_compressed_contents) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("hello", sourcemeta::core::JSON{std::string(4096, 'x')});
  const auto extension{extension_bytes()};

  sourcemeta::core::metapack_write_json(
      path, document, "application/json",
      sourcemeta::core::MetapackEncoding::GZIP, extension);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.media_type, "application/json");
  EXPECT_EQ(parsed.header->encoding, sourcemeta::core::MetapackEncoding::GZIP);
  EXPECT_EQ(parsed.extension.size(), 11);
  EXPECT_LT(parsed.payload.size(), parsed.header->content_bytes);

  const auto *read_extension{
      sourcemeta::core::metapack_extension_as<TestExtension>(parsed.extension)};
  EXPECT_NE(read_extension, nullptr);
  EXPECT_EQ(read_extension->value, 42);

  const auto result{sourcemeta::core::metapack_read_json(parsed).value()};
  EXPECT_EQ(result.at("hello").to_string(), std::string(4096, 'x'));
}

TEST(parse_with_a_long_media_type) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "document.metapack"};
  const auto document{sourcemeta::core::JSON::make_object()};
  const std::string media_type{"application/" + std::string(1000, 'a')};
  const auto extension{extension_bytes()};

  sourcemeta::core::metapack_write_json(
      path, document, media_type, sourcemeta::core::MetapackEncoding::Identity,
      extension);

  const sourcemeta::core::FileView view{path};
  const auto parsed{sourcemeta::core::metapack_parse(view).value()};
  EXPECT_EQ(parsed.header->media_type_length, 1012);
  EXPECT_EQ(parsed.media_type, media_type);
  EXPECT_EQ(parsed.extension.size(), 11);
  EXPECT_EQ(parsed.payload.size(), 2);
}

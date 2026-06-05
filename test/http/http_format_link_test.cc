#include <gtest/gtest.h>

#include <array>
#include <string>
#include <utility>

#include <sourcemeta/core/http.h>

TEST(HTTP_format_link, single_rel_describedby) {
  const auto value{sourcemeta::core::http_format_link(
      {.target = "https://example.com/schema.json", .rel = "describedby"})};
  EXPECT_EQ(value, "<https://example.com/schema.json>; rel=\"describedby\"");
}

TEST(HTTP_format_link, alternative_rel) {
  const auto value{sourcemeta::core::http_format_link(
      {.target = "/style.css", .rel = "stylesheet"})};
  EXPECT_EQ(value, "</style.css>; rel=\"stylesheet\"");
}

TEST(HTTP_format_link, single_parameter) {
  const std::array<std::pair<std::string_view, std::string_view>, 1> parameters{
      {{"title", "Example Title"}}};
  const auto value{sourcemeta::core::http_format_link(
      {.target = "/page", .rel = "next", .parameters = parameters})};
  EXPECT_EQ(value, "</page>; rel=\"next\"; title=\"Example Title\"");
}

TEST(HTTP_format_link, multiple_parameters) {
  const std::array<std::pair<std::string_view, std::string_view>, 2> parameters{
      {{"title", "Doc"}, {"hreflang", "en"}}};
  const auto value{sourcemeta::core::http_format_link(
      {.target = "/x", .rel = "alternate", .parameters = parameters})};
  EXPECT_EQ(value, "</x>; rel=\"alternate\"; title=\"Doc\"; hreflang=\"en\"");
}

TEST(HTTP_format_link, sink_overload_appends) {
  std::string buffer{"prefix:"};
  sourcemeta::core::http_format_link({.target = "/a", .rel = "self"}, buffer);
  EXPECT_EQ(buffer, "prefix:</a>; rel=\"self\"");
}

TEST(HTTP_format_link, sink_overload_starts_empty) {
  std::string buffer;
  sourcemeta::core::http_format_link({.target = "/a", .rel = "self"}, buffer);
  EXPECT_EQ(buffer, "</a>; rel=\"self\"");
}

TEST(HTTP_format_link, empty_target_is_taken_verbatim) {
  const auto value{
      sourcemeta::core::http_format_link({.target = "", .rel = "self"})};
  EXPECT_EQ(value, "<>; rel=\"self\"");
}

TEST(HTTP_format_links, single_entry) {
  const std::array<sourcemeta::core::HTTPLink, 1> links{
      {{.target = "/a", .rel = "self"}}};
  const auto value{sourcemeta::core::http_format_links(links)};
  EXPECT_EQ(value, "</a>; rel=\"self\"");
}

TEST(HTTP_format_links, two_entries) {
  const std::array<sourcemeta::core::HTTPLink, 2> links{
      {{.target = "/a", .rel = "self"}, {.target = "/b", .rel = "next"}}};
  const auto value{sourcemeta::core::http_format_links(links)};
  EXPECT_EQ(value, "</a>; rel=\"self\", </b>; rel=\"next\"");
}

TEST(HTTP_format_links, three_entries) {
  const std::array<sourcemeta::core::HTTPLink, 3> links{
      {{.target = "/a", .rel = "self"},
       {.target = "/b", .rel = "next"},
       {.target = "/c", .rel = "prev"}}};
  const auto value{sourcemeta::core::http_format_links(links)};
  EXPECT_EQ(value,
            "</a>; rel=\"self\", </b>; rel=\"next\", </c>; rel=\"prev\"");
}

TEST(HTTP_format_links, sink_overload_appends) {
  std::string buffer{"X-"};
  const std::array<sourcemeta::core::HTTPLink, 1> links{
      {{.target = "/a", .rel = "self"}}};
  sourcemeta::core::http_format_links(links, buffer);
  EXPECT_EQ(buffer, "X-</a>; rel=\"self\"");
}

TEST(HTTP_format_links, empty_span_produces_empty_output) {
  const std::array<sourcemeta::core::HTTPLink, 0> links{};
  const auto value{sourcemeta::core::http_format_links(links)};
  EXPECT_EQ(value, "");
}

TEST(HTTP_format_links, mixed_with_parameters) {
  const std::array<std::pair<std::string_view, std::string_view>, 1>
      first_parameters{{{"title", "Schema"}}};
  const std::array<sourcemeta::core::HTTPLink, 2> links{
      {{.target = "/schema.json",
        .rel = "describedby",
        .parameters = first_parameters},
       {.target = "/profile", .rel = "profile"}}};
  const auto value{sourcemeta::core::http_format_links(links)};
  EXPECT_EQ(value, "</schema.json>; rel=\"describedby\"; title=\"Schema\", "
                   "</profile>; rel=\"profile\"");
}

TEST(HTTP_format_link, sink_can_be_reused_across_calls) {
  std::string buffer;
  sourcemeta::core::http_format_link({.target = "/a", .rel = "self"}, buffer);
  buffer.append("|");
  sourcemeta::core::http_format_link({.target = "/b", .rel = "next"}, buffer);
  EXPECT_EQ(buffer, "</a>; rel=\"self\"|</b>; rel=\"next\"");
}

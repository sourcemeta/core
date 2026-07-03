#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

TEST(headers_empty_by_default) {
  const sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  EXPECT_TRUE(request.headers().empty());
}

TEST(headers_returns_added_headers_in_order) {
  sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  request.header("Accept", "application/json");
  request.header("X-Custom", "value");
  EXPECT_EQ(request.headers().size(), 2);
  EXPECT_EQ(request.headers().at(0).first, "Accept");
  EXPECT_EQ(request.headers().at(0).second, "application/json");
  EXPECT_EQ(request.headers().at(1).first, "X-Custom");
  EXPECT_EQ(request.headers().at(1).second, "value");
}

TEST(header_lookup_present) {
  sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  request.header("Accept", "application/json");
  EXPECT_TRUE(request.header("Accept").has_value());
  EXPECT_EQ(request.header("Accept").value(), "application/json");
}

TEST(header_lookup_case_insensitive) {
  sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  request.header("Content-Type", "text/plain");
  EXPECT_EQ(request.header("content-type").value(), "text/plain");
  EXPECT_EQ(request.header("CONTENT-TYPE").value(), "text/plain");
}

TEST(header_lookup_absent) {
  const sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  EXPECT_FALSE(request.header("Accept").has_value());
}

TEST(header_lookup_returns_first_of_repeated) {
  sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  request.header("X-Repeated", "first");
  request.header("X-Repeated", "second");
  EXPECT_EQ(request.header("X-Repeated").value(), "first");
}

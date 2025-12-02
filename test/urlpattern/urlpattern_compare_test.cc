#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

TEST(URLPattern_compare, protocol_equality_same_literal) {
  const sourcemeta::core::URLPatternProtocol protocol1{"https"};
  const sourcemeta::core::URLPatternProtocol protocol2{"https"};
  EXPECT_TRUE(protocol1 == protocol2);
  EXPECT_FALSE(protocol1 != protocol2);
}

TEST(URLPattern_compare, protocol_equality_different_literal) {
  const sourcemeta::core::URLPatternProtocol protocol1{"https"};
  const sourcemeta::core::URLPatternProtocol protocol2{"http"};
  EXPECT_FALSE(protocol1 == protocol2);
  EXPECT_TRUE(protocol1 != protocol2);
}

TEST(URLPattern_compare, protocol_equality_asterisk) {
  const sourcemeta::core::URLPatternProtocol protocol1{"*"};
  const sourcemeta::core::URLPatternProtocol protocol2{"*"};
  EXPECT_TRUE(protocol1 == protocol2);
  EXPECT_FALSE(protocol1 != protocol2);
}

TEST(URLPattern_compare, protocol_less_than) {
  const sourcemeta::core::URLPatternProtocol protocol1{"http"};
  const sourcemeta::core::URLPatternProtocol protocol2{"https"};
  EXPECT_TRUE(protocol1 < protocol2);
  EXPECT_FALSE(protocol2 < protocol1);
  EXPECT_TRUE(protocol1 <= protocol2);
  EXPECT_FALSE(protocol2 <= protocol1);
}

TEST(URLPattern_compare, protocol_greater_than) {
  const sourcemeta::core::URLPatternProtocol protocol1{"https"};
  const sourcemeta::core::URLPatternProtocol protocol2{"http"};
  EXPECT_TRUE(protocol1 > protocol2);
  EXPECT_FALSE(protocol2 > protocol1);
  EXPECT_TRUE(protocol1 >= protocol2);
  EXPECT_FALSE(protocol2 >= protocol1);
}

TEST(URLPattern_compare, protocol_less_or_equal_same) {
  const sourcemeta::core::URLPatternProtocol protocol1{"https"};
  const sourcemeta::core::URLPatternProtocol protocol2{"https"};
  EXPECT_TRUE(protocol1 <= protocol2);
  EXPECT_TRUE(protocol1 >= protocol2);
}

TEST(URLPattern_compare, username_equality_same) {
  const sourcemeta::core::URLPatternUsername username1{"user"};
  const sourcemeta::core::URLPatternUsername username2{"user"};
  EXPECT_TRUE(username1 == username2);
  EXPECT_FALSE(username1 != username2);
}

TEST(URLPattern_compare, username_equality_different) {
  const sourcemeta::core::URLPatternUsername username1{"user1"};
  const sourcemeta::core::URLPatternUsername username2{"user2"};
  EXPECT_FALSE(username1 == username2);
  EXPECT_TRUE(username1 != username2);
}

TEST(URLPattern_compare, password_equality_same) {
  const sourcemeta::core::URLPatternPassword password1{"pass"};
  const sourcemeta::core::URLPatternPassword password2{"pass"};
  EXPECT_TRUE(password1 == password2);
  EXPECT_FALSE(password1 != password2);
}

TEST(URLPattern_compare, password_equality_different) {
  const sourcemeta::core::URLPatternPassword password1{"pass1"};
  const sourcemeta::core::URLPatternPassword password2{"pass2"};
  EXPECT_FALSE(password1 == password2);
  EXPECT_TRUE(password1 != password2);
}

TEST(URLPattern_compare, hostname_equality_same) {
  const sourcemeta::core::URLPatternHostname hostname1{"example.com"};
  const sourcemeta::core::URLPatternHostname hostname2{"example.com"};
  EXPECT_TRUE(hostname1 == hostname2);
  EXPECT_FALSE(hostname1 != hostname2);
}

TEST(URLPattern_compare, hostname_equality_different) {
  const sourcemeta::core::URLPatternHostname hostname1{"example.com"};
  const sourcemeta::core::URLPatternHostname hostname2{"test.com"};
  EXPECT_FALSE(hostname1 == hostname2);
  EXPECT_TRUE(hostname1 != hostname2);
}

TEST(URLPattern_compare, port_equality_same) {
  const sourcemeta::core::URLPatternPort port1{"8080"};
  const sourcemeta::core::URLPatternPort port2{"8080"};
  EXPECT_TRUE(port1 == port2);
  EXPECT_FALSE(port1 != port2);
}

TEST(URLPattern_compare, port_equality_different) {
  const sourcemeta::core::URLPatternPort port1{"8080"};
  const sourcemeta::core::URLPatternPort port2{"3000"};
  EXPECT_FALSE(port1 == port2);
  EXPECT_TRUE(port1 != port2);
}

TEST(URLPattern_compare, pathname_equality_same) {
  const sourcemeta::core::URLPatternPathname pathname1{"/foo/bar"};
  const sourcemeta::core::URLPatternPathname pathname2{"/foo/bar"};
  EXPECT_TRUE(pathname1 == pathname2);
  EXPECT_FALSE(pathname1 != pathname2);
}

TEST(URLPattern_compare, pathname_equality_different) {
  const sourcemeta::core::URLPatternPathname pathname1{"/foo/bar"};
  const sourcemeta::core::URLPatternPathname pathname2{"/baz"};
  EXPECT_FALSE(pathname1 == pathname2);
  EXPECT_TRUE(pathname1 != pathname2);
}

TEST(URLPattern_compare, search_equality_same) {
  const sourcemeta::core::URLPatternSearch search1{"?q=test"};
  const sourcemeta::core::URLPatternSearch search2{"?q=test"};
  EXPECT_TRUE(search1 == search2);
  EXPECT_FALSE(search1 != search2);
}

TEST(URLPattern_compare, search_equality_different) {
  const sourcemeta::core::URLPatternSearch search1{"?q=test"};
  const sourcemeta::core::URLPatternSearch search2{"?q=other"};
  EXPECT_FALSE(search1 == search2);
  EXPECT_TRUE(search1 != search2);
}

TEST(URLPattern_compare, hash_equality_same) {
  const sourcemeta::core::URLPatternHash hash1{"#section"};
  const sourcemeta::core::URLPatternHash hash2{"#section"};
  EXPECT_TRUE(hash1 == hash2);
  EXPECT_FALSE(hash1 != hash2);
}

TEST(URLPattern_compare, hash_equality_different) {
  const sourcemeta::core::URLPatternHash hash1{"#section1"};
  const sourcemeta::core::URLPatternHash hash2{"#section2"};
  EXPECT_FALSE(hash1 == hash2);
  EXPECT_TRUE(hash1 != hash2);
}

TEST(URLPattern_compare, protocol_char_vs_name) {
  const sourcemeta::core::URLPatternProtocol protocol1{"https"};
  const sourcemeta::core::URLPatternProtocol protocol2{":protocol"};
  EXPECT_FALSE(protocol1 == protocol2);
  EXPECT_TRUE(protocol1 != protocol2);
  EXPECT_TRUE(protocol2 < protocol1);
  EXPECT_FALSE(protocol1 < protocol2);
}

TEST(URLPattern_compare, protocol_char_vs_asterisk) {
  const sourcemeta::core::URLPatternProtocol protocol1{"https"};
  const sourcemeta::core::URLPatternProtocol protocol2{"*"};
  EXPECT_FALSE(protocol1 == protocol2);
  EXPECT_TRUE(protocol1 != protocol2);
  // WHATWG: less specific (asterisk) sorts first
  EXPECT_TRUE(protocol2 < protocol1);
  EXPECT_FALSE(protocol1 < protocol2);
}

TEST(URLPattern_compare, protocol_char_vs_regex) {
  const sourcemeta::core::URLPatternProtocol protocol1{"https"};
  const sourcemeta::core::URLPatternProtocol protocol2{"(https?)"};
  EXPECT_FALSE(protocol1 == protocol2);
  EXPECT_TRUE(protocol1 != protocol2);
  // WHATWG: less specific (regex) sorts first
  EXPECT_TRUE(protocol2 < protocol1);
  EXPECT_FALSE(protocol1 < protocol2);
}

TEST(URLPattern_compare, protocol_name_vs_asterisk) {
  const sourcemeta::core::URLPatternProtocol protocol1{":protocol"};
  const sourcemeta::core::URLPatternProtocol protocol2{"*"};
  EXPECT_FALSE(protocol1 == protocol2);
  EXPECT_TRUE(protocol1 != protocol2);
  // WHATWG: less specific (asterisk) sorts first
  EXPECT_TRUE(protocol2 < protocol1);
  EXPECT_FALSE(protocol1 < protocol2);
}

TEST(URLPattern_compare, protocol_name_vs_regex) {
  const sourcemeta::core::URLPatternProtocol protocol1{":protocol"};
  const sourcemeta::core::URLPatternProtocol protocol2{"(https?)"};
  EXPECT_FALSE(protocol1 == protocol2);
  EXPECT_TRUE(protocol1 != protocol2);
  // WHATWG: less specific (name/segment-wildcard) sorts first
  EXPECT_TRUE(protocol1 < protocol2);
  EXPECT_FALSE(protocol2 < protocol1);
}

TEST(URLPattern_compare, protocol_asterisk_vs_regex) {
  const sourcemeta::core::URLPatternProtocol protocol1{"*"};
  const sourcemeta::core::URLPatternProtocol protocol2{"(https?)"};
  EXPECT_FALSE(protocol1 == protocol2);
  EXPECT_TRUE(protocol1 != protocol2);
  // WHATWG: less specific (asterisk/full-wildcard) sorts first
  EXPECT_TRUE(protocol1 < protocol2);
  EXPECT_FALSE(protocol2 < protocol1);
}

TEST(URLPattern_compare, username_char_vs_name) {
  const sourcemeta::core::URLPatternUsername username1{"admin"};
  const sourcemeta::core::URLPatternUsername username2{":user"};
  EXPECT_FALSE(username1 == username2);
  EXPECT_TRUE(username1 != username2);
  EXPECT_TRUE(username2 < username1);
  EXPECT_FALSE(username1 < username2);
}

TEST(URLPattern_compare, username_char_vs_asterisk) {
  const sourcemeta::core::URLPatternUsername username1{"admin"};
  const sourcemeta::core::URLPatternUsername username2{"*"};
  EXPECT_FALSE(username1 == username2);
  EXPECT_TRUE(username1 != username2);
  // WHATWG: less specific (asterisk) sorts first
  EXPECT_TRUE(username2 < username1);
  EXPECT_FALSE(username1 < username2);
}

TEST(URLPattern_compare, password_char_vs_name) {
  const sourcemeta::core::URLPatternPassword password1{"secret"};
  const sourcemeta::core::URLPatternPassword password2{":pass"};
  EXPECT_FALSE(password1 == password2);
  EXPECT_TRUE(password1 != password2);
  EXPECT_TRUE(password2 < password1);
  EXPECT_FALSE(password1 < password2);
}

TEST(URLPattern_compare, password_char_vs_asterisk) {
  const sourcemeta::core::URLPatternPassword password1{"secret"};
  const sourcemeta::core::URLPatternPassword password2{"*"};
  EXPECT_FALSE(password1 == password2);
  EXPECT_TRUE(password1 != password2);
  // WHATWG: less specific (asterisk) sorts first
  EXPECT_TRUE(password2 < password1);
  EXPECT_FALSE(password1 < password2);
}

TEST(URLPattern_compare, port_char_vs_name) {
  const sourcemeta::core::URLPatternPort port1{"8080"};
  const sourcemeta::core::URLPatternPort port2{":port"};
  EXPECT_FALSE(port1 == port2);
  EXPECT_TRUE(port1 != port2);
  EXPECT_TRUE(port2 < port1);
  EXPECT_FALSE(port1 < port2);
}

TEST(URLPattern_compare, port_char_vs_asterisk) {
  const sourcemeta::core::URLPatternPort port1{"8080"};
  const sourcemeta::core::URLPatternPort port2{"*"};
  EXPECT_FALSE(port1 == port2);
  EXPECT_TRUE(port1 != port2);
  // WHATWG: less specific (asterisk) sorts first
  EXPECT_TRUE(port2 < port1);
  EXPECT_FALSE(port1 < port2);
}

TEST(URLPattern_compare, search_char_vs_name) {
  const sourcemeta::core::URLPatternSearch search1{"?q=test"};
  const sourcemeta::core::URLPatternSearch search2{":search"};
  EXPECT_FALSE(search1 == search2);
  EXPECT_TRUE(search1 != search2);
  EXPECT_TRUE(search2 < search1);
  EXPECT_FALSE(search1 < search2);
}

TEST(URLPattern_compare, search_char_vs_asterisk) {
  const sourcemeta::core::URLPatternSearch search1{"?q=test"};
  const sourcemeta::core::URLPatternSearch search2{"*"};
  EXPECT_FALSE(search1 == search2);
  EXPECT_TRUE(search1 != search2);
  // WHATWG: less specific (asterisk) sorts first
  EXPECT_TRUE(search2 < search1);
  EXPECT_FALSE(search1 < search2);
}

TEST(URLPattern_compare, hash_char_vs_name) {
  const sourcemeta::core::URLPatternHash hash1{"#section"};
  const sourcemeta::core::URLPatternHash hash2{":hash"};
  EXPECT_FALSE(hash1 == hash2);
  EXPECT_TRUE(hash1 != hash2);
  EXPECT_TRUE(hash2 < hash1);
  EXPECT_FALSE(hash1 < hash2);
}

TEST(URLPattern_compare, hash_char_vs_asterisk) {
  const sourcemeta::core::URLPatternHash hash1{"#section"};
  const sourcemeta::core::URLPatternHash hash2{"*"};
  EXPECT_FALSE(hash1 == hash2);
  EXPECT_TRUE(hash1 != hash2);
  // WHATWG: less specific (asterisk) sorts first
  EXPECT_TRUE(hash2 < hash1);
  EXPECT_FALSE(hash1 < hash2);
}

TEST(URLPattern_compare, hostname_char_vs_name) {
  const sourcemeta::core::URLPatternHostname hostname1{"example.com"};
  const sourcemeta::core::URLPatternHostname hostname2{":domain.com"};
  EXPECT_FALSE(hostname1 == hostname2);
  EXPECT_TRUE(hostname1 != hostname2);
  EXPECT_TRUE(hostname2 < hostname1);
  EXPECT_FALSE(hostname1 < hostname2);
}

TEST(URLPattern_compare, hostname_char_vs_asterisk) {
  const sourcemeta::core::URLPatternHostname hostname1{"example.com"};
  const sourcemeta::core::URLPatternHostname hostname2{"*.com"};
  EXPECT_FALSE(hostname1 == hostname2);
  EXPECT_TRUE(hostname1 != hostname2);
  // WHATWG: less specific (asterisk) sorts first
  EXPECT_TRUE(hostname2 < hostname1);
  EXPECT_FALSE(hostname1 < hostname2);
}

TEST(URLPattern_compare, pathname_char_vs_name) {
  const sourcemeta::core::URLPatternPathname pathname1{"/api/users"};
  const sourcemeta::core::URLPatternPathname pathname2{"/api/:id"};
  EXPECT_FALSE(pathname1 == pathname2);
  EXPECT_TRUE(pathname1 != pathname2);
  EXPECT_TRUE(pathname2 < pathname1);
  EXPECT_FALSE(pathname1 < pathname2);
}

TEST(URLPattern_compare, pathname_char_vs_asterisk) {
  const sourcemeta::core::URLPatternPathname pathname1{"/api/users"};
  const sourcemeta::core::URLPatternPathname pathname2{"/api/*"};
  EXPECT_FALSE(pathname1 == pathname2);
  EXPECT_TRUE(pathname1 != pathname2);
  // WHATWG: less specific (asterisk) sorts first
  EXPECT_TRUE(pathname2 < pathname1);
  EXPECT_FALSE(pathname1 < pathname2);
}

TEST(URLPattern_compare, pattern_default_constructed) {
  const sourcemeta::core::URLPattern pattern1;
  const sourcemeta::core::URLPattern pattern2;
  EXPECT_TRUE(pattern1 == pattern2);
  EXPECT_FALSE(pattern1 != pattern2);
  EXPECT_TRUE(pattern1 <= pattern2);
  EXPECT_TRUE(pattern1 >= pattern2);
  EXPECT_FALSE(pattern1 < pattern2);
  EXPECT_FALSE(pattern1 > pattern2);
}

TEST(URLPattern_compare, pattern_same_protocol) {
  const auto pattern1{
      sourcemeta::core::URLPattern::parse("https://example.com/foo")};
  const auto pattern2{
      sourcemeta::core::URLPattern::parse("https://example.com/foo")};
  EXPECT_TRUE(pattern1 == pattern2);
  EXPECT_FALSE(pattern1 != pattern2);
}

TEST(URLPattern_compare, pattern_different_protocol) {
  const auto pattern1{
      sourcemeta::core::URLPattern::parse("http://example.com/foo")};
  const auto pattern2{
      sourcemeta::core::URLPattern::parse("https://example.com/foo")};
  EXPECT_FALSE(pattern1 == pattern2);
  EXPECT_TRUE(pattern1 != pattern2);
  EXPECT_TRUE(pattern1 < pattern2);
  EXPECT_FALSE(pattern2 < pattern1);
}

TEST(URLPattern_compare, pattern_different_hostname) {
  const auto pattern1{
      sourcemeta::core::URLPattern::parse("https://aaa.com/foo")};
  const auto pattern2{
      sourcemeta::core::URLPattern::parse("https://zzz.com/foo")};
  EXPECT_FALSE(pattern1 == pattern2);
  EXPECT_TRUE(pattern1 != pattern2);
  EXPECT_TRUE(pattern1 < pattern2);
  EXPECT_FALSE(pattern2 < pattern1);
}

TEST(URLPattern_compare, pattern_different_pathname) {
  const auto pattern1{
      sourcemeta::core::URLPattern::parse("https://example.com/aaa")};
  const auto pattern2{
      sourcemeta::core::URLPattern::parse("https://example.com/zzz")};
  EXPECT_FALSE(pattern1 == pattern2);
  EXPECT_TRUE(pattern1 != pattern2);
  EXPECT_TRUE(pattern1 < pattern2);
  EXPECT_FALSE(pattern2 < pattern1);
}

TEST(URLPattern_compare, pattern_different_port) {
  const auto pattern1{
      sourcemeta::core::URLPattern::parse("https://example.com:3000/foo")};
  const auto pattern2{
      sourcemeta::core::URLPattern::parse("https://example.com:8080/foo")};
  EXPECT_FALSE(pattern1 == pattern2);
  EXPECT_TRUE(pattern1 != pattern2);
  EXPECT_TRUE(pattern1 < pattern2);
  EXPECT_FALSE(pattern2 < pattern1);
}

TEST(URLPattern_compare, pattern_protocol_takes_precedence) {
  const auto pattern1{
      sourcemeta::core::URLPattern::parse("https://aaa.com/aaa")};
  const auto pattern2{
      sourcemeta::core::URLPattern::parse("http://zzz.com/zzz")};
  EXPECT_FALSE(pattern1 == pattern2);
  EXPECT_TRUE(pattern1 != pattern2);
  EXPECT_TRUE(pattern2 < pattern1);
  EXPECT_FALSE(pattern1 < pattern2);
}

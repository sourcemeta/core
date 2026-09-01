#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/css.h>
#include <sourcemeta/core/diff.h>
#include <sourcemeta/core/dns.h>
#include <sourcemeta/core/email.h>
#include <sourcemeta/core/error.h>
#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/html.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/core/idna.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/ip.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonl.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpath.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/langtag.h>
#include <sourcemeta/core/markdown.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/memory.h>
#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/options.h>
#include <sourcemeta/core/parallel.h>
#include <sourcemeta/core/preprocessor.h>
#include <sourcemeta/core/process.h>
#include <sourcemeta/core/punycode.h>
#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/semver.h>
#include <sourcemeta/core/stacktrace.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/time.h>
#include <sourcemeta/core/unicode.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>
#include <sourcemeta/core/yaml.h>

#include <cstdint>  // std::uint8_t
#include <cstdlib>  // EXIT_SUCCESS
#include <iostream> // std::cout
#include <string>   // std::string

auto main() -> int {
  const sourcemeta::core::JSON document{"Hello World"};
  sourcemeta::core::stringify(document, std::cout);
  std::cout << std::endl;

  // Every one of these reaches a dependency that the module itself does not
  // name, which is what a consumer needs the packaging to resolve for it
  std::cout << sourcemeta::core::matches_if_valid("^Hello", "Hello World")
            << std::endl;
  std::cout << sourcemeta::core::markdown_to_html("# Hello") << std::endl;
  const std::string input{"Hello World"};
  std::cout << sourcemeta::core::gzip(
                   reinterpret_cast<const std::uint8_t *>(input.data()),
                   input.size())
                   .size()
            << std::endl;
  std::cout << sourcemeta::core::is_idn_email("hello@\xCE\xB4.example")
            << std::endl;
  std::cout << sourcemeta::core::uuidv4().size() << std::endl;

  // Reaches the Swift shim, which autolinks a runtime that lives outside the
  // paths a linker searches on its own
  const auto key{sourcemeta::core::make_eddsa_public_key(
      sourcemeta::core::EdwardsCurve::Ed25519, std::string(32, '\x01'))};
  std::cout << (key.has_value() &&
                sourcemeta::core::eddsa_verify(key.value(), "message",
                                               std::string(64, '\x00')))
            << std::endl;
  return EXIT_SUCCESS;
}

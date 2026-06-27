#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/css.h>
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
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/langtag.h>
#include <sourcemeta/core/markdown.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/options.h>
#include <sourcemeta/core/parallel.h>
#include <sourcemeta/core/preprocessor.h>
#include <sourcemeta/core/process.h>
#include <sourcemeta/core/punycode.h>
#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/semver.h>
#include <sourcemeta/core/stacktrace.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/time.h>
#include <sourcemeta/core/unicode.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uritemplate.h>
#include <sourcemeta/core/yaml.h>

#include <cstdlib>  // EXIT_SUCCESS
#include <iostream> // std::cout

auto main() -> int {
  const sourcemeta::core::JSON document{"Hello World"};
  sourcemeta::core::stringify(document, std::cout);
  std::cout << std::endl;
  return EXIT_SUCCESS;
}

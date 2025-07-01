#include <sourcemeta/core/alterschema.h>
#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonl.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/yaml.h>

#include <cstdlib>  // EXIT_SUCCESS
#include <iostream> // std::cout

auto main() -> int {
  const sourcemeta::core::JSON document{"Hello World"};
  sourcemeta::core::stringify(document, std::cout);
  std::cout << std::endl;
  return EXIT_SUCCESS;
}

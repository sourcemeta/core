#include <sourcemeta/core/alterschema.h>
#include <sourcemeta/core/build.h>
#include <sourcemeta/core/editorschema.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonl.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/md5.h>
#include <sourcemeta/core/options.h>
#include <sourcemeta/core/parallel.h>
#include <sourcemeta/core/schemaconfig.h>
#include <sourcemeta/core/time.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/uuid.h>
#include <sourcemeta/core/yaml.h>

#include <cstdlib>  // EXIT_SUCCESS
#include <iostream> // std::cout

auto main() -> int {
  const sourcemeta::core::JSON document{"Hello World"};
  sourcemeta::core::stringify(document, std::cout);
  std::cout << std::endl;
  return EXIT_SUCCESS;
}

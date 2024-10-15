![JSON Toolkit](./assets/banner.png)

JSON Toolkit is a swiss-army knife for [JSON](https://www.json.org) programming
in modern C++. It comes with a built-in parser and growing support for
standards such as [JSON Schema](http://json-schema.org), [JSON
Pointer](https://www.rfc-editor.org/rfc/rfc6901),
[JSONL](https://jsonlines.org), and more. For example:

```cpp
#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonpointer.h>

#include <sstream>
#include <iostream>
#include <stdlib>
#include <utility>

auto main() -> int {
  // Parsing a JSON document from a string
  auto document{sourcemeta::jsontoolkit::parse(R"JSON({
    "name": "John Doe",
    "age": 20,
    "address": "14 Manchester Road, Coventry CV77 1XN"
  })JSON")};

  // Updating a property using JSON Pointer
  const sourcemeta::jsontoolkit::Pointer pointer{"name"};
  sourcemeta::jsontoolkit::JSON new_value{"Jane Doe"};
  sourcemeta::jsontoolkit::set(document, pointer, std::move(new_value));

  // Pretty-print result
  sourcemeta::jsontoolkit::prettify(document, std::cout);
  std::cout << "\n";

  return EXIT_SUCCESS;
}
```

Documentation
-------------

Refer to the project website for documentation:
[https://jsontoolkit.sourcemeta.com](https://jsontoolkit.sourcemeta.com).

Commercial Use
--------------

This project is governed by the [AGPL-3.0](./LICENSE) copyleft license and our
[Contribution
Agreement](https://github.com/sourcemeta/.github/blob/main/CONTRIBUTING.md). To
ensure the sustainability of the project, you can freely make use of this
software as long the outcome is distributed under the same license. Otherwise,
you must obtain a [commercial license](./LICENSE-COMMERCIAL) that removes such
restrictions. Read more about our licensing approach
[here](https://www.sourcemeta.com/licensing/).

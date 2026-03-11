#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/yaml.h>

#include <sstream> // std::ostringstream

static auto roundtrip(const std::string &input) -> std::string {
  sourcemeta::core::YAMLRoundTrip metadata;
  const auto document{sourcemeta::core::parse_yaml(input, metadata)};
  std::ostringstream stream;
  sourcemeta::core::stringify_yaml(document, stream, metadata);
  return stream.str();
}

TEST(YAML_roundtrip, block_mapping_simple) {
  const std::string input{R"YAML(foo: bar
baz: qux
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_mapping_nested) {
  const std::string input{R"YAML(foo:
  bar: baz
  qux: 1
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_sequence_simple) {
  const std::string input{R"YAML(- one
- two
- three
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_of_mappings) {
  const std::string input{R"YAML(- foo: bar
  baz: qux
- hello: world
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mapping_with_sequence) {
  const std::string input{R"YAML(foo:
  - one
  - two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, deeply_nested) {
  const std::string input{R"YAML(a:
  b:
    c:
      d: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_of_sequences) {
  const std::string input{R"YAML(- - a
  - b
- - c
  - d
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mixed_types_in_mapping) {
  const std::string input{R"YAML(name: app
count: 3
enabled: true
null_val: null
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, kubernetes_like) {
  const std::string input{R"YAML(apiVersion: v1
kind: Service
metadata:
  name: my-service
  labels:
    app: my-app
spec:
  selector:
    app: my-app
  ports:
    - protocol: TCP
      port: 80
      targetPort: 9376
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, github_actions_like) {
  const std::string input{R"YAML(name: CI
on:
  push:
    branches:
      - main
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Build
        run: make build
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

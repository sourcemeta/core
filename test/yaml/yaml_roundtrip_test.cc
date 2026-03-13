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

TEST(YAML_roundtrip, single_quoted_value) {
  const std::string input{R"YAML(foo: 'bar'
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, double_quoted_value) {
  const std::string input{R"YAML(foo: "bar"
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mixed_scalar_styles) {
  const std::string input{R"YAML(plain: hello
single: 'world'
double: "!"
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_quoted_in_sequence) {
  const std::string input{R"YAML(- 'one'
- 'two'
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, double_quoted_in_sequence) {
  const std::string input{R"YAML(- "one"
- "two"
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, literal_block_scalar) {
  const std::string input{R"YAML(foo: |
  hello
  world
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, literal_block_strip) {
  const std::string input{R"YAML(foo: |-
  hello
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, literal_block_keep) {
  const std::string input{R"YAML(foo: |+
  hello

)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, folded_block_scalar) {
  const std::string input{R"YAML(foo: >
  hello
  world
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, folded_block_strip) {
  const std::string input{R"YAML(foo: >-
  hello
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, folded_block_keep) {
  const std::string input{R"YAML(foo: >+
  hello

)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, literal_block_multiline) {
  const std::string input{R"YAML(description: |
  This is a
  multi-line
  description
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, folded_block_multiline) {
  const std::string input{R"YAML(description: >
  This is a
  multi-line
  description
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_quoted_empty) {
  const std::string input{R"YAML(foo: ''
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, double_quoted_empty) {
  const std::string input{R"YAML(foo: ""
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_quoted_with_special_chars) {
  const std::string input{R"YAML(foo: 'bar: baz'
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, double_quoted_with_escapes) {
  const std::string input{"foo: \"hello\\nworld\"\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_quoted_number_string) {
  const std::string input{R"YAML(port: '8080'
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, double_quoted_boolean_string) {
  const std::string input{R"YAML(flag: "true"
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping) {
  const std::string input{R"YAML(foo: {bar: baz, qux: 1}
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence) {
  const std::string input{R"YAML(items: [1, 2, 3]
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_nested_in_block) {
  const std::string input{R"YAML(foo: {bar: baz}
items: [1, 2, 3]
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence_strings) {
  const std::string input{R"YAML(branches: [main, develop]
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, github_actions_with_flow) {
  const std::string input{R"YAML(name: CI
on:
  push:
    branches: [main]
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

TEST(YAML_roundtrip, explicit_document_start) {
  const std::string input{R"YAML(---
foo: bar
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, explicit_document_end) {
  const std::string input{R"YAML(---
foo: bar
...
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, document_start_with_sequence) {
  const std::string input{R"YAML(---
- one
- two
- three
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mixed_everything) {
  const std::string input{R"YAML(---
name: 'My App'
version: "1.0"
description: |
  A multi-line
  description
items: [1, 2, 3]
nested:
  flow: {a: b}
  block:
    - one
    - two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_scalar_in_sequence) {
  const std::string input{R"YAML(steps:
  - name: Build
    run: |
      echo hello
      make build
  - name: Test
    run: make test
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, nested_flow_and_block) {
  const std::string input{R"YAML(rules:
  - pattern: "*.cc"
    options: {verbose: true, strict: false}
  - pattern: "*.h"
    options: {verbose: false}
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_scalar) {
  const std::string input{R"YAML(foo: &tag value
bar: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_and_alias_mapping) {
  const std::string input{R"YAML(foo: &anchor
  bar: baz
ref: *anchor
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, multiple_anchors) {
  const std::string input{R"YAML(a: &x 1
b: &y 2
c: *x
d: *y
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_sequence) {
  const std::string input{R"YAML(items: &list
  - one
  - two
ref: *list
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_reused_multiple) {
  const std::string input{R"YAML(a: &val hello
b: *val
c: *val
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_before_mapping) {
  const std::string input{R"YAML(# header comment
foo: bar
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_inline) {
  const std::string input{"foo: bar # inline comment\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_between_keys) {
  const std::string input{R"YAML(foo: bar
# between
baz: qux
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_before_value) {
  const std::string input{R"YAML(foo:
  # comment
  bar: baz
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_before_sequence) {
  const std::string input{R"YAML(# items
- one
- two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_inline_sequence) {
  const std::string input{"- one # first\n- two # second\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, multiple_comment_lines) {
  const std::string input{R"YAML(# line 1
# line 2
foo: bar
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_at_end) {
  const std::string input{R"YAML(foo: bar
# trailing
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_inline_on_nested_key) {
  const std::string input{"foo: bar # comment\nbaz:\n  qux: 1\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_after_document_start) {
  const std::string input{R"YAML(---
# comment after doc start
foo: bar
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_before_document_start) {
  const std::string input{R"YAML(# comment before doc start
---
foo: bar
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_inline_on_flow_mapping) {
  const std::string input{"foo: {a: 1} # flow comment\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_inline_on_flow_sequence) {
  const std::string input{"items: [1, 2] # list comment\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_last_key_in_mapping) {
  const std::string input{"a: 1\nb: 2 # last\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_with_single_quoted) {
  const std::string input{R"YAML(foo: &tag 'value'
bar: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_with_double_quoted) {
  const std::string input{"foo: &tag \"value\"\nbar: *tag\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_flow_mapping) {
  const std::string input{R"YAML(defaults: &defs {a: 1, b: 2}
foo: *defs
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_flow_sequence) {
  const std::string input{R"YAML(items: &list [1, 2, 3]
ref: *list
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, literal_block_deeply_nested) {
  const std::string input{R"YAML(outer:
  inner:
    script: |
      line one
      line two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, folded_block_deeply_nested) {
  const std::string input{R"YAML(outer:
  inner:
    text: >
      line one
      line two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, literal_block_with_blank_lines) {
  const std::string input{R"YAML(foo: |
  first

  third
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_scalar_followed_by_key) {
  const std::string input{R"YAML(script: |
  echo hello
next: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_in_sequence) {
  const std::string input{R"YAML(- {a: 1}
- {b: 2}
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence_in_sequence) {
  const std::string input{R"YAML(- [1, 2]
- [3, 4]
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mixed_flow_and_block_in_sequence) {
  const std::string input{R"YAML(- name: foo
  tags: [a, b]
- name: bar
  tags: [c, d]
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_scalar_plain) {
  const std::string input{"hello\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_scalar_quoted) {
  const std::string input{"'hello'\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_scalar_double_quoted) {
  const std::string input{"\"hello\"\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_mixed_scalar_styles) {
  const std::string input{R"YAML(- plain
- 'single'
- "double"
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, numeric_key) {
  const std::string input{R"YAML("42": value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, boolean_like_key) {
  const std::string input{R"YAML("true": value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, null_like_key) {
  const std::string input{R"YAML("null": value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, triple_nested_sequence) {
  const std::string input{R"YAML(- - - a
    - b
  - - c
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_between_sequence_items) {
  const std::string input{R"YAML(- one
# between items
- two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_with_block_scalar) {
  const std::string input{R"YAML(template: &tmpl |
  hello world
ref: *tmpl
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, document_markers_with_comments) {
  const std::string input{R"YAML(# header
---
foo: bar
# footer
...
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, multiple_inline_comments) {
  const std::string input{"a: 1 # first\nb: 2 # second\nc: 3 # third\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_alias) {
  const std::string input{"foo: &tag value\nbar: *tag # reuse\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_key_with_nested_mapping) {
  const std::string input{R"YAML(foo: # config section
  bar: baz
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_key_with_nested_sequence) {
  const std::string input{R"YAML(items: # the list
  - one
  - two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_with_inline_comment) {
  const std::string input{"foo: &tag value # tagged\nbar: *tag\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_before_nested_sequence_item) {
  const std::string input{R"YAML(items:
  # first item
  - one
  - two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_empty_flow_mapping) {
  const std::string input{"foo: {} # empty\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_empty_flow_sequence) {
  const std::string input{"foo: [] # empty\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_between_mapping_entries_in_sequence) {
  const std::string input{R"YAML(- name: foo
  # separator
  value: 1
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, multiple_comments_before_key) {
  const std::string input{R"YAML(# line 1
# line 2
# line 3
foo: bar
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_after_document_start_with_sequence) {
  const std::string input{R"YAML(---
# list follows
- one
- two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_sequence_item_flow_mapping) {
  const std::string input{"- {a: 1} # first\n- {b: 2} # second\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_before_and_inline_same_key) {
  const std::string input{"# before\nfoo: bar # inline\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, deeply_nested_comment) {
  const std::string input{R"YAML(a:
  b:
    # deep comment
    c: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_after_block_scalar) {
  const std::string input{R"YAML(script: |
  echo hello
# next section
next: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_anchor_flow_mapping) {
  const std::string input{"defaults: &defs {a: 1} # defaults\nfoo: *defs\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_between_doc_start_and_end) {
  const std::string input{R"YAML(---
foo: bar
# between markers
...
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, multiple_anchors_with_comments) {
  const std::string input{R"YAML(# first anchor
a: &x 1
# second anchor
b: &y 2
c: *x
d: *y
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_sequence_with_nested_mappings) {
  const std::string input{R"YAML(# steps
- name: build
  run: make
# next step
- name: test
  run: make test
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, null_value_then_comment_then_key) {
  const std::string input{R"YAML(a: null
# comment
b: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_non_first_key_with_nested_mapping) {
  const std::string input{R"YAML(first: 1
second: # nested
  a: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_non_first_key_with_nested_sequence) {
  const std::string input{R"YAML(first: 1
second: # list
  - a
  - b
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comments_at_every_nesting_level) {
  const std::string input{R"YAML(# top level
a:
  # level 1
  b:
    # level 2
    c: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_between_sequence_items_nested_mappings) {
  const std::string input{R"YAML(- name: foo
  value: 1
# separator
- name: bar
  value: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_sequence_item_scalar) {
  const std::string input{R"YAML(items:
  - one # first
  - two # second
  - three
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_scalar_then_comment_then_block_scalar) {
  const std::string input{R"YAML(a: |
  content1
# between scalars
b: |
  content2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_null_value_with_comment) {
  const std::string input{R"YAML(# comment before alias
a: &tag value
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_implicit_null) {
  const std::string input{R"YAML(a: &tag
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence_in_sequence_with_comments) {
  const std::string input{R"YAML(# first list
- [1, 2]
# second list
- [3, 4]
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_then_preceding_comment_same_key_series) {
  const std::string input{
      "a: 1 # on a\n# before b\nb: 2 # on b\n# before c\nc: 3\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_after_flow_in_nested_mapping) {
  const std::string input{R"YAML(outer:
  tags: [a, b] # tag list
  name: foo
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, multiple_null_values_with_comments) {
  const std::string input{R"YAML(a: null
b: null
# comment
c: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_nested_mapping_first_key) {
  const std::string input{R"YAML(outer:
  inner: value # the value
  other: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_of_sequences_with_comments) {
  const std::string input{R"YAML(# outer
- - a
  - b
# between
- - c
  - d
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_alias_with_preceding_and_inline_comments) {
  const std::string input{R"YAML(# anchor def
a: &tag value # anchor inline
# alias ref
b: *tag # alias inline
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, deeply_nested_sequence_with_comments) {
  const std::string input{R"YAML(items:
  - sub:
      - deep # inline deep
    name: foo
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_only_before_document_end) {
  const std::string input{R"YAML(---
foo: bar
# only comment
...
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_with_special_yaml_chars) {
  const std::string input{"foo: bar # { } [ ] : - * &\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_after_last_sequence_item_mapping) {
  const std::string input{R"YAML(- name: foo
  value: 1
- name: bar
  value: 2 # last value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_key_null_value_with_inline_comment) {
  const std::string input{"foo: null # empty\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, implicit_null_value) {
  const std::string input{"foo:\nbar: value\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, implicit_null_with_inline_comment) {
  const std::string input{"foo: # empty\nbar: value\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, explicit_null_value) {
  const std::string input{"foo: null\nbar: value\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, implicit_null_multiple) {
  const std::string input{"a:\nb:\nc: value\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, kubernetes_like_with_comments) {
  const std::string input{R"YAML(apiVersion: v1 # required
kind: Service
metadata:
  # identifying info
  name: my-service
  labels: # the labels
    app: my-app
spec:
  ports:
    # main port
    - port: 80
      protocol: TCP
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_block_mapping_key_path) {
  const std::string input{R"YAML(a: 1
b: # nested via block key
  c: 2
  d: 3
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_third_key_nested) {
  const std::string input{R"YAML(a: 1
b: 2
c: # third key nested
  d: 3
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_after_anchor_value) {
  const std::string input{"a: &x 1 # anchored\nb: *x\nc: *x # reused\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_after_nested_flow_sequence_non_first) {
  const std::string input{R"YAML(name: foo
tags: [a, b] # the tags
version: 1
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, preceding_comment_on_second_nested_key) {
  const std::string input{R"YAML(outer:
  a: 1
  # before b
  b: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_nested_last_key) {
  const std::string input{R"YAML(outer:
  a: 1
  b: 2 # last nested
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_between_nested_sequences) {
  const std::string input{R"YAML(items:
  - one
  # between
  - two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_value_in_sequence_with_comment) {
  const std::string input{R"YAML(- name: foo
  config: {debug: true} # cfg
- name: bar
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, double_quoted_value_with_inline_comment) {
  const std::string input{"foo: \"bar\" # quoted\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_quoted_value_with_inline_comment) {
  const std::string input{"foo: 'bar' # quoted\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_scalar_between_commented_keys) {
  const std::string input{R"YAML(# before script
script: |
  echo hello
# before next
next: value # inline
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, three_level_nested_inline_comments) {
  const std::string input{R"YAML(a: # level 0
  b: # level 1
    c: value # level 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_between_doc_start_and_mapping) {
  const std::string input{R"YAML(---
# after start
a: 1
# before end
...
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, alias_in_sequence_with_comment) {
  const std::string input{R"YAML(- &item value
- *item # reused
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mixed_comments_and_anchors_realistic) {
  const std::string input{R"YAML(# Database config
database:
  host: localhost # default host
  port: 5432
  credentials: &db_creds
    user: admin
    pass: secret
# Cache config
cache:
  host: localhost
  credentials: *db_creds # reuse db creds
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_item_with_flow_and_block_mixed) {
  const std::string input{R"YAML(- tags: [a, b] # tags
  name: foo
  script: |
    echo hello
# next item
- tags: [c]
  name: bar
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_after_empty_flow_in_nested) {
  const std::string input{R"YAML(outer:
  empty_map: {} # nothing here
  empty_list: [] # also nothing
  real: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_every_key_in_nested) {
  const std::string input{
      "a: 1 # one\nb: 2 # two\nc: 3 # three\nd: 4 # four\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, multiple_preceding_comments_non_first_key) {
  const std::string input{R"YAML(a: 1
# line 1
# line 2
# line 3
b: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_sequence_first_item_is_mapping) {
  const std::string input{R"YAML(# first entry
- key: value
  other: 2
# second entry
- key: value2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_mapping_inside_first_seq_item) {
  const std::string input{R"YAML(- a: 1 # on a
- b: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_mapping_inside_last_seq_item) {
  const std::string input{R"YAML(- a: 1
- b: 2 # on b
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, two_flow_collections_inline_comments) {
  const std::string input{"a: [1, 2] # first\nb: {x: 1} # second\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_alias_in_sequence) {
  const std::string input{R"YAML(- &x hello
- *x # reused
- world
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_between_nested_mapping_keys_deep) {
  const std::string input{R"YAML(a:
  b:
    x: 1
    # between x and y
    y: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_deeply_nested_last_key) {
  const std::string input{R"YAML(a:
  b:
    x: 1
    y: 2 # deep last
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, preceding_and_inline_on_every_key) {
  const std::string input{R"YAML(# before a
a: 1 # on a
# before b
b: 2 # on b
# before c
c: 3 # on c
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, nested_mapping_in_sequence_with_flow_and_comment) {
  const std::string input{R"YAML(- name: test
  env: {DEBUG: true} # env vars
  run: make
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence_non_first_key_with_comment) {
  const std::string input{R"YAML(name: CI
branches: [main, develop] # tracked
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_nested_mapping_with_comments) {
  const std::string input{R"YAML(defaults: &defs
  # default timeout
  timeout: 30
  retries: 3
job:
  name: build
  settings: *defs # use defaults
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_scalar_keep_with_comment_after) {
  const std::string input{R"YAML(first: |+
  content

# after block
second: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_scalar_strip_with_comment_after) {
  const std::string input{R"YAML(first: |-
  content
# after block
second: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, folded_scalar_then_comment_then_key) {
  const std::string input{R"YAML(text: >
  folded content
# separator
next: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_three_items_comments_between_each) {
  const std::string input{R"YAML(- one
# between 1 and 2
- two
# between 2 and 3
- three
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mapping_value_is_flow_then_nested_mapping) {
  const std::string input{R"YAML(flow: {a: 1} # inline
nested: # section
  b: 2
  c: 3
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, three_keys_middle_has_nested_with_comment) {
  const std::string input{R"YAML(first: 1
second: # has children
  a: 2
  b: 3
third: 4
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_in_mapping_with_preceding_comment) {
  const std::string input{R"YAML(config:
  name: app
  # the items
  items:
    - one
    - two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, doc_start_comment_then_sequence) {
  const std::string input{R"YAML(# header
---
# after start
- one
- two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_single_key_mapping) {
  const std::string input{"foo: bar # only key\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_special_chars_in_text) {
  const std::string input{
      "foo: bar # contains: colons, [brackets], {braces}\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, nested_flow_mapping_non_first_with_comment) {
  const std::string input{R"YAML(a: 1
b: {x: 1, y: 2} # flow map
c: 3
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, four_level_nesting_with_comment) {
  const std::string input{R"YAML(a:
  b:
    c:
      # deepest
      d: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_item_anchor_then_comment_on_next) {
  const std::string input{R"YAML(- &first one
# between
- two
- *first # alias
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mixed_block_scalars_with_comments) {
  const std::string input{R"YAML(# literal
a: |
  hello
# folded
b: >
  world
# plain
c: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_sequence_dash) {
  const std::string input{R"YAML(- # comment
  value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_document_start) {
  const std::string input{R"YAML(--- # start comment
key: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, trailing_comment_nested_sequence) {
  const std::string input{R"YAML(items:
  - one
  - two
# trailing
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_dash_with_nested_mapping) {
  const std::string input{R"YAML(- # item comment
  key: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_dash_with_nested_sequence) {
  const std::string input{R"YAML(- # outer
  - inner
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_dash_multiple_items) {
  const std::string input{R"YAML(- # first
  one
- # second
  two
- three
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_colon_with_next_line_value) {
  const std::string input{R"YAML(key: # on colon
  value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, document_start_comment_with_post_start_comments) {
  const std::string input{R"YAML(--- # start
# after start
key: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, document_end_inline_comment) {
  const std::string input{R"YAML(---
key: value
... # end
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_sequence_item) {
  const std::string input{R"YAML(- &first one
- &second two
- *first
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_scalar_with_indicator_comment) {
  const std::string input{R"YAML(key: | # block comment
  hello
  world
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_dash_first_item_only) {
  const std::string input{R"YAML(- # has comment
  first
- second
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, deeply_nested_comment_on_dash) {
  const std::string input{R"YAML(outer:
  - # nested
    inner: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, alias_with_preceding_comment) {
  const std::string input{R"YAML(a: &tag value
# before alias
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, empty_flow_mapping_with_comment) {
  const std::string input{R"YAML(a: {} # empty
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, empty_flow_sequence_with_comment) {
  const std::string input{R"YAML(a: [] # empty
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, document_start_comment_and_content) {
  const std::string input{R"YAML(--- # doc comment
a: 1
b: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_comment_on_anchor_definition) {
  const std::string input{R"YAML(a: &tag value # on value
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_between_sequence_items_and_mapping) {
  const std::string input{R"YAML(items:
  - one
  # between
  - two
next: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, folded_scalar_with_indicator_comment) {
  const std::string input{R"YAML(key: > # folded comment
  hello
  world
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_colon_with_nested_mapping) {
  const std::string input{R"YAML(outer: # on outer
  inner: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_colon_with_nested_sequence) {
  const std::string input{R"YAML(items: # on items
  - one
  - two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_colon_with_block_scalar) {
  const std::string input{R"YAML(text: |
  hello
  world
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_dash_with_flow_mapping) {
  const std::string input{R"YAML(- # item
  {a: 1}
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_dash_with_anchor) {
  const std::string input{R"YAML(- # anchored
  &tag value
- *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_colon_and_inline_on_value) {
  const std::string input{"key: # on colon\n  value # on value\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, strip_chomping_with_comment) {
  const std::string input{R"YAML(key: |- # stripped
  hello
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, keep_chomping_with_comment) {
  const std::string input{R"YAML(key: |+ # kept
  hello
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_with_comment_on_colon) {
  const std::string input{R"YAML(a: # comment
  &tag value
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, multiple_keys_with_colon_comments) {
  const std::string input{R"YAML(a: # first
  1
b: # second
  2
c: 3
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_dash_null_item) {
  const std::string input{R"YAML(- # null item
- value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, leading_comment_and_document_start_comment) {
  const std::string input{R"YAML(# leading
--- # start
key: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, document_end_comment_with_trailing) {
  const std::string input{R"YAML(---
key: value
... # end
# trailing
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, pre_end_comment_and_end_comment) {
  const std::string input{R"YAML(---
key: value
# before end
... # end
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, double_quoted_with_escapes_roundtrip) {
  const std::string input{"key: \"hello\\nworld\"\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_quoted_with_inner_quotes) {
  const std::string input{"key: 'it''s here'\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_nested_in_flow_sequence) {
  const std::string input{"- [{a: 1}, {b: 2}]\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence_nested_in_flow_mapping) {
  const std::string input{"{a: [1, 2], b: [3, 4]}\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, empty_mapping_value_with_comment_then_next_key) {
  const std::string input{R"YAML(a: # empty
b: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_between_mapping_entries) {
  const std::string input{R"YAML(a: 1
# between
b: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, deeply_nested_colon_indicator_comment) {
  const std::string input{R"YAML(level1:
  level2: # deep
    level3: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mixed_indicator_comments_mapping_and_sequence) {
  const std::string input{R"YAML(data: # on data
  - # on dash
    item
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_alias_same_key_implicit_null) {
  const std::string input{R"YAML(a: &x
b: *x
c: &y
d: *y
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_sequence_at_root) {
  const std::string input{R"YAML(- a
- b
- c
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, nested_empty_collections) {
  const std::string input{R"YAML(a: {}
b: []
c: {x: {}}
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, string_keys_that_look_numeric) {
  const std::string input{R"YAML("1": one
"2": two
"10": ten
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, boolean_values_mixed_styles) {
  const std::string input{R"YAML(a: true
b: false
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, null_explicit_in_sequence) {
  const std::string input{R"YAML(- null
- value
- null
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_with_comment_after) {
  const std::string input{R"YAML(data: {a: 1, b: 2} # inline
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence_with_comment_after) {
  const std::string input{R"YAML(data: [1, 2, 3] # list
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_flow_collection) {
  const std::string input{R"YAML(a: &tag {x: 1}
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_flow_sequence_with_alias) {
  const std::string input{R"YAML(a: &tag [1, 2]
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_block_scalar) {
  const std::string input{R"YAML(a: &tag |
  hello
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_colon_implicit_null_with_comment) {
  const std::string input{R"YAML(a: # null with comment
b: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_every_dash_in_sequence) {
  const std::string input{R"YAML(- # a
  one
- # b
  two
- # c
  three
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_of_mappings_with_comments) {
  const std::string input{R"YAML(# first item
- name: alice
  age: 30
# second item
- name: bob
  age: 25
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, deeply_nested_three_levels) {
  const std::string input{R"YAML(a:
  b:
    c:
      d: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mapping_then_sequence_then_mapping) {
  const std::string input{R"YAML(config:
  items:
    - key: val
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, double_quoted_empty_string) {
  const std::string input{"key: \"\"\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_quoted_empty_string) {
  const std::string input{"key: ''\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, literal_block_strip_empty_lines) {
  const std::string input{R"YAML(key: |-
  hello
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, literal_block_keep_trailing_newlines) {
  const std::string input{"key: |+\n  hello\n\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_single_entry) {
  const std::string input{"{key: value}\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence_single_entry) {
  const std::string input{"[value]\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mixed_flow_and_block_in_mapping) {
  const std::string input{R"YAML(block_key: block_value
flow_key: {nested: value}
list_key: [a, b, c]
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, alias_in_sequence) {
  const std::string input{R"YAML(- &item value
- *item
- *item
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_only_lines_between_keys) {
  const std::string input{R"YAML(a: 1
# line 1
# line 2
# line 3
b: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, indicator_comment_then_inline_comment_different_key) {
  const std::string input{R"YAML(a: # indicator
  value
b: plain # inline
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, nested_flow_three_levels) {
  const std::string input{"{a: {b: {c: 1}}}\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_in_mapping_value_with_indicator_comment) {
  const std::string input{R"YAML(key: # on key
  - first
  - second
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mapping_in_sequence_with_indicator_comment) {
  const std::string input{R"YAML(- # entry
  name: alice
  age: 30
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_nested_block_mapping) {
  const std::string input{R"YAML(a: &ref
  x: 1
  y: 2
b: *ref
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_nested_block_sequence) {
  const std::string input{R"YAML(a: &ref
  - 1
  - 2
b: *ref
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_value_document) {
  const std::string input{"hello\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_quoted_value_document) {
  const std::string input{"'hello'\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, integer_value_document) {
  const std::string input{"42\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, all_comment_types_combined) {
  const std::string input{R"YAML(# leading
--- # start
# after start
# before a
a: 1 # on a
# before b
b: # on colon
  2
# before end
... # end
# trailing
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_colon_with_anchor_value) {
  const std::string input{R"YAML(a: # comment
  &tag value
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_dash_with_double_quoted) {
  const std::string input{R"YAML(- # quoted
  "hello world"
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_dash_with_single_quoted) {
  const std::string input{R"YAML(- # quoted
  'hello world'
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_with_all_null) {
  const std::string input{R"YAML(-
-
-
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mapping_with_all_implicit_null) {
  const std::string input{R"YAML(a:
b:
c:
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_implicit_null_with_inline_comment) {
  const std::string input{R"YAML(- # first
- # second
- # third
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mapping_implicit_null_with_inline_comment) {
  const std::string input{R"YAML(a: # first
b: # second
c: # third
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, literal_block_multiline_with_blank_lines) {
  const std::string input{R"YAML(key: |
  line one

  line three
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, folded_block_multiline_with_blank_lines) {
  const std::string input{R"YAML(key: >
  line one

  line three
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, double_quoted_with_special_chars) {
  const std::string input{"key: \"hello\\tworld\"\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_empty_with_anchor) {
  const std::string input{R"YAML(a: &tag {}
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence_empty_with_anchor) {
  const std::string input{R"YAML(a: &tag []
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_after_last_sequence_item) {
  const std::string input{R"YAML(- one
- two # last
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_after_last_mapping_value) {
  const std::string input{R"YAML(a: 1
b: 2 # last
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, alias_as_sequence_item_with_comment) {
  const std::string input{R"YAML(- &tag value
- *tag # reuse
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mixed_scalar_styles_in_mapping) {
  const std::string input{R"YAML(plain: hello
single: 'world'
double: "foo"
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mixed_scalar_styles_in_sequence) {
  const std::string input{R"YAML(- hello
- 'world'
- "foo"
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_multiple_entries_with_styles) {
  const std::string input{"{a: 1, b: 'two', c: \"three\"}\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_of_flow_mappings) {
  const std::string input{R"YAML(- {name: alice}
- {name: bob}
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_of_flow_sequences) {
  const std::string input{R"YAML(- [1, 2]
- [3, 4]
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_mapping_in_block_sequence_in_mapping) {
  const std::string input{R"YAML(users:
  - name: alice
    role: admin
  - name: bob
    role: user
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_before_first_key_in_mapping) {
  const std::string input{R"YAML(# header comment
key: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_colon_with_flow_mapping_value) {
  const std::string input{R"YAML(key: # on key
  {a: 1}
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_colon_with_flow_sequence_value) {
  const std::string input{R"YAML(key: # on key
  [1, 2]
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_alias_with_flow_collection) {
  const std::string input{R"YAML(defaults: &def
  timeout: 30
  retries: 3
production:
  <<: *def
  timeout: 60
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_pair_flow_mapping_in_sequence) {
  const std::string input{R"YAML(- {key: value}
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, nested_block_sequences) {
  const std::string input{R"YAML(-
  - a
  - b
-
  - c
  - d
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_dash_nested_block_sequences) {
  const std::string input{R"YAML(- # outer1
  - a
  - b
- # outer2
  - c
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, empty_value_between_keys) {
  const std::string input{R"YAML(a:
b: value
c:
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, literal_block_single_line) {
  const std::string input{R"YAML(key: |
  single line
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, folded_block_single_line) {
  const std::string input{R"YAML(key: >
  single line
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_explicit_null) {
  const std::string input{R"YAML(a: &tag null
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, true_false_null_as_quoted_strings) {
  const std::string input{R"YAML(a: "true"
b: "false"
c: "null"
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, yes_no_on_off_plain_scalars) {
  const std::string input{R"YAML(a: yes
b: no
c: on
d: off
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, explicit_null_roundtrip) {
  const std::string input{R"YAML(a: null
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, float_values) {
  const std::string input{R"YAML(a: 1.5
b: -3.14
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, large_integer) {
  const std::string input{R"YAML(big: 9999999999
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, negative_integer) {
  const std::string input{R"YAML(neg: -42
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, colon_in_double_quoted_scalar) {
  const std::string input{"key: \"value: with colon\"\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, hash_in_quoted_string) {
  const std::string input{"key: \"value # not a comment\"\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_colon_with_literal_block) {
  const std::string input{R"YAML(key: # before block
  |
  hello
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, document_start_end_no_content_comments) {
  const std::string input{R"YAML(--- # doc
key: value
...
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_with_preceding_and_inline_comments) {
  const std::string input{R"YAML(# before first
- one # after first
# before second
- two # after second
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, deeply_nested_mixed_with_comments) {
  const std::string input{R"YAML(root:
  # section
  items:
    - name: test # item name
      tags: [a, b] # the tags
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_single_quoted) {
  const std::string input{R"YAML(a: &tag 'hello'
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_double_quoted) {
  const std::string input{R"YAML(a: &tag "hello"
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_integer) {
  const std::string input{R"YAML(a: &tag 42
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_boolean) {
  const std::string input{R"YAML(a: &tag true
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_item_on_next_line_scalar) {
  const std::string input{R"YAML(-
  value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_item_on_next_line_mapping) {
  const std::string input{R"YAML(-
  key: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_mixed_same_and_next_line) {
  const std::string input{R"YAML(- inline
-
  next_line
- also_inline
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_dash_then_next_item_inline) {
  const std::string input{R"YAML(- # commented
  value
- inline
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_next_line_value) {
  const std::string input{R"YAML(-
  &tag value
- *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, deeply_nested_next_line_sequences) {
  const std::string input{R"YAML(-
  -
    -
      deep
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_literal_in_next_line_sequence_item) {
  const std::string input{R"YAML(-
  |
  hello
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_in_next_line_sequence_item) {
  const std::string input{R"YAML(-
  {a: 1}
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence_in_next_line_sequence_item) {
  const std::string input{R"YAML(-
  [1, 2, 3]
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, multiple_anchors_and_aliases) {
  const std::string input{R"YAML(x: &a 1
y: &b 2
z: &c 3
p: *a
q: *b
r: *c
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, alias_in_flow_mapping) {
  const std::string input{R"YAML(a: &tag value
b: {ref: *tag}
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, alias_in_flow_sequence) {
  const std::string input{R"YAML(a: &tag value
b: [*tag, other]
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, empty_string_plain) {
  const std::string input{"key: ''\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, long_plain_scalar) {
  const std::string input{
      "key: this is a fairly long plain scalar value that stays on one line\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, special_chars_in_double_quoted) {
  const std::string input{"key: \"line1\\nline2\\ttab\"\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mapping_value_is_integer_zero) {
  const std::string input{R"YAML(a: 0
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mapping_value_is_empty_string_double_quoted) {
  const std::string input{"a: \"\"\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_of_integers) {
  const std::string input{R"YAML(- 1
- 2
- 3
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_of_booleans) {
  const std::string input{R"YAML(- true
- false
- true
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_of_mixed_types) {
  const std::string input{R"YAML(- hello
- 42
- true
- null
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mapping_with_mixed_value_types) {
  const std::string input{R"YAML(str: hello
int: 42
bool: true
nil: null
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, comment_on_implicit_null_mapping_value) {
  const std::string input{R"YAML(a: # null a
b: # null b
c: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, preceding_comment_on_nested_mapping_key) {
  const std::string input{R"YAML(outer:
  # before inner
  inner: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, preceding_comment_on_nested_sequence_item) {
  const std::string input{R"YAML(items:
  # before first
  - one
  # before second
  - two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, inline_and_preceding_alternating) {
  const std::string input{R"YAML(a: 1 # on a
# before b
b: 2
# before c
c: 3 # on c
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_scalar_strip_with_anchor) {
  const std::string input{R"YAML(a: &tag |-
  hello
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_scalar_keep_with_anchor) {
  const std::string input{"a: &tag |+\n  hello\n\nb: *tag\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, folded_scalar_with_anchor) {
  const std::string input{R"YAML(a: &tag >
  hello
  world
b: *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_with_next_line_then_inline_then_next_line) {
  const std::string input{R"YAML(-
  first
- second
-
  third
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, indicator_comment_on_colon_with_nested_mapping_deep) {
  const std::string input{R"YAML(a: # level1
  b: # level2
    c: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, indicator_comment_mixed_with_nested_and_inline) {
  const std::string input{R"YAML(config: # config section
  name: test # the name
  items: # items section
    - one
    - two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_in_block_with_preceding_comment) {
  const std::string input{R"YAML(# tags
tags: [a, b, c]
# names
names: [x, y]
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, document_markers_with_all_comment_types) {
  const std::string input{R"YAML(# leading 1
# leading 2
--- # start
# post start
a: 1
# pre end
... # end
# trailing
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_implicit_null_in_sequence) {
  const std::string input{R"YAML(- &tag
- *tag
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, next_line_value_with_inline_comment_on_value) {
  const std::string input{R"YAML(-
  value # comment on value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, next_line_mapping_with_inline_comments) {
  const std::string input{R"YAML(-
  a: 1 # on a
  b: 2 # on b
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, three_level_mapping_with_all_indicator_comments) {
  const std::string input{R"YAML(root: # root
  mid: # mid
    leaf: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, sequence_in_mapping_next_line_style) {
  const std::string input{R"YAML(items:
  -
    a
  -
    b
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_item_sequence_next_line) {
  const std::string input{R"YAML(-
  only_item
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, double_quoted_key) {
  const std::string input{"\"special key\": value\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_quoted_key) {
  const std::string input{"'special key': value\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, deeply_nested_mapping_five_levels) {
  const std::string input{R"YAML(a:
  b:
    c:
      d:
        e: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mapping_key_with_spaces) {
  const std::string input{R"YAML(key with spaces: value
another key: other
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_empty) {
  const std::string input{"{}\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence_empty) {
  const std::string input{"[]\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_mapping_single_entry) {
  const std::string input{R"YAML(only: entry
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_sequence_single_entry) {
  const std::string input{R"YAML(- only
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, complex_real_world_config) {
  const std::string input{R"YAML(# Application config
app:
  name: myapp # app name
  version: '1.0'
  debug: false

# Database settings
database:
  host: localhost
  port: 5432
  credentials:
    user: admin
    pass: "s3cret"

# Feature flags
features:
  - logging
  - metrics
  - tracing
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, blank_line_between_mapping_entries) {
  const std::string input{R"YAML(a: 1

b: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, blank_line_between_sequence_items) {
  const std::string input{R"YAML(- one

- two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, multiple_blank_lines) {
  const std::string input{R"YAML(a: 1


b: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, blank_line_before_comment) {
  const std::string input{R"YAML(a: 1

# comment
b: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, blank_line_after_comment) {
  const std::string input{R"YAML(# first group
a: 1

# second group
b: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, blank_line_in_nested_mapping) {
  const std::string input{R"YAML(outer:
  a: 1

  b: 2
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, blank_line_in_nested_sequence) {
  const std::string input{R"YAML(items:
  - first

  - second
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, blank_line_between_sections_with_comments) {
  const std::string input{R"YAML(# Section A
a: 1

# Section B
b: 2

# Section C
c: 3
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, no_blank_lines) {
  const std::string input{R"YAML(a: 1
b: 2
c: 3
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, blank_line_with_document_markers) {
  const std::string input{R"YAML(---
a: 1

b: 2
...)YAML"};
  EXPECT_EQ(roundtrip(input), input + "\n");
}

TEST(YAML_roundtrip, indent_width_four_spaces) {
  const std::string input{R"YAML(parent:
    child: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, indent_width_four_spaces_nested) {
  const std::string input{R"YAML(a:
    b:
        c: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, indent_width_four_spaces_sequence) {
  const std::string input{R"YAML(items:
    - one
    - two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, indent_width_three_spaces) {
  const std::string input{R"YAML(parent:
   child: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, indent_width_four_mapping_and_sequence) {
  const std::string input{R"YAML(config:
    name: test
    items:
        - one
        - two
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, double_quoted_key_with_special_chars) {
  const std::string input{"\"key: with colon\": value\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_quoted_key_plain_value) {
  const std::string input{"'my-key': 42\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, mixed_key_styles) {
  const std::string input{R"YAML(plain_key: 1
'single key': 2
"double key": 3
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, quoted_key_with_nested_value) {
  const std::string input{R"YAML("my key":
  child: value
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, block_scalar_with_extra_indent) {
  const std::string input{R"YAML(key: |
  normal
    indented
  back
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, folded_block_with_extra_indent) {
  const std::string input{R"YAML(key: >
  normal
    indented
  back
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_quoted_keys) {
  const std::string input{"{\"a b\": 1, 'c d': 2}\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_quoted_values) {
  const std::string input{"{a: 'one', b: \"two\"}\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence_quoted_values) {
  const std::string input{"['one', \"two\", three]\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, tilde_null) {
  const std::string input{"key: ~\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, special_float_inf) {
  const std::string input{"key: .inf\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, special_float_neg_inf) {
  const std::string input{"key: -.inf\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, special_float_nan) {
  const std::string input{"key: .nan\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, single_quoted_key_with_apostrophe) {
  const std::string input{"'it''s': value\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, empty_single_quoted_key) {
  const std::string input{"'': value\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, empty_double_quoted_key) {
  const std::string input{"\"\": value\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, value_with_hash_quoted) {
  const std::string input{"key: '#not-a-comment'\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_false) {
  const std::string input{"a: &tag false\nb: *tag\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, anchor_on_float) {
  const std::string input{"a: &tag 3.14\nb: *tag\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, nested_flow_sequences) {
  const std::string input{"[[1, 2], [3, [4, 5]]]\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_null_and_bool) {
  const std::string input{"{a: null, b: true, c: false}\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, zero_in_sequence) {
  const std::string input{"- 0\n- 0.0\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, double_quoted_backslash) {
  const std::string input{"key: \"path\\\\to\\\\file\"\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, false_key) {
  const std::string input{"\"false\": value\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, blank_line_between_sequence_mappings) {
  const std::string input{R"YAML(- name: alice

- name: bob
)YAML"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_mapping_root_with_doc_markers) {
  const std::string input{"---\n{a: 1}\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, flow_sequence_root_with_doc_markers) {
  const std::string input{"---\n[1, 2]\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, scalar_doc_with_markers) {
  const std::string input{"---\nhello\n...\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, boolean_doc) {
  const std::string input{"true\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, null_doc) {
  const std::string input{"null\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, float_doc) {
  const std::string input{"3.14\n"};
  EXPECT_EQ(roundtrip(input), input);
}

TEST(YAML_roundtrip, negative_integer_doc) {
  const std::string input{"-42\n"};
  EXPECT_EQ(roundtrip(input), input);
}

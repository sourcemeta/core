#include <sourcemeta/jsontoolkit/yaml.h>

#include <string_view> // std::string_view

// See https://pyyaml.org/wiki/LibYAML for basic documentation
#include <yaml.h>

static auto yaml_node_to_json(yaml_node_t *const node,
                              yaml_document_t *const document)
    -> sourcemeta::jsontoolkit::JSON {
  if (!node) {
    return sourcemeta::jsontoolkit::JSON{nullptr};
  }

  switch (node->type) {
    case YAML_SCALAR_NODE: {
      const std::string_view input{
          reinterpret_cast<char *>(node->data.scalar.value),
          node->data.scalar.length};

      try {
        // TODO: Avoid this std::string transformation
        return sourcemeta::jsontoolkit::parse(std::string{input});
        // Looks like it is very hard in YAML, given a scalar value, to
        // determine whether it is a string or something else without attempting
        // to parsing it and potentially failing to do so
      } catch (const sourcemeta::jsontoolkit::ParseError &) {
        return sourcemeta::jsontoolkit::JSON{input};
      }
    }

    case YAML_SEQUENCE_NODE: {
      auto result{sourcemeta::jsontoolkit::JSON::make_array()};
      for (yaml_node_item_t *item = node->data.sequence.items.start;
           item < node->data.sequence.items.top; ++item) {
        yaml_node_t *const child = yaml_document_get_node(document, *item);
        result.push_back(yaml_node_to_json(child, document));
      }

      return result;
    }

    case YAML_MAPPING_NODE: {
      auto result{sourcemeta::jsontoolkit::JSON::make_object()};
      for (yaml_node_pair_t *pair = node->data.mapping.pairs.start;
           pair < node->data.mapping.pairs.top; ++pair) {
        yaml_node_t *const key_node =
            yaml_document_get_node(document, pair->key);
        yaml_node_t *const value_node =
            yaml_document_get_node(document, pair->value);
        if (key_node && key_node->type == YAML_SCALAR_NODE) {
          result.assign(reinterpret_cast<char *>(key_node->data.scalar.value),
                        yaml_node_to_json(value_node, document));
        }
      }

      return result;
    }

    default:
      return sourcemeta::jsontoolkit::JSON{nullptr};
  }
}

namespace sourcemeta::jsontoolkit {

auto from_yaml(const JSON::String &input) -> JSON {
  yaml_parser_t parser;
  if (!yaml_parser_initialize(&parser)) {
    throw YAMLError("Could not initialize the YAML parser");
  }

  yaml_parser_set_input_string(
      &parser, reinterpret_cast<const unsigned char *>(input.c_str()),
      input.size());

  yaml_document_t document;
  if (!yaml_parser_load(&parser, &document)) {
    yaml_parser_delete(&parser);
    // TODO: Ideally we would get line/column information like for `ParseError`
    throw YAMLParseError("Failed to parse the YAML document");
  }

  yaml_node_t *const root = yaml_document_get_root_node(&document);
  if (!root) {
    yaml_document_delete(&document);
    yaml_parser_delete(&parser);
    throw YAMLParseError("The input YAML document is empty");
  }

  try {
    const auto result{yaml_node_to_json(root, &document)};
    yaml_document_delete(&document);
    yaml_parser_delete(&parser);
    return result;
  } catch (...) {
    yaml_document_delete(&document);
    yaml_parser_delete(&parser);
    throw;
  }
}

} // namespace sourcemeta::jsontoolkit

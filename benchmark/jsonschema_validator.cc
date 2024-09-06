#include <benchmark/benchmark.h>

#include <cassert> // assert

#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>

static void
JSONSchema_Validate_Draft4_Meta_1_No_Callback(benchmark::State &state) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "type": "string" },
      "bar": { "type": "integer" }
    }
  })JSON")};

  const auto metaschema{sourcemeta::jsontoolkit::metaschema(
      schema, sourcemeta::jsontoolkit::official_resolver)};
  const auto metaschema_template{sourcemeta::jsontoolkit::compile(
      metaschema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  for (auto _ : state) {
    auto result{sourcemeta::jsontoolkit::evaluate(metaschema_template, schema)};
    assert(result);
    benchmark::DoNotOptimize(result);
  }
}

static void
JSONSchema_Validate_Draft4_Required_Properties(benchmark::State &state) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [
      "a", "b", "c", "d", "e", "f", "g", "h", "i",
      "j", "k", "l", "m", "n", "o", "p", "q", "r",
      "s", "t", "u", "v", "w", "x", "y", "z"
    ],
    "properties": {
      "a": { "type": "boolean" },
      "b": { "type": "boolean" },
      "c": { "type": "boolean" },
      "d": { "type": "boolean" },
      "e": { "type": "boolean" },
      "f": { "type": "boolean" },
      "g": { "type": "boolean" },
      "h": { "type": "boolean" },
      "i": { "type": "boolean" },
      "j": { "type": "boolean" },
      "k": { "type": "boolean" },
      "l": { "type": "boolean" },
      "m": { "type": "boolean" },
      "n": { "type": "boolean" },
      "o": { "type": "boolean" },
      "p": { "type": "boolean" },
      "q": { "type": "boolean" },
      "r": { "type": "boolean" },
      "s": { "type": "boolean" },
      "t": { "type": "boolean" },
      "u": { "type": "boolean" },
      "v": { "type": "boolean" },
      "w": { "type": "boolean" },
      "x": { "type": "boolean" },
      "y": { "type": "boolean" },
      "z": { "type": "boolean" }
    }
  })JSON")};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "a": true,
    "b": true,
    "c": true,
    "d": true,
    "e": true,
    "f": true,
    "g": true,
    "h": true,
    "i": true,
    "j": true,
    "k": true,
    "l": true,
    "m": true,
    "n": true,
    "o": true,
    "p": true,
    "q": true,
    "r": true,
    "s": true,
    "t": true,
    "u": true,
    "v": true,
    "w": true,
    "x": true,
    "y": true,
    "z": true
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  for (auto _ : state) {
    auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance)};
    assert(result);
    benchmark::DoNotOptimize(result);
  }
}

static void JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match(
    benchmark::State &state) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "a": { "type": "boolean" },
      "b": { "type": "boolean" },
      "c": { "type": "boolean" },
      "d": { "type": "boolean" },
      "e": { "type": "boolean" },
      "f": { "type": "boolean" },
      "g": { "type": "boolean" },
      "h": { "type": "boolean" },
      "i": { "type": "boolean" },
      "j": { "type": "boolean" },
      "k": { "type": "boolean" },
      "l": { "type": "boolean" },
      "m": { "type": "boolean" },
      "n": { "type": "boolean" },
      "o": { "type": "boolean" },
      "p": { "type": "boolean" },
      "q": { "type": "boolean" },
      "r": { "type": "boolean" },
      "s": { "type": "boolean" },
      "t": { "type": "boolean" },
      "u": { "type": "boolean" },
      "v": { "type": "boolean" },
      "w": { "type": "boolean" },
      "x": { "type": "boolean" },
      "y": { "type": "boolean" },
      "z": { "type": "boolean" }
    }
  })JSON")};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "p": true
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  for (auto _ : state) {
    auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance)};
    assert(result);
    benchmark::DoNotOptimize(result);
  }
}

static void JSONSchema_Validate_Draft4_Items_Schema(benchmark::State &state) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": {
      "items": {
        "items": {
          "items": {
            "items": {
              "items": {
                "type": "string"
              }
            }
          }
        }
      }
    }
  })JSON")};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse(R"JSON([
    [
      [
        [
          [ [ "red", "blue" ], [ "green", "yellow" ] ],
          [ [ "circle", "square" ], [ "triangle", "rectangle" ] ]
        ],
        [
          [ [ "cat", "dog" ], [ "fish", "bird" ] ],
          [ [ "apple", "banana" ], [ "cherry", "date" ] ]
        ]
      ],
      [
        [
          [ [ "hot", "cold" ], [ "wet", "dry" ] ],
          [ [ "big", "small" ], [ "tall", "short" ] ]
        ],
        [
          [ [ "happy", "sad" ], [ "angry", "calm" ] ],
          [ [ "fast", "slow" ], [ "loud", "quiet" ] ]
        ]
      ]
    ],
    [
      [
        [
          [ [ "sun", "moon" ], [ "star", "planet" ] ],
          [ [ "ocean", "mountain" ], [ "forest", "desert" ] ]
        ],
        [
          [ [ "car", "bike" ], [ "train", "plane" ] ],
          [ [ "pencil", "pen" ], [ "paper", "eraser" ] ]
        ]
      ],
      [
        [
          [ [ "jazz", "rock" ], [ "pop", "classical" ] ],
          [ [ "comedy", "drama" ], [ "action", "romance" ] ]
        ],
        [
          [ [ "spring", "summer" ], [ "autumn", "winter" ] ],
          [ [ "breakfast", "lunch" ], [ "dinner", "snack" ] ]
        ]
      ]
    ]
  ])JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  for (auto _ : state) {
    auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance)};
    assert(result);
    benchmark::DoNotOptimize(result);
  }
}

static void JSONSchema_Validate_Draft4_Nested_Object(benchmark::State &state) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON(
{
  "$schema": "http://json-schema.org/draft-04/schema#",
  "additionalProperties": {
    "additionalProperties": {
      "additionalProperties": {
        "additionalProperties": {
          "additionalProperties": {
            "additionalProperties": {
              "additionalProperties": {
                "additionalProperties": {
                  "additionalProperties": {
                    "additionalProperties": {
                      "additionalProperties": {
                        "additionalProperties": {
                          "additionalProperties": {
                            "additionalProperties": {
                              "additionalProperties": {
                                "additionalProperties": {
                                  "additionalProperties": {
                                    "additionalProperties": {
                                      "additionalProperties": true
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
  )JSON")};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse(R"JSON(
{
  "a1": {
    "a2": {
      "a3": {
        "a4": {
          "a5": {
            "a6": {
              "a7": {
                "a8": {
                  "a9": {
                    "a10": {
                      "a11": {
                        "a12": {
                          "a13": {
                            "a14": {
                              "a15": {
                                "a16": {
                                  "a17": {
                                    "a18": {
                                      "a19": {}
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  },
  "b1": {
    "b2": {
      "b3": {
        "b4": {
          "b5": {
            "b6": {
              "b7": {
                "b8": {
                  "b9": {
                    "b10": {
                      "b11": {
                        "b12": {
                          "b13": {
                            "b14": {
                              "b15": {
                                "b16": {
                                  "b17": {
                                    "b18": {
                                      "b19": {}
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
  )JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  for (auto _ : state) {
    auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance)};
    assert(result);
    benchmark::DoNotOptimize(result);
  }
}

static void
JSONSchema_Validate_Draft4_Properties_Triad_Optional(benchmark::State &state) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "a": { "type": "boolean" },
      "b": { "type": "boolean" },
      "c": { "type": "boolean" },
      "d": { "type": "boolean" },
      "e": { "type": "boolean" },
      "f": { "type": "boolean" }
    },
    "patternProperties": {
      "^[A-Z]": { "type": "null" }
    },
    "additionalProperties": {
      "type": "string"
    }
  })JSON")};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "A": null,
    "Z": null,
    "foo": "bar",
    "a": true,
    "c": true,
    "d": true,
    "e": true
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  for (auto _ : state) {
    auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance)};
    assert(result);
    benchmark::DoNotOptimize(result);
  }
}

static void
JSONSchema_Validate_Draft4_Properties_Triad_Required(benchmark::State &state) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "a", "A", "d" ],
    "properties": {
      "a": { "type": "boolean" },
      "b": { "type": "boolean" },
      "c": { "type": "boolean" },
      "d": { "type": "boolean" },
      "e": { "type": "boolean" },
      "f": { "type": "boolean" }
    },
    "patternProperties": {
      "^[A-Z]": { "type": "null" }
    },
    "additionalProperties": {
      "type": "string"
    }
  })JSON")};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "A": null,
    "Z": null,
    "foo": "bar",
    "a": true,
    "c": true,
    "d": true,
    "e": true
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  for (auto _ : state) {
    auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance)};
    assert(result);
    benchmark::DoNotOptimize(result);
  }
}

static void
JSONSchema_Validate_Draft4_Properties_Triad_Closed(benchmark::State &state) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "a": { "type": "boolean" },
      "b": { "type": "boolean" },
      "c": { "type": "boolean" },
      "d": { "type": "boolean" },
      "e": { "type": "boolean" },
      "f": { "type": "boolean" }
    },
    "patternProperties": {
      "^[A-Z]": { "type": "null" }
    },
    "additionalProperties": false
  })JSON")};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "A": null,
    "Z": null,
    "a": true,
    "c": true,
    "d": true,
    "e": true
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  for (auto _ : state) {
    auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance)};
    assert(result);
    benchmark::DoNotOptimize(result);
  }
}

static void
JSONSchema_Validate_Draft4_Non_Recursive_Ref(benchmark::State &state) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalProperties": { "$ref": "#/definitions/one" },
    "definitions": {
      "one": { "$ref": "#/definitions/two" },
      "two": { "$ref": "#/definitions/three" },
      "three": { "$ref": "#/definitions/four" },
      "four": { "$ref": "#/definitions/five" },
      "five": { "$ref": "#/definitions/six" },
      "six": { "$ref": "#/definitions/seven" },
      "seven": { "$ref": "#/definitions/eight" },
      "eight": { "$ref": "#/definitions/nine" },
      "nine": { "$ref": "#/definitions/ten" },
      "ten": { "type": "boolean" }
    }
  })JSON")};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "a": true,
    "b": true,
    "c": true,
    "d": true,
    "e": true,
    "f": true,
    "g": true,
    "h": true,
    "i": true,
    "j": true,
    "k": true
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  for (auto _ : state) {
    auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance)};
    assert(result);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(JSONSchema_Validate_Draft4_Meta_1_No_Callback);
BENCHMARK(JSONSchema_Validate_Draft4_Required_Properties);
BENCHMARK(JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match);
BENCHMARK(JSONSchema_Validate_Draft4_Items_Schema);
BENCHMARK(JSONSchema_Validate_Draft4_Nested_Object);
BENCHMARK(JSONSchema_Validate_Draft4_Properties_Triad_Optional);
BENCHMARK(JSONSchema_Validate_Draft4_Properties_Triad_Closed);
BENCHMARK(JSONSchema_Validate_Draft4_Properties_Triad_Required);
BENCHMARK(JSONSchema_Validate_Draft4_Non_Recursive_Ref);

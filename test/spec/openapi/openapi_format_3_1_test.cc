#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/openapi.h>

#include <sourcemeta/core/test.h>

#include <sstream> // std::ostringstream

namespace {

auto format(const sourcemeta::core::JSON::String &input)
    -> sourcemeta::core::JSON::String {
  auto document{sourcemeta::core::parse_json(input)};
  // The frame borrows from the document and formatting reorders it, so the
  // frame is spent once the call below returns. Destroying it afterwards is
  // safe because that never reads what it borrowed
  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  sourcemeta::core::openapi_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  return stream.str();
}

} // namespace

TEST(schema_objects_get_json_schema_order) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "schemas": {
        "User": {
          "type": "object",
          "description": "A user",
          "title": "User"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "schemas": {
      "User": {
        "title": "User",
        "description": "A user",
        "type": "object"
      }
    }
  }
})JSON");
}

TEST(field_order_document) {
  EXPECT_EQ(format(R"JSON({
    "components": {},
    "paths": {},
    "tags": [],
    "servers": [],
    "security": [],
    "externalDocs": { "url": "https://example.com" },
    "x-audience": "public",
    "info": { "title": "Example", "version": "1.0.0" },
    "jsonSchemaDialect": "https://json-schema.org/draft/2020-12/schema",
    "openapi": "3.1.1"
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "jsonSchemaDialect": "https://json-schema.org/draft/2020-12/schema",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "x-audience": "public",
  "externalDocs": {
    "url": "https://example.com"
  },
  "security": [],
  "servers": [],
  "tags": [],
  "paths": {},
  "components": {}
})JSON");
}

TEST(field_order_info) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": {
      "license": { "name": "MIT" },
      "contact": { "name": "Support" },
      "x-internal": true,
      "termsOfService": "https://example.com/terms",
      "description": "An example",
      "summary": "Example",
      "version": "1.0.0",
      "title": "Example"
    },
    "paths": {}
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0",
    "summary": "Example",
    "description": "An example",
    "termsOfService": "https://example.com/terms",
    "x-internal": true,
    "contact": {
      "name": "Support"
    },
    "license": {
      "name": "MIT"
    }
  },
  "paths": {}
})JSON");
}

TEST(field_order_contact) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": {
      "title": "Example",
      "version": "1.0.0",
      "contact": {
        "email": "support@example.com",
        "url": "https://example.com/support",
        "x-team": "platform",
        "name": "Support"
      }
    },
    "paths": {}
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0",
    "contact": {
      "name": "Support",
      "x-team": "platform",
      "url": "https://example.com/support",
      "email": "support@example.com"
    }
  },
  "paths": {}
})JSON");
}

TEST(field_order_license) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": {
      "title": "Example",
      "version": "1.0.0",
      "license": {
        "x-approved": true,
        "identifier": "MIT",
        "name": "MIT License"
      }
    },
    "paths": {}
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0",
    "license": {
      "name": "MIT License",
      "identifier": "MIT",
      "x-approved": true
    }
  },
  "paths": {}
})JSON");
}

TEST(field_order_license_url_is_substance) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": {
      "title": "Example",
      "version": "1.0.0",
      "license": {
        "url": "https://example.com/license",
        "name": "MIT License"
      }
    },
    "paths": {}
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0",
    "license": {
      "name": "MIT License",
      "url": "https://example.com/license"
    }
  },
  "paths": {}
})JSON");
}

TEST(field_order_external_documentation) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "externalDocs": {
      "url": "https://docs.example.com",
      "x-audience": "public",
      "description": "The manual"
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "externalDocs": {
    "description": "The manual",
    "x-audience": "public",
    "url": "https://docs.example.com"
  },
  "paths": {}
})JSON");
}

TEST(field_order_operation) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "callbacks": {},
          "responses": { "200": { "description": "Present" } },
          "requestBody": { "content": {} },
          "parameters": [],
          "servers": [ { "url": "https://example.com" } ],
          "security": [],
          "tags": [ "users" ],
          "x-code-samples": [],
          "deprecated": false,
          "externalDocs": { "url": "https://docs.example.com" },
          "description": "Lists the users",
          "summary": "List users",
          "operationId": "listUsers"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "operationId": "listUsers",
        "summary": "List users",
        "description": "Lists the users",
        "externalDocs": {
          "url": "https://docs.example.com"
        },
        "deprecated": false,
        "x-code-samples": [],
        "tags": [ "users" ],
        "security": [],
        "servers": [
          {
            "url": "https://example.com"
          }
        ],
        "parameters": [],
        "requestBody": {
          "content": {}
        },
        "responses": {
          "200": {
            "description": "Present"
          }
        },
        "callbacks": {}
      }
    }
  }
})JSON");
}

TEST(field_order_path_item) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "parameters": [],
        "servers": [ { "url": "https://example.com" } ],
        "x-owner": "platform",
        "description": "The users collection",
        "summary": "Users"
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "summary": "Users",
      "description": "The users collection",
      "x-owner": "platform",
      "servers": [
        {
          "url": "https://example.com"
        }
      ],
      "parameters": []
    }
  }
})JSON");
}

TEST(field_order_path_item_methods) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "trace": { "responses": { "200": { "description": "Traced" } } },
        "options": { "responses": { "200": { "description": "Optioned" } } },
        "delete": { "responses": { "200": { "description": "Deleted" } } },
        "patch": { "responses": { "200": { "description": "Patched" } } },
        "put": { "responses": { "200": { "description": "Put" } } },
        "post": { "responses": { "200": { "description": "Posted" } } },
        "head": { "responses": { "200": { "description": "Headed" } } },
        "get": { "responses": { "200": { "description": "Got" } } }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "responses": {
          "200": {
            "description": "Got"
          }
        }
      },
      "head": {
        "responses": {
          "200": {
            "description": "Headed"
          }
        }
      },
      "post": {
        "responses": {
          "200": {
            "description": "Posted"
          }
        }
      },
      "put": {
        "responses": {
          "200": {
            "description": "Put"
          }
        }
      },
      "patch": {
        "responses": {
          "200": {
            "description": "Patched"
          }
        }
      },
      "delete": {
        "responses": {
          "200": {
            "description": "Deleted"
          }
        }
      },
      "options": {
        "responses": {
          "200": {
            "description": "Optioned"
          }
        }
      },
      "trace": {
        "responses": {
          "200": {
            "description": "Traced"
          }
        }
      }
    }
  }
})JSON");
}

TEST(field_order_request_body) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "post": {
          "requestBody": {
            "content": {},
            "x-owner": "platform",
            "required": true,
            "description": "A user"
          },
          "responses": { "200": { "description": "Created" } }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "post": {
        "requestBody": {
          "description": "A user",
          "required": true,
          "x-owner": "platform",
          "content": {}
        },
        "responses": {
          "200": {
            "description": "Created"
          }
        }
      }
    }
  }
})JSON");
}

TEST(field_order_response) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "responses": {
            "200": {
              "content": {},
              "links": {},
              "headers": {},
              "x-cache": 300,
              "description": "Some users"
            }
          }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "responses": {
          "200": {
            "description": "Some users",
            "x-cache": 300,
            "headers": {},
            "links": {},
            "content": {}
          }
        }
      }
    }
  }
})JSON");
}

TEST(field_order_parameter_schema_form) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "parameters": [
            {
              "schema": { "type": "array" },
              "allowReserved": false,
              "allowEmptyValue": false,
              "explode": true,
              "style": "form",
              "x-owner": "platform",
              "example": [ "a" ],
              "deprecated": false,
              "required": false,
              "description": "Filter by tag",
              "in": "query",
              "name": "tags"
            }
          ],
          "responses": { "200": { "description": "Present" } }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "parameters": [
          {
            "name": "tags",
            "in": "query",
            "description": "Filter by tag",
            "required": false,
            "deprecated": false,
            "example": [ "a" ],
            "x-owner": "platform",
            "style": "form",
            "explode": true,
            "allowEmptyValue": false,
            "allowReserved": false,
            "schema": {
              "type": "array"
            }
          }
        ],
        "responses": {
          "200": {
            "description": "Present"
          }
        }
      }
    }
  }
})JSON");
}

TEST(field_order_parameter_content_form) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "parameters": [
            {
              "content": { "application/json": {} },
              "x-owner": "platform",
              "deprecated": false,
              "required": true,
              "description": "A filter",
              "in": "header",
              "name": "X-Filter"
            }
          ],
          "responses": { "200": { "description": "Present" } }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "parameters": [
          {
            "name": "X-Filter",
            "in": "header",
            "description": "A filter",
            "required": true,
            "deprecated": false,
            "x-owner": "platform",
            "content": {
              "application/json": {}
            }
          }
        ],
        "responses": {
          "200": {
            "description": "Present"
          }
        }
      }
    }
  }
})JSON");
}

TEST(field_order_media_type) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "responses": {
            "200": {
              "description": "Present",
              "content": {
                "application/json": {
                  "schema": { "type": "object" },
                  "encoding": {},
                  "x-owner": "platform",
                  "example": { "id": "usr_1" }
                }
              }
            }
          }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "responses": {
          "200": {
            "description": "Present",
            "content": {
              "application/json": {
                "example": {
                  "id": "usr_1"
                },
                "x-owner": "platform",
                "encoding": {},
                "schema": {
                  "type": "object"
                }
              }
            }
          }
        }
      }
    }
  }
})JSON");
}

TEST(field_order_reference) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "responses": {
            "200": {
              "zzz": "ignored",
              "x-thing": "also ignored",
              "$ref": "#/components/responses/Users",
              "description": "An override",
              "summary": "The users"
            }
          }
        }
      }
    },
    "components": {
      "responses": {
        "Users": { "description": "Some users" }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "responses": {
          "200": {
            "summary": "The users",
            "description": "An override",
            "$ref": "#/components/responses/Users",
            "x-thing": "also ignored",
            "zzz": "ignored"
          }
        }
      }
    }
  },
  "components": {
    "responses": {
      "Users": {
        "description": "Some users"
      }
    }
  }
})JSON");
}

TEST(field_order_parameter_examples_sit_with_metadata) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "parameters": [
            {
              "schema": { "type": "string" },
              "examples": { "one": { "value": "a" } },
              "name": "tag",
              "in": "query"
            }
          ],
          "responses": { "200": { "description": "Present" } }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "parameters": [
          {
            "name": "tag",
            "in": "query",
            "examples": {
              "one": {
                "value": "a"
              }
            },
            "schema": {
              "type": "string"
            }
          }
        ],
        "responses": {
          "200": {
            "description": "Present"
          }
        }
      }
    }
  }
})JSON");
}

TEST(field_order_components) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "operationId": "listUsers",
          "responses": { "200": { "description": "Present" } }
        }
      }
    },
    "components": {
      "schemas": { "User": true },
      "pathItems": { "Users": {} },
      "callbacks": { "OnUser": {} },
      "links": { "Next": { "operationId": "listUsers" } },
      "securitySchemes": { "apiKey": { "type": "apiKey", "name": "X-Key", "in": "header" } },
      "headers": { "ETag": { "schema": { "type": "string" } } },
      "requestBodies": { "User": { "content": {} } },
      "examples": { "One": { "value": 1 } },
      "parameters": { "Tag": { "name": "tag", "in": "query", "schema": true } },
      "responses": { "Users": { "description": "Some users" } },
      "x-registry": "internal"
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "operationId": "listUsers",
        "responses": {
          "200": {
            "description": "Present"
          }
        }
      }
    }
  },
  "components": {
    "x-registry": "internal",
    "responses": {
      "Users": {
        "description": "Some users"
      }
    },
    "parameters": {
      "Tag": {
        "name": "tag",
        "in": "query",
        "schema": true
      }
    },
    "examples": {
      "One": {
        "value": 1
      }
    },
    "requestBodies": {
      "User": {
        "content": {}
      }
    },
    "headers": {
      "ETag": {
        "schema": {
          "type": "string"
        }
      }
    },
    "securitySchemes": {
      "apiKey": {
        "type": "apiKey",
        "name": "X-Key",
        "in": "header"
      }
    },
    "links": {
      "Next": {
        "operationId": "listUsers"
      }
    },
    "callbacks": {
      "OnUser": {}
    },
    "pathItems": {
      "Users": {}
    },
    "schemas": {
      "User": true
    }
  }
})JSON");
}

TEST(field_order_example_inline_value) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "examples": {
        "One": {
          "value": { "id": "usr_1" },
          "x-owner": "platform",
          "description": "A user",
          "summary": "One user"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "examples": {
      "One": {
        "summary": "One user",
        "description": "A user",
        "x-owner": "platform",
        "value": {
          "id": "usr_1"
        }
      }
    }
  }
})JSON");
}

TEST(field_order_example_external_value_leads_the_substance) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "examples": {
        "One": {
          "externalValue": "https://example.com/user.json",
          "summary": "One user"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "examples": {
      "One": {
        "summary": "One user",
        "externalValue": "https://example.com/user.json"
      }
    }
  }
})JSON");
}

TEST(field_order_header) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "headers": {
        "ETag": {
          "schema": { "type": "string" },
          "explode": false,
          "style": "simple",
          "x-owner": "platform",
          "example": "abc",
          "deprecated": false,
          "required": true,
          "description": "The entity tag"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "headers": {
      "ETag": {
        "description": "The entity tag",
        "required": true,
        "deprecated": false,
        "example": "abc",
        "x-owner": "platform",
        "style": "simple",
        "explode": false,
        "schema": {
          "type": "string"
        }
      }
    }
  }
})JSON");
}

TEST(field_order_link) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "operationId": "listUsers",
          "responses": { "200": { "description": "Present" } }
        }
      }
    },
    "components": {
      "links": {
        "Next": {
          "requestBody": { "page": 2 },
          "server": { "url": "https://example.com" },
          "parameters": { "page": 2 },
          "x-owner": "platform",
          "description": "The next page",
          "operationId": "listUsers"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "operationId": "listUsers",
        "responses": {
          "200": {
            "description": "Present"
          }
        }
      }
    }
  },
  "components": {
    "links": {
      "Next": {
        "operationId": "listUsers",
        "description": "The next page",
        "x-owner": "platform",
        "parameters": {
          "page": 2
        },
        "server": {
          "url": "https://example.com"
        },
        "requestBody": {
          "page": 2
        }
      }
    }
  }
})JSON");
}

TEST(field_order_security_scheme_api_key) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "securitySchemes": {
        "apiKey": {
          "in": "header",
          "name": "X-Key",
          "x-owner": "platform",
          "description": "A key",
          "type": "apiKey"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "securitySchemes": {
      "apiKey": {
        "type": "apiKey",
        "description": "A key",
        "x-owner": "platform",
        "name": "X-Key",
        "in": "header"
      }
    }
  }
})JSON");
}

TEST(field_order_security_scheme_http_bearer) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "securitySchemes": {
        "bearer": {
          "bearerFormat": "JWT",
          "scheme": "bearer",
          "description": "A token",
          "type": "http"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "securitySchemes": {
      "bearer": {
        "type": "http",
        "description": "A token",
        "scheme": "bearer",
        "bearerFormat": "JWT"
      }
    }
  }
})JSON");
}

TEST(field_order_security_scheme_openid_connect) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "securitySchemes": {
        "oidc": {
          "openIdConnectUrl": "https://example.com/.well-known/openid-configuration",
          "description": "Connect",
          "type": "openIdConnect"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "securitySchemes": {
      "oidc": {
        "type": "openIdConnect",
        "description": "Connect",
        "openIdConnectUrl": "https://example.com/.well-known/openid-configuration"
      }
    }
  }
})JSON");
}

TEST(field_order_oauth_flows_and_flow) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "securitySchemes": {
        "oauth": {
          "type": "oauth2",
          "flows": {
            "authorizationCode": {
              "scopes": { "write": "Write", "read": "Read" },
              "refreshUrl": "https://example.com/refresh",
              "tokenUrl": "https://example.com/token",
              "authorizationUrl": "https://example.com/authorize"
            },
            "x-vendor": "internal"
          }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "securitySchemes": {
      "oauth": {
        "type": "oauth2",
        "flows": {
          "x-vendor": "internal",
          "authorizationCode": {
            "authorizationUrl": "https://example.com/authorize",
            "tokenUrl": "https://example.com/token",
            "refreshUrl": "https://example.com/refresh",
            "scopes": {
              "read": "Read",
              "write": "Write"
            }
          }
        }
      }
    }
  }
})JSON");
}

TEST(map_order_paths_puts_extensions_last) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "x-audience": "public",
      "/zebra": {},
      "/apple": {}
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/apple": {},
    "/zebra": {},
    "x-audience": "public"
  }
})JSON");
}

TEST(map_order_component_schemas) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "schemas": {
        "Zebra": true,
        "Apple": true
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "schemas": {
      "Apple": true,
      "Zebra": true
    }
  }
})JSON");
}

TEST(map_order_responses_puts_default_after_codes) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "responses": {
            "default": { "description": "Anything else" },
            "5XX": { "description": "Server error" },
            "404": { "description": "Absent" },
            "200": { "description": "Present" }
          }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "responses": {
          "200": {
            "description": "Present"
          },
          "404": {
            "description": "Absent"
          },
          "5XX": {
            "description": "Server error"
          },
          "default": {
            "description": "Anything else"
          }
        }
      }
    }
  }
})JSON");
}

TEST(map_order_content_and_headers_and_links) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "operationId": "listUsers",
          "responses": {
            "200": {
              "description": "Present",
              "headers": {
                "X-Total": { "schema": true },
                "ETag": { "schema": true }
              },
              "links": {
                "Prev": { "operationId": "listUsers" },
                "Next": { "operationId": "listUsers" }
              },
              "content": {
                "text/plain": {},
                "application/json": {}
              }
            }
          }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "operationId": "listUsers",
        "responses": {
          "200": {
            "description": "Present",
            "headers": {
              "ETag": {
                "schema": true
              },
              "X-Total": {
                "schema": true
              }
            },
            "links": {
              "Next": {
                "operationId": "listUsers"
              },
              "Prev": {
                "operationId": "listUsers"
              }
            },
            "content": {
              "application/json": {},
              "text/plain": {}
            }
          }
        }
      }
    }
  }
})JSON");
}

TEST(a_schema_property_named_like_a_keyword_keeps_its_order) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "schemas": {
        "Request": {
          "type": "object",
          "properties": {
            "responses": { "type": "string" },
            "get": { "type": "string" },
            "parameters": { "type": "string" }
          }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "schemas": {
      "Request": {
        "type": "object",
        "properties": {
          "responses": {
            "type": "string"
          },
          "get": {
            "type": "string"
          },
          "parameters": {
            "type": "string"
          }
        }
      }
    }
  }
})JSON");
}

TEST(schema_containers_and_arrays_keep_authored_order) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "schemas": {
        "User": {
          "type": "object",
          "required": [ "name", "id" ],
          "properties": {
            "name": { "type": "string" },
            "id": { "type": "string" }
          },
          "$defs": {
            "Zebra": true,
            "Apple": true
          }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "schemas": {
      "User": {
        "type": "object",
        "required": [ "name", "id" ],
        "properties": {
          "name": {
            "type": "string"
          },
          "id": {
            "type": "string"
          }
        },
        "$defs": {
          "Zebra": true,
          "Apple": true
        }
      }
    }
  }
})JSON");
}

TEST(operation_arrays_keep_authored_order) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "tags": [ "zebra", "apple" ],
          "servers": [
            { "url": "https://zebra.example.com" },
            { "url": "https://apple.example.com" }
          ],
          "responses": { "200": { "description": "Present" } }
        }
      }
    },
    "tags": [ { "name": "zebra" }, { "name": "apple" } ]
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "tags": [
    {
      "name": "zebra"
    },
    {
      "name": "apple"
    }
  ],
  "paths": {
    "/users": {
      "get": {
        "tags": [ "zebra", "apple" ],
        "servers": [
          {
            "url": "https://zebra.example.com"
          },
          {
            "url": "https://apple.example.com"
          }
        ],
        "responses": {
          "200": {
            "description": "Present"
          }
        }
      }
    }
  }
})JSON");
}

TEST(a_boolean_subschema_does_not_crash) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": { "schemas": { "Anything": true } }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "schemas": {
      "Anything": true
    }
  }
})JSON");
}

TEST(a_schema_is_ordered_against_the_declared_dialect) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "jsonSchemaDialect": "https://json-schema.org/draft/2020-12/schema",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "schemas": {
        "User": {
          "$defs": { "Name": true },
          "type": "object",
          "description": "A user",
          "title": "User"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "jsonSchemaDialect": "https://json-schema.org/draft/2020-12/schema",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "schemas": {
      "User": {
        "title": "User",
        "description": "A user",
        "type": "object",
        "$defs": {
          "Name": true
        }
      }
    }
  }
})JSON");
}

TEST(member_order_does_not_change_the_result) {
  const auto one{format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};
  const auto other{format(R"JSON({
    "paths": {},
    "info": { "version": "1.0.0", "title": "Example" },
    "openapi": "3.1.1"
  })JSON")};
  EXPECT_EQ(one, other);
}

TEST(map_order_responses_groups_a_range_with_its_own_class) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "responses": {
            "x-audience": "public",
            "default": { "description": "Anything else" },
            "3XX": { "description": "Redirected" },
            "300": { "description": "Choices" },
            "2XX": { "description": "Any success" },
            "201": { "description": "Created" },
            "200": { "description": "Present" }
          }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.1.1",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "get": {
        "responses": {
          "200": {
            "description": "Present"
          },
          "201": {
            "description": "Created"
          },
          "2XX": {
            "description": "Any success"
          },
          "300": {
            "description": "Choices"
          },
          "3XX": {
            "description": "Redirected"
          },
          "default": {
            "description": "Anything else"
          },
          "x-audience": "public"
        }
      }
    }
  }
})JSON");
}

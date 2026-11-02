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

TEST(field_order_document_with_self) {
  EXPECT_EQ(format(R"JSON({
    "paths": {},
    "webhooks": {},
    "info": { "title": "Example", "version": "1.0.0" },
    "$self": "https://example.com/openapi.json",
    "openapi": "3.2.0"
  })JSON"),
            R"JSON({
  "openapi": "3.2.0",
  "$self": "https://example.com/openapi.json",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "webhooks": {},
  "paths": {}
})JSON");
}

TEST(field_order_server) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.2.0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "servers": [
      {
        "variables": { "region": { "default": "us-east-1" } },
        "url": "https://{region}.api.example.com",
        "x-tier": "production",
        "description": "The production host",
        "name": "production"
      }
    ]
  })JSON"),
            R"JSON({
  "openapi": "3.2.0",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "servers": [
    {
      "name": "production",
      "description": "The production host",
      "x-tier": "production",
      "url": "https://{region}.api.example.com",
      "variables": {
        "region": {
          "default": "us-east-1"
        }
      }
    }
  ],
  "paths": {}
})JSON");
}

TEST(field_order_server_variable) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.2.0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "servers": [
      {
        "url": "https://{region}.api.example.com",
        "variables": {
          "region": {
            "enum": [ "us-east-1", "eu-west-1" ],
            "default": "us-east-1",
            "x-owner": "platform",
            "description": "The deployment region"
          }
        }
      }
    ]
  })JSON"),
            R"JSON({
  "openapi": "3.2.0",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "servers": [
    {
      "url": "https://{region}.api.example.com",
      "variables": {
        "region": {
          "description": "The deployment region",
          "x-owner": "platform",
          "default": "us-east-1",
          "enum": [ "us-east-1", "eu-west-1" ]
        }
      }
    }
  ],
  "paths": {}
})JSON");
}

TEST(field_order_tag) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.2.0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "tags": [
      { "name": "root" },
      {
        "x-display": "Users",
        "externalDocs": { "url": "https://docs.example.com/users" },
        "description": "Everything about users",
        "summary": "Users",
        "kind": "nav",
        "parent": "root",
        "name": "users"
      }
    ]
  })JSON"),
            R"JSON({
  "openapi": "3.2.0",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "tags": [
    {
      "name": "root"
    },
    {
      "name": "users",
      "parent": "root",
      "kind": "nav",
      "summary": "Users",
      "description": "Everything about users",
      "externalDocs": {
        "url": "https://docs.example.com/users"
      },
      "x-display": "Users"
    }
  ],
  "paths": {}
})JSON");
}

TEST(field_order_path_item_with_query_and_additional_operations) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.2.0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "additionalOperations": {
          "LOCK": { "responses": { "200": { "description": "Locked" } } },
          "COPY": { "responses": { "200": { "description": "Copied" } } }
        },
        "trace": { "responses": { "200": { "description": "Traced" } } },
        "query": { "responses": { "200": { "description": "Queried" } } },
        "get": { "responses": { "200": { "description": "Got" } } }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.2.0",
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
      "query": {
        "responses": {
          "200": {
            "description": "Queried"
          }
        }
      },
      "trace": {
        "responses": {
          "200": {
            "description": "Traced"
          }
        }
      },
      "additionalOperations": {
        "COPY": {
          "responses": {
            "200": {
              "description": "Copied"
            }
          }
        },
        "LOCK": {
          "responses": {
            "200": {
              "description": "Locked"
            }
          }
        }
      }
    }
  }
})JSON");
}

TEST(field_order_response_with_summary) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.2.0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "responses": {
            "200": {
              "content": {},
              "description": "Some users",
              "summary": "The users"
            }
          }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.2.0",
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
            "description": "Some users",
            "content": {}
          }
        }
      }
    }
  }
})JSON");
}

TEST(field_order_components_with_media_types) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.2.0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "schemas": { "User": true },
      "mediaTypes": { "Json": { "schema": true } },
      "requestBodies": { "User": { "content": {} } }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.2.0",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "requestBodies": {
      "User": {
        "content": {}
      }
    },
    "mediaTypes": {
      "Json": {
        "schema": true
      }
    },
    "schemas": {
      "User": true
    }
  }
})JSON");
}

TEST(field_order_security_scheme_oauth2_with_metadata_url) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.2.0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "securitySchemes": {
        "oauth": {
          "flows": {
            "deviceAuthorization": {
              "scopes": {},
              "tokenUrl": "https://example.com/token",
              "deviceAuthorizationUrl": "https://example.com/device"
            }
          },
          "oauth2MetadataUrl": "https://example.com/.well-known/oauth",
          "deprecated": false,
          "description": "An OAuth scheme",
          "type": "oauth2"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.2.0",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "securitySchemes": {
      "oauth": {
        "type": "oauth2",
        "description": "An OAuth scheme",
        "deprecated": false,
        "oauth2MetadataUrl": "https://example.com/.well-known/oauth",
        "flows": {
          "deviceAuthorization": {
            "deviceAuthorizationUrl": "https://example.com/device",
            "tokenUrl": "https://example.com/token",
            "scopes": {}
          }
        }
      }
    }
  }
})JSON");
}

TEST(field_order_example_with_data_value) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.2.0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "examples": {
        "One": {
          "serializedValue": "id=1",
          "dataValue": { "id": 1 },
          "summary": "One user"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.2.0",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "examples": {
      "One": {
        "summary": "One user",
        "dataValue": {
          "id": 1
        },
        "serializedValue": "id=1"
      }
    }
  }
})JSON");
}

TEST(field_order_media_type_with_item_schema) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.2.0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "responses": {
            "200": {
              "description": "Present",
              "content": {
                "application/jsonl": {
                  "itemSchema": { "type": "object", "title": "One" },
                  "schema": { "type": "array" },
                  "itemEncoding": { "contentType": "application/json" },
                  "example": "line"
                }
              }
            }
          }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.2.0",
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
              "application/jsonl": {
                "example": "line",
                "itemEncoding": {
                  "contentType": "application/json"
                },
                "schema": {
                  "type": "array"
                },
                "itemSchema": {
                  "title": "One",
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

TEST(field_order_encoding) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.2.0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "post": {
          "requestBody": {
            "content": {
              "multipart/form-data": {
                "schema": { "type": "object" },
                "encoding": {
                  "avatar": {
                    "headers": { "X-Size": { "schema": true } },
                    "allowReserved": false,
                    "explode": false,
                    "style": "form",
                    "x-limit": "5MB",
                    "contentType": "image/png"
                  }
                }
              }
            }
          },
          "responses": { "200": { "description": "Created" } }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.2.0",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {
    "/users": {
      "post": {
        "requestBody": {
          "content": {
            "multipart/form-data": {
              "encoding": {
                "avatar": {
                  "x-limit": "5MB",
                  "contentType": "image/png",
                  "style": "form",
                  "explode": false,
                  "allowReserved": false,
                  "headers": {
                    "X-Size": {
                      "schema": true
                    }
                  }
                }
              },
              "schema": {
                "type": "object"
              }
            }
          }
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

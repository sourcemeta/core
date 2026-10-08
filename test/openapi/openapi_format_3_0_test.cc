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

TEST(schema_objects_get_the_order_of_this_revision) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.0.4",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "schemas": {
        "Pet": {
          "xml": { "name": "Pet" },
          "properties": {
            "age": { "minimum": 0, "exclusiveMinimum": true, "type": "integer" },
            "kind": { "type": "string" }
          },
          "discriminator": { "propertyName": "kind" },
          "nullable": true,
          "type": "object",
          "externalDocs": { "url": "https://example.com/pets" },
          "example": { "kind": "dog" },
          "description": "A pet"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.0.4",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "paths": {},
  "components": {
    "schemas": {
      "Pet": {
        "description": "A pet",
        "example": {
          "kind": "dog"
        },
        "externalDocs": {
          "url": "https://example.com/pets"
        },
        "type": "object",
        "nullable": true,
        "discriminator": {
          "propertyName": "kind"
        },
        "properties": {
          "age": {
            "type": "integer",
            "exclusiveMinimum": true,
            "minimum": 0
          },
          "kind": {
            "type": "string"
          }
        },
        "xml": {
          "name": "Pet"
        }
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
    "openapi": "3.0.4"
  })JSON"),
            R"JSON({
  "openapi": "3.0.4",
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
    "openapi": "3.0.4",
    "info": {
      "license": {
        "url": "https://example.com/license",
        "x-approved": true,
        "name": "MIT"
      },
      "contact": {
        "email": "support@example.com",
        "url": "https://example.com/support",
        "x-team": "api",
        "name": "Support"
      },
      "x-internal": true,
      "termsOfService": "https://example.com/terms",
      "description": "An example",
      "version": "1.0.0",
      "title": "Example"
    },
    "paths": {}
  })JSON"),
            R"JSON({
  "openapi": "3.0.4",
  "info": {
    "title": "Example",
    "version": "1.0.0",
    "description": "An example",
    "termsOfService": "https://example.com/terms",
    "x-internal": true,
    "contact": {
      "name": "Support",
      "x-team": "api",
      "url": "https://example.com/support",
      "email": "support@example.com"
    },
    "license": {
      "name": "MIT",
      "x-approved": true,
      "url": "https://example.com/license"
    }
  },
  "paths": {}
})JSON");
}

TEST(field_order_components) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.0.4",
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
      "schemas": { "User": { "type": "object" } },
      "callbacks": { "OnUser": {} },
      "links": { "Next": { "operationId": "listUsers" } },
      "securitySchemes": { "apiKey": { "type": "apiKey", "name": "X-Key", "in": "header" } },
      "headers": { "ETag": { "schema": { "type": "string" } } },
      "requestBodies": { "User": { "content": {} } },
      "examples": { "One": { "value": 1 } },
      "parameters": { "Tag": { "name": "tag", "in": "query", "schema": { "type": "string" } } },
      "responses": { "Users": { "description": "Some users" } },
      "x-registry": "internal"
    }
  })JSON"),
            R"JSON({
  "openapi": "3.0.4",
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
        "schema": {
          "type": "string"
        }
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
    "schemas": {
      "User": {
        "type": "object"
      }
    }
  }
})JSON");
}

TEST(field_order_path_item_and_its_methods) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.0.4",
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
        "get": { "responses": { "200": { "description": "Got" } } },
        "parameters": [],
        "servers": [ { "url": "https://example.com" } ],
        "x-owner": "platform",
        "description": "The users collection",
        "summary": "Users"
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.0.4",
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
      "parameters": [],
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

TEST(field_order_response) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.0.4",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "operationId": "listUsers",
          "responses": {
            "200": {
              "content": { "application/json": { "schema": { "type": "array" } } },
              "links": { "Next": { "operationId": "listUsers" } },
              "headers": { "ETag": { "schema": { "type": "string" } } },
              "x-owner": "platform",
              "description": "Present"
            }
          }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.0.4",
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
            "x-owner": "platform",
            "headers": {
              "ETag": {
                "schema": {
                  "type": "string"
                }
              }
            },
            "links": {
              "Next": {
                "operationId": "listUsers"
              }
            },
            "content": {
              "application/json": {
                "schema": {
                  "type": "array"
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

TEST(field_order_example) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.0.4",
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
  "openapi": "3.0.4",
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

TEST(field_order_media_type_and_encoding) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.0.4",
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
                    "headers": { "X-Rate": { "schema": { "type": "string" } } },
                    "allowReserved": false,
                    "explode": false,
                    "style": "form",
                    "contentType": "image/png",
                    "x-owner": "platform"
                  }
                },
                "x-owner": "platform",
                "example": { "name": "Fido" }
              }
            }
          },
          "responses": { "200": { "description": "Present" } }
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.0.4",
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
              "example": {
                "name": "Fido"
              },
              "x-owner": "platform",
              "encoding": {
                "avatar": {
                  "x-owner": "platform",
                  "contentType": "image/png",
                  "style": "form",
                  "explode": false,
                  "allowReserved": false,
                  "headers": {
                    "X-Rate": {
                      "schema": {
                        "type": "string"
                      }
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
            "description": "Present"
          }
        }
      }
    }
  }
})JSON");
}

TEST(field_order_security_scheme_oauth2) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.0.4",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "securitySchemes": {
        "oauth": {
          "flows": {
            "authorizationCode": {
              "scopes": { "write": "Write", "read": "Read" },
              "refreshUrl": "https://example.com/refresh",
              "tokenUrl": "https://example.com/token",
              "authorizationUrl": "https://example.com/authorize"
            },
            "clientCredentials": {
              "scopes": {},
              "tokenUrl": "https://example.com/token"
            },
            "password": {
              "scopes": {},
              "tokenUrl": "https://example.com/token"
            },
            "implicit": {
              "scopes": {},
              "authorizationUrl": "https://example.com/authorize"
            },
            "x-vendor": "internal"
          },
          "x-owner": "platform",
          "description": "An OAuth scheme",
          "type": "oauth2"
        }
      }
    }
  })JSON"),
            R"JSON({
  "openapi": "3.0.4",
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
        "x-owner": "platform",
        "flows": {
          "x-vendor": "internal",
          "implicit": {
            "authorizationUrl": "https://example.com/authorize",
            "scopes": {}
          },
          "password": {
            "tokenUrl": "https://example.com/token",
            "scopes": {}
          },
          "clientCredentials": {
            "tokenUrl": "https://example.com/token",
            "scopes": {}
          },
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

TEST(field_order_server_whose_url_carries_a_query) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.0.4",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "servers": [
      {
        "variables": {
          "region": {
            "enum": [ "us-east-1", "eu-west-1" ],
            "default": "us-east-1",
            "x-owner": "platform",
            "description": "The deployment region"
          }
        },
        "x-tier": "production",
        "url": "https://{region}.api.example.com?tenant=acme",
        "description": "The production host"
      }
    ]
  })JSON"),
            R"JSON({
  "openapi": "3.0.4",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "servers": [
    {
      "description": "The production host",
      "x-tier": "production",
      "url": "https://{region}.api.example.com?tenant=acme",
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
    "openapi": "3.0.4",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "tags": [
      {
        "x-display": "Users",
        "externalDocs": { "url": "https://docs.example.com/users" },
        "description": "Everything about users",
        "name": "users"
      }
    ]
  })JSON"),
            R"JSON({
  "openapi": "3.0.4",
  "info": {
    "title": "Example",
    "version": "1.0.0"
  },
  "tags": [
    {
      "name": "users",
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

TEST(field_order_reference_extras_this_revision_ignores) {
  EXPECT_EQ(format(R"JSON({
    "openapi": "3.0.4",
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
  "openapi": "3.0.4",
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

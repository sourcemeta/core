#include <sourcemeta/core/json.h>
#include <sourcemeta/core/openapi.h>

#include <sourcemeta/core/test.h>

#include <functional>  // std::ref
#include <string_view> // std::string_view

TEST(version_patch_zero) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(version_patch_one) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(version_patch_two) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.2",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(version_pre_release) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.0-rc0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(version_with_components_and_no_paths) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "components": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(version_with_webhooks_and_no_paths) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "webhooks": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(version_agrees_with_json_export) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
  EXPECT_EQ(frame.to_json().at("version"), sourcemeta::core::JSON{"3.1"});
}

TEST(info_required_fields_only) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.info().title, "Example");
  EXPECT_EQ(frame.info().version, "1.0.0");
  EXPECT_FALSE(frame.info().summary.has_value());
  EXPECT_FALSE(frame.info().description.has_value());
  EXPECT_FALSE(frame.info().terms_of_service.has_value());
  EXPECT_FALSE(frame.info().contact.has_value());
  EXPECT_FALSE(frame.info().license.has_value());
}

TEST(info_every_field) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": {
      "title": "Example Pet Store App",
      "summary": "A pet store manager.",
      "description": "This is an example server for a pet store.",
      "termsOfService": "https://example.com/terms/",
      "contact": {
        "name": "API Support",
        "url": "https://www.example.com/support",
        "email": "support@example.com"
      },
      "license": {
        "name": "Apache 2.0",
        "url": "https://www.apache.org/licenses/LICENSE-2.0.html"
      },
      "version": "1.0.1"
    },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.info().title, "Example Pet Store App");
  EXPECT_EQ(frame.info().version, "1.0.1");
  EXPECT_EQ(frame.info().summary.value(), "A pet store manager.");
  EXPECT_EQ(frame.info().description.value(),
            "This is an example server for a pet store.");
  EXPECT_EQ(frame.info().terms_of_service.value(),
            "https://example.com/terms/");
  EXPECT_EQ(frame.info().contact.value().name.value(), "API Support");
  EXPECT_EQ(frame.info().contact.value().url.value(),
            "https://www.example.com/support");
  EXPECT_EQ(frame.info().contact.value().email.value(), "support@example.com");
  EXPECT_EQ(frame.info().license.value().name, "Apache 2.0");
  EXPECT_FALSE(frame.info().license.value().identifier.has_value());
  EXPECT_EQ(frame.info().license.value().url.value(),
            "https://www.apache.org/licenses/LICENSE-2.0.html");
}

TEST(info_empty_string_values_are_kept) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "", "version": "", "summary": "" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.info().title, "");
  EXPECT_EQ(frame.info().version, "");
  EXPECT_TRUE(frame.info().summary.has_value());
  EXPECT_EQ(frame.info().summary.value(), "");
}

TEST(info_relative_terms_of_service) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": {
      "title": "Example",
      "version": "1.0.0",
      "termsOfService": "/terms"
    },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.info().terms_of_service.value(), "/terms");
}

TEST(info_empty_contact) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0", "contact": {} },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_TRUE(frame.info().contact.has_value());
  EXPECT_FALSE(frame.info().contact.value().name.has_value());
  EXPECT_FALSE(frame.info().contact.value().url.has_value());
  EXPECT_FALSE(frame.info().contact.value().email.has_value());
}

TEST(info_license_with_spdx_identifier) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": {
      "title": "Example",
      "version": "1.0.0",
      "license": { "name": "Apache 2.0", "identifier": "Apache-2.0" }
    },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.info().license.value().name, "Apache 2.0");
  EXPECT_EQ(frame.info().license.value().identifier.value(), "Apache-2.0");
  EXPECT_FALSE(frame.info().license.value().url.has_value());
}

TEST(info_license_identifier_is_not_validated) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": {
      "title": "Example",
      "version": "1.0.0",
      "license": { "name": "Nonsense", "identifier": "not an SPDX expression" }
    },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.info().license.value().identifier.value(),
            "not an SPDX expression");
}

TEST(info_extensions_are_accepted) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": {
      "title": "Example",
      "version": "1.0.0",
      "x-internal-id": 42,
      "x-oai-reserved": null,
      "x-oas-reserved": [ 1, 2 ],
      "x-": "the prefix alone is a legal field name",
      "contact": { "name": "Support", "x-slack": "#support" },
      "license": { "name": "MIT", "x-approved": true }
    },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.info().title, "Example");
  EXPECT_EQ(frame.info().contact.value().name.value(), "Support");
  EXPECT_EQ(frame.info().license.value().name, "MIT");
}

TEST(base_is_absent_by_default) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_TRUE(frame.base().empty());
}

TEST(base_is_what_the_caller_established) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  EXPECT_EQ(frame.base(), "https://example.com/openapi.json");
}

TEST(base_is_canonicalised) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver,
      "HTTPS://Example.COM:443/a/../openapi.json"};
  EXPECT_EQ(frame.base(), "https://example.com/openapi.json");
}

TEST(base_does_not_borrow_from_the_caller) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  // Canonicalising means the frame owns what it reports, so a base that goes
  // away afterwards cannot dangle
  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver,
      sourcemeta::core::JSON::String{"https://example.com/openapi.json"}};
  EXPECT_EQ(frame.base(), "https://example.com/openapi.json");
}

TEST(base_agrees_with_json_export) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  EXPECT_EQ(frame.to_json().at("base"),
            sourcemeta::core::JSON{"https://example.com/openapi.json"});
}

TEST(base_absent_is_the_empty_uri_reference_in_json_export) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.to_json().at("base"), sourcemeta::core::JSON{""});
}

// A resolver may hand back a document it owns rather than one the caller keeps
// alive, in which case that document is gone once it has been read. Nothing
// the frame carries on may point into it
TEST(standalone_without_references) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_TRUE(frame.standalone());
}

TEST(standalone_with_an_internal_reference) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "components": {
      "responses": {
        "Ok": { "description": "ok" },
        "R": { "$ref": "#/components/responses/Ok" }
      }
    }
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_TRUE(frame.standalone());
}

TEST(standalone_with_an_external_reference) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "components": {
      "responses": {
        "R": { "$ref": "https://example.com/common.json#/components/responses/Ok" }
      }
    }
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  EXPECT_FALSE(frame.standalone());
  EXPECT_EQ(frame.to_json()
                .at("locations")
                .at("https://example.com/openapi.json#/components/responses/R")
                .at("dangling"),
            sourcemeta::core::JSON{true});
}

TEST(schemas_frames_every_schema_object_position) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "components": {
      "schemas": {
        "Pet": { "$ref": "#/components/schemas/Order" },
        "Order": { "type": "object" }
      }
    }
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};

  EXPECT_EQ(frame.schemas().mode(),
            sourcemeta::core::SchemaFrame::Mode::References);
  EXPECT_TRUE(frame.schemas().standalone());
  EXPECT_EQ(frame.schemas().location_count(), 2);
  EXPECT_EQ(frame.schemas().reference_count(), 1);

  // Every `schema` location of the description names its part of that frame by
  // the very key it is recorded under
  const auto pet{frame.schemas().location(
      sourcemeta::core::SchemaReferenceType::Static,
      "https://example.com/openapi.json#/components/schemas/Pet")};
  EXPECT_TRUE(pet.has_value());
  EXPECT_EQ(sourcemeta::core::to_string(pet.value().get().pointer),
            "/components/schemas/Pet");
  EXPECT_EQ(pet.value().get().dialect,
            "https://spec.openapis.org/oas/3.1/dialect/base");

  const auto order{frame.schemas().location(
      sourcemeta::core::SchemaReferenceType::Static,
      "https://example.com/openapi.json#/components/schemas/Order")};
  EXPECT_TRUE(order.has_value());
  EXPECT_EQ(sourcemeta::core::to_string(order.value().get().pointer),
            "/components/schemas/Order");
}

TEST(schemas_holds_nothing_when_the_description_declares_no_schema) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};

  EXPECT_EQ(frame.schemas().location_count(), 0);
  EXPECT_EQ(frame.schemas().reference_count(), 0);
  EXPECT_TRUE(frame.schemas().standalone());
  EXPECT_TRUE(frame.schemas().root().empty());
}

TEST(schemas_does_not_stand_alone_when_a_schema_reaches_out) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "components": {
      "schemas": { "Pet": { "$ref": "https://elsewhere.test/absent" } }
    }
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};

  // Nothing of the shell dangles, so the description falls short on what its
  // Schema Objects reach for alone
  EXPECT_FALSE(frame.schemas().standalone());
  EXPECT_FALSE(frame.standalone());
  const auto result{frame.to_json()};
  EXPECT_EQ(result.at("standalone"), sourcemeta::core::JSON{false});
  EXPECT_TRUE(result.at("locations")
                  .at("https://example.com/openapi.json#/components/schemas/"
                      "Pet")
                  .defines("defaultDialect"));
}

TEST(dangling_is_empty_when_the_frame_stands_alone) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  EXPECT_TRUE(frame.standalone());
}

TEST(dangling_is_reported_on_each_reference_that_shares_a_destination) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "components": {
      "responses": {
        "R": { "$ref": "https://example.com/common.json#/components/responses/Ok" },
        "S": { "$ref": "https://example.com/common.json#/components/responses/Ok" }
      }
    }
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  const auto result{frame.to_json()};
  const auto &locations{result.at("locations")};
  EXPECT_EQ(
      locations.at("https://example.com/openapi.json#/components/responses/R")
          .at("dangling"),
      sourcemeta::core::JSON{true});
  EXPECT_EQ(
      locations.at("https://example.com/openapi.json#/components/responses/S")
          .at("dangling"),
      sourcemeta::core::JSON{true});
}

TEST(standalone_agrees_with_json_export) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "components": {
      "responses": {
        "R": { "$ref": "https://example.com/common.json#/components/responses/Ok" }
      }
    }
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  EXPECT_EQ(frame.to_json().at("standalone"), sourcemeta::core::JSON{false});
}

TEST(positions_of_every_place_the_export_names) {
  sourcemeta::core::PointerPositionTracker tracker;
  sourcemeta::core::JSON document{nullptr};
  sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.2.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "security": [ { "#/components/securitySchemes/apiKey": [] } ],
    "components": {
      "securitySchemes": {
        "apiKey": { "type": "apiKey", "name": "key", "in": "header" }
      },
      "schemas": {
        "Pet": {
          "oneOf": [ { "$ref": "#/components/schemas/Cat" } ],
          "discriminator": {
            "propertyName": "petType",
            "mapping": { "cat": "Cat" }
          }
        },
        "Cat": { "type": "object" }
      }
    },
    "paths": {}
  })JSON",
                               document, std::ref(tracker));

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  const auto result{frame.to_json(tracker)};
  const auto &locations{result.at("locations")};
  const auto &schemas{result.at("schemas").at("locations").at("static")};
  EXPECT_EQ(locations.at("https://example.com/openapi.json").at("position"),
            sourcemeta::core::parse_json("[ 1, 1, 21, 3 ]"));
  EXPECT_EQ(locations.at("https://example.com/openapi.json#/components")
                .at("position"),
            sourcemeta::core::parse_json("[ 5, 5, 19, 5 ]"));
  EXPECT_EQ(
      locations.at("https://example.com/openapi.json#/components/schemas/Cat")
          .at("position"),
      sourcemeta::core::parse_json("[ 17, 9, 17, 35 ]"));
  EXPECT_EQ(
      locations.at("https://example.com/openapi.json#/components/schemas/Pet")
          .at("position"),
      sourcemeta::core::parse_json("[ 10, 9, 16, 9 ]"));
  EXPECT_EQ(locations
                .at("https://example.com/openapi.json#/components/"
                    "securitySchemes/apiKey")
                .at("position"),
            sourcemeta::core::parse_json("[ 7, 9, 7, 69 ]"));
  EXPECT_EQ(
      locations.at("https://example.com/openapi.json#/info").at("position"),
      sourcemeta::core::parse_json("[ 3, 5, 3, 54 ]"));
  EXPECT_EQ(
      locations.at("https://example.com/openapi.json#/paths").at("position"),
      sourcemeta::core::parse_json("[ 20, 5, 20, 15 ]"));
  EXPECT_EQ(locations.at("https://example.com/openapi.json#/security/0")
                .at("position"),
            sourcemeta::core::parse_json("[ 4, 19, 4, 63 ]"));
  EXPECT_EQ(
      schemas.at("https://example.com/openapi.json#/components/schemas/Cat")
          .at("position"),
      sourcemeta::core::parse_json("[ 17, 9, 17, 35 ]"));
  EXPECT_EQ(
      schemas.at("https://example.com/openapi.json#/components/schemas/Pet")
          .at("position"),
      sourcemeta::core::parse_json("[ 10, 9, 16, 9 ]"));
  EXPECT_EQ(schemas
                .at("https://example.com/openapi.json#/components/schemas/Pet/"
                    "oneOf/0")
                .at("position"),
            sourcemeta::core::parse_json("[ 11, 22, 11, 59 ]"));
  EXPECT_EQ(result.at("schemas").at("references").at(0).at("position"),
            sourcemeta::core::parse_json("[ 11, 24, 11, 57 ]"));
  EXPECT_EQ(result.at("discriminators").at(0).at("position"),
            sourcemeta::core::parse_json("[ 14, 26, 14, 37 ]"));
  EXPECT_EQ(result.at("securityReferences").at(0).at("position"),
            sourcemeta::core::parse_json("[ 4, 21, 4, 61 ]"));
}

TEST(positions_are_none_when_the_tracker_holds_nothing) {
  const sourcemeta::core::PointerPositionTracker tracker;
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.2.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "security": [ { "#/components/securitySchemes/apiKey": [] } ],
    "components": {
      "securitySchemes": {
        "apiKey": { "type": "apiKey", "name": "key", "in": "header" }
      },
      "schemas": {
        "Pet": {
          "oneOf": [ { "$ref": "#/components/schemas/Cat" } ],
          "discriminator": {
            "propertyName": "petType",
            "mapping": { "cat": "Cat" }
          }
        },
        "Cat": { "type": "object" }
      }
    },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  const auto result{frame.to_json(tracker)};
  const auto &locations{result.at("locations")};
  const auto &schemas{result.at("schemas").at("locations").at("static")};
  EXPECT_EQ(locations.at("https://example.com/openapi.json").at("position"),
            sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(locations.at("https://example.com/openapi.json#/components")
                .at("position"),
            sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(
      locations.at("https://example.com/openapi.json#/components/schemas/Cat")
          .at("position"),
      sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(
      locations.at("https://example.com/openapi.json#/components/schemas/Pet")
          .at("position"),
      sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(locations
                .at("https://example.com/openapi.json#/components/"
                    "securitySchemes/apiKey")
                .at("position"),
            sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(
      locations.at("https://example.com/openapi.json#/info").at("position"),
      sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(
      locations.at("https://example.com/openapi.json#/paths").at("position"),
      sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(locations.at("https://example.com/openapi.json#/security/0")
                .at("position"),
            sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(
      schemas.at("https://example.com/openapi.json#/components/schemas/Cat")
          .at("position"),
      sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(
      schemas.at("https://example.com/openapi.json#/components/schemas/Pet")
          .at("position"),
      sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(schemas
                .at("https://example.com/openapi.json#/components/schemas/Pet/"
                    "oneOf/0")
                .at("position"),
            sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("schemas").at("references").at(0).at("position"),
            sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("discriminators").at(0).at("position"),
            sourcemeta::core::JSON{nullptr});
  EXPECT_EQ(result.at("securityReferences").at(0).at("position"),
            sourcemeta::core::JSON{nullptr});
}

TEST(positions_are_absent_when_no_tracker_is_given) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.2.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "security": [ { "#/components/securitySchemes/apiKey": [] } ],
    "components": {
      "securitySchemes": {
        "apiKey": { "type": "apiKey", "name": "key", "in": "header" }
      },
      "schemas": {
        "Pet": {
          "oneOf": [ { "$ref": "#/components/schemas/Cat" } ],
          "discriminator": {
            "propertyName": "petType",
            "mapping": { "cat": "Cat" }
          }
        },
        "Cat": { "type": "object" }
      }
    },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  const auto result{frame.to_json()};
  const auto &locations{result.at("locations")};
  const auto &schemas{result.at("schemas").at("locations").at("static")};
  EXPECT_FALSE(
      locations.at("https://example.com/openapi.json").defines("position"));
  EXPECT_FALSE(locations.at("https://example.com/openapi.json#/components")
                   .defines("position"));
  EXPECT_FALSE(
      locations.at("https://example.com/openapi.json#/components/schemas/Cat")
          .defines("position"));
  EXPECT_FALSE(
      locations.at("https://example.com/openapi.json#/components/schemas/Pet")
          .defines("position"));
  EXPECT_FALSE(locations
                   .at("https://example.com/openapi.json#/components/"
                       "securitySchemes/apiKey")
                   .defines("position"));
  EXPECT_FALSE(locations.at("https://example.com/openapi.json#/info")
                   .defines("position"));
  EXPECT_FALSE(locations.at("https://example.com/openapi.json#/paths")
                   .defines("position"));
  EXPECT_FALSE(locations.at("https://example.com/openapi.json#/security/0")
                   .defines("position"));
  EXPECT_FALSE(
      schemas.at("https://example.com/openapi.json#/components/schemas/Cat")
          .defines("position"));
  EXPECT_FALSE(
      schemas.at("https://example.com/openapi.json#/components/schemas/Pet")
          .defines("position"));
  EXPECT_FALSE(schemas
                   .at("https://example.com/openapi.json#/components/schemas/"
                       "Pet/oneOf/0")
                   .defines("position"));
  EXPECT_FALSE(result.at("schemas").at("references").at(0).defines("position"));
  EXPECT_FALSE(result.at("discriminators").at(0).defines("position"));
  EXPECT_FALSE(result.at("securityReferences").at(0).defines("position"));
}

TEST(kind_name_of_every_object) {
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Document),
            "openapi");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::PathItem),
            "path-item");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Parameter),
            "parameter");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::RequestBody),
            "request-body");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Response),
            "response");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Example),
            "example");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Header),
            "header");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Link),
            "link");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Callbacks),
            "callback");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::SecurityScheme),
            "security-scheme");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::MediaType),
            "media-type");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Info),
            "info");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Contact),
            "contact");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::License),
            "license");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Server),
            "server");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::ServerVariable),
            "server-variable");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Components),
            "components");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Paths),
            "paths");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Operation),
            "operation");
  EXPECT_EQ(
      sourcemeta::core::openapi_kind_name(
          sourcemeta::core::OpenAPIFrame::ObjectKind::ExternalDocumentation),
      "external-documentation");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Encoding),
            "encoding");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Responses),
            "responses");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Tag),
            "tag");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Reference),
            "reference");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::Schema),
            "schema");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::OAuthFlows),
            "oauth-flows");
  EXPECT_EQ(sourcemeta::core::openapi_kind_name(
                sourcemeta::core::OpenAPIFrame::ObjectKind::OAuthFlow),
            "oauth-flow");
  EXPECT_EQ(
      sourcemeta::core::openapi_kind_name(
          sourcemeta::core::OpenAPIFrame::ObjectKind::SecurityRequirement),
      "security-requirement");
}

TEST(kind_name_of_every_route_to_an_operation) {
  EXPECT_EQ(sourcemeta::core::openapi_operation_kind_name(
                sourcemeta::core::OpenAPIFrame::OperationKind::Path),
            "path");
  EXPECT_EQ(sourcemeta::core::openapi_operation_kind_name(
                sourcemeta::core::OpenAPIFrame::OperationKind::Webhook),
            "webhook");
  EXPECT_EQ(sourcemeta::core::openapi_operation_kind_name(
                sourcemeta::core::OpenAPIFrame::OperationKind::Callback),
            "callback");
}

TEST(parent_of_every_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": {
      "title": "Example",
      "version": "1.0.0",
      "contact": { "name": "Jane Doe" }
    },
    "paths": {
      "/people": {
        "get": { "responses": { "200": { "description": "Some people" } } }
      }
    }
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  const auto &locations{frame.locations()};
  EXPECT_FALSE(
      locations.at("https://example.com/openapi.json").parent.has_value());
  EXPECT_EQ(locations.at("https://example.com/openapi.json#/info").parent,
            "https://example.com/openapi.json");
  EXPECT_EQ(
      locations.at("https://example.com/openapi.json#/info/contact").parent,
      "https://example.com/openapi.json#/info");
  EXPECT_EQ(locations.at("https://example.com/openapi.json#/paths").parent,
            "https://example.com/openapi.json");
  EXPECT_EQ(
      locations.at("https://example.com/openapi.json#/paths/~1people").parent,
      "https://example.com/openapi.json#/paths");
  EXPECT_EQ(locations.at("https://example.com/openapi.json#/paths/~1people/get")
                .parent,
            "https://example.com/openapi.json#/paths/~1people");
  EXPECT_EQ(locations
                .at("https://example.com/openapi.json#/paths/~1people/get/"
                    "responses")
                .parent,
            "https://example.com/openapi.json#/paths/~1people/get");
  EXPECT_EQ(locations
                .at("https://example.com/openapi.json#/paths/~1people/get/"
                    "responses/200")
                .parent,
            "https://example.com/openapi.json#/paths/~1people/get/responses");
}

TEST(parent_skips_a_position_that_is_no_object_of_its_own) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "components": { "schemas": { "Person": { "type": "object" } } }
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  const auto &locations{frame.locations()};
  EXPECT_FALSE(locations.contains(
      "https://example.com/openapi.json#/components/schemas"));
  EXPECT_EQ(locations
                .at("https://example.com/openapi.json#/components/schemas/"
                    "Person")
                .parent,
            "https://example.com/openapi.json#/components");
}

TEST(version_name_of_every_revision) {
  EXPECT_EQ(sourcemeta::core::openapi_version_name(
                sourcemeta::core::OpenAPIVersion::OPENAPI_3_1),
            "3.1");
  EXPECT_EQ(sourcemeta::core::openapi_version_name(
                sourcemeta::core::OpenAPIVersion::OPENAPI_3_2),
            "3.2");
}

// The default a position inherits and what a Schema Object is actually written
// against are different questions, and they part ways exactly when the schema
// declares its own. Reading one for the other validates a schema against the
// wrong meta-schema, which passes for everything the drafts agree on
TEST(default_dialect_is_not_the_dialect_a_schema_declares) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "jsonSchemaDialect": "https://json-schema.org/draft/2020-12/schema",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "schemas": {
        "Plain": { "type": "string" },
        "Older": {
          "$schema": "http://json-schema.org/draft-07/schema#",
          "type": "string"
        }
      }
    }
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};

  const std::string older{
      "https://example.com/openapi.json#/components/schemas/Older"};
  const auto older_location{frame.locations().find(older)};
  EXPECT_TRUE(older_location != frame.locations().cend());
  EXPECT_EQ(older_location->second.default_dialect,
            "https://json-schema.org/draft/2020-12/schema");
  const auto older_schema{frame.schemas().location(
      sourcemeta::core::SchemaReferenceType::Static, older)};
  EXPECT_TRUE(older_schema.has_value());
  EXPECT_EQ(older_schema.value().get().dialect,
            "http://json-schema.org/draft-07/schema#");

  // A schema that declares nothing takes the default, so there the two agree
  const std::string plain{
      "https://example.com/openapi.json#/components/schemas/Plain"};
  const auto plain_location{frame.locations().find(plain)};
  EXPECT_TRUE(plain_location != frame.locations().cend());
  const auto plain_schema{frame.schemas().location(
      sourcemeta::core::SchemaReferenceType::Static, plain)};
  EXPECT_TRUE(plain_schema.has_value());
  EXPECT_EQ(plain_location->second.default_dialect,
            plain_schema.value().get().dialect);
}

TEST(default_dialect_export_is_distinct_from_the_schema_frame_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "jsonSchemaDialect": "https://json-schema.org/draft/2020-12/schema",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "components": {
      "schemas": {
        "Older": {
          "$schema": "http://json-schema.org/draft-07/schema#",
          "type": "string"
        }
      }
    }
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  const auto result{frame.to_json()};

  const std::string older{
      "https://example.com/openapi.json#/components/schemas/Older"};
  const auto &entry{result.at("locations").at(older)};
  EXPECT_FALSE(entry.defines("dialect"));
  EXPECT_EQ(
      entry.at("defaultDialect"),
      sourcemeta::core::JSON{"https://json-schema.org/draft/2020-12/schema"});
  EXPECT_EQ(
      result.at("schemas").at("locations").at("static").at(older).at("dialect"),
      sourcemeta::core::JSON{"http://json-schema.org/draft-07/schema#"});
}

TEST(openapi_base_without_a_self) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  EXPECT_EQ(sourcemeta::core::openapi_base(document,
                                           "https://example.com/openapi.json"),
            "https://example.com/openapi.json");

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  EXPECT_EQ(frame.base(), sourcemeta::core::openapi_base(
                              document, "https://example.com/openapi.json"));
}

TEST(openapi_base_canonicalises_the_retrieval_uri) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  EXPECT_EQ(sourcemeta::core::openapi_base(
                document, "HTTPS://Example.COM:443/./foo/../openapi.json"),
            "https://example.com/openapi.json");

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver,
      "HTTPS://Example.COM:443/./foo/../openapi.json"};
  EXPECT_EQ(frame.base(), "https://example.com/openapi.json");
}

TEST(openapi_base_without_a_retrieval_uri) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  EXPECT_EQ(sourcemeta::core::openapi_base(document, ""), "");

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.base(), sourcemeta::core::openapi_base(document, ""));
}

TEST(openapi_base_of_an_absolute_self) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.2.0",
    "$self": "https://example.com/api",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  EXPECT_EQ(sourcemeta::core::openapi_base(document,
                                           "https://example.com/openapi.json"),
            "https://example.com/api");

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  EXPECT_EQ(frame.base(), "https://example.com/api");
}

TEST(openapi_base_of_a_relative_self) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.2.0",
    "$self": "common/api.json",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  EXPECT_EQ(sourcemeta::core::openapi_base(
                document, "https://example.com/v1/openapi.json"),
            "https://example.com/v1/common/api.json");

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/v1/openapi.json"};
  EXPECT_EQ(frame.base(), "https://example.com/v1/common/api.json");
}

TEST(openapi_base_of_a_self_that_carries_a_fragment) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.2.0",
    "$self": "https://example.com/api#/info",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  EXPECT_EQ(sourcemeta::core::openapi_base(document,
                                           "https://example.com/openapi.json"),
            "https://example.com/api");

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
  EXPECT_EQ(frame.base(), "https://example.com/api");
}

TEST(openapi_base_of_a_relative_self_with_nothing_to_resolve_against) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.2.0",
    "$self": "common/api.json",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  EXPECT_EQ(sourcemeta::core::openapi_base(document, ""), "");

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.base(), "");
}

TEST(openapi_base_of_a_self_that_is_not_a_string) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.2.0",
    "$self": 42,
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  EXPECT_EQ(sourcemeta::core::openapi_base(document,
                                           "https://example.com/openapi.json"),
            "https://example.com/openapi.json");
}

TEST(openapi_base_of_a_self_under_a_revision_that_does_not_define_it) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "$self": "https://example.com/api",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  EXPECT_EQ(sourcemeta::core::openapi_base(document,
                                           "https://example.com/openapi.json"),
            "https://example.com/openapi.json");
}

TEST(openapi_base_of_a_revision_we_do_not_recognise) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.0.4",
    "$self": "https://example.com/api",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  EXPECT_EQ(sourcemeta::core::openapi_base(document,
                                           "https://example.com/openapi.json"),
            "https://example.com/openapi.json");
}

TEST(openapi_base_of_a_value_that_is_not_an_object) {
  const auto document{sourcemeta::core::parse_json("\"3.2.0\"")};

  EXPECT_EQ(sourcemeta::core::openapi_base(document,
                                           "https://example.com/openapi.json"),
            "https://example.com/openapi.json");
}

TEST(openapi_base_of_a_retrieval_uri_without_a_scheme) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  try {
    [[maybe_unused]] const auto base{
        sourcemeta::core::openapi_base(document, "example.com/openapi.json")};
    FAIL();
  } catch (const sourcemeta::core::OpenAPIError &error) {
    EXPECT_EQ(error.location(), sourcemeta::core::EMPTY_POINTER);
  }
}

TEST(openapi_base_of_a_document_that_does_not_conform) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.2.0",
    "$self": "https://example.com/api",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {},
    "typo": true
  })JSON")};

  // What a document answers to is what it declares rather than how far reading
  // it got, so a refusal over anything else still names the document by the URI
  // it declared for itself
  EXPECT_EQ(sourcemeta::core::openapi_base(document,
                                           "https://example.com/openapi.json"),
            "https://example.com/api");

  try {
    [[maybe_unused]] const sourcemeta::core::OpenAPIFrame frame{
        document, sourcemeta::core::schema_walker,
        sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
    FAIL();
  } catch (const sourcemeta::core::OpenAPIError &error) {
    EXPECT_EQ(error.base(), sourcemeta::core::openapi_base(
                                document, "https://example.com/openapi.json"));
    EXPECT_EQ(error.location(), sourcemeta::core::Pointer{"typo"});
  }
}

TEST(openapi_base_of_a_self_in_a_revision_that_rejects_the_field) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "$self": "https://example.com/api",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  EXPECT_EQ(sourcemeta::core::openapi_base(document,
                                           "https://example.com/openapi.json"),
            "https://example.com/openapi.json");

  try {
    [[maybe_unused]] const sourcemeta::core::OpenAPIFrame frame{
        document, sourcemeta::core::schema_walker,
        sourcemeta::core::schema_resolver, "https://example.com/openapi.json"};
    FAIL();
  } catch (const sourcemeta::core::OpenAPIError &error) {
    EXPECT_EQ(error.base(), sourcemeta::core::openapi_base(
                                document, "https://example.com/openapi.json"));
    EXPECT_EQ(error.location(), sourcemeta::core::Pointer{"$self"});
  }
}

TEST(openapi_base_of_an_absolute_self_without_a_retrieval_uri) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.2.0",
    "$self": "https://example.com/api",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  // Establishing nothing leaves the document to name itself, which an absolute
  // self identifier does whatever it was retrieved by
  EXPECT_EQ(sourcemeta::core::openapi_base(document, ""),
            "https://example.com/api");

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.base(), "https://example.com/api");
}

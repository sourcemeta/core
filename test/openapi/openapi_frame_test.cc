#include <sourcemeta/core/json.h>
#include <sourcemeta/core/openapi.h>

#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view
<<<<<<< HEAD
=======
#include <vector>      // std::vector

namespace {

auto description_with_everything() -> sourcemeta::core::JSON {
  return sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {
      "/users": {
        "get": {
          "operationId": "listUsers",
          "security": [ { "apiKey": [] } ],
          "responses": {
            "200": { "$ref": "#/components/responses/Users" }
          }
        }
      }
    },
    "components": {
      "securitySchemes": {
        "apiKey": { "type": "apiKey", "name": "X-Key", "in": "header" }
      },
      "responses": {
        "Users": { "description": "Some users" }
      }
    }
  })JSON");
}

auto object_kinds_of(const sourcemeta::core::OpenAPIFrame &frame)
    -> std::vector<sourcemeta::core::OpenAPIFrame::ObjectKind> {
  std::vector<sourcemeta::core::OpenAPIFrame::ObjectKind> result;
  frame.for_each_object([&result](const auto &, const auto &location) -> void {
    result.push_back(location.type);
  });
  return result;
}

auto references_of(const sourcemeta::core::OpenAPIFrame &frame)
    -> std::vector<sourcemeta::core::OpenAPIFrame::Reference> {
  std::vector<sourcemeta::core::OpenAPIFrame::Reference> result;
  frame.for_each_reference(
      [&result](const auto &, const auto &reference) -> void {
        result.push_back(reference);
      });
  return result;
}

auto security_reference_count(const sourcemeta::core::OpenAPIFrame &frame)
    -> std::size_t {
  std::size_t result{0};
  frame.for_each_security_reference(
      [&result](const auto &, const auto &) -> void { result += 1; });
  return result;
}

auto operations_of(const sourcemeta::core::OpenAPIFrame &frame)
    -> std::vector<sourcemeta::core::OpenAPIFrame::Operation> {
  std::vector<sourcemeta::core::OpenAPIFrame::Operation> result;
  frame.for_each_operation([&result](const auto &operation) -> void {
    result.push_back(operation);
  });
  return result;
}

auto discriminator_count(const sourcemeta::core::OpenAPIFrame &frame)
    -> std::size_t {
  std::size_t result{0};
  frame.for_each_discriminator(
      [&result](const auto &) -> void { result += 1; });
  return result;
}

} // namespace
>>>>>>> b0ce1bd24 (Simpler)

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
                  .defines("dialect"));
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
<<<<<<< HEAD
=======

TEST(for_each_object_reports_every_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  const auto kinds{object_kinds_of(frame)};
  EXPECT_EQ(kinds.size(), 3);
  EXPECT_EQ(kinds.at(0), sourcemeta::core::OpenAPIFrame::ObjectKind::Document);
  EXPECT_EQ(kinds.at(1), sourcemeta::core::OpenAPIFrame::ObjectKind::Info);
  EXPECT_EQ(kinds.at(2), sourcemeta::core::OpenAPIFrame::ObjectKind::Paths);
}

TEST(any_object_finds_a_kind_the_description_holds) {
  const auto document{description_with_everything()};
  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_TRUE(frame.any_object([](const auto &, const auto &location) -> bool {
    return location.type ==
           sourcemeta::core::OpenAPIFrame::ObjectKind::Operation;
  }));
}

TEST(any_object_reports_nothing_for_a_kind_it_does_not) {
  const auto document{description_with_everything()};
  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_FALSE(frame.any_object([](const auto &, const auto &location) -> bool {
    return location.type ==
           sourcemeta::core::OpenAPIFrame::ObjectKind::Encoding;
  }));
}

TEST(traverse_finds_a_reference_destination) {
  const auto document{description_with_everything()};
  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  const auto references{references_of(frame)};
  EXPECT_EQ(references.size(), 1);
  EXPECT_EQ(references.at(0).original, "#/components/responses/Users");
  EXPECT_EQ(references.at(0).destination, "#/components/responses/Users");
  EXPECT_FALSE(references.at(0).dangling);
  EXPECT_EQ(references.at(0).expected,
            sourcemeta::core::OpenAPIFrame::ObjectKind::Response);

  const auto *target{frame.traverse(references.at(0).destination)};
  EXPECT_TRUE(target != nullptr);
  EXPECT_EQ(target->type, sourcemeta::core::OpenAPIFrame::ObjectKind::Response);
}

TEST(traverse_reports_nothing_for_an_unknown_uri) {
  const auto document{description_with_everything()};
  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.traverse("#/components/responses/Nowhere"), nullptr);
}

TEST(uri_is_the_inverse_of_traverse) {
  const auto document{description_with_everything()};
  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.uri(sourcemeta::core::Pointer{"paths"}), "#/paths");
  const auto *target{
      frame.traverse(frame.uri(sourcemeta::core::Pointer{"paths"}))};
  EXPECT_TRUE(target != nullptr);
  EXPECT_EQ(target->type, sourcemeta::core::OpenAPIFrame::ObjectKind::Paths);
}

TEST(for_each_security_reference_reports_nothing_for_a_name) {
  const auto document{description_with_everything()};
  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(security_reference_count(frame), 0);
}

TEST(for_each_operation_reports_the_one_operation) {
  const auto document{description_with_everything()};
  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  const auto operations{operations_of(frame)};
  EXPECT_EQ(operations.size(), 1);
  EXPECT_EQ(operations.at(0).kind,
            sourcemeta::core::OpenAPIFrame::OperationKind::Path);
  EXPECT_EQ(operations.at(0).path, "/users");
  EXPECT_EQ(operations.at(0).method, "get");
  EXPECT_EQ(operations.at(0).origin, "#/paths/~1users/get");
  EXPECT_EQ(operations.at(0).endpoint, "#/paths/~1users");
}

TEST(for_each_discriminator_reports_nothing_without_one) {
  const auto document{description_with_everything()};
  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(discriminator_count(frame), 0);
}

TEST(object_count_matches_what_iteration_reports) {
  const auto document{description_with_everything()};
  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.object_count(), object_kinds_of(frame).size());
}

TEST(reference_count_matches_what_iteration_reports) {
  const auto document{description_with_everything()};
  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.reference_count(), references_of(frame).size());
}
>>>>>>> b0ce1bd24 (Simpler)

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/openapi.h>

#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

TEST(version_patch_zero) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(version_patch_one) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(version_patch_two) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.2",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(version_pre_release) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.0-rc0",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(version_with_components_and_no_paths) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "components": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(version_with_webhooks_and_no_paths) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "webhooks": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

TEST(version_agrees_with_json_export) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
  EXPECT_EQ(frame.to_json().at("version"), sourcemeta::core::JSON{"3.1"});
}

TEST(info_required_fields_only) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document};
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

  const sourcemeta::core::OpenAPIFrame frame{document};
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

  const sourcemeta::core::OpenAPIFrame frame{document};
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

  const sourcemeta::core::OpenAPIFrame frame{document};
  EXPECT_EQ(frame.info().terms_of_service.value(), "/terms");
}

TEST(info_empty_contact) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0", "contact": {} },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document};
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

  const sourcemeta::core::OpenAPIFrame frame{document};
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

  const sourcemeta::core::OpenAPIFrame frame{document};
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

  const sourcemeta::core::OpenAPIFrame frame{document};
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

  const sourcemeta::core::OpenAPIFrame frame{document};
  EXPECT_TRUE(frame.base().empty());
}

TEST(base_is_what_the_caller_established) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, "https://example.com/openapi.json"};
  EXPECT_EQ(frame.base(), "https://example.com/openapi.json");
}

TEST(base_is_canonicalised) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, "HTTPS://Example.COM:443/a/../openapi.json"};
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
      document,
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
      document, "https://example.com/openapi.json"};
  EXPECT_EQ(frame.to_json().at("base"),
            sourcemeta::core::JSON{"https://example.com/openapi.json"});
}

TEST(base_absent_is_the_empty_uri_reference_in_json_export) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{document};
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

  const sourcemeta::core::OpenAPIFrame frame{document};
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

  const sourcemeta::core::OpenAPIFrame frame{document};
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
      document, "https://example.com/openapi.json"};
  EXPECT_FALSE(frame.standalone());
  EXPECT_EQ(frame.dangling().size(), 1);
  EXPECT_TRUE(frame.dangling().contains(
      "https://example.com/common.json#/components/responses/Ok"));
}

TEST(dangling_is_empty_when_the_frame_stands_alone) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "paths": {}
  })JSON")};

  const sourcemeta::core::OpenAPIFrame frame{
      document, "https://example.com/openapi.json"};
  EXPECT_TRUE(frame.standalone());
  EXPECT_TRUE(frame.dangling().empty());
}

TEST(dangling_reports_one_destination_that_two_references_share) {
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
      document, "https://example.com/openapi.json"};
  EXPECT_EQ(frame.dangling().size(), 1);
  EXPECT_TRUE(frame.dangling().contains(
      "https://example.com/common.json#/components/responses/Ok"));
}

TEST(dangling_agrees_with_json_export) {
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
      document, "https://example.com/openapi.json"};
  auto expected{sourcemeta::core::JSON::make_array()};
  expected.push_back(sourcemeta::core::JSON{
      "https://example.com/common.json#/components/responses/Ok"});
  EXPECT_EQ(frame.to_json().at("dangling"), expected);
}

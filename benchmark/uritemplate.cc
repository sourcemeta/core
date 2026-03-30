#include <benchmark/benchmark.h>

#include <sourcemeta/core/uritemplate.h>

#include <array>       // std::array
#include <cassert>     // assert
#include <cstdint>     // std::uint16_t, std::int64_t
#include <filesystem>  // std::filesystem
#include <string_view> // std::string_view

static constexpr std::string_view ROUTES[] = {
    "/api/v1",
    "/api/v2",
    "/api/v3",
    "/api/v1/health",
    "/api/v2/health",
    "/api/v3/health",
    "/api/v1/users",
    "/api/v1/users/{user_id}",
    "/api/v1/users/{user_id}/profile",
    "/api/v1/users/{user_id}/settings",
    "/api/v1/users/{user_id}/preferences",
    "/api/v1/users/{user_id}/avatar",
    "/api/v1/users/{user_id}/posts",
    "/api/v1/users/{user_id}/posts/{post_id}",
    "/api/v1/users/{user_id}/posts/{post_id}/comments",
    "/api/v1/users/{user_id}/posts/{post_id}/comments/{comment_id}",
    "/api/v1/users/{user_id}/posts/{post_id}/likes",
    "/api/v1/users/{user_id}/followers",
    "/api/v1/users/{user_id}/following",
    "/api/v1/users/{user_id}/notifications",
    "/api/v1/users/{user_id}/notifications/{notification_id}",
    "/api/v1/organizations",
    "/api/v1/organizations/{org_id}",
    "/api/v1/organizations/{org_id}/members",
    "/api/v1/organizations/{org_id}/members/{member_id}",
    "/api/v1/organizations/{org_id}/teams",
    "/api/v1/organizations/{org_id}/teams/{team_id}",
    "/api/v1/organizations/{org_id}/teams/{team_id}/members",
    "/api/v1/organizations/{org_id}/projects",
    "/api/v1/organizations/{org_id}/projects/{project_id}",
    "/api/v1/organizations/{org_id}/billing",
    "/api/v1/organizations/{org_id}/invoices",
    "/api/v1/organizations/{org_id}/invoices/{invoice_id}",
    "/api/v1/projects",
    "/api/v1/projects/{project_id}",
    "/api/v1/projects/{project_id}/issues",
    "/api/v1/projects/{project_id}/issues/{issue_id}",
    "/api/v1/projects/{project_id}/issues/{issue_id}/comments",
    "/api/v1/projects/{project_id}/issues/{issue_id}/comments/{comment_id}",
    "/api/v1/projects/{project_id}/issues/{issue_id}/labels",
    "/api/v1/projects/{project_id}/issues/{issue_id}/assignees",
    "/api/v1/projects/{project_id}/milestones",
    "/api/v1/projects/{project_id}/milestones/{milestone_id}",
    "/api/v1/projects/{project_id}/releases",
    "/api/v1/projects/{project_id}/releases/{release_id}",
    "/api/v1/projects/{project_id}/releases/{release_id}/assets",
    "/api/v1/projects/{project_id}/branches",
    "/api/v1/projects/{project_id}/branches/{branch_name}",
    "/api/v1/projects/{project_id}/commits",
    "/api/v1/projects/{project_id}/commits/{commit_sha}",
    "/api/v1/projects/{project_id}/pulls",
    "/api/v1/projects/{project_id}/pulls/{pull_id}",
    "/api/v1/projects/{project_id}/pulls/{pull_id}/reviews",
    "/api/v1/projects/{project_id}/pulls/{pull_id}/reviews/{review_id}",
    "/api/v1/projects/{project_id}/pulls/{pull_id}/commits",
    "/api/v1/projects/{project_id}/pulls/{pull_id}/files",
    "/api/v1/documents",
    "/api/v1/documents/{document_id}",
    "/api/v1/documents/{document_id}/versions",
    "/api/v1/documents/{document_id}/versions/{version_id}",
    "/api/v1/documents/{document_id}/permissions",
    "/api/v1/documents/{document_id}/comments",
    "/api/v1/documents/{document_id}/comments/{comment_id}",
    "/api/v1/documents/{document_id}/shares",
    "/api/v1/documents/{document_id}/exports/{format}",
    "/api/v1/storage/buckets",
    "/api/v1/storage/buckets/{bucket_id}",
    "/api/v1/storage/buckets/{bucket_id}/objects",
    "/api/v1/storage/buckets/{bucket_id}/objects/{object_key}",
    "/api/v1/storage/buckets/{bucket_id}/objects/{object_key}/metadata",
    "/api/v1/storage/buckets/{bucket_id}/objects/{object_key}/versions",
    "/api/v1/storage/buckets/{bucket_id}/policies",
    "/api/v1/analytics/events",
    "/api/v1/analytics/events/{event_id}",
    "/api/v1/analytics/reports",
    "/api/v1/analytics/reports/{report_id}",
    "/api/v1/analytics/dashboards",
    "/api/v1/analytics/dashboards/{dashboard_id}",
    "/api/v1/analytics/dashboards/{dashboard_id}/widgets",
    "/api/v1/analytics/dashboards/{dashboard_id}/widgets/{widget_id}",
    "/api/v1/webhooks",
    "/api/v1/webhooks/{webhook_id}",
    "/api/v1/webhooks/{webhook_id}/deliveries",
    "/api/v1/webhooks/{webhook_id}/deliveries/{delivery_id}",
    "/api/v1/search/users",
    "/api/v1/search/projects",
    "/api/v1/search/documents",
    "/api/v1/search/issues",
    "/api/v1/admin/users",
    "/api/v1/admin/users/{user_id}",
    "/api/v1/admin/organizations",
    "/api/v1/admin/organizations/{org_id}",
    "/api/v1/admin/logs",
    "/api/v1/admin/logs/{log_id}",
    "/api/v1/admin/settings",
    "/api/v1/admin/settings/{setting_key}",
    "/api/v1/organizations/{org_id}/teams/{team_id}/projects/{project_id}/"
    "issues/{issue_id}/comments/{comment_id}/reactions/{reaction_id}",
    "/api/v1/workspaces/{workspace_id}/folders/{folder_id}/documents/"
    "{document_id}/sections/{section_id}/paragraphs/{paragraph_id}/comments/"
    "{comment_id}"};

static constexpr std::size_t ROUTE_COUNT = sizeof(ROUTES) / sizeof(ROUTES[0]);

static void URITemplateRouter_Create(benchmark::State &state) {
  for (auto _ : state) {
    sourcemeta::core::URITemplateRouter router;
    for (std::size_t index = 0; index < ROUTE_COUNT; ++index) {
      router.add(ROUTES[index],
                 static_cast<sourcemeta::core::URITemplateRouter::Identifier>(
                     index + 1));
    }

    benchmark::DoNotOptimize(router);
  }
}

static void URITemplateRouter_Match(benchmark::State &state) {
  sourcemeta::core::URITemplateRouter router;
  for (std::size_t index = 0; index < ROUTE_COUNT; ++index) {
    router.add(ROUTES[index],
               static_cast<sourcemeta::core::URITemplateRouter::Identifier>(
                   index + 1));
  }

  for (auto _ : state) {
    auto result = router.match(
        "/api/v1/organizations/12345/teams/67890/projects/abc/issues/999/"
        "comments/42/reactions/1",
        [](auto, auto, auto) {});
    assert(result == ROUTE_COUNT - 1);
    benchmark::DoNotOptimize(result);
  }
}

static void URITemplateRouterView_Restore(benchmark::State &state) {
  const auto path{std::filesystem::temp_directory_path() /
                  "uritemplate_benchmark.bin"};

  {
    sourcemeta::core::URITemplateRouter router;
    for (std::size_t index = 0; index < ROUTE_COUNT; ++index) {
      router.add(ROUTES[index],
                 static_cast<sourcemeta::core::URITemplateRouter::Identifier>(
                     index + 1));
    }

    sourcemeta::core::URITemplateRouterView::save(router, path);
  }

  for (auto _ : state) {
    sourcemeta::core::URITemplateRouterView view{path};
    benchmark::DoNotOptimize(view);
  }

  std::filesystem::remove(path);
}

static void URITemplateRouterView_Match(benchmark::State &state) {
  const auto path{std::filesystem::temp_directory_path() /
                  "uritemplate_benchmark.bin"};

  {
    sourcemeta::core::URITemplateRouter router;
    for (std::size_t index = 0; index < ROUTE_COUNT; ++index) {
      router.add(ROUTES[index],
                 static_cast<sourcemeta::core::URITemplateRouter::Identifier>(
                     index + 1));
    }

    sourcemeta::core::URITemplateRouterView::save(router, path);
  }

  // Make sure the router view destructor runs BEFORE we attempt to delete the
  // file
  {
    sourcemeta::core::URITemplateRouterView view{path};
    for (auto _ : state) {
      auto result = view.match(
          "/api/v1/organizations/12345/teams/67890/projects/abc/issues/999/"
          "comments/42/reactions/1",
          [](auto, auto, auto) {});
      assert(result == ROUTE_COUNT - 1);
      benchmark::DoNotOptimize(result);
    }
  }

  std::filesystem::remove(path);
}

static void URITemplateRouterView_Arguments(benchmark::State &state) {
  const auto path{std::filesystem::temp_directory_path() /
                  "uritemplate_benchmark_arguments.bin"};

  // clang-format off
  constexpr std::string_view long_value =
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
      "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
      "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"
      "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "gggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg"
      "hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh";
  const std::array<sourcemeta::core::URITemplateRouter::Argument, 100>
      many_arguments{{
          {"argument_00", std::string_view{"short_value_0"}},
          {"argument_01", std::int64_t{1}},
          {"argument_02", true},
          {"argument_03", long_value},
          {"argument_04", std::int64_t{-100}},
          {"argument_05", false},
          {"argument_06", std::string_view{"response/schema/path"}},
          {"argument_07", std::int64_t{999999}},
          {"argument_08", true},
          {"argument_09", std::string_view{"another_short"}},
          {"argument_10", std::int64_t{0}},
          {"argument_11", false},
          {"argument_12", long_value},
          {"argument_13", std::int64_t{42}},
          {"argument_14", true},
          {"argument_15", std::string_view{"path/to/resource"}},
          {"argument_16", std::int64_t{-1}},
          {"argument_17", false},
          {"argument_18", std::string_view{"value_18"}},
          {"argument_19", std::int64_t{12345}},
          {"argument_20", true},
          {"argument_21", long_value},
          {"argument_22", std::int64_t{67890}},
          {"argument_23", false},
          {"argument_24", std::string_view{"config/key"}},
          {"argument_25", std::int64_t{-999}},
          {"argument_26", true},
          {"argument_27", std::string_view{"value_27"}},
          {"argument_28", std::int64_t{100}},
          {"argument_29", false},
          {"argument_30", long_value},
          {"argument_31", std::int64_t{200}},
          {"argument_32", true},
          {"argument_33", std::string_view{"schemas/api/list"}},
          {"argument_34", std::int64_t{300}},
          {"argument_35", false},
          {"argument_36", std::string_view{"value_36"}},
          {"argument_37", std::int64_t{400}},
          {"argument_38", true},
          {"argument_39", long_value},
          {"argument_40", std::int64_t{500}},
          {"argument_41", false},
          {"argument_42", std::string_view{"target/url/path"}},
          {"argument_43", std::int64_t{600}},
          {"argument_44", true},
          {"argument_45", std::string_view{"value_45"}},
          {"argument_46", std::int64_t{700}},
          {"argument_47", false},
          {"argument_48", long_value},
          {"argument_49", std::int64_t{800}},
          {"argument_50", true},
          {"argument_51", std::string_view{"redirect/path"}},
          {"argument_52", std::int64_t{900}},
          {"argument_53", false},
          {"argument_54", std::string_view{"value_54"}},
          {"argument_55", std::int64_t{1000}},
          {"argument_56", true},
          {"argument_57", long_value},
          {"argument_58", std::int64_t{1100}},
          {"argument_59", false},
          {"argument_60", std::string_view{"webhook/endpoint"}},
          {"argument_61", std::int64_t{1200}},
          {"argument_62", true},
          {"argument_63", std::string_view{"value_63"}},
          {"argument_64", std::int64_t{1300}},
          {"argument_65", false},
          {"argument_66", long_value},
          {"argument_67", std::int64_t{1400}},
          {"argument_68", true},
          {"argument_69", std::string_view{"proxy/upstream"}},
          {"argument_70", std::int64_t{1500}},
          {"argument_71", false},
          {"argument_72", std::string_view{"value_72"}},
          {"argument_73", std::int64_t{1600}},
          {"argument_74", true},
          {"argument_75", long_value},
          {"argument_76", std::int64_t{1700}},
          {"argument_77", false},
          {"argument_78", std::string_view{"static/directory"}},
          {"argument_79", std::int64_t{1800}},
          {"argument_80", true},
          {"argument_81", std::string_view{"value_81"}},
          {"argument_82", std::int64_t{1900}},
          {"argument_83", false},
          {"argument_84", long_value},
          {"argument_85", std::int64_t{2000}},
          {"argument_86", true},
          {"argument_87", std::string_view{"cache/control"}},
          {"argument_88", std::int64_t{2100}},
          {"argument_89", false},
          {"argument_90", std::string_view{"value_90"}},
          {"argument_91", std::int64_t{2200}},
          {"argument_92", true},
          {"argument_93", long_value},
          {"argument_94", std::int64_t{2300}},
          {"argument_95", false},
          {"argument_96", std::string_view{"auth/token/path"}},
          {"argument_97", std::int64_t{2400}},
          {"argument_98", true},
          {"argument_99", std::string_view{"final_value"}},
      }};
  // clang-format on

  {
    sourcemeta::core::URITemplateRouter router;
    const std::array<sourcemeta::core::URITemplateRouter::Argument, 1>
        small_args{{{"schema", std::string_view{"schemas/health"}}}};
    router.add("/api/v1/health", 1, small_args);
    router.add("/api/v1/users/{id}", 2, small_args);
    router.add("/api/v1/many", 3, many_arguments);
    sourcemeta::core::URITemplateRouterView::save(router, path);
  }

  {
    sourcemeta::core::URITemplateRouterView view{path};

    for (auto _ : state) {
      auto result = view.match("/api/v1/many", [](auto, auto, auto) {});
      assert(result == 3);
      benchmark::DoNotOptimize(result);

      std::size_t argument_count = 0;
      view.arguments(
          3, [&argument_count](auto, const auto &) { ++argument_count; });
      assert(argument_count == 100);
      benchmark::DoNotOptimize(argument_count);
    }
  }

  std::filesystem::remove(path);
}

BENCHMARK(URITemplateRouter_Create);
BENCHMARK(URITemplateRouter_Match);
BENCHMARK(URITemplateRouterView_Restore);
BENCHMARK(URITemplateRouterView_Match);
BENCHMARK(URITemplateRouterView_Arguments);

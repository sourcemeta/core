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
          {"arg_00", std::string_view{"short_value_0"}},
          {"arg_01", std::int64_t{1}},
          {"arg_02", true},
          {"arg_03", long_value},
          {"arg_04", std::int64_t{-100}},
          {"arg_05", false},
          {"arg_06", std::string_view{"response/schema/path"}},
          {"arg_07", std::int64_t{999999}},
          {"arg_08", true},
          {"arg_09", std::string_view{"another_short"}},
          {"arg_10", std::int64_t{0}},
          {"arg_11", false},
          {"arg_12", long_value},
          {"arg_13", std::int64_t{42}},
          {"arg_14", true},
          {"arg_15", std::string_view{"path/to/resource"}},
          {"arg_16", std::int64_t{-1}},
          {"arg_17", false},
          {"arg_18", std::string_view{"value_18"}},
          {"arg_19", std::int64_t{12345}},
          {"arg_20", true},
          {"arg_21", long_value},
          {"arg_22", std::int64_t{67890}},
          {"arg_23", false},
          {"arg_24", std::string_view{"config/key"}},
          {"arg_25", std::int64_t{-999}},
          {"arg_26", true},
          {"arg_27", std::string_view{"value_27"}},
          {"arg_28", std::int64_t{100}},
          {"arg_29", false},
          {"arg_30", long_value},
          {"arg_31", std::int64_t{200}},
          {"arg_32", true},
          {"arg_33", std::string_view{"schemas/api/list"}},
          {"arg_34", std::int64_t{300}},
          {"arg_35", false},
          {"arg_36", std::string_view{"value_36"}},
          {"arg_37", std::int64_t{400}},
          {"arg_38", true},
          {"arg_39", long_value},
          {"arg_40", std::int64_t{500}},
          {"arg_41", false},
          {"arg_42", std::string_view{"target/url/path"}},
          {"arg_43", std::int64_t{600}},
          {"arg_44", true},
          {"arg_45", std::string_view{"value_45"}},
          {"arg_46", std::int64_t{700}},
          {"arg_47", false},
          {"arg_48", long_value},
          {"arg_49", std::int64_t{800}},
          {"arg_50", true},
          {"arg_51", std::string_view{"redirect/path"}},
          {"arg_52", std::int64_t{900}},
          {"arg_53", false},
          {"arg_54", std::string_view{"value_54"}},
          {"arg_55", std::int64_t{1000}},
          {"arg_56", true},
          {"arg_57", long_value},
          {"arg_58", std::int64_t{1100}},
          {"arg_59", false},
          {"arg_60", std::string_view{"webhook/endpoint"}},
          {"arg_61", std::int64_t{1200}},
          {"arg_62", true},
          {"arg_63", std::string_view{"value_63"}},
          {"arg_64", std::int64_t{1300}},
          {"arg_65", false},
          {"arg_66", long_value},
          {"arg_67", std::int64_t{1400}},
          {"arg_68", true},
          {"arg_69", std::string_view{"proxy/upstream"}},
          {"arg_70", std::int64_t{1500}},
          {"arg_71", false},
          {"arg_72", std::string_view{"value_72"}},
          {"arg_73", std::int64_t{1600}},
          {"arg_74", true},
          {"arg_75", long_value},
          {"arg_76", std::int64_t{1700}},
          {"arg_77", false},
          {"arg_78", std::string_view{"static/directory"}},
          {"arg_79", std::int64_t{1800}},
          {"arg_80", true},
          {"arg_81", std::string_view{"value_81"}},
          {"arg_82", std::int64_t{1900}},
          {"arg_83", false},
          {"arg_84", long_value},
          {"arg_85", std::int64_t{2000}},
          {"arg_86", true},
          {"arg_87", std::string_view{"cache/control"}},
          {"arg_88", std::int64_t{2100}},
          {"arg_89", false},
          {"arg_90", std::string_view{"value_90"}},
          {"arg_91", std::int64_t{2200}},
          {"arg_92", true},
          {"arg_93", long_value},
          {"arg_94", std::int64_t{2300}},
          {"arg_95", false},
          {"arg_96", std::string_view{"auth/token/path"}},
          {"arg_97", std::int64_t{2400}},
          {"arg_98", true},
          {"arg_99", std::string_view{"final_value"}},
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

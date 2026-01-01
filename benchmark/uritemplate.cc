#include <benchmark/benchmark.h>

#include <sourcemeta/core/uritemplate.h>

#include <cassert>     // assert
#include <cstdint>     // std::uint16_t
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
  sourcemeta::core::URITemplateRouter router;
  for (std::size_t index = 0; index < ROUTE_COUNT; ++index) {
    router.add(ROUTES[index],
               static_cast<sourcemeta::core::URITemplateRouter::Identifier>(
                   index + 1));
  }

  const std::filesystem::path path =
      std::filesystem::temp_directory_path() / "uritemplate_benchmark.bin";
  sourcemeta::core::URITemplateRouterView::save(router, path);

  for (auto _ : state) {
    sourcemeta::core::URITemplateRouterView view{path};
    benchmark::DoNotOptimize(view);
  }

  std::filesystem::remove(path);
}

// TODO: There is a crash on Windows that we are investigating
#if !defined(_MSC_VER)
static void URITemplateRouterView_Match(benchmark::State &state) {
  sourcemeta::core::URITemplateRouter router;
  for (std::size_t index = 0; index < ROUTE_COUNT; ++index) {
    router.add(ROUTES[index],
               static_cast<sourcemeta::core::URITemplateRouter::Identifier>(
                   index + 1));
  }

  const std::filesystem::path path =
      std::filesystem::temp_directory_path() / "uritemplate_benchmark.bin";
  sourcemeta::core::URITemplateRouterView::save(router, path);
  sourcemeta::core::URITemplateRouterView view{path};

  for (auto _ : state) {
    auto result = view.match(
        "/api/v1/organizations/12345/teams/67890/projects/abc/issues/999/"
        "comments/42/reactions/1",
        [](auto, auto, auto) {});
    assert(result == ROUTE_COUNT - 1);
    benchmark::DoNotOptimize(result);
  }

  std::filesystem::remove(path);
}
#endif

BENCHMARK(URITemplateRouter_Create);
BENCHMARK(URITemplateRouter_Match);
BENCHMARK(URITemplateRouterView_Restore);
#if !defined(_MSC_VER)
BENCHMARK(URITemplateRouterView_Match);
#endif

#include <sourcemeta/core/http_problem.h>

#include <cassert> // assert

namespace {

const auto HTTP_HASH_TYPE{sourcemeta::core::JSON::Object::hash("type")};
const auto HTTP_HASH_TITLE{sourcemeta::core::JSON::Object::hash("title")};
const auto HTTP_HASH_STATUS{sourcemeta::core::JSON::Object::hash("status")};
const auto HTTP_HASH_DETAIL{sourcemeta::core::JSON::Object::hash("detail")};
const auto HTTP_HASH_INSTANCE{sourcemeta::core::JSON::Object::hash("instance")};

} // namespace

namespace sourcemeta::core {

auto http_make_problem_details(const HTTPProblemDetails &fields)
    -> sourcemeta::core::JSON {
  assert(fields.status.code >= 100 && fields.status.code <= 599);
  assert(!fields.status.phrase.empty());
  assert(fields.type.find_first_of("\r\n") == JSON::StringView::npos);
  assert(fields.title.find_first_of("\r\n") == JSON::StringView::npos);
  assert(fields.instance.find_first_of("\r\n") == JSON::StringView::npos);

  auto object{sourcemeta::core::JSON::make_object()};
  object.assign_assume_new(
      "type",
      sourcemeta::core::JSON{fields.type.empty() ? "about:blank" : fields.type},
      HTTP_HASH_TYPE);
  object.assign_assume_new("title",
                           sourcemeta::core::JSON{fields.title.empty()
                                                      ? fields.status.phrase
                                                      : fields.title},
                           HTTP_HASH_TITLE);
  object.assign_assume_new(
      "status",
      sourcemeta::core::JSON{static_cast<std::int64_t>(fields.status.code)},
      HTTP_HASH_STATUS);
  if (!fields.detail.empty()) {
    object.assign_assume_new("detail", sourcemeta::core::JSON{fields.detail},
                             HTTP_HASH_DETAIL);
  }
  if (!fields.instance.empty()) {
    object.assign_assume_new("instance",
                             sourcemeta::core::JSON{fields.instance},
                             HTTP_HASH_INSTANCE);
  }
  return object;
}

} // namespace sourcemeta::core

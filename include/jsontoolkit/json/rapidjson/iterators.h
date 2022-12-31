#ifndef JSONTOOLKIT_JSON_RAPIDJSON_ITERATORS_H_
#define JSONTOOLKIT_JSON_RAPIDJSON_ITERATORS_H_

#include "common.h"
#include "read.h"

#include <cassert>  // assert
#include <iterator> // std::reverse_iterator

namespace sourcemeta::jsontoolkit {

inline auto value(const rapidjson::Value::Member &member) -> const JSONValue & {
  return member.value;
}

inline auto key(rapidjson::Value::Member &member) -> std::string {
  return member.name.GetString();
}

inline auto value(rapidjson::Value::Member &member) -> JSONValue & {
  return member.value;
}

inline auto cbegin_array(const JSONValue &value)
    -> rapidjson::Value::ConstValueIterator {
  assert(is_array(value));
  return value.Begin();
}

inline auto cend_array(const JSONValue &value)
    -> rapidjson::Value::ConstValueIterator {
  assert(is_array(value));
  return value.End();
}

// See https://github.com/Tencent/rapidjson/issues/678
inline auto crbegin_array(const JSONValue &value)
    -> std::reverse_iterator<rapidjson::Value::ConstValueIterator> {
  assert(is_array(value));
  return std::reverse_iterator<rapidjson::Value::ConstValueIterator>{
      value.End()};
}

// See https://github.com/Tencent/rapidjson/issues/678
inline auto crend_array(const JSONValue &value)
    -> std::reverse_iterator<rapidjson::Value::ConstValueIterator> {
  assert(is_array(value));
  return std::reverse_iterator<rapidjson::Value::ConstValueIterator>{
      value.Begin()};
}

inline auto begin_array(JSONValue &value) -> rapidjson::Value::ValueIterator {
  assert(is_array(value));
  return value.Begin();
}

inline auto end_array(JSONValue &value) -> rapidjson::Value::ValueIterator {
  assert(is_array(value));
  return value.End();
}

inline auto cbegin_object(const JSONValue &value)
    -> rapidjson::Value::ConstMemberIterator {
  assert(is_object(value));
  return value.MemberBegin();
}

inline auto cend_object(const JSONValue &value)
    -> rapidjson::Value::ConstMemberIterator {
  assert(is_object(value));
  return value.MemberEnd();
}

inline auto begin_object(JSONValue &value) -> rapidjson::Value::MemberIterator {
  assert(is_object(value));
  return value.MemberBegin();
}

inline auto end_object(JSONValue &value) -> rapidjson::Value::MemberIterator {
  assert(is_object(value));
  return value.MemberEnd();
}

} // namespace sourcemeta::jsontoolkit

#endif

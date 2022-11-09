#pragma once

#include <functional>

#include <nlohmann/json.hpp>

#include "pinecone/domain/operation.hpp"
#include "pinecone/domain/operation_type.hpp"
#include "pinecone/types/index_types.hpp"

using json = nlohmann::json;

namespace pinecone::domain
{
template <>
struct operation_args<operation_type::index_list>
    : public list_operation_args<operation_type::index_list> {
  using list_operation_args::list_operation_args;

  using parsed_type = types::indexes;

  static const std::function<result<parsed_type>(json&&)> parser;
};
inline std::function<result<types::indexes>(json&&)> const
    operation_args<operation_type::index_list>::parser =
        [](auto&& json) { return parsed_type::build(std::forward<decltype(json)>(json)); };

template <>
struct operation_args<operation_type::index_describe>
    : public describe_delete_operation_args<operation_type::index_describe> {
  using describe_delete_operation_args::describe_delete_operation_args;

  using parsed_type = types::database;

  static const std::function<result<parsed_type>(json&&)> parser;
};
inline std::function<result<types::database>(json&&)> const
    operation_args<operation_type::index_describe>::parser =
        [](auto&& json) { return parsed_type::build(std::forward<decltype(json)>(json)); };

template <>
struct operation_args<operation_type::index_delete>
    : public describe_delete_operation_args<operation_type::index_delete> {
  using describe_delete_operation_args::describe_delete_operation_args;

  using parsed_type = json;

  static const std::function<result<parsed_type>(json&&)> parser;
};
inline std::function<result<json>(json&&)> const
    operation_args<operation_type::index_delete>::parser = [](auto&& json) { return json; };

template <>
struct operation_args<operation_type::collection_list>
    : public list_operation_args<operation_type::collection_list> {
  using list_operation_args::list_operation_args;

  using parsed_type = types::collections;

  static const std::function<result<parsed_type>(json&&)> parser;
};
inline std::function<result<types::collections>(json&&)> const
    operation_args<operation_type::collection_list>::parser =
        [](auto&& json) { return parsed_type::build(std::forward<decltype(json)>(json)); };

template <>
struct operation_args<operation_type::collection_describe>
    : public describe_delete_operation_args<operation_type::collection_describe> {
  using describe_delete_operation_args::describe_delete_operation_args;

  using parsed_type = types::collection;

  static const std::function<result<parsed_type>(json&&)> parser;
};
inline std::function<result<types::collection>(json&&)> const
    operation_args<operation_type::collection_describe>::parser =
        [](auto&& json) { return parsed_type::build(std::forward<decltype(json)>(json)); };

template <>
struct operation_args<operation_type::collection_delete>
    : public describe_delete_operation_args<operation_type::collection_delete> {
  using describe_delete_operation_args::describe_delete_operation_args;

  using parsed_type = json;

  static const std::function<result<json>(json&&)> parser;
};
inline std::function<result<json>(json&&)> const
    operation_args<operation_type::collection_delete>::parser = [](auto&& json) { return json; };
}  // namespace pinecone::domain